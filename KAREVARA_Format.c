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

#include "KAREVARA_Format.h"

/*****************************************************
Init_KAREVARA_Format:
Sets up KAREVARA_Format for use

Input:		Format - ptr to Format to setup

Return: 	0 Success
			1 Failure

*****************************************************/
int Init_KAREVARA_Format(KAREVARA_Format *Format)
{
	/* Init num */
	Format->num_rec = 0;
	Format->num_blk = 0;

	/*Ensure byte_req is up to date*/
	Compute_Byte_Req(Format);

	return 0;
}

/*****************************************************
Init_KAREVARA_Format_Rec:
zero initialise function

Input:		Record - Record to init.

Return: 	void
*****************************************************/
static void Init_KAREVARA_Format_Rec(KAREVARA_Format_Rec *Record)
{
	Record->name[0] = '\0';
	Record->details[0] = '\0';
	Record->length = 0;
	Record->type = NUL_TYPE;
	Record->byteref = 0;
	Record->bytelength = 0;
	Record->encrypt = 0;
}

/*****************************************************
Update_Record:
Basic copy function
*****************************************************/
static void Update_Record(KAREVARA_Format_Rec *Dest, KAREVARA_Format_Rec *Source)
{
	strncpy(Dest->name,Source->name,MAX_FORMAT_NAME_LEN);
	strncpy(Dest->details,Source->details,MAX_FORMAT_NAME_LEN);
	Dest->length = Source->length;
	Dest->type = Source->type;
	Dest->byteref = Source->byteref;
	Dest->bytelength = Source->bytelength;
	Dest->encrypt = Source->encrypt;
	return;
}

/*****************************************************
Add_Record_To_KAREVARA_Format:
Adds a KAREVARA_Format_Rec Record to KAREVARA_Format

Input:		thisFormat - ptr to Format
			thisRec - ptr to Record to add

Return: 	0 Success
			1 Failure

*****************************************************/
int Add_Record_To_KAREVARA_Format(KAREVARA_Format *thisFormat, KAREVARA_Format_Rec *thisRec)
{
	uint32_t i;
	uint32_t num = thisFormat->num_rec;
	uint8_t update = 0;
	KAREVARA_Format_Rec *pRec;
	if(num+1 > MAX_FORMAT_RECORDS)
	{
		Error("Add_Record_To_Format: Too many Records.");
		return 1;
	}

	/* Check to see if we already have this record
	Note we could use a more complex structure (e.g. hashmap)
	but this will work and won't be too slow */
	pRec = thisFormat->Record;
	for(i=0;i<num;i++)
	{
		if(!strcmp(pRec->name,thisRec->name))
		{
			Update_Record(pRec,thisRec);
			update = 1;
		}
		pRec++;
	}

	if(!update)
	{
		/* Add to end */
		Update_Record(pRec,thisRec);
		thisFormat->num_rec++;
	}

	/*Ensure byte_req is up to date*/
	Compute_Byte_Req(thisFormat);


	return 0;
}

