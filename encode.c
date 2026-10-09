#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "types.h"


uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);/*width is in 18th place*/
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image);/*height is in 22nd place since int 18+4*/
    printf("height = %u\n", height);
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)/*opening 3 files source file,secret file and stego(destination)file*/
{
    encInfo->fptr_src_image =fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        fprintf(stderr,"ERROR: Unable to open file %s\n",encInfo->src_image_fname);
        return e_failure;
    }
    encInfo->fptr_secret =fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        fprintf(stderr,"ERROR: Unable to open file %s\n",encInfo->secret_fname);
        fclose(encInfo->fptr_src_image);
        return e_failure;
    }
    encInfo->fptr_stego_image =fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        fprintf(stderr,"ERROR: Unable to open file %s\n",encInfo->stego_image_fname);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        return e_failure;
    }
    return e_success;
}
/*checking capacity whether it is capable of storing whole secret file or not*/
Status check_capacity(EncodeInfo *encInfo)
{
    uint image_capacity;
    uint required_capacity;

    image_capacity =get_image_size_for_bmp(encInfo->fptr_src_image);
    required_capacity =(2 +strlen(encInfo->extn_secret_file)+1+4+encInfo->size_secret_file)*8;
    printf("Image capacity = %u bytes\n",image_capacity);
    printf("Required capacity = %u bytes\n",required_capacity);
    if (image_capacity >= required_capacity)
    {
        return e_success;
    }
    else
    {
        printf("ERROR: Image does not have enough capacity\n");
        return e_failure;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        return e_failure;
    }
    /*checking whether it contain . here since storing in pointer and that too checking
    with strrchr it will be returning the address of the . and thus takes the substring
    .txt into dot*/
    char *dot = strrchr(encInfo->secret_fname, '.');
    if (dot != NULL)/*contains .txt then !=NULL*/
    {
        if (strlen(dot) >= MAX_FILE_SUFFIX)/*it will obviusly be 5((strlen).txt)<20 thus false*/
        {
            printf("ERROR: File extension is too long\n");
            fclose(encInfo->fptr_src_image);
            fclose(encInfo->fptr_secret);
            fclose(encInfo->fptr_stego_image);
            return e_failure;
        }
        strcpy(encInfo->extn_secret_file, dot);
    }
    else
    {
        encInfo->extn_secret_file[0] = '\0';
    }
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file =ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    if (check_capacity(encInfo) == e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    char header[54];
    if (fread(header,sizeof(char),54,encInfo->fptr_src_image) != 54)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    fwrite(header,sizeof(char),54,encInfo->fptr_stego_image);
    char magic_string[] = "*#";
    if (encode_magic_string(magic_string, encInfo)== e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    if (encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    if (encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    if (encode_secret_file_data(encInfo)== e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    if (copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_failure)
    {
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);
        return e_failure;
    }
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}

Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    char buff[8];
    for (int i = 0;i < strlen(magic_string);i++)
    {
        if (fread(buff,sizeof(char),8,encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }
        if (encode_byte_to_lsb(magic_string[i],buff) == e_failure)
        {
            return e_failure;
        }
        fwrite(buff,sizeof(char),8,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
    char buff[8];
    for (int i = 0;i <= strlen(file_extn);i++)
    {
        if (fread(buff,sizeof(char),8,encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }
        if (encode_byte_to_lsb(file_extn[i],buff) == e_failure)
        {
            return e_failure;
        }
        fwrite(buff,sizeof(char),8,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    char buff[32];
    if (fread(buff,sizeof(char),32,encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }
    if (encode_size_to_lsb((int)file_size,buff) == e_failure)
    {
        return e_failure;
    }
    fwrite(buff,sizeof(char),32,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buff[8];
    while (fread(&ch,sizeof(char),1,encInfo->fptr_secret) == 1)
    {
        if (fread(buff,sizeof(char),8,encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }
        if (encode_byte_to_lsb(ch,buff) == e_failure)
        {
            return e_failure;
        }
        fwrite(buff,sizeof(char),8,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char buff[8];
    for (int i = 0; i < size; i++)
    {
        if (fread(buff,sizeof(char),8,fptr_src_image) != 8)
        {
            return e_failure;
        }
        if (encode_byte_to_lsb(data[i],buff) == e_failure)
        {
            return e_failure;
        }
        fwrite(buff,sizeof(char),8,fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] =(image_buffer[i] & 0xFE)|((data >> (7 - i)) & 1);
        /*here img buffer & 0xFE means it is eliminating the lsb bit and in data>>(31-i)&1 means
        data >> (7 - 1)
        data >> 6
        01000001
                ↓ shift right 6
        00000001

        00000001
        &00000001
        ---------
        00000001
        */
    }
    return e_success;
}

Status encode_size_to_lsb(int data,char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] =(image_buffer[i] & 0xFE)|((data >> (31 - i)) & 1);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;
    while (fread(&ch,sizeof(char),1,fptr_src) == 1)
    {
        if (fwrite(&ch,sizeof(char),1,fptr_dest) != 1)
        {
            return e_failure;
        }
    }
    return e_success;
}