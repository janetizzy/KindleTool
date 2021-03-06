//
//  kindle_tool.c
//  KindleTool
//
//  Copyright (C) 2011-2015  Yifan Lu
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
#include "kindle_table.h"

// Ugly global.
unsigned int kt_with_unknown_devcodes;

void md(unsigned char *bytes, size_t length)
{
    unsigned int i;
    for(i = 0; i < length; i++)
    {
        bytes[i] = (unsigned char)ptog[bytes[i]];
    }
}

void dm(unsigned char *bytes, size_t length)
{
    unsigned int i;
    for(i = 0; i < length; i++)
    {
        bytes[i] = (unsigned char)gtop[bytes[i]];
    }
}

int munger(FILE *input, FILE *output, size_t length, const unsigned int fake_sign)
{
    unsigned char bytes[BUFFER_SIZE];
    size_t bytes_read;
    size_t bytes_written;

    while((bytes_read = fread(bytes, sizeof(unsigned char), (length < BUFFER_SIZE && length > 0 ? length : BUFFER_SIZE), input)) > 0)
    {
        // Don't munge if we asked for a fake package
        if(!fake_sign)
            md(bytes, bytes_read);
        bytes_written = fwrite(bytes, sizeof(unsigned char), bytes_read, output);
        if(ferror(output) != 0)
        {
            fprintf(stderr, "Error munging, cannot write to output: %s.\n", strerror(errno));
            return -1;
        }
        else if(bytes_written < bytes_read)
        {
            fprintf(stderr, "Error munging, read %zu bytes but only wrote %zu bytes.\n", bytes_read, bytes_written);
            return -1;
        }
        length -= bytes_read;
    }
    if(ferror(input) != 0)
    {
        fprintf(stderr, "Error munging, cannot read input: %s.\n", strerror(errno));
        return -1;
    }

    return 0;
}

int demunger(FILE *input, FILE *output, size_t length, const unsigned int fake_sign)
{
    unsigned char bytes[BUFFER_SIZE];
    size_t bytes_read;
    size_t bytes_written;

    while((bytes_read = fread(bytes, sizeof(unsigned char), (length < BUFFER_SIZE && length > 0 ? length : BUFFER_SIZE), input)) > 0)
    {
        // Don't demunge if we supplied a fake package
        if(!fake_sign)
            dm(bytes, bytes_read);
        bytes_written = fwrite(bytes, sizeof(unsigned char), bytes_read, output);
        if(ferror(output) != 0)
        {
            fprintf(stderr, "Error demunging, cannot write to output: %s.\n", strerror(errno));
            return -1;
        }
        else if(bytes_written < bytes_read)
        {
            fprintf(stderr, "Error demunging, read %zu bytes but only wrote %zu bytes.\n", bytes_read, bytes_written);
            return -1;
        }
        length -= bytes_read;
    }
    if(ferror(input) != 0)
    {
        fprintf(stderr, "Error demunging, cannot read input: %s.\n", strerror(errno));
        return -1;
    }

    return 0;
}

