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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "KAREVARA_IO.h"



/*****************************************************
KAREVARA_Create_OUT:
Generates a .OUT File using KAREVARA. Also creates .csv file containing unencrypted values.

Input:		IN		master_key_string	- MK string. 32 hex characters.
			IN		customer			- string. Max 100 chars.
			IN		batch				- digit string. Max 7 digits.
			IN		start_IMSI			- 16 (max) digit string
			IN		quantity			- integer. Max 8 digits
			IN		type				- type string. Can be NULL
			IN		Profile				- profile string. Can be NULL
			IN		Address str			- can be NULL. Can be multiline.
			IN		Graph_ref			- string containing graph ref. Can be NULL
			IN		MSISDN				- string containing digits. Can be NULL
			IN		ICCID				- string containing digits. Can be NULL.
			IN		additional			- Any additional parameters required. Should be of form 'name : value'
			IN		format_string		- (/../) string specifying output format. Max length 10000.
			IN		encrypt				- transport enc algorithm (e.g. DES=1, TDES=2, AES=3). Use 0 to turn off encryption.
			IN		transport_key_string - transport key hex char string
			OUT		outstr				- pointer to .OUT output string. Can be NULL if not required.
			IN		outstrlen			- max length of above string
			OUT		unenc_outcsv		- pointer to (unencrypted) .csv output string. Can be NULL if not required.
			IN		unenc_outcsvlen		- max length of above string
			OUT		logstr				- pointer to string for log. Can be NULL if not required.
			IN		loglength			- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_OUT( IN char master_key_string[],
						 IN char customer[],
						 IN char batch[],
						 IN char start_IMSI[],
						 IN uint32_t quantity,
						 IN char Type[],
						 IN char Profile[],
						 IN char Address[],
						 IN char Graph_ref[],
						 IN char MSISDN[],
						 IN char ICCID[],
						 IN char additional[],
						 IN char format_string[],
						 IN uint8_t encrypt,
						 IN uint32_t tran_key_num,
						 IN char transport_key_string[],
						 OUT char outstr[], IN uint32_t outstrlen,
						 OUT char unenc_outcsv[], IN uint32_t unenc_outcsvlen,
						 OUT char logstr[], IN uint32_t loglength)
{
	KAREVARA_Inst Main_Inst;

	if(logstr != NULL) {
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Create_OUT Start");
	}

	if((outstr == NULL) && (unenc_outcsv == NULL))
	{
		Error("KAREVARA_Create_OUT: Output strings are both NULL.");
		goto Error_Point;
	}

	/* Create the KAREVARA Instance */
	if(KAREVARA_Create_Inst(master_key_string,
		customer,
		batch,
		quantity,
		format_string,
		encrypt,
		transport_key_string,
		&Main_Inst,NULL,0))
	{
		Error("KAREVARA_Create_OUT: KAREVARA_Create_Inst failed.");
		goto Error_Point;
	}

	/* Add IMSI */
	if((start_IMSI != NULL) && (strlen(start_IMSI) > 0) && (strspn(start_IMSI,MASKNUM) == strlen(start_IMSI)))
	{
		if(Add_Inc_To_KAREVARA_Format(&(Main_Inst.Format_Inst),"IMSI",start_IMSI))
		{
			Error("KAREVARA_Create_OUT: Add_Inc_To_KAREVARA_Format failed.");
			goto Error_Point;
		}
	}

	/* Add MSISDN */
	if((MSISDN != NULL) && (strlen(MSISDN) > 0) && (strspn(MSISDN,MASKNUM) == strlen(MSISDN)))
	{
		if(Add_Inc_To_KAREVARA_Format(&(Main_Inst.Format_Inst),"MSISDN",MSISDN))
		{
			Error("KAREVARA_Create_OUT: Add_Inc_To_KAREVARA_Format failed.");
			goto Error_Point;
		}
	}

	/* Add ICCID */
	if((ICCID != NULL) && (strlen(ICCID) > 0)&& (strspn(ICCID,MASKNUM) == strlen(ICCID)))
	{
		if(Add_Inc_To_KAREVARA_Format(&(Main_Inst.Format_Inst),"ICCID",ICCID))
		{
			Error("KAREVARA_Create_OUT: Add_Inc_To_KAREVARA_Format failed.");
			goto Error_Point;
		}
	}

	/* Create the Header for this type of file */
	if(unenc_outcsv != NULL)
	{
		if(KAREVARA_CSV_Header(&Main_Inst,unenc_outcsv,unenc_outcsvlen))
		{
			Error("KAREVARA_Create_OUT: KAREVARA_CSV_Header failed.");
			goto Error_Point;
		}
	}

	/* Output Process header*/
	if(outstr != NULL)
	{
		/* Add Process Header */
		if(KAREVARA_Process_Header("OUT PROCESS DESCRIPTION",&Main_Inst,outstr,outstrlen))
		{
			Error("KAREVARA_Create_OUT: KAREVARA_Process_Header failed.");
			goto Error_Point;
		}

		/* Add the rest of the Header (INP file) */
		if(KAREVARA_Create_INP_fm_Inst(&Main_Inst,
						start_IMSI,
						Type,
						Profile,
						tran_key_num,
						Address,
						Graph_ref,
						MSISDN,
						ICCID,
						additional,
						outstr,outstrlen,NULL,0))
		{
			Error("KAREVARA_Create_OUT: KAREVARA_Create_INP failed.");
			goto Error_Point;
		}

		/* Next is the Data section */
		printheader(outstr,"DATA SECTION",outstrlen);
	}

	/* Get the data */
	if(KAREVARA(&Main_Inst,outstr,outstrlen," ",
						   unenc_outcsv,unenc_outcsvlen,",",NULL,0))
	{
		Error("KAREVARA_Create_OUT: KAREVARA_CSV_Header failed.");
		goto Error_Point;
	}

	/* Finish nicely */

	if(logstr != NULL) {
	LogHeader("KAREVARA_Create_OUT Complete");
	Close_Log();}
	return 0;

	/* Something went wrong -> cleanup and finish */
