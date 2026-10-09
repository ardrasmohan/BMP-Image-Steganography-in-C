#include <stdio.h>
#include <string.h>

#include "decode.h"
#include "types.h"


/* Open files for decoding */
Status open_decode_files(DecodeInfo *decInfo)
{
    /* Open stego image */
    decInfo->fptr_stego_image =fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->stego_image_fname);
        return e_failure;
    }
    /* Open output file */
    decInfo->fptr_output =fopen(decInfo->output_fname, "wb");
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->output_fname);
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    return e_success;
}
/* Decode one byte from 8 image bytes */
Status decode_byte_from_lsb(char *image_buffer,char *data)
{
    char value = 0;
    for (int i = 0; i < 8; i++)
    {
        value = (value << 1) |(image_buffer[i] & 1);
    }
    *data = value;
    return e_success;
}
/* Decode magic string */
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo)
{
    char buff[8];
    char decoded_char;
    for (int i = 0;i < strlen(magic_string);i++)
    {
        if (fread(buff, sizeof(char), 8,decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
        if (decode_byte_from_lsb(buff, &decoded_char)== e_failure)
        {
            return e_failure;
        }
        if (decoded_char != magic_string[i])
        {
            return e_failure;
        }
    }
    return e_success;
}
/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buff[8];
    char decoded_char;
    int i = 0;
    while (1)
    {
        if (fread(buff, sizeof(char), 8,decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
        if (decode_byte_from_lsb(buff, &decoded_char)== e_failure)
        {
            return e_failure;
        }
        /* End of extension */
        if (decoded_char == '\0')
        {
            break;
        }
        if (i >= sizeof(decInfo->extn_secret_file) - 1)
        {
            return e_failure;
        }
        decInfo->extn_secret_file[i] =decoded_char;
        i++;
    }
    decInfo->extn_secret_file[i] = '\0';
    return e_success;
}
/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buff[32];
    if (fread(buff, sizeof(char), 32,decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }
    long size = 0;
    for (int i = 0; i < 32; i++)
    {
        size = (size << 1) |(buff[i] & 1);
    }
    decInfo->size_secret_file = size;
    return e_success;
}
/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buff[8];
    char decoded_char;
    for (long i = 0;i < decInfo->size_secret_file;i++)
    {
        if (fread(buff, sizeof(char), 8,decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
        if (decode_byte_from_lsb(buff, &decoded_char)== e_failure)
        {
            return e_failure;
        }
        fwrite(&decoded_char,sizeof(char),1,decInfo->fptr_output);
    }
    return e_success;
}
/* Perform complete decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    /* Open files */
    if (open_decode_files(decInfo)== e_failure)
    {
        return e_failure;
    }
    if (fseek(decInfo->fptr_stego_image,54,SEEK_SET) != 0)
    {
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }
    /* Decode magic string */
    char magic_string[] = "*#";
    if (decode_magic_string(magic_string,decInfo) == e_failure)
    {
        printf("ERROR: Magic string does not match\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }
    printf("Magic string decoded successfully\n");
    /* Decode secret file extension */
    if (decode_secret_file_extn(decInfo)== e_failure)
    {
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }
    printf("Secret file extension: %s\n",decInfo->extn_secret_file);
    /* Decode secret file size */
    if (decode_secret_file_size(decInfo)== e_failure)
    {
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }
    printf("Secret file size: %ld bytes\n",decInfo->size_secret_file);
    /* Decode secret file data */
    if (decode_secret_file_data(decInfo)== e_failure)
    {
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }
    /* Close files */
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);
    return e_success;
}