const char *convert_device_id(Device dev)
{
    switch(dev)
    {
        case Kindle1:
            return "Kindle 1";
        case Kindle2US:
            return "Kindle 2 US";
        case Kindle2International:
            return "Kindle 2 International";
        case KindleDXUS:
            return "Kindle DX US";
        case KindleDXInternational:
            return "Kindle DX International";
        case KindleDXGraphite:
            return "Kindle DX Graphite";
        case Kindle3Wifi:
            return "Kindle 3 Wifi";
        case Kindle3Wifi3G:
            return "Kindle 3 Wifi+3G";
        case Kindle3Wifi3GEurope:
            return "Kindle 3 Wifi+3G Europe";
        case Kindle4NonTouch:
            return "Kindle 4 Non-Touch Silver (2011)";
        case Kindle5TouchWifi:
            return "Kindle 5 Touch Wifi";
        case Kindle5TouchWifi3G:
            return "Kindle 5 Touch Wifi+3G";
        case Kindle5TouchWifi3GEurope:
            return "Kindle 5 Touch Wifi+3G Europe";
        case Kindle5TouchUnknown:
            return "Kindle 5 Touch (Unknown Variant)";
        case Kindle4NonTouchBlack:
            return "Kindle 4 Non-Touch Black (2012)";
        case KindlePaperWhiteWifi:
            return "Kindle PaperWhite Wifi";
        case KindlePaperWhiteWifi3G:
            return "Kindle PaperWhite Wifi+3G";
        case KindlePaperWhiteWifi3GCanada:
            return "Kindle PaperWhite Wifi+3G Canada";
        case KindlePaperWhiteWifi3GEurope:
            return "Kindle PaperWhite Wifi+3G Europe";
        case KindlePaperWhiteWifi3GJapan:
            return "Kindle PaperWhite Wifi+3G Japan";
        case KindlePaperWhiteWifi3GBrazil:
            return "Kindle PaperWhite Wifi+3G Brazil";
        case KindlePaperWhite2Wifi:
            return "Kindle PaperWhite 2 (2013) Wifi";
        case KindlePaperWhite2WifiJapan:
            return "Kindle PaperWhite 2 (2013) Wifi Japan";
        case KindlePaperWhite2Wifi3G:
            return "Kindle PaperWhite 2 (2013) Wifi+3G";
        case KindlePaperWhite2Wifi3GCanada:
            return "Kindle PaperWhite 2 (2013) Wifi+3G Canada";
        case KindlePaperWhite2Wifi3GEurope:
            return "Kindle PaperWhite 2 (2013) Wifi+3G Europe";
        case KindlePaperWhite2Wifi3GRussia:
            return "Kindle PaperWhite 2 (2013) Wifi+3G Russia";
        case KindlePaperWhite2Wifi3GJapan:
            return "Kindle PaperWhite 2 (2013) Wifi+3G Japan";
        case KindlePaperWhite2Wifi4GBInternational:
            return "Kindle PaperWhite 2 (2013) Wifi (4GB) International";
        case KindlePaperWhite2Wifi3G4GBEurope:
            return "Kindle PaperWhite 2 (2013) Wifi+3G (4GB) Europe";
        case KindlePaperWhite2Unknown_0xF4:
            return "Kindle PaperWhite 2 (2013) (Unknown Variant 0xF4)";
        case KindlePaperWhite2Unknown_0xF9:
            return "Kindle PaperWhite 2 (2013) (Unknown Variant 0xF9)";
        case KindlePaperWhite2Wifi3G4GB:
            return "Kindle PaperWhite 2 (2013) Wifi+3G (4GB)";
        case KindlePaperWhite2Unknown_0x61:
            return "Kindle PaperWhite 2 (2013) (Unknown Variant 0x61)";
        case KindlePaperWhite2Wifi3G4GBCanada:
            return "Kindle PaperWhite 2 (2013) Wifi+3G (4GB) Canada";
        case KindleBasic:
            return "Kindle Basic (2014)";
        case KindleVoyageWifi:
            return "Kindle Voyage WiFi";
        case ValidKindleUnknown_0x16:
            return "Unknown Kindle (0x16)";
        case ValidKindleUnknown_0x21:
            return "Unknown Kindle (0x21)";
        case KindleVoyageWifi3G:
            return "Kindle Voyage WiFi+3G";
        case KindleVoyageUnknown_0x2A:
            return "Kindle Voyage (Unknown Variant 0x2A)";
        case KindleVoyageUnknown_0x4F:
            return "Kindle Voyage (Unknown Variant 0x4F)";
        case KindleVoyageUnknown_0x52:
            return "Kindle Voyage (Unknown Variant 0x52)";
        case KindleVoyageWifi3GEurope:
            return "Kindle Voyage WiFi+3G Europe";
        case ValidKindleUnknown_0x07:
            return "Unknown Kindle (0x07)";
        case ValidKindleUnknown_0x0B:
            return "Unknown Kindle (0x0B)";
        case ValidKindleUnknown_0x0C:
            return "Unknown Kindle (0x0C)";
        case ValidKindleUnknown_0x0D:
            return "Unknown Kindle (0x0D)";
        case ValidKindleUnknown_0x99:
            return "Unknown Kindle (0x99)";
        case KindleBasicUnknown_0xDD:
            return "Kindle Basic (2014) (Unknown Variant 0xDD)";
        case KindlePaperWhite3Wifi:
            return "Kindle PaperWhite 3 (2015) WiFi";
        case KindlePaperWhite3Unknown_0G2:
            return "Kindle PaperWhite 3 (2015) (Unknown Variant 0G2)";
        case KindlePaperWhite3Unknown_0G4:
            return "Kindle PaperWhite 3 (2015) (Unknown Variant 0G4)";
        case KindlePaperWhite3Unknown_0G5:
            return "Kindle PaperWhite 3 (2015) (Unknown Variant 0G5)";
        case KindlePaperWhite3Unknown_0G6:
            return "Kindle PaperWhite 3 (2015) (Unknown Variant 0G6)";
        case KindlePaperWhite3Unknown_0G7:
            return "Kindle PaperWhite 3 (2015) (Unknown Variant 0G7)";
        case KindleUnknown:
        default:
            return "Unknown";
    }
}

const char *convert_platform_id(Platform plat)
{
    switch(plat)
    {
        case Plat_Unspecified:
            return "Unspecified";
        case MarioDeprecated:
            return "Mario (Deprecated)";
        case Luigi:
            return "Luigi";
        case Banjo:
            return "Banjo";
        case Yoshi:
            return "Yoshi";
        case YoshimeProto:
            return "Yoshime (Prototype)";
        case Yoshime:
            return "Yoshime (Yoshime3)";
        case Wario:
            return "Wario";
        default:
            return "Unknown";
    }
}

const char *convert_board_id(Board board)
{
    switch(board)
    {
        case Board_Unspecified:
            return "Unspecified";
        case Tequila:
            return "Tequila";
        case Whitney:
            return "Whitney";
        default:
            return "Unknown";
    }
}

const char *convert_bundle_version(BundleVersion bundlev)
{
    switch(bundlev)
    {
        case UpdateSignature:
            return "Signature";
        case OTAUpdateV2:
            return "OTA V2";
        case OTAUpdate:
            return "OTA V1";
        case RecoveryUpdate:
            return "Recovery";
        case RecoveryUpdateV2:
            return "Recovery V2";
        case UnknownUpdate:
        default:
            return "Unknown";
    }
}