Error_Point:
	LogHeader("KAREVARA_Create_OUT Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}


/*****************************************************
KAREVARA_Create_OUT_fm_INP:
Creates a .OUT file from a .INP file using KAREVARA.

Input:		IN	master_key		- string containing master key (hex)
			IN	inpstr			- str containing INP file
			IN	transport_key_str - string containing transport key (hex) (optional)
			IN  trankey_csv_str	  - string containing transport key csv file (e.g 'tknum, tk_str') (optional)
			OUT	outstr			- pointer to string for .OUT file. Can be NULL if not required.
			IN maxoutlen		- max length of above string
			OUT	csvstr			- pointer to string for unencrypted csv file. Can be NULL if not required.
			IN maxcsvlen		- max length of above string
			OUT	logstr			- pointer to string for log (NULL if already setup)
			IN	loglength		- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_OUT_fm_INP(	IN char master_key[],
							IN char inpstr[],
							IN char transport_key_str[],
							IN char trankey_csv_str[],
							OUT char outstr[], IN uint32_t maxoutlen,
						    OUT char csvstr[], IN uint32_t maxcsvlen,
							char logstr[], uint32_t loglength)
{
	KAREVARA_Inst Main_Inst;

	/*Setup the Logs (if necessary)*/
	if(logstr != NULL) {
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Create_OUT_fm_INP Started");
	}

	if((outstr == NULL) && (csvstr == NULL))
	{
		Error("KAREVARA_Create_OUT_fm_INP: Both outputs are null.");
		goto Error_Point;
	}

	/* Create instance from INP file */
	KAREVARA_Create_Inst_fm_INP(&Main_Inst,
		master_key,
		inpstr,
		transport_key_str,
		trankey_csv_str,NULL,0);

	/* For unenc CSV file, Put CSV header */
	if(csvstr != NULL)
	{
		if(KAREVARA_CSV_Header(&Main_Inst,csvstr,maxcsvlen))
		{
			Error("KAREVARA_Create_OUT_fm_INP: KAREVARA_CSV_Header failed.");
			goto Error_Point;
		}
	}

	if(outstr != NULL)
	{
		/* For OUT file, Put process details at the top */
		/* Produce Process Info */
		if(KAREVARA_Process_Header("OUT PROCESS DESCRIPTION",&Main_Inst,
			outstr,maxoutlen))
		{
			Error("KAREVARA_Create_OUT_fm_INP: KAREVARA_Process_Header failed.");
			goto Error_Point;
		}

		/* then put the .inp file */
		strncat(outstr,inpstr,maxoutlen-strlen(outstr)-1);
		strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);

		/* next is the data section */
		printheader(outstr,"DATA SECTION",maxoutlen);
	}

	if(KAREVARA(&Main_Inst,
		outstr,maxoutlen," ",
		csvstr,maxcsvlen,",",NULL,0))
	{
		Error("KAREVARA_Create_OUT_fm_INP: KAREVARA failed.");
		goto Error_Point;
	}


	if(logstr != NULL) {
	LogHeader("KAREVARA_Create_OUT_fm_INP Successful");
	Close_Log();}
	return 0;