/*****************************************************
String_To_KAREVARA_Format:
Convert a format string to format record
e.g. /Ki:32x/PIN:4d/PUK:8d/ADM:16x/

Input:		IN		format_str - string to convert
			OUT		Format - format records

Return: 	0 Success
			1 Failure

*****************************************************/
int String_To_KAREVARA_Format(char format_str[], OUT KAREVARA_Format *Format)
{
	char *substring;
	int i;
	/* Place to hold copy to prevent corruption of string */
	char formatstring[MAX_FORMAT_STRING_LEN+1];

	if(Format == NULL)
	{
		Error("String_To_KAREVARA_Format: Format is NULL.");
		return 1;
	}

	/* Check format string:*/
	if((format_str != NULL) && (strlen(format_str) != 0))
	{
		if(format_str[0] != '/' || format_str[strlen(format_str)-1] != '/')
		{
			/* Fail */
			Error("String_To_KAREVARA_Format: Format string is not of form \"/.../\".");
			return 1;
		}
	}
	else
	{
		/* Use default */
		Warning("String_To_KAREVARA_Format: Format string not provided. Loading Default.");
		format_str = default_format_string;
	}

	/* Make a copy */
	strncpy(formatstring,format_str,MAX_FORMAT_STRING_LEN+1);
	/*if(P_LOGGING) fprintf(P_log,"Converting Format String '%s' into records.\n",formatstring);*/


	/* Split format string into record substrings */
	substring = strtok(formatstring,"/");
	i = 0;

	while(substring != NULL)
	{
		/* Check string before processing */

		if((strlen(substring) != 0) && (strlen(substring) < 1000))
		{
			if(Add_Rec_String_To_KAREVARA_Format(Format,substring))
			{
				Error("String_To_KAREVARA_Format: Add_Rec_String_To_KAREVARA_Format failed.");
				return 1;
			}
			i++;
		}
		/* Get next string */
		substring = strtok(NULL,"/");
	}
	strcpy(formatstring,"");
	return 0;
}

/*****************************************************
Add_Rec_String_To_KAREVARA_Format:
Convert a single format string to our format record
e.g. Ki:32x

Input:		OUT	Format - format records
			IN	str - string to convert

Return: 	0 Success
			1 Failure

*****************************************************/
static int Add_Rec_String_To_KAREVARA_Format(KAREVARA_Format *thisFormat, char str[])
{
	KAREVARA_Format_Rec Record;
	if(String_To_KAREVARA_Format_Rec(str,&Record))
	{
		Error("Add_Rec_String_To_KAREVARA_Format: String_To_KAREVARA_Format_Rec failed.");
		return 1;
	}
	if(Add_Record_To_KAREVARA_Format(thisFormat, &Record))
	{
		Error("Add_Rec_String_To_KAREVARA_Format: Add_Record_To_KAREVARA_Format failed.");
		return 1;
	}
	return 0;
}

/*****************************************************
Add_Inc_To_KAREVARA_Format:
Adds Increment parameter to KAREVARA Format
e.g. IMSI+1000

Input:		IN	Format - format record ptr
			IN	name - name of parameter
			IN	val - parameter value

Return: 	0 Success
			1 Failure

*****************************************************/
int Add_Inc_To_KAREVARA_Format(IN KAREVARA_Format *thisFormat,IN char name[], IN char val[])
{
	if((thisFormat == NULL) && (name != NULL) && (val != NULL))
	{
		Error("Add_Inc_To_KAREVARA_Format: KAREVARA_Format NULL.");
		return 1;
	}
	trimstr(name,MASKALPHANUMS);
	trimstr(val,MASKNUM);
	if((strlen(val) > 0) && (strlen(name) > 0))
	{
		char tempstr[100];
		sprintf(tempstr,"%s+%s",name,val);
		if(Add_Rec_String_To_KAREVARA_Format(thisFormat,tempstr))
		{
			Error("Add_Inc_To_KAREVARA_Format: Add_Rec_String_To_KAREVARA_Format failed.");
			return 1;
		}
	}
	else
	{
		Error("Add_Fix_To_KAREVARA_Format: Nothing to add or Parameters not recongised.");
		return 1;

	}

	return 0;
}

/*****************************************************
Add_Fix_To_KAREVARA_Format:
Adds Fixed parameter to KAREVARA Format
e.g. PIN=1000

Input:		IN	Format - format record ptr
			IN	name - name of parameter
			IN	val - parameter value

Return: 	0 Success
			1 Failure

*****************************************************/
int Add_Fix_To_KAREVARA_Format(KAREVARA_Format *thisFormat,char name[], char val[])
{
	if((thisFormat == NULL) && (name != NULL) && (val != NULL))
	{
		Error("Add_Fix_To_KAREVARA_Format: NULL Input.");
		return 1;
	}
	trimstr(name,MASKALPHANUMS);
	trimstr(val,MASKALPHANUMS);
	if((strlen(val) > 0) && (strlen(name) > 0))
	{
		char tempstr[100];
		sprintf(tempstr,"%s=%s",name,val);
		if(Add_Rec_String_To_KAREVARA_Format(thisFormat,tempstr))
		{
			Error("Add_Fix_To_KAREVARA_Format: Add_Rec_String_To_KAREVARA_Format failed.");
			return 1;
		}
	}
	else
	{
		Error("Add_Fix_To_KAREVARA_Format: Nothing to add or Parameters not recongised.");
		return 1;
	}

	return 0;
}

