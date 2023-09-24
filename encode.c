#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/*function for check operation type*/
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
	return e_encode;
    }
    else
    {
	return e_unsupported;
    }
}
/*function to check the read and validate*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*condition to check the argument vector 2 is .bmp or not*/
    if(strcmp((strstr(argv[2], ".")),".bmp") == 0)
    {
	encInfo->src_image_fname = argv[2];
	printf("source file has .bmp extension\n");
	/*condition to check the argument vector 3 is .txt or .c*/
	if(strcmp((strstr(argv[3], ".")),".txt") == 0 || strcmp((strstr(argv[3], ".")),".c") == 0  )
	{
	    encInfo->secret_fname = argv[3];
	    printf("Secret file have only .txt or .c extension\n");
	     /*condition to check the argument vector is present or not*/
	    if(argv[4] != NULL)
	    {
		/*if true then check the argument vector 2 is .bmp or not*/
		if(strcmp((strstr(argv[4], ".")), ".bmp") == 0 )
		{
		    encInfo->stego_image_fname = argv[4];
		}
		else
		{
		    return e_failure;
		}
	    }
	    else
	    {
		/*if argument is not present then store the default file name*/
		encInfo -> stego_image_fname = "default.bmp";
	    }

	}
	else
	{
	    printf("Error : secret file doesn't have .txt or .c extension\n");
	    return e_failure;
	}
    }
    else
    {
	printf("Error : source file doesn't have .bmp extension\n");
	return e_failure;
    }
    return e_success;
}
/*function to check the encoding function*/
Status do_encoding(EncodeInfo *encInfo)
{
    /*function call for open the files*/
    if(open_files(encInfo) == e_success)
    {
	printf("open file operation is completed\n");
	/*function call for check capacity of the file*/
	if(check_capacity(encInfo) == e_success)
	{
	    printf("Check capacity is completed\n");
	    /*function call for copy the header file*/
	    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
	    {
		printf("copy bmp header is completed\n");
		/*function call for copy the magic string*/
		if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
		{
		    printf("encoding magic string is completed\n");
		    int a=strlen(strstr(encInfo->secret_fname,"."));
		    /*function call for copy the extension file size*/
		    if(encode_secret_file_extn_size(a,encInfo) == e_success)
		    {
			printf("encode secret file extn size completed\n");
			char str[20];
			/*copying the secret file extention*/
			strcpy(str,(strstr(encInfo->secret_fname,".")));
			if(encode_secret_file_extn(str,encInfo) == e_success)
			{
			    printf("encode secret file extn is completed\n");
			    /*setting file pointer to the eof*/
			    fseek(encInfo->fptr_secret,0,SEEK_END);
			    int b=ftell(encInfo->fptr_secret);
			    if(encode_secret_file_size(b,encInfo) == e_success)
			    {
				printf("encode secret file size is completed\n");
				/*function call for encode the secret file data*/
				if(encode_secret_file_data(encInfo) == e_success)
				{
				    printf("secret file data is encoded\n");
				    /*function call for copy the remaining data*/
				    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
				    {
					printf("remaining data is copied\n");
				    }
				    else
				    {
					printf("Error : Copying remaining data failed\n");
				    }
				}
				else
				{
				    printf("Error : secret file data is not encoded\n");
				}
			    }
			    else
			    {
				printf("Error : encoding secret file size is failed\n");
			    }
			}
			else
			{
			    printf("Error : encoding secret file extn is failed\n");
			}
		    }
		    else
		    {
			printf("Error : encoding secret file extn size is failed\n");
		    }
		}
		else
		{
		    printf("Encoding magic string is failed\n");
		}
	    }
	    else
	    {
		printf("Error : copy bmp header is failed\n");
	    }
	}
	else
	{
	    printf("Error : check capacity is failed\n");
	}
    }
    else
    {
	printf("Error : error opening file\n");
    }
    return e_success;
}
/*function definition of check capacity*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > 54+((strlen(MAGIC_STRING )+4+4+4+encInfo->size_secret_file)*8))
    {
	return e_success;
    }
}
/*function definition for copying bmp header*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];
    rewind(fptr_src_image);
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}
/*function definition for encoding magic string*/
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image);
}
/*function definition for encoding data to image*/
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[8];
    for( int i=0; i<size; i++)
    {
	fread(buffer, 8, 1, fptr_src_image);
	encode_byte_to_lsb(data[i], buffer);
	fwrite(buffer, 8, 1, fptr_dest_image);
    }
    return e_success;
}
/*function definition for storing bytes to lsb*/
Status encode_byte_to_lsb(char data, char * Image_buffer)
{
    for(int i=0; i<8; i++)
    {
	Image_buffer[i] = Image_buffer[i] & 0xfe | (data & (1 << i)) >> i;
    }
    return e_success;
}
/*function definition encoding size to lsb*/
Status encode_size_to_lsb(int data, char * Image_buffer)
{
    for(int i=0; i<32; i++)
    {
	Image_buffer[i] = Image_buffer[i] & 0xfe | (data & (1 << i)) >> i;
    }
}
/*function definition for encoding file extension size*/
Status encode_secret_file_extn_size(long file_extn,EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_extn,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/*function definition for encoding file extension*/
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
/*function definition for encoding secret file size*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/*function definition for encoding secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char arr[encInfo->size_secret_file];
    fread(arr,encInfo->size_secret_file,1,encInfo->fptr_secret);
    encode_data_to_image(arr, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
/*function definition for encoding remaining data */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char c;
    while(fread(&c, 1,1, fptr_src) > 0)
    {
	fwrite(&c,1,1,fptr_dest);
    }
    return e_success;
}
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

