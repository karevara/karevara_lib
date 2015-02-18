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

#include "KAREVARA.h"


/*****************************************************
KAREVARA:
Main Function for KAREVARA. Takes inputs, calculates data requirement, produces data, formats (and optionally encrypts) data,
then outputs to a string. Can also produce an unencrypted version of the string. Log wrapper for KAREVARA_Run_Output

Input:		IN	KAREVARA_Inst	- KAREVARA Input Instance
			OUT res				- string to put data into. Can be NULL if not required.
			IN	reslength		- max length of res
			IN	resdelim		- delimiting string between fields of res (e.g. "," or " ")
			OUT	unencres		- string to put unencrypted data into. Can be NULL if not required.
			IN	unencreslength	- max length of unencres
			IN	unencresdelim	- delimiting string between fields of unencres
			OUT logstr			- string containing log. If log already setup, set to NULL.
			IN	loglength		- maximum length of logstr.

Return: 	0 Success
			1 Failure
*****************************************************/

int KAREVARA (IN KAREVARA_Inst *Main_Inst,
		OUT char res[],
		IN uint32_t reslength,
		IN char resdelim[],
		OUT char unencres[],
		IN uint32_t unencreslength,
		IN char unencresdelim[],
		OUT char logstr[],
		IN uint32_t loglength)
{
	/*Setup the Logs*/
	if(logstr != NULL) {
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA Start");
	}

	if((res == NULL) && (unencres == NULL))
	{
		Error("KAREVARA: Outputs are NULL.");
		goto Error_Point;
	}

	if(Check_KAREVARA_Inst(Main_Inst))
	{
		Error("KAREVARA: KAREVARA_Inst not initialised.");
		goto Error_Point;
	}

	/* Before running, Log what we've got */
	Log_KAREVARA(Main_Inst);

	/*Run & Output*/
	if(KAREVARA_Run_Output(Main_Inst,
		res,
		reslength,
		resdelim,
		unencres,
		unencreslength,
		unencresdelim))
	{
		Error("KAREVARA: KAREVARA_Run_Output failed.");
		goto Error_Point;
	}

	/* Finish nicely */

	if(logstr != NULL) {
	LogHeader("KAREVARA Complete");
	Close_Log();}
	return 0;

	/* Something went wrong -> cleanup and finish */
Error_Point:
	LogHeader("KAREVARA Failed");
	if(logstr != NULL) Close_Log();
	return 1;

}