/*****************************************************
Add_Rand_To_KAREVARA_Format:
Adds Random parameter to KAREVARA Format
e.g. PIN=1000

Input:		IN	Format - format record ptr
			IN	name - name of parameter
			IN	len - length of random value
			IN	type - type (x for hex, d for decimal)

Return: 	0 Success
			1 Failure

*****************************************************/
int Add_Rand_To_KAREVARA_Format(KAREVARA_Format *thisFormat,char name[], uint32_t len, char type)
{
	if((thisFormat == NULL) && (name != NULL))
	{
		Error("Add_Rand_To_KAREVARA_Format: NULL Input.");
		return 1;
	}
	trimstr(name,MASKALPHANUMS);
	if((strlen(name) > 0)  && (len < 10000000) && (type == 'x' || type == 'd'))
	{
		char tempstr[100];
		sprintf(tempstr,"%s:%d%c",name,len,type);
		if(Add_Rec_String_To_KAREVARA_Format(thisFormat,tempstr))
		{
			Error("Add_Rand_To_KAREVARA_Format: Add_Rec_String_To_KAREVARA_Format failed.");
			return 1;
		}
	}
	else
	{
		Error("Add_Rand_To_KAREVARA_Format: Nothing to add or Parameters not recongised.");
		return 1;
	}
	return 0;
}


/*****************************************************
String_To_KAREVARA_Format_Rec:
Creates a KAREVARA_Format_Rec Record from a 'Ki:32x' String

Input:		IN	Record str to use
			OUT Updated KAREVARA_Format_Rec Record

Return: 	0 Success
			1 Failure

*****************************************************/
static int String_To_KAREVARA_Format_Rec(IN char record_str[],OUT KAREVARA_Format_Rec *Record)
{
	char sep_sym = 'x';
	int scanret;
	uint32_t reclen = strlen(record_str), detaillen = 0;

	if(Record == NULL)
	{
		Error("String_To_KAREVARA_Format_Rec: NULL Input.");
		return 1;
	}
	/* If there is a Record, we should initialise it */
	else Init_KAREVARA_Format_Rec(Record);


	if(reclen > MAX_FORMAT_REC_LEN)
	{
		Error("String_To_KAREVARA_Format_Rec: Format String is too long.");
		return 1;
	}

	if(reclen > 0)
	{
		/* Encrypt? Does the string end in a ^ */
		if(record_str[reclen-1] == '^')
		{
			Record->encrypt = 1;
			record_str[reclen-1] = '\0';
		}
		else Record->encrypt = 0;
	}

	/* Extract information from record string */
	scanret = sscanf(record_str,"%[^=:+]%c%s",
		Record->name,
		&sep_sym,
		Record->details);

	if(scanret == 3)
	{
		detaillen = strlen(Record->details);
		if(detaillen == 0)
		{
			/* I don't think this is possible */
			Error("String_To_KAREVARA_Format_Rec: No Format String after separator \n");
			return 1;
		}

		/* Generate symbol */
		if(sep_sym == ':')
		{
			if(sscanf(Record->details,"%d%c",
				&(Record->length),
				&(Record->type)) != 2)
			{
				Error("String_To_KAREVARA_Format_Rec: Incorrect Format String after separator \n");
				return 1;
			}
		}
		/* Fixed symbol */
		else if(sep_sym == '=')
		{
			Record->type = FIX_TYPE;
			Record->length = detaillen;

			/* Don't need these */
			Record->encrypt = 0;

		}
		/* Inc symbol */
		else if(sep_sym == '+')
		{

			Record->type = INC_TYPE;
			Record->length = detaillen;
			if(strspn(Record->details,MASKNUM) != Record->length)
			{
				Error("String_To_KAREVARA_Format_Rec: incrementation string is not an integer; %s: %s\n",Record->name,Record->details);
				return 1;
			}

			/* Don't need these */
			Record->encrypt = 0;
			/* Don't recognise the symbol */
		}
		else
		{
			char temp[100];
			sprintf(temp,"Error: String_To_KAREVARA_Format_Rec: Separator not supported: %s",record_str);
			Error(temp);
			return 1;
		}
	}
	else if(scanret == 2)
	{
		Error("String_To_KAREVARA_Format_Rec: No Format String after separator \n");
		return 1;
	}
	else if(scanret == 1)
	{
		/* No symbol provided: Use Defaults */
		KAREVARA_Format_Rec_Load_Default(Record);
	}
	return 0;
}