BundleVersion get_bundle_version(char magic_number[MAGIC_NUMBER_LENGTH])
{
    if(!strncmp(magic_number, "FB02", MAGIC_NUMBER_LENGTH) || !strncmp(magic_number, "FB01", MAGIC_NUMBER_LENGTH))
        return RecoveryUpdate;
    else if(!strncmp(magic_number, "FB03", MAGIC_NUMBER_LENGTH))
        return RecoveryUpdateV2;
    else if(!strncmp(magic_number, "FC02", MAGIC_NUMBER_LENGTH) || !strncmp(magic_number, "FD03", MAGIC_NUMBER_LENGTH))
        return OTAUpdate;
    else if(!strncmp(magic_number, "FC04", MAGIC_NUMBER_LENGTH) || !strncmp(magic_number, "FD04", MAGIC_NUMBER_LENGTH) || !strncmp(magic_number, "FL01", MAGIC_NUMBER_LENGTH))
        return OTAUpdateV2;
    else if(!strncmp(magic_number, "SP01", MAGIC_NUMBER_LENGTH))
        return UpdateSignature;
    else if(!memcmp(magic_number, "\x1F\x8B\x08\x00", MAGIC_NUMBER_LENGTH))       // GZIP magic number
        return UserDataPackage;
    else
        return UnknownUpdate;
}

const char *convert_magic_number(char magic_number[MAGIC_NUMBER_LENGTH])
{
    if(!strncmp(magic_number, "FB02", MAGIC_NUMBER_LENGTH))
        return "(Fullbin [signed?])";           // /mnt/us/update-full.bin
    else if(!strncmp(magic_number, "FB03", MAGIC_NUMBER_LENGTH))
        return "(Fullbin [OTA?, fwo?])";        // /mnt/us/update-%lld-fwo.bin
    else if(!strncmp(magic_number, "FB", MAGIC_NUMBER_LENGTH/2))
        return "(Fullbin)";
    else if(!strncmp(magic_number, "FC", MAGIC_NUMBER_LENGTH/2))
        return "(OTA [ota])";                   // /mnt/us/Update_%lld_%lld.bin
    else if(!strncmp(magic_number, "FD", MAGIC_NUMBER_LENGTH/2))
        return "(Versionless [vls])";           // /mnt/us/Update_VLS_%lld.bin
    else if(!strncmp(magic_number, "FL", MAGIC_NUMBER_LENGTH/2))
        return "(Language [lang])";             // /mnt/us/Update_LANG_%s.bin
    else if(!strncmp(magic_number, "SP", MAGIC_NUMBER_LENGTH/2))
        return "(Signing Envelope)";
    else if(!memcmp(magic_number, "\x1F\x8B\x08\x00", MAGIC_NUMBER_LENGTH))
        return "(Userdata tarball)";
    else
        return "Unknown";
}

int md5_sum(FILE *input, char output_string[BASE16_ENCODE_LENGTH(MD5_DIGEST_SIZE)])
{
    unsigned char bytes[BUFFER_SIZE];
    size_t bytes_read;
    struct md5_ctx md5;
    uint8_t digest[MD5_DIGEST_SIZE];

    md5_init(&md5);
    while((bytes_read = fread(bytes, sizeof(unsigned char), BUFFER_SIZE, input)) > 0)
    {
        md5_update(&md5, bytes_read, bytes);
    }
    if(ferror(input) != 0)
    {
        fprintf(stderr, "Error reading input file: %s.\n", strerror(errno));
        return -1;
    }
    md5_digest(&md5, MD5_DIGEST_SIZE, digest);
    // And build the hex checksum the nettle way ;)
    base16_encode_update((uint8_t *)output_string, MD5_DIGEST_SIZE, digest);

    return 0;
}

// Pilfered from http://rosettacode.org/wiki/Non-decimal_radices/Convert#C
char *to_base(int64_t num, unsigned int base)
{
    char *tbl = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char buf[66] = {'\0'};
    char *out;
    uint64_t n;
    unsigned int i, len = 0, neg = 0;
    if(base > 36)
    {
        fprintf(stderr, "base %d too large\n", base);
        return 0;
    }

    // safe against most negative integer
    n = ((neg = num < 0)) ? (uint64_t) (~num) + 1 : (uint64_t) num;

    do { buf[len++] = tbl[n % base]; } while(n /= base);

    out = malloc(len + neg + 1);
    memset(out, 0, len + neg + 1);
    for(i = neg; len > 0; i++)
        out[i] = buf[--len];
    if(neg)
        out[0] = '-';

    return out;
}