/*****************************************************
KAREVARA_Create_Inst:
Loads the input parameters

Input:		IN		master_key		- master key string. 32 Hex characters.
			IN		customer		- generator string. Max 100 characters.
			IN		batch			- string. Max 7 digits
			IN		quantity		- integer. Max 8 digits.
			IN		format_string	- string defining format fields e.g. '/Ki:32x/PIN=9999/'. Max length 10000.
			IN		encrypt			- type of encryption to use
			IN		transport_key_string - string containing transport key (hex characters)
			OUT		Main_Inst		- Output Instance to create
			OUT		logstr			- String for logs. (Set to NULL if already setup)
			IN		loglength		- maximum length of logstr.

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Create_Inst(IN char master_key_string[],
		IN char customer[],
		IN char batch[],
		IN uint32_t quantity,
		IN char format_string[],
		IN uint8_t encrypt,
		IN char transport_key_string[],
		OUT KAREVARA_Inst *Main_Inst,
		OUT char logstr[],
		IN uint32_t loglength)
{
	/*Setup the Logs (if necessary)*/
	if(logstr != NULL) {
		Start_Log(logstr,loglength);
		LogHeader("KAREVARA_Create_Inst Start");
	}

	/* Initialise */
	Init_KAREVARA_Inst(Main_Inst);

	/* Check inputs aren't bad */
	if(Check_Inputs(&master_key_string,
		&customer,
		&batch,
		&quantity,
		&encrypt,
		format_string))
	{
		Error("KAREVARA_Create_Inst: Check_Inputs failed.");
		goto Error_Point;
	}

	/* Copy parameters directly into KAREVARA Instance */
	strncpy(Main_Inst->customer,customer,CUS_LEN+1);
	strncpy(Main_Inst->batch,batch,BATCH_LEN+1);
	Main_Inst->quantity = quantity;

	/* Init the Format Records (and add IMSI as first record) */
	if(Init_KAREVARA_Format(&(Main_Inst->Format_Inst)))
	{
		Error("KAREVARA_Create_Inst: Init_KAREVARA_Format failed.");
		goto Error_Point;
	}

	/*Convert format_string to records we can work with*/
	if(String_To_KAREVARA_Format(format_string,&(Main_Inst->Format_Inst)))
	{
		Error("KAREVARA_Create_Inst: String_To_KAREVARA_Format failed.");
		goto Error_Point;
	}

	/* Check that the encryption parameter matches the Format_Inst */
	KAREVARA_Format_Check_Encrypt(&(Main_Inst->Format_Inst),&encrypt);

	/*Setup 'Main_Inst' for KAREVARA to go*/
	if(Load_KAREVARA_Run(master_key_string,
		Main_Inst->customer,
		Main_Inst->batch,
		Main_Inst->quantity,
		Main_Inst->Format_Inst.num_blk,
		&(Main_Inst->Run_Inst)))
	{
		Error("KAREVARA_Create_Inst: Initialise_KAREVARA failed.");
		goto Error_Point;
	}

	/* Setup transport encryption parameters */
	if(Setup_Encryption(&(Main_Inst->Encrypt_Inst),
		encrypt,
		transport_key_string))
	{
		Error("KAREVARA_Create_Inst: Setup_Encryption failed.");
		goto Error_Point;
	}

	/* Create Time and Job ID */
	if(CreateTimeJob(Main_Inst))
	{
		Error("KAREVARA_Create_Inst: CreateTimeJob failed.");
		goto Error_Point;
	}

	Main_Inst->ready = 1;

	/* Finish nicely */

	if(logstr != NULL) {
	LogHeader("KAREVARA_Create_Inst Complete");
	Close_Log();}
	return 0;

	/* Something went wrong -> cleanup and finish */
Error_Point:
	LogHeader("KAREVARA_Create_Inst Failed");
	if(logstr != NULL) Close_Log();
	return 1;
}


/*****************************************************
Check_KARAVARA_Inst:
Checks Instance is initialized.

Input:		IN		KAREVARA_Inst

Return: 	0 Success
			1 Failure
*****************************************************/
int Check_KAREVARA_Inst(KAREVARA_Inst *Main_Inst)
{
	if((Main_Inst != NULL) && (Main_Inst->ready))
	{
		return 0;
	}
	return 1;
}

