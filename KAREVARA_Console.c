/*
 ---------------------------------------------------------------------------
 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include this list of conditions
      and the following disclaimer;

   2. distributions in binary form include this list of conditions and
      the following disclaimer in the documentation and/or other
      associated materials;

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 03/09/2012
*/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "stdint.h"
#include "KAREVARA_IO.h"

#define MAX_FILENAME_LEN 128
#define MAX_FILE_SIZE 10000000
#define MAX_LOG_SIZE 100000

/* This file provides a console-based front-end to KAREVARA.c */


int main(int argc, char *argv[])
{
	uint32_t quantity;
	FILE *outfp = stdout, *unencfp = NULL, *logfp = stderr;
	uint8_t outfp_open = 0, unencfp_open = 0, logfp_open = 0;
	char tempfilename[MAX_FILENAME_LEN+1] = "",
		tempfilename2[MAX_FILENAME_LEN+1] = "",
		logfilename[MAX_FILENAME_LEN+1] = "",
		master_key[MASTER_KEY_LEN+1] = "",
		customer[CUS_LEN+1] = "",
		batch[BATCH_LEN+1] = "",
		start_IMSI[MAX_IMSI_LEN+1] = "",
		transport_key[MAX_TRANS_KEY_LEN+1] = "",
		transport_key_file[MAX_FILENAME_LEN+1] = "",
		format_string[MAX_FORMAT_STRING_LEN+1] = "",
		msisdn[MSISDN_LEN+1] = "",
		iccid[ICCID_LEN+1] = "",
		type[TYPE_LEN+1] = "",
		graph_ref[GRAPH_REF_LEN+1] = "",
		profile[PROFILE_LEN+1] = "",
		additional_str[ADD_STR_LEN+1] = "";
	char *inpstr = NULL;
	char *trancsvstr = NULL;
	uint32_t key_num = 0;
	int b_log = 0;
	int input_error = 0;
	uint8_t encrypt = NO_ENCRYPT;
	int32_t tran_key_num = -1;
	uint8_t no_data = 0;
	int i;
	char *logstr = NULL,
		*unencstr = NULL,
		*outstr = NULL;

	if(argc < 3)
	{
		input_error = 1;
		fprintf(stderr,"Error: KAREVARA_Console: Too few arguments.\n");
	}
	else
	{
		/* Processing Arguments */

		/* Iterate over remaining arguments to pull out options*/
		while ((--argc > 0) && (!input_error))
		{
			if((*++argv)[0] == '-')
			{
				char c = *++(argv[0]);
				switch (c)
				{
					/* Starting IMSI */
					case 's':
						/* Get start_IMSI */
						++argv; --argc;
						if(argc && argv[0][0] != '-' && strlen(*argv) <= MAX_IMSI_LEN)
						{
							strncpy(start_IMSI,*argv,MAX_IMSI_LEN);
						}
						else
						{
							input_error = 1;
							fprintf(stderr,"Error: KAREVARA_Console: IMSI too long or not supplied.\n",MAX_IMSI_LEN);
						}
						break;

					/* Quantity */
					case 'q':
						++argv; --argc;
						sscanf(*argv,"%d",&quantity);
						if(quantity > 99999999)
						{
							input_error = 1;
							fprintf(stderr,"Error: KAREVARA_Console: Batch Size too large.\n");
						}
						break;

					/* Output file: */
					case 'o':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) < MAX_FILENAME_LEN)
							{
								strncpy(tempfilename,*argv,MAX_FILENAME_LEN);
								if(openfile(&outfp, tempfilename, "w", &outfp_open))
								{
									fprintf(stderr,"Error: Cannot open %s.\n", tempfilename);
									input_error = 1;
								}
							}
							else fprintf(stderr,"Warning: KAREVARA_Console: Filename too long. \n");
						}
						break;

					/* unencrypted output (CSV) file: */
					case 'u':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) < MAX_FILENAME_LEN)
							{
								strncpy(tempfilename,*argv,MAX_FILENAME_LEN);
								if(openfile(&unencfp, tempfilename, "w", &unencfp_open))
								{
									fprintf(stderr,"Error: Cannot open %s.\n", tempfilename);
									input_error = 1;
								}
							}
							else fprintf(stderr,"Warning: KAREVARA_Console: Filename too long. \n");
						}
						else
						{
							unencfp = stdout;
						}
						break;

					/* Input File: .INP file to read data from */
					case 'i':
						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) < MAX_FILENAME_LEN)
							{
								strncpy(tempfilename,*argv,MAX_FILENAME_LEN);
								if(filetostr(tempfilename,&inpstr,NULL))
								{
									fprintf(stderr,"Error: Cannot Load %s.\n", tempfilename);
									input_error = 1;
								}
							}
							else fprintf(stderr,"Warning: KAREVARA_Console: Filename too long. \n");
						}
						else
						{
							fprintf(stderr,"No .INP file provided. \n");
						}
						break;

					/* Master Secret: If not specified a default will be used */
					case 'm':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) > MASTER_KEY_LEN)
							{
								fprintf(stderr,"Warning: KAREVARA_Console: Master Secret is too long.\n");
								strncpy(master_key,*argv,MASTER_KEY_LEN);
							}
							else
							{
								strncpy(master_key,*argv,strlen(*argv));

								/* If too short, pad with zero */
								for(i = strlen(*argv);i<MASTER_KEY_LEN;i++)
									master_key[i] = '0';
							}
							master_key[MASTER_KEY_LEN] = '\0';
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: Master Secret Provided. \n");
							input_error = 1;
						}
						break;

					/* Batch_ID */
					case 'b':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(batch,*argv,BATCH_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}
						break;

					/*Customer */
					case 'c':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(customer,*argv,CUS_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: Customer not provided. \n");
							input_error = 1;
						}
						break;

					/* Format String */
					case 'f':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) < MAX_FORMAT_STRING_LEN)
							{
								strncpy(format_string,*argv,MAX_FORMAT_STRING_LEN);
							}
							else
							{
								fprintf(stderr,"Warning: KAREVARA_Console: Format String too long. \n");
							}
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Format String provided. \n");
							input_error = 1;
						}
						break;

					case 'a':
					{
						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(!strcmp(*argv,"DES"))
							{
								encrypt = ENCRYPT_DES;
							}
							else if(!strcmp(*argv,"TDES"))
							{
								encrypt = ENCRYPT_TDES;
							}
							else if(!strcmp(*argv,"AES"))
							{
								encrypt = ENCRYPT_AES;
							}
							else
							{
								fprintf(stderr,"Error: KAREVARA_Console: Encryption Algorithm not recognised. \n");
								input_error = 1;
							}
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Algorithm for Encryption Provided. \n");
							input_error = 1;
						}


					}

					/* Specify Transport Key Number */
					case 'n':
					{
						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							sscanf(*argv,"%d",&tran_key_num);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}

						/* We want some kind of encryption */
						if(encrypt == NO_ENCRYPT)
						{
							encrypt = ENCRYPT_DES;
						}
						break;
					}

					/* Transport Key */
					case 'k':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(transport_key,*argv,MAX_TRANS_KEY_LEN+1);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Provided. \n");
							input_error = 1;
						}

						/* We want some kind of encryption */
						if(encrypt == NO_ENCRYPT)
						{
							encrypt = ENCRYPT_DES;
						}
						break;

					/* Specify transport filename: */
					case 't':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) < MAX_FILENAME_LEN)
							{
								strncpy(tempfilename2,*argv,MAX_FILENAME_LEN);
								if(filetostr(tempfilename2,&trancsvstr,NULL))
								{
									fprintf(stderr,"Error: Cannot Load %s.\n", tempfilename);
									input_error = 1;
								}
							}
							else fprintf(stderr,"Warning: KAREVARA_Console: Filename too long. \n");
						}
						else
						{
							fprintf(stderr,"No Transport Key CSV file provided. \n");
						}
						/* We want some kind of encryption */
						if(encrypt == NO_ENCRYPT)
						{
							encrypt = ENCRYPT_DES;
						}
						break;

					/* logfile output: default will be stdout & stderr */
					case 'v':
					{
						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							if(strlen(*argv) < MAX_FILENAME_LEN)
							{
								strncpy(logfilename,*argv,MAX_FILENAME_LEN);
								if(openfile(&logfp, logfilename, "a", &logfp_open))
								{
									fprintf(stderr,"Error: Cannot open %s.\n", tempfilename);
									input_error = 1;
								}
							}
							else fprintf(stderr,"Warning: KAREVARA_Console: Filename too long. \n");
						}
						break;
					}

					/* No Data */
					case 'x':
					{
						no_data = 1;
						break;
					}

					/* Type */
					case 'y':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(type,*argv,TYPE_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}
						break;

					/* Profile */
					case 'p':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(profile,*argv,PROFILE_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}
						break;

					/* Graph_Ref */
					case 'g':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(graph_ref,*argv,GRAPH_REF_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}
						break;

					/* MSISDN */
					case 'h':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(msisdn,*argv,MSISDN_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}
						break;

					/* ICCID */
					case 'd':

						argv++; argc--;
						if(argc && argv[0][0] != '-')
						{
							strncpy(iccid,*argv,ICCID_LEN);
						}
						else
						{
							fprintf(stderr,"Error: KAREVARA_Console: No Transport Key Number Provided. \n");
							input_error = 1;
						}
						break;

					default:

						fprintf(stderr,"Warning: KAREVARA_Console: Do not recognise argument: -%c\n",c);

				/* switch */
				}
			/* if */
			}
		/* while */
		}
	}
	if(input_error)
	{
		fprintf(stderr,"\nKAREVARA_Console Usage: [Arguments]\n\n");
		fprintf(stderr,"See README file for more details. \n\n");
		fprintf(stderr,"Arguments: \n\n");
		fprintf(stderr,"\t -s [Starting IMSI] Required unless within .inp file.\n");
		fprintf(stderr,"\t -q [Quantity] Required unless within .inp file.\n");
		fprintf(stderr,"\t -m [Key] \t Specify Master Key (32 hex digits). Required unless testing.\n");
		fprintf(stderr,"\nOptional Arguments: \n\n");
		fprintf(stderr,"\t -o [.OUT filename] Output .OUT file.\n");
		fprintf(stderr,"\t -i [.INP filename] Load values from .INP file.\n");
		fprintf(stderr,"\t -u [.CSV filename] Output unencrypted csv file containing parameters\n");
		fprintf(stderr,"\t -c [Customer] Customer name\n");
		fprintf(stderr,"\t -b [Batch ID] Decimal Batch ID\n");
		fprintf(stderr,"\t -f [Format field] Output format string (see readme)\n");
		fprintf(stderr,"\t -v [log filename] Log file for output (rather than stdout and stderr)\n");
		fprintf(stderr,"\t -x No data is produced, only a .INP file.\n");
		fprintf(stderr,"\nSee README for additional arguments. \n\n");
		return 1;
	}
	else
	{
		logstr = (char *) malloc(MAX_LOG_SIZE),
		unencstr = (char *) malloc(MAX_FILE_SIZE),
		outstr = (char *) malloc(MAX_FILE_SIZE);

		if((logstr == NULL) || (unencstr == NULL) || (outstr == NULL))
		{
			fprintf(stderr,"Error: KAREVARA_Console: Failed to allocate data.\n");
			goto Error_Point;
		}

		logstr[0] = '\0';
		unencstr[0] = '\0';
		outstr[0] = '\0';

		if(no_data)
		{
			/* Create INP file */
			if(KAREVARA_Create_INP( master_key,
						 customer,
						 batch,
						 start_IMSI,
						 quantity,
						 type,//type,
						 profile,//profile,
						 NULL,//address,
						 graph_ref,//graph_ref,
						 msisdn,//MSISDN,
						 iccid,//ICCID,
						 NULL,
						 format_string,
						 encrypt,
						 tran_key_num,
						 outstr, MAX_FILE_SIZE,
						 logstr, MAX_LOG_SIZE))
			{
				fprintf(stderr,"Error: KAREVARA_Console: KAREVARA_Create_INP failed.\n");
				goto Error_Point;
			}
		}
		else if(inpstr != NULL)
		{
			/* Generate OUT from INP */
			if(KAREVARA_Create_OUT_fm_INP(master_key,
				inpstr,
				transport_key,
				trancsvstr,
				outstr,MAX_FILE_SIZE,
				unencstr,MAX_FILE_SIZE,
				logstr,MAX_LOG_SIZE))
			{
				fprintf(stderr,"Error: KAREVARA_Console: KAREVARA_Create_OUT_fm_INP failed.\n");
				goto Error_Point;
			}
		}
		else if((strlen(start_IMSI) != 0) && (quantity != 0))
		{
			/* Get the tranport_key. This can fail - we may not need one */
			Get_Tran_Key_fm_CSV(transport_key,tran_key_num,trancsvstr,NULL,0);

			/* Create the OUT file */
			if(KAREVARA_Create_OUT( master_key,
						 customer,
						 batch,
						 start_IMSI,
						 quantity,
						 type,//type,
						 profile,//profile,
						 NULL,//address,
						 graph_ref,//graph_ref,
						 msisdn,//MSISDN,
						 iccid,//ICCID,
						 NULL,
						 format_string,
						 encrypt,
						 tran_key_num,
						 transport_key,
						 outstr, MAX_FILE_SIZE,
						 unencstr, MAX_FILE_SIZE,
						 logstr, MAX_LOG_SIZE))
			{
				fprintf(stderr,"Error: KAREVARA_Console: KAREVARA_Create_OUT failed.\n");
				goto Error_Point;
			}
		}
		else
		{
			fprintf(stderr,"Error: KAREVARA_Console: start_IMSI or quantity not provided\n");
			goto Error_Point;
		}

		if((logfp != NULL) && (strlen(logstr) > 0))
		{
			fwrite(logstr, sizeof(char), strlen(logstr), logfp);
			if(logfp_open)
			{
				closefile(logfp,&logfp_open);
				movelastlogtofront(logfilename);
			}
		}
		if((outfp != NULL)  && (strlen(outstr) > 0))
		{
			if(outfp == stdout)
			{
				fprintf(outfp, "\nOutput File: \n\n");
			}
			fwrite(outstr, sizeof(char), strlen(outstr), outfp);
			closefile(outfp,&outfp_open);
		}
		if((unencfp != NULL)  && (strlen(unencstr) > 0))
		{
			fwrite(unencstr, sizeof(char), strlen(unencstr), unencfp);
			closefile(unencfp,&unencfp_open);
		}
	}
	free(outstr); free(unencstr); free(logstr);
	free(inpstr); free(trancsvstr);
	closefile(outfp,&outfp_open);
	closefile(unencfp,&unencfp_open);
	closefile(logfp,&logfp_open);
	return 0;

Error_Point:

	if((logfp != NULL) && (strlen(logstr) > 0))
	{
		fwrite(logstr, sizeof(char), strlen(logstr), logfp);
		if(logfp_open)
		{
			closefile(logfp,&logfp_open);
			movelastlogtofront(logfilename);
		}
	}
	free(outstr); free(unencstr); free(logstr);
	free(inpstr); free(trancsvstr);
	closefile(outfp,&outfp_open);
	closefile(unencfp,&unencfp_open);
	closefile(logfp,&logfp_open);
	return 0;


}