struct rsa_private_key get_default_key(void)
{
    // Make nettle happy... (Array created from the bin2h output of pkcs1-conv on our pem file)
    static char sign_key_sexp[] =
        "\x28\x31\x31\x3A\x70\x72\x69\x76\x61\x74\x65\x2D\x6B\x65\x79\x28\x39\x3A\x72\x73"
        "\x61\x2D\x70\x6B\x63\x73\x31\x28\x31\x3A\x6E\x31\x32\x39\x3A\x00\xC9\x9F\x58\xD6"
        "\x53\xEC\x71\x56\xFF\xDE\x44\xA7\xC2\x3D\x1F\x5E\xE3\xB9\x4F\x58\xDD\xAB\x1F\x7D"
        "\xF3\xF5\x06\xDF\x9E\xA9\x82\xC4\x14\x4B\x3F\xA9\x8C\x8C\x6C\xBA\x00\xFC\xB2\x71"
        "\x05\xE0\xDE\x73\xE2\xE5\xF7\x1B\xEF\x96\xA5\x66\x8F\x8E\x87\x4D\x76\x1E\x93\x1E"
        "\xF4\xB9\xE9\x78\x48\x25\xA0\x87\x66\xD4\x4E\x0B\x3A\xCC\xAB\xCF\x89\x2D\xB5\x0B"
        "\x46\x46\x5C\xC2\x12\xB9\x81\x1A\xDE\xBE\x70\x05\x44\x57\xCE\xB2\xDA\x98\x4E\x27"
        "\x79\x8B\x93\x41\x24\xF5\x44\x17\x6C\x85\x1F\xAE\xFC\x89\x9D\x2D\x8C\x28\xB1\xB6"
        "\x71\xCC\xE3\x95\x29\x28\x31\x3A\x65\x33\x3A\x01\x00\x01\x29\x28\x31\x3A\x64\x31"
        "\x32\x38\x3A\x48\xBC\xA6\xD4\xF3\x83\xDA\x43\xB3\x9D\x21\x11\x90\x5E\x72\xA1\xCD"
        "\xEF\xBD\x73\x66\xCC\xE4\x58\x91\x19\x35\x78\x99\x09\xB8\x36\x3A\xC8\x06\xD8\x88"
        "\xEE\xE4\x0E\x9A\x6A\x8F\x89\x7C\xC0\x6A\x20\x4E\x9B\xFD\xF0\xE3\x17\x6A\xE6\x3C"
        "\x26\x04\x23\xEA\xD8\x0E\xE4\xB9\x18\xDA\xEA\x6D\xB6\xE9\x03\xAF\xCB\xA1\x13\x6C"
        "\xFD\x0E\x1E\xC7\x31\x95\x7F\xAC\x36\x1A\xFB\xDA\xF2\x6C\x9B\xAC\x46\x20\x10\x0E"
        "\x61\x7E\x54\x2C\xD8\xD8\x78\xAB\x8E\x9B\x12\xCE\x04\x6E\xD2\xBF\x36\x34\x2F\x33"
        "\x9C\xD9\xB6\x78\x63\x91\xCA\xCF\x41\xBE\x61\x29\x28\x31\x3A\x70\x36\x35\x3A\x00"
        "\xE8\x22\x89\x0E\xAF\x47\xD8\xCF\x75\x13\x49\xB1\xDF\x0F\x77\xA7\x81\x71\x4F\x67"
        "\xE2\x5A\x26\xA5\x3C\xC5\xAC\x91\xEC\x2F\x86\xA7\x92\x34\x0A\x04\xA7\x08\x34\xD0"
        "\x56\x07\x64\x54\x66\xCF\xB8\xB5\x58\x89\x60\xC8\x70\x46\xB1\x8E\xF5\x6B\x85\x76"
        "\x2D\xD8\x07\x3D\x29\x28\x31\x3A\x71\x36\x35\x3A\x00\xDE\x59\xC4\x46\x08\x34\x46"
        "\x65\x81\x0B\x72\xBC\xB6\x80\xB2\x7C\x3B\xEB\xF1\xE5\xDA\xA3\xEC\x60\x50\x9D\xE5"
        "\x35\x66\xEA\x4B\x41\xED\xC3\x17\x33\xC2\x72\x04\x1F\x8F\x48\x20\x3A\x23\x6D\x39"
        "\xCB\x52\xBD\xCE\x8A\xD1\x4C\x66\xE6\x89\xB9\x3D\x8C\xB5\x6C\xD3\x39\x29\x28\x31"
        "\x3A\x61\x36\x35\x3A\x00\xAE\x86\x08\x75\x39\xE2\xD2\x66\x66\xA6\xF1\xA9\x01\x03"
        "\x27\xFA\x8F\x9F\x19\x0C\x09\x69\xAD\xD4\x5D\x34\x60\xE1\xF4\xA8\x66\x9C\x65\x97"
        "\x2A\x51\x05\x23\x6E\x51\x93\xDC\x4A\xDA\x09\xD1\xF2\x14\xA5\x53\xE3\xA7\xCE\x81"
        "\xD7\xCC\x9B\x47\x13\x38\x1E\x8F\x64\x21\x29\x28\x31\x3A\x62\x36\x35\x3A\x00\xC8"
        "\xB3\x96\x6A\xF0\x74\xDF\x26\x38\x39\x31\x34\x0E\x38\x54\xE3\xB6\xE2\xDE\xD2\x6F"
        "\x6C\x8F\xAC\xD0\x97\xF5\x91\x22\x78\x51\xBE\x0C\xF3\x90\x39\xF4\x46\x1E\x5A\xAE"
        "\x66\x98\x50\x62\x31\xF1\x7D\x0A\x0E\xB2\x24\xB3\x8F\x97\x42\x79\x06\x6F\xFC\x56"
        "\xB7\x08\x61\x29\x28\x31\x3A\x63\x36\x35\x3A\x00\xDC\x57\x67\xAE\xC1\x62\x08\xD3"
        "\x49\x86\xF8\xAD\xD9\xA4\xE6\xB4\xBC\xD7\xC5\x4E\x3A\x2B\xEB\x15\xE8\xD2\x18\xD6"
        "\xD1\x09\x1B\xE4\x45\xCC\xB4\x70\x3B\x82\x05\x0D\x8E\x1A\xFD\xDA\x28\x87\x56\x21"
        "\xD6\x21\x45\x1A\x37\x26\xA6\xAC\xDA\xEA\xD4\x6E\xB5\xAC\x3C\xCC\x29\x29\x29";

