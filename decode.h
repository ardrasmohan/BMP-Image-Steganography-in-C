#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct
{
    /* Stego image */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output secret file */
    char *output_fname;
    FILE *fptr_output;

    /* Secret file information */
    char extn_secret_file[20];
    long size_secret_file;

} DecodeInfo;


/* Function declarations */

Status open_decode_files(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *image_buffer,char *data);

#endif