Error_Point:
	LogHeader("KAREVARA_Create_OUT_fm_INP Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
KAREVARA_Create_INP:
Function for creating an INP file. Does not use KAREVARA algorithm.

Input:		IN		master_key_string	- MK string. 32 hex characters.
			IN		customer			- string. Max 100 chars.
			IN		batch				- digit string. Max 7 digits.
			IN		start_IMSI			- 16 (max) digit string
			IN		quantity			- integer. Max 8 digits
			IN		type				- type string. Can be NULL
			IN		Profile				- profile string. Can be NULL
			IN		Address str			- can be NULL. Can be multiline.
			IN		Graph_ref			- string containing graph ref. Can be NULL
			IN		MSISDN				- string containing digits. Can be NULL
			IN		ICCID				- string containing digits. Can be NULL.
			IN		additional			- Any additional parameters required. Should be of form 'name : value'
			IN		format_string		- (/../) string specifying output format. Max length 10000.
			IN		encrypt				- transport enc algorithm (e.g. NULL=0, DES=1, TDES=2, AES=3).
			IN		transport_key_num	- transport key number
			OUT		outstr				- output pointer to write .INP string.
			IN		outstrlen			- max length of above string
			OUT		logstr				- pointer to string for log. Can be NULL if not required.
			IN		loglength			- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_INP( IN char master_key[],
						 IN char customer[],
						 IN char batch[],
						 IN char start_IMSI[],
						 IN uint32_t quantity,
						 IN char Type[],
						 IN char Profile[],
						 IN char Address[],
						 IN char Graph_ref[],
						 IN char MSISDN[],
						 IN char ICCID[],
						 IN char additional[],
						 IN char format_string[],
						 IN uint8_t encrypt,
						 IN uint32_t tran_key_num,
						 OUT char outstr[], IN uint32_t outstrlen,
						 OUT char logstr[], IN uint32_t loglength)
{
	KAREVARA_Inst Main_Inst;
	/*Setup the Logs (if necessary)*/
	if(logstr != NULL){
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Create_INP Started");
	}

	if(outstr == NULL)
	{
		Error("KAREVARA_Create_INP: Output str ptr is NULL.");
		goto Error_Point;
	}

	if(KAREVARA_Create_Inst(master_key,
		customer,
		batch,
		quantity,
		format_string,
		encrypt,
		"0000000000000000",
		&Main_Inst,NULL,0))
	{
		Error("KAREVARA_Create_INP: KAREVARA_Create_Inst Failed.");
		goto Error_Point;
	}

	/* Log what we've got */
	Log_KAREVARA(&Main_Inst);

	if(KAREVARA_Process_Header("INP PROCESS DESCRIPTION",&Main_Inst,
		outstr,outstrlen))
	{
		Error("KAREVARA_Create_INP: KAREVARA_Process_Header failed.");
		goto Error_Point;
	}

	if(KAREVARA_Create_INP_fm_Inst(&Main_Inst,
		start_IMSI,
		Type,
		Profile,
		tran_key_num,
		Address,
		Graph_ref,
		MSISDN,
		ICCID,
		additional,
		outstr,
		outstrlen,NULL,0))
	{
		Error("KAREVARA_Create_INP: KAREVARA_Create_INP_fm_Inst Failed.");
		goto Error_Point;
	}


	if(logstr != NULL)
	{
		LogHeader("KAREVARA_Create_INP Successful");
		Close_Log();
	}
	return 0;

Error_Point:
	LogHeader("KAREVARA_Create_INP Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
KAREVARA_Create_INP_fm_Inst:
Function for creating an INP file from a KAREVARA Inst. Does not use KAREVARA algorithm.

Input:		IN		Main_Inst			- KAREVARA Inst
			IN		start_IMSI			- start_IMSI
			IN		type				- type string. Can be NULL
			IN		Profile				- profile string. Can be NULL
			IN		transport_key_num	- transport key number
			IN		Address str			- can be NULL. Can be multiline.
			IN		Graph_ref			- string containing graph ref. Can be NULL
			IN		MSISDN				- string containing digits. Can be NULL
			IN		ICCID				- string containing digits. Can be NULL.
			IN		additional			- Any additional parameters (of form 'name : value'). Can be NULL.
			OUT		outstr				- output pointer to write .INP string.
			IN		maxoutlen			- max length of above string
			OUT		logstr				- pointer to string for log. Can be NULL if not required.
			IN		loglength			- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_INP_fm_Inst(KAREVARA_Inst *Main_Inst,
						IN char start_IMSI[],
						IN char Type[],
						IN char Profile[],
						IN uint32_t Tran_key_num,
						IN char Address[],
						IN char Graph_ref[],
						IN char MSISDN[],
						IN char ICCID[],
						IN char Additional_Str[],
						OUT char outstr[], IN uint32_t maxoutlen,
						char logstr[], uint32_t loglength)
{
	char str[1000];

	/*Setup the Logs (if necessary)*/
	if(logstr != NULL)
	{
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Create_INP_fm_Inst Started");
	}

	if(outstr == NULL)
	{
		Error("KAREVARA_Create_INP_fm_Inst: Output str ptr is NULL.");
		goto Error_Point;
	}

	if(Check_KAREVARA_Inst(Main_Inst))
	{
		Error("KAREVARA_Create_INP_fm_Inst: KAREVARA Instance not loaded.");
		goto Error_Point;
	}

	/* Add IMSI */
	if((start_IMSI != NULL) && (strlen(start_IMSI) > 0) && (strspn(start_IMSI,MASKNUM) == strlen(start_IMSI)))
	{
		if(Add_Inc_To_KAREVARA_Format(&(Main_Inst->Format_Inst),"IMSI",start_IMSI))
		{
			Error("KAREVARA_Create_OUT: Add_Inc_To_KAREVARA_Format failed.");
			goto Error_Point;
		}
	}

	/* Add MSISDN to INST */
	if((MSISDN != NULL) && (strlen(MSISDN) > 0) && (strspn(MSISDN,MASKNUM) == strlen(MSISDN)))
	{
		if(Add_Inc_To_KAREVARA_Format(&(Main_Inst->Format_Inst),"MSISDN",MSISDN))
		{
			Error("KAREVARA_Create_OUT: Add_Inc_To_KAREVARA_Format failed.");
			goto Error_Point;
		}
	}

	/* Add ICCID to INST  */
	if((ICCID != NULL) && (strlen(ICCID) > 0)&& (strspn(ICCID,MASKNUM) == strlen(ICCID)))
	{
		if(Add_Inc_To_KAREVARA_Format(&(Main_Inst->Format_Inst),"ICCID",ICCID))
		{
			Error("KAREVARA_Create_OUT: Add_Inc_To_KAREVARA_Format failed.");
			goto Error_Point;
		}
	}

	/* Produce Process Info */
	/* Only do so if no one else has started using this string */
	if(strlen(outstr) == 0) {
		KAREVARA_Process_Header("INP PROCESS DESCRIPTION",Main_Inst,outstr,maxoutlen);
	}

	/* Produce Header */
	printheader(outstr,"HEADER DESCRIPTION",maxoutlen);

	sprintf(str,"CUSTOMER:        %s\n",Main_Inst->customer);
	strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	sprintf(str,"QUANTITY:        %05d\n",Main_Inst->quantity);
	strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	if((Type != NULL) && (strlen(Type) > 0))
	{
		sprintf(str,"TYPE:            %s\n",Type);
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	}
	if((Profile != NULL) && (strlen(Profile) > 0))
	{
		sprintf(str,"PROFILE:         %s\n",Profile);
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	}
	sprintf(str,"BATCH:           %s\n",Main_Inst->batch);
	strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);
	/* Transport Encryption */
	if(Main_Inst->Encrypt_Inst.type != NO_ENCRYPT)
	{
		if((Tran_key_num < 99999))
		{
			sprintf(str,"TRANSPORT_KEY:   %03d\n",Tran_key_num);
			strncat(outstr,str,maxoutlen-strlen(outstr)-1);
		}
		if(Main_Inst->Encrypt_Inst.type == ENCRYPT_DES){
			strncat(outstr,"TRANSPORT_ALG:   DES\n",maxoutlen-strlen(outstr)-1);
		}
		else if(Main_Inst->Encrypt_Inst.type == ENCRYPT_TDES){
			strncat(outstr,"TRANSPORT_ALG:   TDES\n",maxoutlen-strlen(outstr)-1);
		}
		else if(Main_Inst->Encrypt_Inst.type == ENCRYPT_AES) {
			strncat(outstr,"TRANSPORT_ALG:   AES\n",maxoutlen-strlen(outstr)-1);
		}
	}
	else
	{
		sprintf(str,"TRANSPORT_ALG:   NO_TRANSPORT_ENCRYPTION\n");
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	}
	/* For Address, put each line as own field */
	if((Address != NULL) && (strlen(Address) > 0))
	{
		char *add = malloc(strlen(Address)+2);
		char *line = NULL;
		uint32_t i = 1;
		if(add == NULL)
		{
			Error("KAREVARA_Create_INP_fm_Inst: Cannot allocate data.");
			goto Error_Point;
		}
		strcpy(add,Address);
		strcat(Address,"\n");
		line = strtok(add,"\n");
		while((line != NULL) && (i<10) && strlen(line)>0)
		{
			sprintf(str,"ADDRESS%1d:        %s\n",i,line);
			strncat(outstr,str,maxoutlen-strlen(outstr)-1);
			line = strtok(NULL,"\n");
			i++;
		}
		free(add);
		strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);
	}

	/* Graph_ref */
	if((Graph_ref != NULL) && (strlen(Graph_ref) > 0))
	{
		sprintf(str,"GRAPH_REF:       %s\n",Graph_ref);
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
		strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);
	}

	/* Are there further parameters we would like to add?*/
	if((Additional_Str != NULL) && (strlen(Additional_Str) > 0))
	{
		char *add = malloc(strlen(Additional_Str)+2);
		char *line = NULL;
		if(add == NULL)
		{
			Error("KAREVARA_Create_INP_fm_Inst: Cannot allocate data.");
			goto Error_Point;
		}
		strcpy(add,Additional_Str);
		strcat(Additional_Str,"\n");
		line = strtok(add,"\n");
		while(line != NULL)
		{
			char name[101], detail[1001];
			if(sscanf(line,"%100[^:]:%1000s",
					name,
					detail) == 2)
			{
					strncat(outstr,line,maxoutlen-strlen(outstr)-1);
					strncat(outstr,"\n",maxoutlen-strlen(outstr)-1);
			}
			line = strtok(NULL,"\n");
		}
		free(add);
		strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);
	}

	/* Sort out next part of header */
	printheader(outstr,"INPUT VARIABLES DESCRIPTION",maxoutlen);
	strncat(outstr,"Var_In_List:\n",maxoutlen-strlen(outstr)-1);

	/* MSISDN */
	if((MSISDN != NULL) && (strlen(MSISDN) > 0) && (strspn(MSISDN,MASKNUM) == strlen(MSISDN)))
	{
		sprintf(str,"MSISDN:         %s\n",MSISDN);
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	}

	/*IMSI */
	if((start_IMSI != NULL) && (strlen(start_IMSI) > 0) && (strspn(start_IMSI,MASKNUM) == strlen(start_IMSI)))
	{
		sprintf(str,"IMSI:           %s\n",start_IMSI);
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	}


	/* ICCID */
	if((ICCID != NULL) && (strlen(ICCID) > 0)&& (strspn(ICCID,MASKNUM) == strlen(ICCID)))
	{
		sprintf(str,"Ser_nb:         %s\n",ICCID);
		strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	}
	strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);

	/* Produce Var_Out */
	str[0] = '\0';
	printheader(outstr,"OUTPUT VARIABLES DESCRIPTION",maxoutlen);
	if(KAREVARA_VAR_OUT(&(Main_Inst->Format_Inst), str, MAX_FORMAT_STRING_LEN))
	{
		Error("KAREVARA_Create_INP_fm_Inst: KAREVARA_VAR_OUT failed.");
		goto Error_Point;
	}
	strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	str[0] = '\0';
	if(KAREVARA_FORMAT_OUT(&(Main_Inst->Format_Inst), str, MAX_FORMAT_STRING_LEN))
	{
		Error("KAREVARA_Create_INP_fm_Inst: KAREVARA_VAR_OUT failed.");
		goto Error_Point;
	}
	strncat(outstr,str,maxoutlen-strlen(outstr)-1);
	strncat(outstr,"*\n",maxoutlen-strlen(outstr)-1);

	if(logstr != NULL) {
		LogHeader("KAREVARA_Create_INP_fm_Inst Successful");
		Close_Log();
	}
	return 0;

