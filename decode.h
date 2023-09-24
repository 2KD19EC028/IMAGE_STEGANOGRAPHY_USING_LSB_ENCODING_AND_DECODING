#ifndef DECODE_H
#define DECODE_H
#include "types.h" // Contains user defined types
#include<string.h>
/* 
 * Structure to store information required for
 * decoding secret file data to output file
 * Info about output and intermediate data is
 * also stored
 */
typedef struct DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* output file Info */
    char *output_fname;
    FILE *fptr_output;
    char arr[10];
    int extn_size;
    int secret_file_size;

} DecodeInfo;

/* Check operation type */
OperationType check_operation_type_d(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args_d(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding_d(DecodeInfo *decInfo);

/* Get File pointers for i/p files */
Status open_files_d(DecodeInfo *decInfo);

/* decode Magic String */
Status decode_magic_string_d(char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size_d(DecodeInfo *decInfo);
/* Decode secret file extenstion */
Status decode_secret_file_extn_d(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size_d(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data_d(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_to_image_d(int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_to_lsb_d(char *data);
/* Decode a size from LSB of image data array*/
Status decode_size_to_lsb_d(char *image_buffer);
/*Get file pointer for o/p files*/
Status open_files_output_d(DecodeInfo *decInfo);
#endif