/*****************************************************
Compute_Byte_Req:
Find where and how we will need to generate bits

Input:		IN	Format - the format record database

Return: 	0 Success
			1 Failure

*****************************************************/
static int Compute_Byte_Req(IN KAREVARA_Format *Format)
{
	uint32_t i, recordbytes;
	int totbytes = 0;
	KAREVARA_Format_Rec *pRec = Format->Record;

	if(Format == NULL)
	{
		Error("Compute_Byte_Req: NULL Input.");
		return 1;
	}

	for(i=0;i<Format->num_rec;i++)
	{
		recordbytes = 0;
		if(pRec->type == HEX_TYPE || pRec->type == DEC_TYPE)
		{
			/* Need bits */
			if(pRec->type == HEX_TYPE)
			{
				/* 1 byte is converted into 2 hex digits */
				pRec->bytelength = (pRec->length+1) / 2;
			}
			else
			{
				/* 4 bytes are converted into 4 decimal digits */
				pRec->bytelength = ((pRec->length+3) / 4)*4;
			}

			/* Avoiding Crossing block boundary.
				Pick byte start location appropriately */
			if((totbytes%16)+pRec->bytelength > 16)
			{
				pRec->byteref = totbytes+((16-totbytes)%16);
			}
			else pRec->byteref = totbytes;

			/*Update totalbytes used */
			totbytes = pRec->byteref+pRec->bytelength;
		}
		else
		{
			/* We don't need any random bits */
			pRec->byteref = 0;
			pRec->bytelength = 0;
		}
		pRec++;
	}
	Format->num_blk = (totbytes+15)/16;
	return 0;
}

/*****************************************************
KAREVARA_Format_Rec_Load_Default:
Loads default values. This allows us to say /Ki/ rather than /Ki:32x/

Input:		IN	Record to load default into (must already have a name)

Return: 	0 Success
			1 Failure

*****************************************************/
static void KAREVARA_Format_Rec_Load_Default(KAREVARA_Format_Rec *Record)
{
	char uppername[MAX_FORMAT_NAME_LEN];

	/* Convert name to Upper Case */
	strncpy(uppername,Record->name,MAX_FORMAT_NAME_LEN-1);
	toupperstr(uppername);

	/* Do we recognise it? */

	/* Ki: */
	if(!strcmp(uppername, "KI") || !strcmp(uppername, "K") ||
		!strcmp(uppername, "EK") || !strcmp(uppername, "EKI") ||
		!strcmp(uppername, "A4_KI") || !strcmp(uppername, "A4_K") ||
		!strcmp(uppername, "KI_A4") || !strcmp(uppername, "K_A4"))
	{
		Record->type = HEX_TYPE;
		Record->length = 32;
		Record->encrypt = 1;
	}
	/* Does it begin with IMSI? */
	else if(strstr(uppername, "IMSI") == uppername)
	{
		Record->type = INC_TYPE;
		strcpy(Record->details,"000000000000000");
		Record->length = strlen(Record->details);
	}
	/* Does it begin with PIN? */
	else if(strstr(uppername, "PIN") == uppername)
	{
		Record->type = FIX_TYPE;
		strcpy(Record->details,"0000");
		Record->length = 4;
	}
	/* Does it begin with PUK? */
	else if(strstr(uppername, "PUK") == uppername)
	{
		Record->type = DEC_TYPE;
		Record->length = 8;
	}
	/* Default case is 64-bit Random Hex */
	else
	{
		Record->type = HEX_TYPE;
		Record->length = 16;
	}
}