/*****************************************************
Check_Inputs:
Checks and if necessary modifies the batch input parameters

Input:		IN		pmaster_key - master secret string. 32 chars.
			IN		pcustomer - generator string. 3 chars.
			IN		pbatch - integer string. Max 7 digits.
			IN		pquantity - integer. Max 8 digits
			IN		pencrypt - encryption var
			IN		format_string - format string

Return: 	0 Success
			1 Failure
*****************************************************/
static int Check_Inputs(char **pmaster_key,
		char **pcustomer,
		char **pbatch,
		uint32_t *pquantity,
		uint8_t *pencrypt,
		char *format_string)
{
	char *master_key,*customer,*batch;
	uint32_t quantity;
	uint8_t encrypt;

	/* Check for NULL */
	if(	pcustomer == NULL ||
		pbatch == NULL ||
		pmaster_key == NULL ||
		pquantity == NULL ||
		pencrypt == NULL)
	{
		/* Fail */
		Error("Check_Inputs: An input is a NULL ptr.");
		return 1;
	}

	/* Get back vars */
	master_key = *pmaster_key;
	customer = *pcustomer;
	batch = *pbatch;
	quantity = *pquantity;
	encrypt = *pencrypt;

	if(master_key != NULL)
	{
		/* Check master_key: */
		if(strlen(master_key) == 0)
		{
			/* Won't exit it is handy not to have to provide anything when testing. */
			Warning("Check_Inputs: MASTER KEY not provided. Using Default - should only be used for testing.");
			strncpy(master_key,default_master_key,MASTER_KEY_LEN+1);
		}
		else if(strlen(master_key) > MASTER_KEY_LEN)
		{
			/* Too long. This is ok, trim but warn */
			Warning("Check_Inputs: Master key too long. Trimming to 32 hex digits.");
			master_key[MASTER_KEY_LEN] = '\0';
		}
		else if(strlen(master_key) > 0 && strlen(master_key) < MASTER_KEY_LEN)
		{
			/* Too short. Will continue but warn. */
			Warning("Check_Inputs: MASTER KEY too short. This usage is not fully secure.");
			leftpad('0',master_key,MASTER_KEY_LEN);
		}

		if(strspn(master_key,MASKHEX) != strlen(master_key))
		{
			/* Invalid. This cannot be intended, hence should exit. */
			Error("Check_Inputs: MASTER KEY INVALID.");
			return 1;
		}
	}
	else
	{
		Warning("Check_Inputs: Master Key not provided. Using Default - should only be used for testing.");
		master_key = default_master_key;
	}

	/* Check customer: */
	if(customer != NULL)
	{
		if(strlen(customer) == 0)
		{
			Warning("Check_Inputs: No Customer Provided. Loading Default.");
			strncpy(customer,default_customer,CUS_LEN+1);
		}
		if(strlen(customer) > CUS_LEN)
		{
			Warning("Check_Inputs: Customer too long - trimming.");
			customer[CUS_LEN] = '\0';
		}
		if(strspn(customer,MASKALPHA) != strlen(customer) )
		{
			/* Provide default */
			Warning("Check_Inputs: Customer contains a special character. Loading Default.");
			strncpy(customer,default_customer,CUS_LEN+1);
		}
	}
	else
	{
		Warning("Check_Inputs: Customer not provided. Loading Default.");
		customer = default_customer;
	}

	/* Check Batch:*/
	if(batch != NULL)
	{
		if(strlen(batch) == 0)
		{
			/* Provide default */
			Warning("Check_Inputs: Batch not provided. Loading Default.");
			batch = default_batch;
		}

		if(strlen(batch) > BATCH_LEN ||
			strspn(batch,MASKNUM) != strlen(batch))
		{
			/* Provide default */
			Warning("Check_Inputs: Batch is too long or contains a non-number. Loading Default.");
			batch = default_batch;
		}
		/* Pad (on the left) with zeros to right length */
		leftpad('0',batch,BATCH_LEN);
	}
	else
	{
		/* Provide default */
		Warning("Check_Inputs: Batch not provided. Loading Default.");
		batch = default_batch;
	}

	/* Check quantity */
	if(quantity > MAX_QUANTITY)
	{
		/* Fail */
		Error("Check_Inputs: quantity larger than supported.");
		return 1;
	}

	/* Check encrypt:*/
	if((encrypt != NO_ENCRYPT) && (encrypt != ENCRYPT_DES) && (encrypt != ENCRYPT_TDES) && (encrypt != ENCRYPT_AES))
	{
		Warning("Check_Inputs: encrypt parameter not recognised. Setting to 0.");
		encrypt = NO_ENCRYPT;
	}

	/* Check Format String */
	if(format_string != NULL && strlen(format_string) != 0)
	{
		addslash(format_string,MAX_FORMAT_STRING_LEN);
	}

	/* Put back values */
	*pmaster_key = master_key;
	*pcustomer = customer;
	*pbatch = batch;
	*pquantity = quantity;
	*pencrypt = encrypt;

	return 0;
}

