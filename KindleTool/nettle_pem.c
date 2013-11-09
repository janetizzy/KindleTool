//
//  nettle_pem.c
//  KindleTool
//
//  Copyright (C) 2011-2013  Yifan Lu
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "kindle_tool.h"

#ifdef KT_USE_NETTLE
// Most of this is *heavily* inspired from the pkcs1-conv nettle tool...
enum object_type
{
    /* Use a range of values which also work as option id:s */
    RSA_PRIVATE_KEY = 0x200,
    RSA_PUBLIC_KEY,
    DSA_PRIVATE_KEY,
    /* DSA public keys only supported as part of a SubjectPublicKeyInfo, i.e., the GENERAL_PUBLIC_KEY case. */
    GENERAL_PUBLIC_KEY,
};

/* Return 1 on success, 0 on error, -1 on eof */
static int read_line(struct nettle_buffer *buffer, FILE *f)
{
    int c;

    while((c = getc(f)) != EOF)
    {
        if(!NETTLE_BUFFER_PUTC(buffer, c))
            return 0;

        if(c == '\n')
            return 1;
    }
    if(ferror(f))
    {
        fprintf(stderr, "Read failed: %s\n", strerror(errno));
        return 0;
    }

    else
        return -1;
}

static int read_file(struct nettle_buffer *buffer, FILE *f)
{
    int c;

    while((c = getc(f)) != EOF)
        if(!NETTLE_BUFFER_PUTC(buffer, c))
            return 0;

    if(ferror(f))
    {
        fprintf(stderr, "Read failed: %s\n", strerror(errno));
        return 0;
    }
    else
        return 1;
}

static const uint8_t pem_start_pattern[11] = "-----BEGIN ";

static const uint8_t pem_end_pattern[9] = "-----END ";

static const uint8_t pem_trailer_pattern[5] = "-----";

static const char pem_ws[33] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0, 0, /* \t, \n, \v, \f, \r */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1 /* SPC */
};

#define PEM_IS_SPACE(c) ((c) < sizeof(pem_ws) && pem_ws[(c)])

/* Returns 1 on match, otherwise 0. */
static int match_pem_start(size_t length, const uint8_t *line, size_t *marker_start, size_t *marker_length)
{
    while(length > 0 && PEM_IS_SPACE(line[length - 1]))
        length--;

    if(length > (sizeof(pem_start_pattern) + sizeof(pem_trailer_pattern)) && memcmp(line, pem_start_pattern, sizeof(pem_start_pattern)) == 0 && memcmp(line + length - sizeof(pem_trailer_pattern), pem_trailer_pattern, sizeof(pem_trailer_pattern)) == 0)
    {
        *marker_start = 11;
        *marker_length = length - (sizeof(pem_start_pattern) + sizeof(pem_trailer_pattern));

        return 1;
    }
    else
        return 0;
}

/* Returns 1 on match, -1 if the line is of the right form except for
   the marker, otherwise 0. */
static int match_pem_end(size_t length, const uint8_t *line, size_t marker_length, const uint8_t *marker)
{
    while(length > 0 && PEM_IS_SPACE(line[length - 1]))
        length--;

    if(length > (sizeof(pem_end_pattern) + sizeof(pem_trailer_pattern)) && memcmp(line, pem_end_pattern, sizeof(pem_end_pattern)) == 0 && memcmp(line + length - sizeof(pem_trailer_pattern), pem_trailer_pattern, sizeof(pem_trailer_pattern)) == 0)
    {
        /* Right form. Check marker */
        if(length == marker_length + (sizeof(pem_end_pattern) + sizeof(pem_trailer_pattern)) && memcmp(line + sizeof(pem_end_pattern), marker, marker_length) == 0)
            return 1;
        else
            return -1;
    }
    else
        return 0;
}

struct pem_info
{
    /* The FOO part in "-----BEGIN FOO-----" */
    size_t marker_start;
    size_t marker_length;
    size_t data_start;
    size_t data_length;
};

static int read_pem(struct nettle_buffer *buffer, FILE *f, struct pem_info *info)
{
    /* Find start line */
    for(;;)
    {
        int res;

        nettle_buffer_reset(buffer);

        res = read_line(buffer, f);
        if(res != 1)
            return res;

        if(match_pem_start(buffer->size, buffer->contents, &info->marker_start, &info->marker_length))
            break;
    }

    /* NUL-terminate the marker. Don't care to check for embedded NULs. */
    buffer->contents[info->marker_start + info->marker_length] = 0;

    info->data_start = buffer->size;

    for(;;)
    {
        size_t line_start = buffer->size;

        if(read_line(buffer, f) != 1)
            return 0;

        switch(match_pem_end(buffer->size - line_start, buffer->contents + line_start, info->marker_length, buffer->contents + info->marker_start))
        {
            case 0:
                break;
            case -1:
                fprintf(stderr, "PEM END line doesn't match BEGIN.\n");
                return 0;
            case 1:
                /* Return base 64 data; let caller do the decoding */
                info->data_length = line_start - info->data_start;
                return 1;
        }
    }
}