/*****************************************************
Apply_KAREVARA_Format:
Main Function. Converts the 'Data' stream into individual record values based on format,
encrypts those where needed and outputs to a string

Input:		IN		Data - psuedorandom data
			IN		Record - List of format records
			IN		KAREVARA_Encrypt - encryption variables
			OUT		str - output string
					maxlen - maximum length of string

'enc_vars' is passed as pointer as it may be large in size.

Return: 	0 Success
			1 Failure

*****************************************************/
int Apply_KAREVARA_Format(IN uint8_t Data[],
						  IN KAREVARA_Format *Format,
						  IN KAREVARA_Encrypt *enc_vars,
						  OUT char out_str[],
						  uint32_t maxlen,
						  char delimit[])
{
	uint32_t i,j;
	uint8_t temp_data[DATA_BLOCK_SIZE];
	uint8_t temp_data2[DATA_BLOCK_SIZE];
	char str[MAX_FORMAT_REC_OUT_LEN];
	uint8_t blocklen = enc_vars->blocklen;
	KAREVARA_Format_Rec *pRec = Format->Record;

	if((out_str == NULL) || (delimit == NULL))
	{
		Error("Apply_KAREVARA_Format: Either output string or delimiting string is NULL.");
		return 1;
	}

	/* Init out_str */
	out_str[0] = '\0';

	/* Output actual data */
	for(i=0;i<Format->num_rec;i++)
	{
		uint8_t *inptrData8 = &Data[pRec->byteref];
		uint8_t *outptrData8 = temp_data2;
		uint32_t length = pRec->length;
		uint32_t bytelen = pRec->bytelength;

		str[0] = '\0';

		switch (pRec->type)
		{
			/* Hex */
			case HEX_TYPE:

				/* Encrypt (as hex)*/
				if((enc_vars->type != NO_ENCRYPT) && pRec->encrypt)
				{

					/* Is the length a multiple of the encryption block length? */
					/* If not we need to pad */
					/* no space in current location so will need to copy first */
					if(bytelen %blocklen != 0)
					{
						/* Check there is space */
						if(bytelen > DATA_BLOCK_SIZE)
						{
							Error("Apply_KAREVARA_Format: Not enough space in buffer.");
							return 1;
						}
						/* Copy and pad data*/
						for(j=0;j<bytelen;j++)
						{
							temp_data[j] = inptrData8[j];
						}

						/* Do we need to zero low bits */
						if(length%2 == 1) temp_data[bytelen-1] &= 240;

						/* Pad the block with zeros (note we're also updating bytelen in this loop) */
						for(;bytelen%blocklen!=0;bytelen++) temp_data[bytelen] = 0;

						/* Update length */
						length = bytelen*2;

						/* Redirect ptr to our temporary array */
						inptrData8 = temp_data;
					}

					/* Perform Encryption */
					for(j=0;j<bytelen;j+=blocklen)
					{
						enc_vars->encrypt_func(
							enc_vars->p_ctx,
							inptrData8,
							outptrData8);
						inptrData8 += blocklen;
						outptrData8 += blocklen;
					}
					inptrData8 = temp_data2;
				}


				/* Check there is space */
				if(bytelen > MAX_FORMAT_REC_OUT_LEN)
				{
					Error("Apply_KAREVARA_Format: Not enough space in character buffer.");
					return 1;
				}
				/*Convert to string */
				hextoascii(inptrData8,str,length,bytelen);

				break;


			/* Decimal */
			case DEC_TYPE:


				/* Check there is space */
				if(bytelen > MAX_FORMAT_REC_OUT_LEN)
				{
					Error("Apply_KAREVARA_Format: Not enough space in character buffer.");
					return 1;
				}
				/*Convert to digits */
				hextodigits(inptrData8,str,length,bytelen);

				/* Encrypt (as an ascii decimal string)*/
				if((enc_vars->type != NO_ENCRYPT) && pRec->encrypt)
				{

					/* Pad (if we need to pad). This will update bytelen*/
					for(bytelen = strlen(str);bytelen%blocklen != 0;bytelen++)
					{
						str[bytelen] = '\0';
					}
					length = bytelen*2;
					inptrData8 = (uint8_t *)str;

					/* Check there is space */
					if(bytelen > DATA_BLOCK_SIZE)
					{
						Error("Apply_KAREVARA_Format: Not enough space in buffer.");
						return 1;
					}

					/* Perform Encryption */
					for(j=0;j<bytelen;j+=blocklen)
					{
						enc_vars->encrypt_func(
							enc_vars->p_ctx,
							inptrData8,
							outptrData8);
						inptrData8 += blocklen;
						outptrData8 += blocklen;

					}
					/* Check there is space */
					if(bytelen > MAX_FORMAT_REC_OUT_LEN)
					{
						Error("Apply_KAREVARA_Format: Not enough space in character buffer.");
						return 1;
					}
					/* Convert hex to string */
					hextoascii(temp_data2,str,length,bytelen);
				}
				break;


			case FIX_TYPE:

				/* Check there is space */
				if(pRec->length+1 > MAX_FORMAT_REC_OUT_LEN)
				{
					Error("Apply_KAREVARA_Format: Not enough space in character buffer.");
					return 1;
				}
				/* Output */
				strncpy(str,pRec->details,pRec->length+1);
				break;


			case INC_TYPE:

				/* Check there is space */
				if(pRec->length+1 > MAX_FORMAT_REC_OUT_LEN)
				{
					Error("Apply_KAREVARA_Format: Not enough space in character buffer.");
					return 1;
				}
				/* Output */
				strncpy(str,pRec->details,pRec->length+1);
				break;


			/* Haven't defined others */
			default:

				Error("Apply_KAREVARA_Format: Format type not supported.");
				return 1;
		}

		/* end switch */

		/* end str with delimit character */
		strcat(str,delimit);

		/* Check for space */
		if(strlen(str) + strlen(out_str) > maxlen)
		{
			Error("Apply_KAREVARA_Format: Not enough space in output buffer.");
			return 1;
		}
		/* Add to output string */
		strncat(out_str,str,MAX_FORMAT_REC_OUT_LEN);
		/* step */
		pRec++;
	/* for loop */
	}
	return 0;
}


