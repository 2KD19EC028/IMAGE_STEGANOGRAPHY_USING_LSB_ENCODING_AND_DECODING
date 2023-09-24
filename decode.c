#include <stdio.h>
#include "types.h"
#include "common.h"
#include "decode.h"
#include<string.h>
/*function definition to check operation type*/
OperationType check_operation_type_d(char *argv[])
{
    if(strcmp(argv[1], "-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}
/*function definition to read and validate the files passed through command line*/
Status read_and_validate_decode_args_d(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp((strstr(argv[2], ".")),".bmp") == 0)
    {
	decInfo->stego_image_fname = argv[2];
	printf("encode data file has .bmp extension\n");
	if(argv[3] != NULL)
	{
	    decInfo->output_fname = strtok(argv[3],".");
	}
	else
	{
	    decInfo->output_fname ="output";
	}
    }
    else
    {
	printf("Error : encode data file doesn't have .bmp extension\n");
	return e_failure;
    }
    return e_success;
}
/*function definition for decoding process*/
Status do_decoding_d(DecodeInfo *decInfo)
{
    /*function call to open the image file*/
    if(open_files_d(decInfo) == e_success)
    {
	/*setting file pointer at 54th position using fseek*/
	fseek(decInfo->fptr_stego_image,54,SEEK_SET);
	printf("open file operation is completed\n");
	/*function call to decode magic string*/
	if(decode_magic_string_d(MAGIC_STRING,decInfo) == e_success)
	{
	    /*comparing the decoded magic string and user passed magic string*/
	    if(strcmp(decInfo->arr,MAGIC_STRING)==0)
	    {
		printf("decoding magic string is completed\n");
		/*function call to decode the secret file extension size*/
		if(decode_secret_file_extn_size_d(decInfo) == e_success)
		{
		    printf("decoding secret file extn size completed\n");
		    /*function call to decode secret file extension*/
		    if(decode_secret_file_extn_d(decInfo) == e_success)
		    {
			printf("decoding secret file extn is completed\n");
			/*concatinating the output file name*/
			char str1[strlen(decInfo->output_fname)];
			strcpy(str1,decInfo->output_fname);
			char str2[decInfo->extn_size];
			strcpy(str2,decInfo->arr);
			decInfo->output_fname = strcat(str1,str2);
			/*function call to decode the secret file size*/
			if(decode_secret_file_size_d(decInfo) == e_success)
			{
			    printf("decoding secret file size is completed\n");
			    /*function call to open the output file*/
			    if(open_files_output_d(decInfo) == e_success)
			    {
				/*function call to decode the decret file data*/
				if(decode_secret_file_data_d(decInfo) == e_success)
				{
				    printf("secret file data is decoded\n");
				}
				else
				{
				    printf("Error : secret file data is not decoded\n");
				}
			    }
			    else
			    {
				printf("Error : opening output file\n");
			    }
				}
			else
			{
			    printf("Error : decoding secret file size is failed\n");
			}
		    }
		    else
		    {
			printf("Error : decoding secret file extn is failed\n");
		    }
		}
		else
		{
		    printf("Error : decoding secret file extn size is failed\n");
		}
	    }
	    else
	    {
		printf("Decoding magic string is failed\n");
	    }
	}
	else
	{
	    printf("Error : error opening file\n");
	}
    }
    return e_success;
}
/*function definition for decoding magic string*/
Status decode_magic_string_d(char *magic_string, DecodeInfo *decInfo)
{
    char data[8];
    for( int i=0; i<strlen(magic_string); i++)
    {
	fread(data, 8, 1, decInfo->fptr_stego_image);
	decInfo->arr[i]=decode_byte_to_lsb_d(data);
    }
    decInfo->arr[strlen(magic_string)]='\0';
    return e_success;
}
/*fucnction definition for decoding data from image*/
Status decode_data_to_image_d(int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    char data[8];
    for( int i=0; i<size; i++)
    {
	fread(data, 8, 1, fptr_stego_image);
	decInfo->arr[i]=decode_byte_to_lsb_d(data);
    }
    decInfo->arr[size]='\0';
    return e_success;
}
/*function definition for decoding byte by byte data from image*/
Status decode_byte_to_lsb_d(char *data)
{
    char ch=0;
    for(int i=0; i<8; i++)
    {
	ch = ch | ((data[i] & 0x01) << i);
    }
    return ch;
}
/*function definition for decoding size from image*/
Status decode_size_to_lsb_d(char * Image_buffer)
{
    int a=0;
    for(int i=0; i<32; i++)
    {
	a = (a | ((Image_buffer[i] & 0x01) << i));
    }
    return a;
}
/*function definition to decode secret file extention size*/
Status decode_secret_file_extn_size_d(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decInfo->extn_size=decode_size_to_lsb_d(buffer);
    return e_success;
}
/*function definition to decode secret file extention*/
Status decode_secret_file_extn_d(DecodeInfo *decInfo)
{
    decode_data_to_image_d(decInfo->extn_size, decInfo->fptr_stego_image, decInfo);
    return e_success;
}
/*function definition to decode secret file size*/
Status decode_secret_file_size_d(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decInfo->secret_file_size=decode_size_to_lsb_d(buffer);
    return e_success;
}
/*function definition to decode secret file data*/
Status decode_secret_file_data_d(DecodeInfo *decInfo)
{
    char c;
    char data[8];
    for( int i=0; i<decInfo->secret_file_size; i++)
    {
	fread(data, 8, 1, decInfo->fptr_stego_image);
	c=decode_byte_to_lsb_d(data);
	fputc(c,decInfo->fptr_output);
    }
    fclose(decInfo->fptr_output);
    return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_d(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
	return e_failure;
    }
    // No failure return e_success
    return e_success;
}
/*function definition to open the output file*/
Status open_files_output_d(DecodeInfo *decInfo)
{
    // output file
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    // Do Error handling
    if (decInfo->fptr_output == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);
	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