static int decode_base64(struct nettle_buffer *buffer, size_t start, size_t *length)
{
    struct base64_decode_ctx ctx;

    base64_decode_init(&ctx);

    /* Decode in place */
    if(base64_decode_update(&ctx, length, buffer->contents + start, *length, buffer->contents + start) && base64_decode_final(&ctx))
        return 1;

    else
    {
        fprintf(stderr, "Invalid base64 date.\n");
        return 0;
    }
}

static int convert_rsa_private_key(struct nettle_buffer *buffer, size_t length, const uint8_t *data)
{
    struct rsa_public_key pub;
    struct rsa_private_key priv;
    int res;

    rsa_public_key_init(&pub);
    rsa_private_key_init(&priv);

    if(rsa_keypair_from_der(&pub, &priv, 0, length, data))
    {
        /* Reuses the buffer */
        nettle_buffer_reset(buffer);
        res = rsa_keypair_to_sexp(buffer, NULL, &pub, &priv);
    }
    else
    {
        fprintf(stderr, "Invalid PKCS#1 private key.\n");
        res = 0;
    }
    rsa_public_key_clear(&pub);
    rsa_private_key_clear(&priv);

    return res;
}

/* NOTE: Destroys contents of buffer */
/* Returns 1 on success, 0 on error, and -1 for unsupported algorithms. */
static int convert_type(struct nettle_buffer *buffer, enum object_type type, size_t length, const uint8_t *data)
{
    int res;

    switch(type)
    {
        default:
            abort();

        case RSA_PRIVATE_KEY:
            res = convert_rsa_private_key(buffer, length, data);
            break;
    }

    /*
    if(res > 0)
        res = write_file(buffer, stdout);
    */

    return res;
}

static int convert_file(struct nettle_buffer *buffer, FILE *f, enum object_type type, int base64)
{
    if(type)
    {
        read_file(buffer, f);
        if(base64 && !decode_base64(buffer, 0, &buffer->size))
            return 0;

        if(convert_type(buffer, type, buffer->size, buffer->contents) != 1)
            return 0;

        return 1;
    }
    else
    {
        /* PEM processing */
        for(;;)
        {
            struct pem_info info;
            const uint8_t *marker;

            nettle_buffer_reset(buffer);
            switch(read_pem(buffer, f, &info))
            {
                default:
                    return 0;
                case 1:
                    break;
                case -1:
                    /* EOF */
                    return 1;
            }

            if(!decode_base64(buffer, info.data_start, &info.data_length))
                return 0;

            marker = buffer->contents + info.marker_start;

            type = 0;
            switch(info.marker_length)
            {
                case 10:
                    if(memcmp(marker, "PUBLIC KEY", 10) == 0)
                    {
                        type = GENERAL_PUBLIC_KEY;
                        break;
                    }
                case 14:
                    if(memcmp(marker, "RSA PUBLIC KEY", 14) == 0)
                    {
                        type = RSA_PUBLIC_KEY;
                        break;
                    }

                case 15:
                    if(memcmp(marker, "RSA PRIVATE KEY", 15) == 0)
                    {
                        type = RSA_PRIVATE_KEY;
                        break;
                    }
                    if(memcmp(marker, "DSA PRIVATE KEY", 15) == 0)
                    {
                        type = DSA_PRIVATE_KEY;
                        break;
                    }
            }

            if(!type)
                fprintf(stderr, "Ignoring unsupported object type `%s'.\n", marker);

            else if(convert_type(buffer, type, info.data_length, buffer->contents + info.data_start) != 1)
                return 0;
        }
    }
}

int main_pem_import(char *pem_filename)
{
    struct nettle_buffer buffer;
    enum object_type type = 0;
    int base64 = 0;

    nettle_buffer_init_realloc(&buffer, NULL, nettle_xrealloc);

    const char *mode = (type || base64) ? "r" : "rb";

    FILE *f = fopen(pem_filename, mode);
    if(!f)
    {
        fprintf(stderr, "Failed to open `%s': %s.\n", pem_filename, strerror(errno));
        return EXIT_FAILURE;
    }

    if(!convert_file(&buffer, f, type, base64))
        return EXIT_FAILURE;

    fclose(f);
    nettle_buffer_clear(&buffer);

    return EXIT_SUCCESS;
}

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs on;