    struct rsa_private_key rsa_pkey;
    rsa_private_key_init(&rsa_pkey);

    if(!rsa_keypair_from_sexp(NULL, &rsa_pkey, 0, sizeof(sign_key_sexp), (uint8_t *) sign_key_sexp))
    {
        fprintf(stderr, "Invalid default private key!\n");
        // In the unlikely event this ever happens, it'll be caught later on in sign_file ;).
    }

    return rsa_pkey;
}

int kindle_print_help(const char *prog_name)
{
    printf(
        "usage:\n"
        "  %s md [ <input> ] [ <output> ]\n"
        "    Obfuscates data using Amazon's update algorithm.\n"
        "    If no input is provided, input from stdin\n"
        "    If no output is provided, output to stdout\n"
        "    \n"
        "  %s dm [ <input> ] [ <output> ]\n"
        "    Deobfuscates data using Amazon's update algorithm.\n"
        "    If no input is provided, input from stdin\n"
        "    If no output is provided, output to stdout\n"
        "    \n"
        "  %s convert [options] <input>...\n"
        "    Converts a Kindle update package to a gzipped tar archive file, and delete input.\n"
        "    \n"
        "    Options:\n"
        "      -c, --stdout                Write to standard output, keeping original files unchanged.\n"
        "      -i, --info                  Just print the package information, no conversion done.\n"
        "      -s, --sig                   OTA V2 updates only. Extract the payload signature.\n"
        "      -k, --keep                  Don't delete the input package.\n"
        "      -u, --unsigned              Assume input is an unsigned & mangled userdata package.\n"
        "      -w, --unwrap                Just unwrap the package, if it's wrapped in an UpdateSignature header (especially useful for userdata packages).\n"
        "      \n"
        "  %s extract [options] <input> <output>\n"
        "    Extracts a Kindle update package to a directory.\n"
        "    \n"
        "    Options:\n"
        "      -u, --unsigned              Assume input is an unsigned & mangled userdata package.\n"
        "      \n"
        "  %s create <type> <devices> [options] <dir|file>... [ <output> ]\n"
        "    Creates a Kindle update package.\n"
        "    You should be able to throw a mix of files & directories as input without trouble.\n"
        "    Just keep in mind that by default, if you feed it absolute paths, it will archive absolute paths, which usually isn't what you want!\n"
        "    If input is a single gzipped tarball (\".tgz\" or \".tar.gz\") file, we assume it is properly packaged (bundlefile & sigfile), and will only convert it to an update.\n"
        "    Output should be a file with the extension \".bin\", if it is not provided, or if it's a single dash, outputs to standard output.\n"
        "    In case of OTA updates, all files with the extension \".ffs\" or \".sh\" will be treated as update scripts.\n"
        "    \n"
        "    Type:\n"
        "      ota                         OTA V1 update package. Works on Kindle 3 and older.\n"
        "      ota2                        OTA V2 signed update package. Works on Kindle 4 and newer.\n"
        "      recovery                    Recovery package for restoring partitions.\n"
        "      recovery2                   Recovery V2 package for restoring partitions. Works on Kindle 5 (PaperWhite) and newer.\n"
        "      sig                         Signature envelope. Use this to build a signed userdata package with the -U switch (FW >= 5.1 only, but device agnostic).\n"
        "    \n"
        "    Devices:\n"
        "      OTA V1 & Recovery packages only support one device. OTA V2 & Recovery V2 packages can support multiple devices.\n"
        "      \n"
        "      -d, --device k1             Kindle 1\n"
        "      -d, --device k2             Kindle 2 US\n"
        "      -d, --device k2i            Kindle 2 International\n"
        "      -d, --device dx             Kindle DX US\n"
        "      -d, --device dxi            Kindle DX International\n"
        "      -d, --device dxg            Kindle DX Graphite\n"
        "      -d, --device k3w            Kindle 3 Wifi\n"
        "      -d, --device k3g            Kindle 3 Wifi+3G\n"
        "      -d, --device k3gb           Kindle 3 Wifi+3G Europe\n"
        "      -d, --device k4             Kindle 4 (No Touch) Silver (2011)\n"
        "      -d, --device k4b            Kindle 4 (No Touch) Black (2012)\n"
        "      -d, --device k5w            Kindle 5 (Kindle Touch) Wifi\n"
        "      -d, --device k5g            Kindle 5 (Kindle Touch) Wifi+3G\n"
        "      -d, --device k5gb           Kindle 5 (Kindle Touch) Wifi+3G Europe\n"
        "      -d, --device k5u            Kindle 5 (Kindle Touch) Unknown Variant (4th device code found in Touch official updates).\n"
        "      -d, --device pw             Kindle PaperWhite Wifi\n"
        "      -d, --device pwg            Kindle PaperWhite Wifi+3G\n"
        "      -d, --device pwgc           Kindle PaperWhite Wifi+3G Canada\n"
        "      -d, --device pwgb           Kindle PaperWhite Wifi+3G Europe\n"
        "      -d, --device pwgj           Kindle PaperWhite Wifi+3G Japan\n"
        "      -d, --device pwgbr          Kindle PaperWhite Wifi+3G Brazil\n"
        "      -d, --device pw2            Kindle PaperWhite 2 (2013) Wifi\n"
        "      -d, --device pw2j           Kindle PaperWhite 2 (2013) Wifi Japan\n"
        "      -d, --device pw2g           Kindle PaperWhite 2 (2013) Wifi+3G\n"
        "      -d, --device pw2gc          Kindle PaperWhite 2 (2013) Wifi+3G Canada\n"
        "      -d, --device pw2gb          Kindle PaperWhite 2 (2013) Wifi+3G Europe\n"
        "      -d, --device pw2gr          Kindle PaperWhite 2 (2013) Wifi+3G Russia\n"
        "      -d, --device pw2gj          Kindle PaperWhite 2 (2013) Wifi+3G Japan\n"
        "      -d, --device pw2il          Kindle PaperWhite 2 (2013) Wifi (4GB) International\n"
        "      -d, --device pw2gbl         Kindle PaperWhite 2 (2013) Wifi+3G (4GB) Europe\n"
        "      -d, --device pw2gl          Kindle PaperWhite 2 (2013) Wifi+3G (4GB)\n"
        "      -d, --device pw2gcl         Kindle PaperWhite 2 (2013) Wifi+3G (4GB) Canada\n"
        "      -d, --device kt2            Kindle Basic (2014)\n"
        "      -d, --device kv             Kindle Voyage WiFi\n"
        "      -d, --device kvg            Kindle Voyage WiFi+3G\n"
        "      -d, --device kvgb           Kindle Voyage WiFi+3G Europe\n"
        "      -d, --device pw3            Kindle PaperWhite 3 (2015) WiFi\n"
        "      -d, --device kindle2        Alias for k2 & k2i\n"
        "      -d, --device kindledx       Alias for dx, dxi & dxg\n"
        "      -d, --device kindle3        Alias for k3w, k3g & k3gb\n"
        "      -d, --device legacy         Alias for kindle2, kindledx & kindle3\n"
        "      -d, --device kindle4        Alias for k4 & k4b\n"
        "      -d, --device touch          Alias for k5w, k5g & k5gb\n"
        "      -d, --device paperwhite     Alias for pw, pwg, pwgc, pwgb, pwgj & pwgbr\n"
        "      -d, --device paperwhite2    Alias for pw2, pw2j, pw2g, pw2gc, pw2gb, pw2gr, pw2gj, pw2il, pw2gbl, pw2gl & pw2gcl\n"
        "      -d, --device basic          Alias for kt2\n"
        "      -d, --device voyage         Alias for kv, kvg & kvgb\n"
        "      -d, --device paperwhite3    Alias for pw3\n"
        "      -d, --device kindle5        Alias for touch, paperwhite, paperwhite2, basic, voyage & paperwhite3\n"
        "      -d, --device none           No specific device (Recovery V2 & Recovery FB02 with header rev 2 only, default).\n"
        "      -d, --device auto           The current device (Obviously, has to be run from a Kindle).\n"
        "      \n"
        "    Platforms:\n"
        "      Recovery V2 & recovery FB02 with header rev 2 updates only. Use a single platform per package.\n"
        "      \n"
        "      -p, --platform unspecified  Don't target a specific platform.\n"
        "      -p, --platform mario        Mario (mostly devices shipped on FW 1.x?) [Deprecated].\n"
        "      -p, --platform luigi        Luigi (mostly devices shipped on FW 2.x?).\n"
        "      -p, --platform banjo        Banjo (devices shipped on FW 3.x?).\n"
        "      -p, --platform yoshi        Yoshi (mostly devices shipped on FW <= 5.1).\n"
        "      -p, --platform yoshime-p    Yoshime (Prototype).\n"
        "      -p, --platform yoshime      Yoshime (Also known as Yoshime3, mostly devices shipped on FW >= 5.2).\n"
        "      -p, --platform wario        Wario (mostly devices shipped on FW >= 5.4).\n"
        "      \n"
        "    Boards:\n"
        "      Recovery V2 & Recovery FB02 with header rev 2 updates only. Use a single board per package.\n"
        "      \n"
        "      -B, --board unspecified     Don't target a specific board, skip the device check.\n"
        "      -B, --board tequila         Tequila (Kindle 4)\n"
        "      -B, --board whitney         Whitney (Kindle Touch)\n"
        "      \n"
        "    Options:\n"
        "      All the following options are optional and advanced.\n"
        "      -k, --key <file>            PEM file containing RSA private key to sign update. Default is popular jailbreak key.\n"
        "      -b, --bundle <type>         Manually specify package magic number. May override the value dictated by \"type\", if it makes sense. Valid bundle versions:\n"
        "                                    FB01, FB02 = recovery; FB03 = recovery2; FC02, FD03 = ota; FC04, FD04, FL01 = ota2; SP01 = sig\n"
        "      -s, --srcrev <ulong|uint>   OTA updates only. Source revision. OTA V1 uses uint, OTA V2 uses ulong.\n"
        "                                    Lowest version of device that package supports. Default is 0.\n"
        "      -t, --tgtrev <ulong|uint>   OTA & Recovery V2 updates only. Target revision. OTA V1 uses uint, OTA V2 & Recovery V2 uses ulong.\n"
        "                                    Highest version of device that package supports. Default is ulong/uint max value.\n"
        "      -h, --hdrrev <uint>         Recovery V2 & Recovery FB02 updates only. Header Revision. Default is 0.\n"
        "      -1, --magic1 <uint>         Recovery updates only. Magic number 1. Default is 0.\n"
        "      -2, --magic2 <uint>         Recovery updates only. Magic number 2. Default is 0.\n"
        "      -m, --minor <uint>          Recovery updates only. Minor number. Default is 0.\n"
        "      -c, --cert <ushort>         OTA V2 updates only. The number of the certificate to use (found in /etc/uks on device). Default is 0.\n"
        "                                    0 = pubdevkey01.pem, 1 = pubprodkey01.pem, 2 = pubprodkey02.pem\n"
        "      -o, --opt <uchar>           OTA V1 updates only. One byte optional data expressed as a number. Default is 0.\n"
        "      -r, --crit <uchar>          OTA V2 updates only. One byte optional data expressed as a number. Default is 0.\n"
        "      -x, --meta <str>            OTA V2 updates only. An optional string to add. Multiple \"--meta\" options supported.\n"
        "                                    Format of metastring must be: key=value\n"
        "      -a, --archive               Keep the intermediate archive.\n"
        "      -u, --unsigned              Build an unsigned & mangled userdata package.\n"
        "      -U, --userdata              Build an userdata package (can only be used with the sig update type).\n"
        "      -C, --legacy                Emulate the behaviour of yifanlu's KindleTool regarding directories. By default, we behave like tar:\n"
        "                                    every path passed on the commandline is stored as-is in the archive. This switch changes that, and store paths\n"
        "                                    relative to the path passed on the commandline, like if we had chdir'ed into it.\n"
        "      \n"
        "  %s info <serialno>\n"
        "    Get the default root password.\n"
        "    Unless you changed your password manually, the first password shown will be the right one.\n"
        "    (The Kindle defaults to DES hashed passwords, which are truncated to 8 characters).\n"
        "    If you're looking for the recovery MMC export password, that's the second one.\n"
        "    \n"
        "  %s version\n"
        "    Show some info about this KindleTool build.\n"
        "    \n"
        "  %s help\n"
        "    Show this help screen.\n"
        "    \n"
        "notices:\n"
        "  1)  If the variable KT_WITH_UNKNOWN_DEVCODES is set in your environment (no matter the value), some device checks will be relaxed with the create command.\n"
        "  \n"
        "  2)  Kindle 4.0+ has a known bug that prevents some updates with meta-strings to run.\n"
        "  3)  Currently, even though OTA V2 supports updates that run on multiple devices, it is not possible to create an update package that will run on both the Kindle 4 (No Touch) and Kindle 5 (Touch/PW).\n"
        , prog_name, prog_name, prog_name, prog_name, prog_name, prog_name, prog_name, prog_name);
    return 0;
}