Error_Point:
	LogHeader("KAREVARA_Create_INP_fm_Inst Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
KAREVARA_Load_INP:
Loads the input parameters from a string containing a .INP file.

Input:		IN		inpstr			- input string containing INP file to load details
			OUT		customer		- string. Max 100 characters.
			OUT		batch			- string. Max 7 characters.
			OUT		start_IMSI		- string. Max 16 characters.
			OUT		quantity		- integer. Max 8 digits (99999999).
			OUT		type			- string. Max 8 characters.
			OUT		Profile			- string. Max 8 characters.
			OUT		Address str		- string. Max 1000 characters.
			OUT		Graph_ref		- string. Max 8 characters
			OUT		MSISDN			- string containing digits. Max 15 characters
			OUT		ICCID			- string containing digitis. Max 20 characters
			OUT		format_string	- string specifying output format. Max 10000 characters.
			OUT		encrypt			- integer. Algorithm used for encryption (0=NULL,1=DES,2=TDES,3=AES)
			OUT		transport_key_num - integer. transport key number
			OUT		additional		- output string for output of any additional parameters. Can be NULL if not required.
			IN		additionallen	- maximum length of above string.
			OUT		logstr			- string for log. Can be NULL if not required.
			IN		loglen			- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Load_INP(IN char inpstr[],
					OUT char customer[],
					OUT char batch[],
					OUT char start_IMSI[],
					OUT uint32_t *quantity,
					OUT char type[],
					OUT char profile[],
					OUT char address[],
					OUT char graph_ref[],
					OUT char MSISDN[],
					OUT char ICCID[],
					OUT char format_string[],
					OUT uint8_t *encrypt,
					OUT uint32_t *tran_key_num,
					OUT char additional[],
					IN uint32_t additionallen,
					OUT char logstr[],
					IN uint32_t loglength)
{
	uint32_t i = 0;
	char name[101] = "", detail[1001] = "", tempstr[1100] = "";
	char *str2;
	char *line;
	KAREVARA_Format fmt;

	/*Setup the Logs (if necessary)*/
	if(logstr != NULL)
	{
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Load_INP Started");
	}


	Init_KAREVARA_Format(&fmt);

	/* Copy inpfile (strtok will modify) */
	str2 = (char *) malloc(strlen(inpstr)+1);
	if(str2 == NULL)
	{
		Error("KAREVARA_Load_INP: Cannot allocate data.");
		goto Cleanup;
	}
	strncpy(str2,inpstr,strlen(inpstr)+1);

	/* First I'm going to look for the format string */
	line = strtok(str2,"\n");
	while(line != NULL)
	{
		/* Check string before processing */
		if(sscanf(line,"%100[^:]:%1000s",name,detail) == 2)
		{
			if((strlen(detail) > 0) && (strlen(name) > 0))
			{
				trimstr(name,MASKALPHANUM);
				toupperstr(name);
				/* Load Format string */
				if(!strcmp(name,"FORMAT_OUT") && (format_string != NULL))
				{
					/* Remove whitespace */
					trimcstr(detail,MASKWHITESPACE);
					/* Add slashes */
					addslash(detail,1000);
					/* Load */
					if(String_To_KAREVARA_Format(detail,&fmt))
					{
						Error("KAREVARA_Load_INP: String_To_KAREVARA_Format failed.");
						return 1;
					}
					/* Copy */
					if(fmt.num_rec == 0)
					{
						Error("KAREVARA_Load_INP: Format not found.");
						return 1;
					}
					Log("Loaded Format String: %s",detail);
				}
				/* Load VAR_OUT (if we haven't got format string) */
				else if((!strcmp(name,"VAR_OUT")) && (format_string != NULL) && (fmt.num_rec == 0))
				{
					/* Remove whitespace */
					trimcstr(detail,MASKWHITESPACE);
					/* Add slashes */
					addslash(detail,1000);
					/* Load */
					if(String_To_KAREVARA_Format(detail,&fmt))
					{
						Error("KAREVARA_Load_INP: String_To_KAREVARA_Format failed.");
						return 1;
					}
					/* Copy */
					if(fmt.num_rec == 0)
					{
						Error("KAREVARA_Load_INP: Format not found.");
						return 1;
					}
					/* Log */
					Log("Loaded Format String: %s",detail);
				}
			}
		}
		line = strtok(NULL,"\n");
	}

	/* Start again */
	strncpy(str2,inpstr,strlen(inpstr)+1);

	/* Look for other parameters */
	line = strtok(str2,"\n");
	while(line != NULL)
	{
		/* Check string before processing */
		if(sscanf(line,"%100[^:]:%1000s",name,detail) == 2)
		{
			if((strlen(detail) > 0) && (strlen(name) > 0))
			{
				trimstr(name,MASKALPHANUM);
				toupperstr(name);
				/* Load Customer */
				if(!strcmp(name,"CUSTOMER") && (customer != NULL))
				{
					trimstr(detail,MASKALPHANUM);
					strncpy(customer,detail,CUS_LEN+1);
					/* Log */
					Log("Loaded Customer: %s",customer);
				}
				/* Batch Quantity */
				else if((!strcmp(name, "QUANTITY") || !strcmp(name, "SIZE") ||
					!strcmp(name, "BATCH_SIZE")) && (quantity != NULL))
				{
					trimstr(detail,MASKNUM);
					*quantity = (uint32_t) atoi(detail);
					Log("Loaded Quantity: %d",*quantity);
				}
				/* Load Identifier for batch */
				else if((!strcmp(name, "BATCH") || !strcmp(name, "BATCH_NO")) && (batch != NULL))
				{
					trimstr(detail,MASKNUM);
					strncpy(batch,detail,BATCH_LEN+1);
					/* Log */
					Log("Loaded Batch: %s",batch);
				}
				/* IMSI */
				else if((!strcmp(name, "IMSI") || !strcmp(name, "START_IMSI") ||
					!strcmp(name, "STARTIMSI") || !strcmp(name, "SIMSI") ) && (start_IMSI != NULL))
				{
					trimstr(detail,MASKNUM);
					strncpy(start_IMSI,detail,MAX_IMSI_LEN+1);
					/* Add this detail to the format record */
					if(fmt.num_rec > 0)
					{
						if(Add_Inc_To_KAREVARA_Format(&fmt,"IMSI",start_IMSI))
						{
							Error("KAREVARA_Load_INP: Add_Inc_To_KAREVARA_Format failed.");
							return 1;
						}
					}
					/* Log */
					Log("Loaded IMSI: %s",start_IMSI);
				}
				else if((!strcmp(name, "TRANSPORT_ENCRYPTION_ALG") || !strcmp(name, "TRANSPORT_ENCRYPTION")
					|| !strcmp(name, "TRANSPORT_ALG") || !strcmp(name, "TRANSPORT_ALGORITHM"))
					&& (encrypt != NULL))
				{
					trimstr(detail,MASKALPHANUM);
					if(!strcmp(detail,"DES"))
					{
						*encrypt = ENCRYPT_DES;
						Log("Loaded Encryption: DES");
					}
					else if(!strcmp(detail,"TDES"))
					{
						*encrypt = ENCRYPT_TDES;
						Log("Loaded Encryption: TDES");
					}
					else if(!strcmp(detail,"AES"))
					{
						*encrypt = ENCRYPT_AES;
						Log("Loaded Encryption: AES");
					}
					else
					{
						Warning("KAREVARA_Load_INP: Do not recognise encryption algorithm.");
					}
				}
				/* Transport key number */
				else if((!strcmp(name, "TRANSPORT_KEY_NUM") || !strcmp(name, "TRANSPORT_KEY")) && (tran_key_num != NULL))
				{
					trimstr(detail,MASKNUM);
					*tran_key_num = atoi(detail);
					Log("Loaded Transport Key Num: %d",*tran_key_num);
				}
				else if((!strcmp(name, "ICCID") || !strcmp(name, "SER_NB")) && (ICCID != NULL))
				{
					trimstr(detail,MASKNUM);
					strncpy(ICCID,detail,ICCID_LEN+1);
					/* Add this detail to the format record */
					if(fmt.num_rec > 0)
					{
						if(Add_Inc_To_KAREVARA_Format(&fmt,"ICCID",ICCID))
						{
							Error("KAREVARA_Load_INP: Add_Inc_To_KAREVARA_Format failed.");
							return 1;
						}
					}
					/* Log */
					Log("Loaded ICCID: %s",ICCID);
				}
				/* MSISDN */
				else if((!strcmp(name, "MSISDN"))  && (MSISDN != NULL))
				{
					trimstr(detail,MASKNUM);
					strncpy(MSISDN,detail,MSISDN_LEN+1);
					/* Add this detail to the format record */
					if(fmt.num_rec > 0)
					{
						if(Add_Inc_To_KAREVARA_Format(&fmt,"MSISDN",MSISDN))
						{
							Error("KAREVARA_Load_INP: Add_Inc_To_KAREVARA_Format failed.");
							return 1;
						}
					}
					/* Log */
					Log("Loaded MSISDN: %s",MSISDN);
				}
				/* Type */
				else if((!strcmp(name,"TYPE"))  && (type != NULL))
				{
					trimstr(detail,MASKALPHANUM);
					strncpy(type,detail,TYPE_LEN+1);
					/* Log */
					Log("Loaded Type: %s",type);
				}
				/* Profile */
				else if((!strcmp(name,"PROFILE")) && (profile != NULL))
				{
					trimstr(detail,MASKALPHANUM);
					strncpy(profile,detail,PROFILE_LEN+1);
					/* Log */
					Log("Loaded Profile: %s",profile);
				}
				/* graph_ref */
				else if((!strcmp(name,"GRAPH_REF") || (!strcmp(name,"GRAPH"))) && (profile != NULL))
				{
					trimstr(detail,MASKALPHANUM);
					strncpy(graph_ref,detail,GRAPH_REF_LEN+1);
					/* Log */
					Log("Loaded Graph_Ref: %s",graph_ref);
				}
				/* Does it contain address (can happen more than once) */
				else if((strstr(name,"ADDRESS") != NULL) && (address != NULL))
				{
					trimstr(detail,MASKALPHANUM);
					strncat(address,detail,ADDRESS_LEN-strlen(address)+1);
					/* Log */
					Log("Loaded Address: %s",detail);
				}

				/* Generation Algorithm should be KAREVARA */
				else if(!strcmp(name, "GEN_ALGORITHM"))
				{
					trimstr(detail,MASKALPHANUM);
					if(strcmp(detail,"KAREVARA"))
					{
						Error("KAREVARA_Load_INP: ALGORITHM_ID not 'KAREVARA'.");
					}
				}

				/* Do nothing in this case */
				else if(!strcmp(name, "JOB_ID") ||
					!strcmp(name, "TIME_ID") ||
					!strcmp(name, "VAR_OUT") ||
					!strcmp(name, "FORMAT_OUT"))
				{

				}

				/* Store anything else */
				else if(additional != NULL)
				{
					sprintf(tempstr,"%s: %s\n",name,detail);;
					strncat(additional,tempstr,additionallen-strlen(additional));
				}
			}
		}
		line = strtok(NULL,"\n");
	}
	/* Produce format string */
	if(fmt.num_rec > 0)
	{
		if(KAREVARA_Format_To_String(&fmt,format_string,MAX_FORMAT_STRING_LEN))
		{
			Error("KAREVARA_Load_INP: KAREVARA_Format_To_String failed.");
			return 1;
		}
	}

	if(str2 != NULL) free(str2);

	/* Extract Log (if required) */
	if(logstr != NULL)
	{
		LogHeader("KAREVARA_Load_INP Complete");
		Close_Log();
	}
	return 0;

Cleanup:
	if(str2 != NULL) free(str2);
	LogHeader("KAREVARA_Load_INP Failed");
	/* Extract Log (if required) */
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
KAREVARA_Create_Inst_fm_INP:
Loads the Instance from a string containing a .INP file.

Input:		OUT Main_Inst			- Created KAREVARA Instance
			IN	master_key			- string containing master key (hex)
			IN	inpstr				- str containing INP file
			IN	transport_key_str	- string containing transport key (hex) (optional)
			IN  trankey_csv_str		- string containing transport key csv file (e.g 'tknum, tk_str') (optional)
			OUT	logstr				- string to setup log. Can be NULL if not required
			IN	loglength			- max length of above string.

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_Inst_fm_INP(OUT KAREVARA_Inst *Main_Inst,
								IN char master_key[],
								IN char inpstr[],
								IN char transport_key_str[],
								IN char trankey_csv_str[],
								char logstr[], uint32_t loglength)
{
	uint32_t i = 0;
	char name[101] = "", detail[1001] = "", tempstr[1100] = "";
	char customer[CUS_LEN+1] = "";
	char batch[BATCH_LEN+1] = "";
	char ICCID[ICCID_LEN+1] = "";
	char MSISDN[MSISDN_LEN+1] = "";
	uint32_t quantity = 0;
	char format_string[MAX_FORMAT_STRING_LEN+1] = "";
	uint8_t encrypt = NO_ENCRYPT;
	uint32_t tran_key_num;
	char transport_key_string[MAX_TRANS_KEY_LEN+1] = "";

	/*Setup the Logs (if necessary)*/
	if(logstr != NULL) {Start_Log(logstr,loglength);
	LogHeader("KAREVARA_Create_Inst_fm_INP Started");}

	if(transport_key_str != NULL) strncpy(transport_key_string,transport_key_str,MAX_TRANS_KEY_LEN);

	/* Load the file */
	if(KAREVARA_Load_INP(inpstr,customer,batch,NULL,&quantity,NULL,NULL,NULL,NULL,MSISDN,ICCID,
		format_string,&encrypt,&tran_key_num,NULL,0,NULL,0))
	{
		Error("KAREVARA_Create_Inst_fm_INP: KAREVARA_Load_INP failed.");
		goto Cleanup;
	}

	Get_Tran_Key_fm_CSV(transport_key_string,tran_key_num,trankey_csv_str,NULL,0);

	/* Create the Instance */
	if(KAREVARA_Create_Inst(master_key,
		customer,
		batch,
		quantity,
		format_string,
		encrypt,
		transport_key_string,
		Main_Inst,
		NULL,0))
	{
		Warning("KAREVARA_Create_Inst_fm_INP: KAREVARA_Create_Inst failed.");
		goto Cleanup;
	}


	/* Extract Log (if required) */
	if(logstr != NULL) {
		LogHeader("KAREVARA_Create_Inst_fm_INP Complete");
		Close_Log();
	}
	return 0;

Cleanup:
	LogHeader("KAREVARA_Create_Inst_fm_INP Failed");
	/* Extract Log (if required) */
	if(logstr != NULL) Close_Log();
	return 1;
}



/*****************************************************
KAREVARA_Create_CSV:
Generates CSV strings using KAREVARA. Both encrypted and unencrypted file formats will be CSV format.

Input:		IN		master_key_string	- MK string. 32 character. Can be NULL.
			IN		customer			- generator string. 3 chars.
			IN		batch				- string. Max 7 digits.
			IN		quantity			- integer. Max 8 digits
			IN		format_string		- string specifying output fields. Max length 10000.
			IN		encrypt				- algorithm used for encryption (e.g. ENCRYPT_DES)
			IN		transport_key_string - transport key char string
			OUT		outcsv				- csv output string (transport enc). Can be NULL if not required.
			IN		outcsvlen			- max length of above string
			OUT		unencoutcsv			- csv output string (unencrypted). Can be NULL if not required.
			IN		unencoutcsvlen		- max length of above string
			OUT		logstr				- string to setup log. Can be NULL.
			IN		loglength			- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_CSV ( IN char master_key_string[],
						 IN char customer[],
						 IN char batch[],
						 IN uint32_t quantity,
						 IN char format_string[],
						 IN uint8_t encrypt,
						 IN char transport_key_string[],
						 OUT char outcsv[], IN uint32_t outcsvlen,
						 OUT char unencoutcsv[], IN uint32_t unencoutcsvlen,
						 OUT char logstr[], IN uint32_t loglength)
{
	KAREVARA_Inst Main_Inst;

	if(logstr != NULL) {Start_Log(logstr,loglength);
	LogHeader("KAREVARA_Create_CSV Start");}

	if(outcsv == NULL && unencoutcsv == NULL)
	{
		Error("KAREVARA_Create_CSV: Output strings are both NULL.");
		goto Error_Point;
	}

	/* Create the KAREVARA Instance */
	if(KAREVARA_Create_Inst(master_key_string,
		customer,
		batch,
		quantity,
		format_string,
		encrypt,
		transport_key_string,
		&Main_Inst,
		NULL,
		0))
	{
		Error("KAREVARA_Create_CSV: KAREVARA_Create_Inst failed.");
		goto Error_Point;
	}

	/* Create the Header for this type of file */
	if(outcsv != NULL)
	{
		if(KAREVARA_CSV_Header(&Main_Inst,outcsv,outcsvlen))
		{
			Error("KAREVARA_Create_CSV: KAREVARA_CSV_Header failed.");
			goto Error_Point;
		}
	}

	if(unencoutcsv != NULL)
	{
		if(KAREVARA_CSV_Header(&Main_Inst,unencoutcsv,unencoutcsvlen))
		{
			Error("KAREVARA_Create_CSV: KAREVARA_CSV_Header failed.");
			goto Error_Point;
		}
	}

	/* Get the data */
	if(KAREVARA(&Main_Inst,outcsv,outcsvlen,",",unencoutcsv,unencoutcsvlen,",",NULL,0))
	{
		Error("KAREVARA_Create_CSV: KAREVARA_CSV_Header failed.");
		goto Error_Point;
	}

	/* Finish nicely */

	if(logstr != NULL) {
		LogHeader("KAREVARA_Create_CSV Complete");
		Close_Log();
	}
	return 0;

	/* Something went wrong -> cleanup and finish */
Error_Point:
	LogHeader("KAREVARA_Create_CSV Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
KAREVARA_Create_CSV_fm_INP:
Creates a .CSV file from a .INP file using KAREVARA.

Input:		IN	master_key			- string containing master key (hex)
			IN	inpstr				- input str containing INP file
			IN	transport_key_str	- string containing transport key (hex) (optional)
			IN  trankey_csv_str		- string containing transport key csv file (e.g 'tknum, tk_str') optional
			OUT	outcsv				- string for (tranpsort encrypted) csv file. Can be NULL if not required.
			IN  maxoutlen			- max length of above string
			OUT	unencoutcsv			- string for unencrypted csv file. Can be NULL if not required.
			IN  maxcsvlen			- max length of above string
			OUT	logstr				- string for log (NULL if already setup)
			IN	loglength			- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_CSV_fm_INP ( IN char master_key[],
							IN char inpstr[],
							IN char transport_key_str[],
							IN char trankey_csv_str[],
							OUT char outcsv[], IN uint32_t outcsvlen,
						    OUT char unencoutcsv[], IN uint32_t unencoutcsvlen,
							char logstr[], uint32_t loglength)
{
	KAREVARA_Inst Main_Inst;

	/*Setup the Logs (if necessary)*/
	if(logstr != NULL) {
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Create_CSV_fm_INP Started");
	}

	if((outcsv == NULL) && (unencoutcsv == NULL))
	{
		Error("KAREVARA_Create_CSV_fm_INP: Both outputs are null.");
		goto Error_Point;
	}

	/* Create instance from INP file */
	KAREVARA_Create_Inst_fm_INP(&Main_Inst,master_key,inpstr,
		transport_key_str,trankey_csv_str,NULL,0);


	/* Create the Header for this type of file */
	if(outcsv != NULL)
	{
		if(KAREVARA_CSV_Header(&Main_Inst,outcsv,outcsvlen))
		{
			Error("KAREVARA_Create_CSV_fm_INP: KAREVARA_CSV_Header failed.");
			goto Error_Point;
		}
	}

	if(unencoutcsv != NULL)
	{
		if(KAREVARA_CSV_Header(&Main_Inst,unencoutcsv,unencoutcsvlen))
		{
			Error("KAREVARA_Create_CSV_fm_INP: KAREVARA_CSV_Header failed.");
			goto Error_Point;
		}
	}

	/* Get the data */
	if(KAREVARA(&Main_Inst,outcsv,outcsvlen,",",unencoutcsv,unencoutcsvlen,",",NULL,0))
	{
		Error("KAREVARA_Create_CSV_fm_INP: KAREVARA_CSV_Header failed.");
		goto Error_Point;
	}

	/* Finish nicely */

	if(logstr != NULL) {
		LogHeader("KAREVARA_Create_CSV_fm_INP Complete");
		Close_Log();
	}
	return 0;

	/* Something went wrong -> cleanup and finish */
Error_Point:
	LogHeader("KAREVARA_Create_CSV Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
KAREVARA_CSV_Header
Creates a Header for a CSV file from a KAREVARA Instance

Input:		IN KAREVARA_Inst  - input instance
			OUT	out			  - string to output CSV header
			IN	outlen		  - max length of output.

Return: 	0 Success
			1 Failure
*****************************************************/
static int KAREVARA_CSV_Header(IN KAREVARA_Inst *Main_Inst,
					OUT char out[], IN uint32_t outlen)
{
	char str[1000];
	sprintf(str,"JOB_ID: %s, TIME_ID: %s\n",Main_Inst->job,Main_Inst->time);
	strncat(out,str,outlen);
	if(KAREVARA_CSV_OUT(&(Main_Inst->Format_Inst),out,outlen))
	{
		return 1;
	}
	strcat(out,"\n");
	return 0;
}

/*****************************************************
KAREVARA_Process_Header
Creates a 'Process' Header for a .OUT file containing details of the processing (time, jobid)

Input:		IN char[]		  - string containing name of Process to go in header
			IN KAREVARA_Inst  - input instance
			OUT	out			  - string to output header
			IN	outlen		  - max length of output.

Return: 	0 Success
			1 Failure
*****************************************************/
static int KAREVARA_Process_Header(IN char instr[], IN KAREVARA_Inst *Main_Inst,
					OUT char outstr[], IN uint32_t outlen)
{
	/* Produce Process Info */
	if(outlen > strlen(instr) + strlen(Main_Inst->time)
		+ 40 + strlen(Main_Inst->job)
		+ strlen(TOPHEADER) + strlen(BOTHEADER))
	{
		char str[100];
		printheader(outstr,instr,outlen);
		sprintf(str,"JOB_ID:          %s\n",Main_Inst->job);
		strncat(outstr,str,outlen);
		sprintf(str,"TIME_ID:         %s",Main_Inst->time);
		strncat(outstr,str,outlen);
		strncat(outstr,"*\n",outlen);
		return 0;
	}
	return 1;
}

/*****************************************************
Get_Tran_Key_fm_CSV
Extracts a transport key for a CSV file of the form:

tran_key_num, transport_key_string
001, 0123456789ABCDEF
002, 0011223344556677
etc.

Input:		OUT		tran_key		- If non-empty function will immediately return successful.
										Otherwise will be added if found.
			IN		tran_key_num	- key number to look for in CSV
			IN		trancsv			- CSV file of the above form
			OUT		logstr			- string for log. Should be NULL if log is already setup.
			IN		loglength		- max length of above string

Return: 	0 Success
			1 Failure
*****************************************************/
int Get_Tran_Key_fm_CSV(OUT char tran_key[],
						IN uint32_t tran_key_num,
						IN char trancsv[],
						OUT char logstr[], IN uint32_t loglength)
{
	/*Setup the Logs (if necessary)*/
	if(logstr != NULL)
	{
		Start_Log(logstr,loglength);
		LogHeader("Get_Tran_Key_fm_CSV Started");
	}

	if(tran_key == NULL)
	{
		Warning("Get_Tran_Key_fm_CSV: tran_key NULL.");
		goto Error_Point;
	}

	if(strlen(tran_key) > 0)
	{
		/* We already have it. Do nothing.*/
		return 0;
	}
	/* Otherwise need to extract from CSV */
	if((trancsv != NULL) && (strlen(trancsv) > 0))
	{
		char tempstr[100];
		if(Read_TK_from_CSV(trancsv,tran_key_num,tran_key))
		{
			Warning("Get_Tran_Key_fm_CSV: Read_TK_from_CSV failed.");
			goto Error_Point;
		}
		sprintf(tempstr,"Loaded Transport Key Num: %d\n",tran_key_num);
		Log(tempstr);
	}
	/* already have tran_key */

	/* Extract Log (if required) */
	if(logstr != NULL) {
		LogHeader("Get_Tran_Key_fm_CSV Complete");
		Close_Log();
	}
	return 0;

Error_Point:
	LogHeader("Get_Tran_Key_fm_CSV Failed");
	/* Extract Log (if required) */
	if(logstr != NULL) Close_Log();
	return 1;
}

/*****************************************************
Read_TK_from_CSV_File:
Obtains an encryption key from a CSV file
File should be of form:

tran_key_num, tran_key_str
001,0123456789abcdef
002,1123456789abcdef

Input:		IN		instr - input string of CSV file
			IN		tran_key_num transport key number to look for
			OUT		tran_key_str - string for the key to go in

Return: 	0 Success
			1 Failure
*****************************************************/
static int Read_TK_from_CSV(IN char instr[],
					 IN int32_t tran_key_num,
					 OUT char tran_key_str[])
{
	char key[101] = "";
	char *line = NULL;
	int32_t num = -1;
	char *str = NULL;

	/* check input (don't need to check infile, openfile does that */
	if(tran_key_str == NULL || instr == NULL)
	{
		Error("Read_TK_from_CSV: NULL input.");
		return 1;
	}

	str = (char *) malloc(strlen(instr)+1);
	if(str == NULL)
	{
		Error("Read_TK_from_CSV: Cannot allocate data.");
		return 1;
	}
	/* Copy */
	strncpy(str,instr,strlen(instr)+1);

	line = strtok(str,"\n");
	while(line != NULL)
	{
		/* Get the data */
		num = -1;
		key[0] = '\0';
		if(sscanf(line,"%d,%100s",&num,key) == 2)
		{
			if((num >= 0) && (strlen(key) > 0))
			{
				if(num == tran_key_num)
				{
					/* trim non hex characters */
					trimstr(key,MASKHEX);
					/* Check length */
					if(strlen(key) <= MAX_TRANS_KEY_LEN)
					{
						char temp[100];
						/* Return the key */
						sprintf(tran_key_str,"%s",key);
						/* Log */
						sprintf(temp,"Reading transport key '%d'.",tran_key_num);
						Log(temp);
					}
					else
					{
						Error("Read_TK_from_CSV: Key too long to be read.");
						free(str);
						return 1;
					}
				}
			}
		}
		/* Get next string */
		line = strtok(NULL,"\n");
	}
	free(str);
	if(strlen(tran_key_str) == 0)
	{
		Warning("Read_TK_from_CSV: Can't find Transport Key.");
		return 1;
	}
	return 0;
}
