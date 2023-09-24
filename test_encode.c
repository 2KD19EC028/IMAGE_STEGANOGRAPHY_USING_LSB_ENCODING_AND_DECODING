/*
Name:- Pratik B Chavan
Date:- 15/09/2023
Description:- Project on Stegnography

Encoding:
Sample input:- ./a.out -e beautiful.bmp secret.txt

Sample output:-
User's choice is to perform Encode operation
source file has .bmp extension
Secret file have only .txt or .c extension
read and validate is completed
open file operation is completed
width = 1024
height = 768
Check capacity is completed
copy bmp header is completed
encoding magic string is completed
encode secret file extn size completed
encode secret file extn is completed
encode secret file size is completed
secret file data is encoded
remaining data is copied
Encoding is completed

Decoding:
Sample input:-./a.out -d default.bmp output

Sample output:-
User's choice is to perform decode operation
encode data file has .bmp extension
read and validate is completed
open file operation is completed
decoding magic string is completed
decode secret file extn size completed
decode secret file extn is completed
decode secret file size is completed
output file is opened
secret file data is encoded
Decoding is completed
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"
/*main function consisting all function calls along with command line arguments*/
int main(int argc, char *argv[])
{
    /*Condition to check the whether the sufficient command line arguements are passed or not*/ 
    if(argc<3)
    {
	//if argument count less than 3 the put error
	printf("Error : Insufficient agrguements\nusage : ./a.out -e <source_file> <secret_file> or ./a.out -d <stego_file> <output_file>\n");
	return e_failure;
    }
    else
    {
	/*declaring structure variable*/
	EncodeInfo encInfo;
	/*if return type of check operation is e_encode than perform encoding operation*/
	if(check_operation_type(argv) == e_encode)
	{
	    printf("User's choice is to perform Encode operation\n");
	    /*check whether read and validate operation is successfull or not*/
	    if(read_and_validate_encode_args(argv, &encInfo) == e_success)
	    {
		printf("read and validate is completed\n");
		/*check whether decoding operation is successful or not*/
		if(do_encoding(&encInfo) == e_success)
		{ 
		    printf("Encoding is completed\n");
		    return e_success;
		}	
	    }
	} /*if return type of check operation is e_decode than perform decoding operation*/
	else if(check_operation_type_d(argv) == e_decode)
	{
	    DecodeInfo decInfo;
	    printf("User's choice is to perform decode operation\n");
	    /*check whether read and validate operation is successful or not in decoding*/
	    if(read_and_validate_decode_args_d(argv, &decInfo) == e_success)
	    {
		printf("read and validate is completed\n");
		/*check whether decoding operation is successful or not in decoding*/
		if(do_decoding_d(&decInfo) == e_success)
		{ 
		    printf("Decoding is completed\n");
		    return e_success;
		}
	    }
	}
    }
}