/*****************************************************
KAREVARA_Format_To_String:
Creates a format string from KAREVARA_Format records

Input:		IN	thisFormat - Format Records
			OUT format_str - string for output
			maxlen - maximum length of output string

Return: 	0 Success
			1 Failure

*****************************************************/
int KAREVARA_Format_To_String(IN KAREVARA_Format *thisFormat, OUT char format_str[],uint32_t maxlen)
{
	uint32_t i;
	KAREVARA_Format_Rec *pRec = thisFormat->Record;
	for(i=0;i<thisFormat->num_rec;i++)
	{
		/* Add to the format string */
		if(KAREVARA_Format_Rec_To_String(pRec,format_str,maxlen))
		{
			Error("KAREVARA_Format_To_String: KAREVARA_Format_Rec_To_String failed.");
			return 1;
		}
		pRec++;
	}
	return 0;
}

/*****************************************************
KAREVARA_Var_To_String:
Creates a format string from KAREVARA_Format records

Input:		IN	thisFormat - Format Records
			OUT var_str - string for output
			IN var_delimit - string to delimit fields
			maxlen - maximum length of output string

Return: 	0 Success
			1 Failure

*****************************************************/
int KAREVARA_Var_To_String(IN KAREVARA_Format *thisFormat, OUT char var_str[],IN char var_delimit[],uint32_t maxlen)
{
	uint32_t i;
	KAREVARA_Format_Rec *pRec = thisFormat->Record;
	if((var_str == NULL) || (var_delimit == NULL))
	{
		Error("KAREVARA_Var_To_String: NULL input.");
		return 1;
	}
	for(i=0;i<thisFormat->num_rec;i++)
	{
		/* Add to the format string */
		if(strlen(var_str)+strlen(pRec->name)+1 > maxlen)
		{
			Error("KAREVARA_Var_To_String: Not enough space.");
			return 1;
		}
		strncat(var_str,pRec->name,MAX_FORMAT_NAME_LEN);
		strcat(var_str,var_delimit);
		pRec++;
	}
	return 0;
}