/*****************************************************
CreateTimeJob:
Creates the Time and Job values for KAREVARA

Input:	IN		Main_Inst - the KAREVARA Instance we need to record the time and job id for.

Return: 	0 Success
			1 Failure
*****************************************************/
static int CreateTimeJob(KAREVARA_Inst *Main_Inst)
{
	uint32_t i;
	uint8_t jobhex[32];
	time_t tval;
	/* Create strings for the time and the job id */
	/* Time: */
	time(&tval);
	strncpy(Main_Inst->time,spacetouscore(asctime(gmtime(&tval))),50);
	/* Job ID: */

	/* Can we used our PRNG? Has Add_Input been used yet?*/
	if((Main_Inst->Run_Inst.init) && (Main_Inst->Run_Inst.fresh_add_input))
	{
		/* Copy run inst */
		KAREVARA_Run_Inst JobID_Run;
		Copy_KAREVARA_Run_Inst(&JobID_Run,&(Main_Inst->Run_Inst));
		/* Put Job_ID on the end of the additional input */
		/* This ensures the result is independant of the actual output */
		strcat(JobID_Run.additional_input,"_JOBID");
		if(Run_KAREVARA_Inst(&JobID_Run,jobhex,16))
		{
			Error( "Error: CreateTimeJob: Run_KAREVARA_Inst failed.");
			return 1;
		}
		hextoascii(jobhex,Main_Inst->job,32,16);
	}
	else
	{
		Warning("Creating JobID from time");
		/* Just create a Job ID from the time */
		srand((uint32_t)tval);
		for(i=0;i<16;i++) jobhex[i] = rand();
		hextoascii(jobhex,Main_Inst->job,32,16);
	}
	return 0;

}

/*****************************************************
KAREVARA_Run_Output:
Takes inputs, calculates data requirement, produces data, formats (and optionally encrypts) data,
then outputs to a string. Can also produce an unencrypted version of the string.

Input:	IN	KAREVARA_Inst	- Input Instance
		OUT res				- string to put data into
		IN	reslength		- max length of res
		IN	resdelim		- delimiting string between fields of res
		OUT	unencres		- string to put unencrypted data into (can be NULL if not required)
		IN	unencreslength	- max length of unencres
		IN	unencresdelim	- delimiting string between fields of unencres

Return: 	0 Success
			1 Failure
*****************************************************/
static int KAREVARA_Run_Output(IN KAREVARA_Inst *Main_Inst,
		OUT char res[],
		IN uint32_t reslength,
		IN char resdelim[],
		OUT char unencres[],
		IN uint32_t unencreslength,
		IN char unencresdelim[])
{
	uint8_t Data_Block[DATA_BLOCK_SIZE];
	uint32_t field_size;
	uint32_t batch_step;
	KAREVARA_Encrypt no_enc_vars;

	if((res == NULL) && (unencres == NULL))
	{
		Error("KAREVARA_Run_Output: Outputs are both NULL.");
		return 1;
	}

	Log("Outputting: %d Records using %d Blocks.",
			Main_Inst->quantity,Main_Inst->quantity*Main_Inst->Format_Inst.num_blk);

	/*Perform KAREVARA. We do it a subscriber at a time*/
	/* field_size is number of bytes per user (16*num_blk as each block is 16 bytes) */
	field_size = Main_Inst->Format_Inst.num_blk*16;
	if( DATA_BLOCK_SIZE < field_size)
	{
		Error("KAREVARA_Run_Output: DATA_BLOCK_SIZE too small.");
		return 1;
	}

	/* Run & Output */
	batch_step = 0;
	while(batch_step < Main_Inst->quantity)
	{
		/*Generate a user's data*/
		if(Run_KAREVARA_Inst(&(Main_Inst->Run_Inst),Data_Block,field_size))
		{
			Error("KAREVARA_Run_Output: Run_KAREVARA_Inst failed.");
			return 1;
		}

		/*Construct Output*/

		/* First file does use encryption) */
		if(res != NULL)
		{
			if(KAREVARA_Output_Block(res,
				reslength,
				resdelim,
				Data_Block,
				&(Main_Inst->Format_Inst),
				&(Main_Inst->Encrypt_Inst)))
			{
				Error("KAREVARA_Run_Output: KAREVARA_Output_Block failed.");
				return 1;
			}
		}

		/* Second file does not use encryption) */
		if(unencres != NULL)
		{
			no_enc_vars.type = NO_ENCRYPT;

			if(KAREVARA_Output_Block(unencres,
				unencreslength,
				unencresdelim,
				Data_Block,
				&(Main_Inst->Format_Inst),
				&no_enc_vars))
			{
				Error("KAREVARA_Run_Output: KAREVARA_Output_Block (Encrypted) failed.");
				return 1;
			}
		}

		/* Next */
		Inc_KAREVARA_Format_Rec(&(Main_Inst->Format_Inst));
		batch_step++;
	}
	Log("Output completed.");
	return 0;
}