int kindle_print_version(const char *prog_name)
{
    printf("%s (KindleTool) %s built by %s with ", prog_name, KT_VERSION, KT_USERATHOST);
#ifdef __clang__
    printf("Clang %s ", __clang_version__);
#else
    printf("GCC %s ", __VERSION__);
#endif
    printf("on %s @ %s against %s ", __DATE__, __TIME__, ARCHIVE_VERSION_STRING);
    printf("& nettle %s\n", NETTLE_VERSION);            // NOTE: This is completely custom, I couldn't find a way to get this info at buildtime in a saner way...
    return 0;
}

int kindle_obfuscate_main(int argc, char *argv[])
{
    FILE *input;
    FILE *output;
    input = stdin;
    output = stdout;
    // Skip command
    argv++;
    argc--;
    if(argc > 1)
    {
        if((output = fopen(argv[1], "wb")) == NULL)
        {
            fprintf(stderr, "Cannot open output for writing: %s.\n", strerror(errno));
            return -1;
        }
    }
    if(argc > 0)
    {
        if((input = fopen(argv[0], "rb")) == NULL)
        {
            fprintf(stderr, "Cannot open input for reading: %s.\n", strerror(errno));
            fclose(output);
            return -1;
        }
    }
    if(munger(input, output, 0, 0) < 0)
    {
        fprintf(stderr, "Cannot obfuscate.\n");
        fclose(input);
        fclose(output);
        return -1;
    }
    fclose(input);
    fclose(output);
    return 0;
}