/*****************************************************
KAREVARA_Format_Rec_To_String:
Creates a format string from single KAREVARA_Format_Rec record

Input:		IN	Record - ptr to Format Record
			OUT format_str - string for output
			maxlen - maximum length of output string

Return: 	0 Success
			1 Failure

*****************************************************/
int KAREVARA_Format_Rec_To_String(IN KAREVARA_Format_Rec *Record, OUT char format_str[], uint32_t maxlen)
{
	char recstr[MAX_FORMAT_STRING_LEN];
	/*char endstr[MAX_FORMAT_STRING_LEN], startstr[MAX_FORMAT_STRING_LEN];
	char *ptr, *sptr; */

	/* If empty, start the string */
	if(strlen(format_str) == 0 && maxlen > 0)
	{
		format_str[0] = '/';
		format_str[1] = '\0';
	}

	/* Convert Record to record string */
	if(Record->type == FIX_TYPE)
	{
		sprintf(recstr,"%s=%s",Record->name,Record->details);
	}
	else if(Record->type == INC_TYPE)
	{
		sprintf(recstr,"%s+%s",Record->name,Record->details);
	}
	else if(Record->type == HEX_TYPE)
	{
		sprintf(recstr,"%s:%dx",Record->name,Record->length);
	}
	else if(Record->type == DEC_TYPE)
	{
		sprintf(recstr,"%s:%dd",Record->name,Record->length);
	}
	else
	{
		Error("KAREVARA_Format_Rec_To_String: Type not supported.");
		return 1;
	}
	/* Is the parameter encrypted? */
	if(Record->encrypt)
	{
		strcat(recstr,"^");
	}
	/* Add end slash */
	strcat(recstr,"/");

	/* Add this record onto the format string */
	if(strlen(format_str)+strlen(recstr) < maxlen)
	{
		strncat(format_str,recstr,MAX_FORMAT_STRING_LEN-1);
	}
	else
	{
		Error("Add_To_Format_String: Not enough space in format_str.");
		return 1;
	}

	return 0;
}

/*****************************************************
KAREVARA_VAR_OUT:
Produces VAR_OUT (list of parameters, '/' separated, no format info.

Input:		IN		thisFormat - ptr to Format
			OUT		out_str - string for output
			OUT		maxlen -  maximum length of output string

Return: 	0 Success
			1 Failure

*****************************************************/
int KAREVARA_VAR_OUT(KAREVARA_Format *thisFormat, char out_str[], uint32_t maxlen)
{
	/* Produce VAR_OUT */
	if(out_str == NULL)
	{
		Error("KAREVARA_VAR_OUT: NULL input.\n");
		return 1;
	}
	strncat(out_str,"VAR_OUT: \t/",maxlen-strlen(out_str));
	if(KAREVARA_Var_To_String(thisFormat, out_str, "/", maxlen))
	{
		Error("KAREVARA_VAR_OUT: KAREVARA_Var_To_String failed.\n");
		return 1;
	}
	strcat(out_str,"\n");
	return 0;
}

