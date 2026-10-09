/*NAME: ARDRA S MOHAN
BATCH: ECEP 26015D
PROJECT NO 5 : STEGNOGRAPHY*/



#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "decode.h"
#include "types.h"


OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}


int main(int argc, char *argv[])
{
    OperationType op;

    if (argc < 2)
    {
        printf("Usage:\n");
        printf("Encoding : %s -e\n", argv[0]);
        printf("Decoding : %s -d\n", argv[0]);
        return 1;
    }
    op = check_operation_type(argv);
    /* ENCODING */
    if (op == e_encode)
    {
        EncodeInfo encInfo = {0};
        encInfo.src_image_fname = "beautiful.bmp";
        encInfo.secret_fname = "secret.txt";
        encInfo.stego_image_fname = "stego_img.bmp";
        Status ret = do_encoding(&encInfo);
        if (ret == e_success)
        {
            printf("Encoding successful\n");
        }
        else
        {
            printf("Encoding failed\n");
            return 1;
        }
    }
    /* DECODING */
    else if (op == e_decode)
    {
        DecodeInfo decInfo = {0};
        decInfo.stego_image_fname = "stego_img.bmp";
        decInfo.output_fname = "decoded.txt";
        Status ret = do_decoding(&decInfo);
        if (ret == e_success)
        {
            printf("Decoding successful\n");
            printf("Decoded file: decoded.txt\n");
        }
        else
        {
            printf("Decoding failed\n");
            return 1;
        }
    }
    /* INVALID OPERATION */
    else
    {
        printf("Unsupported operation\n");
        printf("Use -e for encoding or -d for decoding\n");
        return 1;
    }
    return 0;
}