int kindle_deobfuscate_main(int argc, char *argv[])
{
    FILE *input;
    FILE *output;
    input = stdin;
    output = stdout;
    // Skip command
    argv++;
    argc--;
    if(argc > 1)
    {
        if((output = fopen(argv[1], "wb")) == NULL)
        {
            fprintf(stderr, "Cannot open output for writing: %s.\n", strerror(errno));
            return -1;
        }
    }
    if(argc > 0)
    {
        if((input = fopen(argv[0], "rb")) == NULL)
        {
            fprintf(stderr, "Cannot open input for reading: %s.\n", strerror(errno));
            fclose(output);
            return -1;
        }
    }
    if(demunger(input, output, 0, 0) < 0)
    {
        fprintf(stderr, "Cannot deobfuscate.\n");
        fclose(input);
        fclose(output);
        return -1;
    }
    fclose(input);
    fclose(output);
    return 0;
}

int kindle_info_main(int argc, char *argv[])
{
    char *serial_no;
    char md5[MD5_HASH_LENGTH];
    char device_code[4] = {'\0'};
    Device device;
    FILE *temp;
    unsigned int i;
    // Skip command
    argv++;
    argc--;
    if(argc < 1)
    {
        fprintf(stderr, "Missing argument. You must pass a serial number.\n");
        return -1;
    }
    serial_no = argv[0];
    temp = tmpfile();
    if(strlen(serial_no) != SERIAL_NO_LENGTH)
    {
        fprintf(stderr, "Serial number must be 16 digits long (no spaces). Example: %s\n", "B0NNXXXXXXXXXXXX");
        return -1;
    }
    for(i = 0; i < SERIAL_NO_LENGTH; i++)
    {
        if(islower((int)serial_no[i]))
        {
            serial_no[i] = (char)toupper((int)serial_no[i]);
        }
    }
    // Find root password
    if(fprintf(temp, "%s\n", serial_no) < SERIAL_NO_LENGTH)
    {
        fprintf(stderr, "Cannot write serial to temporary file: %s.\n", strerror(errno));
        fclose(temp);
        return -1;
    }
    rewind(temp);
    if(md5_sum(temp, md5) < 0)
    {
        fprintf(stderr, "Cannot calculate MD5 of serial number.\n");
        fclose(temp);
        return -1;
    }

    snprintf(device_code, 3, "%.*s", 2, &serial_no[2]);
    device = (Device)strtoul(device_code, NULL, 16);
    // Handle the new device ID position, since the PW3
    if(strcmp(convert_device_id(device), "Unknown") == 0)
    {
        snprintf(device_code, 4, "%.*s", 3, &serial_no[3]);
        device = (Device)strtoul(device_code, NULL, 32);
        if(strcmp(convert_device_id(device), "Unknown") == 0)
        {
            fprintf(stderr, "Unknown device!\n");
            fclose(temp);
            return -1;
        }
        else
        {
            fprintf(stderr, "Device uses the new device ID scheme\n");
        }
    }
    // Handle the Wario (>= PW2) passwords while we're at it... Thanks to npoland for this one ;).
    // NOTE: Remember to check if this is still sane w/ kindle_model_sort.py when new stuff comes out!
    if(device == KindleVoyageWifi || device == KindlePaperWhite2Wifi4GBInternational || device >= KindleVoyageUnknown_0x2A)
    {
        fprintf(stderr, "Platform is Wario or newer\n");
        fprintf(stderr, "Root PW            %s%.*s\nRecovery PW        %s%.*s\n", "fiona", 3, &md5[13], "fiona", 4, &md5[13]);
    }
    else
    {
        fprintf(stderr, "Platform is pre Wario\n");
        fprintf(stderr, "Root PW            %s%.*s\nRecovery PW        %s%.*s\n", "fiona", 3, &md5[7], "fiona", 4, &md5[7]);
    }
    // Default root passwords are DES hashed, so we only care about the first 8 chars. On the other hand,
    // the recovery MMC export option expects a 9 chars password, so, provide both...
    fclose(temp);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *prog_name;
    const char *cmd;

    // Do we want to use unknown devcodes? Very lame test, only check if the var actually exists, we don't check the value...
    if(getenv("KT_WITH_UNKNOWN_DEVCODES") == NULL)
        kt_with_unknown_devcodes = 0;
    else
        kt_with_unknown_devcodes = 1;

    prog_name = argv[0];
    // Discard program name for easier parsing
    argv++;
    argc--;

    if(argc > 0)
    {
        if(strncmp(argv[0], "--", 2) == 0)
        {
            // Allow our commands to be passed in longform
            argv[0] += 2;
        }
    }
    else
    {
        // No command was given, print help and die
        fprintf(stderr, "No command was specified!\n\n");
        kindle_print_help(prog_name);
        exit(1);
    }
    cmd = argv[0];

#if defined(_WIN32) && !defined(__CYGWIN__)
    // Set binary mode properly on MingW, MSVCRT craps out when freopen'ing NULL ;)
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#else
    if(freopen(NULL, "rb", stdin) == NULL)
    {
        fprintf(stderr, "Cannot set stdin to binary mode: %s.\n", strerror(errno));
        return -1;
    }
    if(freopen(NULL, "wb", stdout) == NULL)
    {
        fprintf(stderr, "Cannot set stdout to binary mode: %s.\n", strerror(errno));
        return -1;
    }
#endif

    if(strncmp(cmd, "md", 2) == 0)
        return kindle_obfuscate_main(argc, argv);
    else if(strncmp(cmd, "dm", 2) == 0)
        return kindle_deobfuscate_main(argc, argv);
    else if(strncmp(cmd, "convert", 7) == 0)
        return kindle_convert_main(argc, argv);
    else if(strncmp(cmd, "extract", 7) == 0)
        return kindle_extract_main(argc, argv);
    else if(strncmp(cmd, "create", 6) == 0)
        return kindle_create_main(argc, argv);
    else if(strncmp(cmd, "info", 4) == 0)
        return kindle_info_main(argc, argv);
    else if(strncmp(cmd, "version", 7) == 0)
        return kindle_print_version(prog_name);
    else if(strncmp(cmd, "help", 4) == 0 || strncmp(cmd, "-help", 5) == 0 || strncmp(cmd, "-h", 2) == 0 || strncmp(cmd, "-?", 2) == 0 || strncmp(cmd, "/?", 2) == 0 || strncmp(cmd, "/h", 2) == 0 || strncmp(cmd, "/help", 2) == 0)
        return kindle_print_help(prog_name);
    else
    {
        fprintf(stderr, "Unknown command '%s'!\n\n", cmd);
        kindle_print_help(prog_name);
        exit(1);
    }

    return 1;
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs on;