/*****************************************************
KAREVARA_Output_Block:
Formats (and encrypts) data and outputs to a file

Input:		OUT res				- string to put data into
			IN	reslength		- max length of res
			IN	resdelim		- delimiting string between fields of res
			IN	Data			- generated data to pull the output from
			IN	Format			- Format Inst
			IN	Encrypt			- Encrypt Inst

Return: 	0 Success
			1 Failure

*****************************************************/
static int KAREVARA_Output_Block(OUT char res[],
		IN uint32_t reslength,
		IN char resdelim[],
		IN uint8_t Data[],
		IN KAREVARA_Format *Format,
		IN KAREVARA_Encrypt *Encrypt)
{
	uint32_t len = 0;
	char output_buffer[MAX_OUTPUT_BUFFER_LEN];
	if(Apply_KAREVARA_Format(Data, Format, Encrypt, output_buffer, MAX_OUTPUT_BUFFER_LEN, resdelim))
	{
		Error("KAREVARA_Output_Block: Apply_KAREVARA_Format failed.");
		return 1;
	}
	if(strlen(output_buffer)+strlen(res)+1 > reslength)
	{
		Error("KAREVARA_Output_Block: No space in output string.");
		return 1;
	}
	strcat(res,output_buffer);
	strcat(res,"\n");
	return 0;
}

/*****************************************************
Init_KAREVARA_Inst:
Initialise KAREVARA_Inst

Input:		IN		Main_Inst - what to initialise

Return: 	void
*****************************************************/

static void Init_KAREVARA_Inst(KAREVARA_Inst *Main_Inst)
{
	if(Main_Inst != NULL)
	{
		Main_Inst->ready = 0;
		strncpy(Main_Inst->job,"",JOB_LEN);
		strncpy(Main_Inst->time,"",TIME_LEN);
		strncpy(Main_Inst->customer,"",CUS_LEN);
		strncpy(Main_Inst->batch,"",BATCH_LEN);
		Main_Inst->quantity = 0;
	}
	Init_KAREVARA_Run_Inst(&(Main_Inst->Run_Inst));
	Init_KAREVARA_Encrypt(&(Main_Inst->Encrypt_Inst));
	Init_KAREVARA_Format(&(Main_Inst->Format_Inst));
}

/*****************************************************
Log_KAREVARA:
Logs what's in KAREVARA_Inst

Input:		IN		Main_Inst - what to log

Return: 	void
*****************************************************/
void Log_KAREVARA(KAREVARA_Inst *Main_Inst)
{
	if((Main_Inst != NULL) && (Main_Inst->ready))
	{
		Log("Job_ID:                 %s",Main_Inst->job);
		Log("GMT_Time:               %s",Main_Inst->time);
		Log("KAREVARA Inputs: ");
		Log("Customer:               %s",Main_Inst->customer);
		Log("Batch:                  %s",Main_Inst->batch);
		Log("Quantity:               %d",Main_Inst->quantity);
		Log("");
		Log_KAREVARA_Run(&(Main_Inst->Run_Inst));
		Log("");
		Log_KAREVARA_Format(&(Main_Inst->Format_Inst));
		Log("");
		Log_KAREVARA_Encrypt(&(Main_Inst->Encrypt_Inst));
		Log("");
	}
}