/*****************************************************
KAREVARA_CSV_OUT:
Produces CSV Header (list of parameters, comma separated, no format info.

Input:		IN		thisFormat - ptr to Format
			OUT		out_str - string for output
			OUT		maxlen -  maximum length of output string

Return: 	0 Success
			1 Failure

*****************************************************/
int KAREVARA_CSV_OUT(KAREVARA_Format *thisFormat, char out_str[], uint32_t maxlen)
{
	/* Produce CSV Header */
	if(out_str == NULL)
	{
		Error("KAREVARA_CSV_OUT: NULL input.\n");
		return 1;
	}
	if(KAREVARA_Var_To_String(thisFormat, out_str, ",", maxlen))
	{
		Error("KAREVARA_CSV_OUT: KAREVARA_Var_To_String failed.\n");
		return 1;
	}
	return 0;
}

/*****************************************************
KAREVARA_FORMAT_OUT:
Produces FORMAT_OUT (list of parameters with format info.)

Input:		IN		thisFormat - ptr to Format
			OUT		out_str - string for output
			OUT		maxlen -  maximum length of output string

Return: 	0 Success
			1 Failure

*****************************************************/
int KAREVARA_FORMAT_OUT(KAREVARA_Format *thisFormat, char out_str[], uint32_t maxlen)
{
	/* Produce FORMAT_OUT */
	if(out_str == NULL)
	{
		Error("KAREVARA_FORMAT_OUT: NULL input.\n");
		return 1;
	}
	strncat(out_str,"FORMAT_OUT: \t/",maxlen-strlen(out_str));
	if(KAREVARA_Format_To_String(thisFormat, out_str, maxlen))
	{
		Error("KAREVARA_FORMAT_OUT: KAREVARA_Var_To_String failed.\n");
		return 1;
	}
	strcat(out_str,"\n");
	return 0;
}

/*****************************************************
KAREVARA_Format_Check_Encrypt:
Checks whether the Encrypt parameter provided matches the format requirement

Input:		IN		KAREVARA_Format Format
					encrypt algorithm indicator

Return: 	void

*****************************************************/
void KAREVARA_Format_Check_Encrypt(IN KAREVARA_Format *Format, uint8_t *encrypt)
{
	uint32_t i;
	uint8_t need_enc = 0;
	for(i=0;i<Format->num_rec;i++)
	{
		need_enc |= Format->Record[i].encrypt;
	}
	if(need_enc == 0)
	{
		*encrypt = NO_ENCRYPT;
	}
	else if(*encrypt == NO_ENCRYPT)
	{
		Warning("KAREVARA_Format_Check_Encrypt: Encryption requested in specified format fields, but turned off.");
	}
	return;
}

/*****************************************************
Inc_KAREVARA_Format_Rec:
Numerically increments any INC_TYPE fields in KAREVARA_Format

Input:		IN		Format - ptr to the KAREVARA_Format Inst

Return: 	void

*****************************************************/
void Inc_KAREVARA_Format_Rec(KAREVARA_Format *Format)
{
	uint32_t i;
	KAREVARA_Format_Rec *pRec = Format->Record;
	for(i=0;i<Format->num_rec;i++)
	{
		if(pRec->type == INC_TYPE)
		{
			pRec->length = inc_str(pRec->details);

		}
		pRec++;
	}
}

/*****************************************************
Log_KAREVARA_Format:
Logs what's in KAREVARA_Inst

Input:		IN		Main_Inst - what to log

Return: 	void
*****************************************************/
void Log_KAREVARA_Format(KAREVARA_Format *Format)
{
	char format_string[MAX_FORMAT_STRING_LEN] = "";
	if(Format != NULL)
	{
		Log("Random_Blocks_per_IMSI: %d",Format->num_blk);
		Log("Num_Format_Records:     %d",Format->num_rec);
		if(!KAREVARA_Format_To_String(Format,format_string,MAX_FORMAT_STRING_LEN))
		{
			Log("Format_String:          %s",format_string);
		}
	}
}

