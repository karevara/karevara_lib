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

#include "KAREVARA_Run.h"


/*****************************************************
Init_KAREVARA_Run_Inst:
Inits the MQ_DRBG parameters for the KAREVARA run

Input:		OUT	Handle - KAREVARA_Run_Inst Handle

Return: 	void
*****************************************************/

void Init_KAREVARA_Run_Inst(KAREVARA_Run_Inst *Handle)
{
	strncpy(Handle->additional_input,"",SIZE_OF_ADDINFO);
	Handle->init = 0;
	Handle->fresh_add_input = 0;
	Init_MQ_State(&(Handle->master_key));
	Handle->bytes_req = 0;
	Handle->algorithm_parameters = Default_Parameters;
}

/*****************************************************
Copies_KAREVARA_Run_Inst:
Inits the MQ_DRBG parameters for the KAREVARA run

Input:		OUT	Dest - KAREVARA_Run_Inst Handle
			IN	Source - KAREVARA_Run_Inst Handle

Return: 	void
*****************************************************/

void Copy_KAREVARA_Run_Inst(OUT KAREVARA_Run_Inst *Dest, IN KAREVARA_Run_Inst *Source)
{
	strncpy(Dest->additional_input,Source->additional_input,SIZE_OF_ADDINFO);
	Dest->init = Source->init;
	Dest->fresh_add_input = Source->fresh_add_input;
	Copy_MQ_State(&(Dest->master_key),&(Source->master_key));
	Dest->bytes_req = Source->bytes_req;
	Dest->algorithm_parameters = Source->algorithm_parameters;
}

/*****************************************************
Load_KAREVARA_Run:
Sets up parameters for the KAREVARA run

Input:		IN		master_key_string - MS string. 32 character.
			IN		customer - generator string. 3 chars.
			IN		batch - integer string. Max 7 digits.
			IN		quantity - integer. Max 8 digits
			IN		no_block - integer. Max 4 digits.
			OUT		Handle - KAREVARA_Run_Inst Handle

Return: 	0 Success
			1 Failure
*****************************************************/

int Load_KAREVARA_Run (char master_key_string[],
		char customer[],
		char batch[],
		uint32_t quantity,
		uint32_t no_blocks,
		OUT KAREVARA_Run_Inst *Handle)
{
	uint32_t byte_requirement;

	/* Initialise */
	Init_KAREVARA_Run_Inst(Handle);

	/*  initial state as master secret */
	if(strlen(master_key_string) == 32)
	{
		if(Make_MQ_State(master_key_string, &(Handle->master_key)))
		{
			Error( "Error: Initialise_KAREVARA: Make_MQ_State failed.");
			return 1;
		}
		Handle->algorithm_parameters.S = Handle->master_key;
	}
	else
	{
		Error("Initialise_KAREVARA: No Master Key Provided.");
		return 1;
	}


	/* Store additional_input */
	if(Generate_Add_Info(customer,
			batch,
			quantity,
			no_blocks,
			Handle->additional_input))
	{
		Error("Initialise_KAREVARA: Generate_Add_Info failed.");
		return 1;
	}

	/* Record that add_input is ready to be used */
	Handle->fresh_add_input = 1;

	/* Compute Byte Requirement */
	byte_requirement = quantity*16*no_blocks;

	if(byte_requirement*8 >= Handle->algorithm_parameters.reseed_interval)
	{
		Error("Initialise_KAREVARA: Data requirement too great - split into sub-batches.");
		return 1;
	}
	/* record byte requirement */
	Handle->bytes_req = byte_requirement;

	/* Record that KAREVARA is ready to go */
	Handle->init = 1;

	return 0;
}

/*****************************************************
Generate_Add_Info:
Converts batch parameters into 'additional_input', the string used to uniquely identify a batch.

Input:	IN		(optional) customer - generator string. 3 chars.
			IN		(optional) batch - integer string. Max 7 digits.
			IN		start_IMSI - 15 (max) integer string
			IN		quantity - integer. Max 8 digits
			IN		no_block - integer. Max 4 digits.
			OUT	additional_input. Character string.

Return: 	0 Success
			1 Failure
*****************************************************/
static int Generate_Add_Info (char customer[],
		char batch[],
		uint32_t quantity,
		uint32_t no_blocks,
		OUT char *additional_input)
{
	char output[MAX_LINE];
	char format[MAX_LINE];
	char temp_cus[ADDINFO_CUS_LEN+1];
	uint32_t temp_batch;
	uint32_t temp_num_blocks;
	uint32_t temp_quantity;

	/*Should have already checked parameters with Check_Inputs*/

	/* Make customer uppercase */
	strncpy(temp_cus,customer,ADDINFO_CUS_LEN+1);
	temp_cus[ADDINFO_CUS_LEN] = '\0';
	toupperstr(temp_cus);

	/* Just take bottom 5 digits of batch */
	if(strspn(batch,MASKNUM) != strlen(batch))
	{
		Error("Generate_Add_Info: batch is non-numeric.");
		return 1;
	}
	temp_batch = atoi(batch);

	temp_batch = temp_batch%ADDINFO_BATCH_MAX;
	temp_num_blocks = no_blocks % ADDINFO_MAX_BLOCKS;
	temp_quantity = quantity % ADDINFO_MAX_QUANTITY;

	/*Generate format string "%8.8s_%3.3s_%5.5d_%8.8d_%4.4d"*/
	sprintf(format, "%%%d.%ds_%%%d.%ds_%%%d.%dd_%%%d.%dd_%%%d.%dd",
		ADDINFO_ALG_LEN,ADDINFO_ALG_LEN,ADDINFO_CUS_LEN,ADDINFO_CUS_LEN,ADDINFO_BATCH_LEN,ADDINFO_BATCH_LEN,
		ADDINFO_QUAN_LEN,ADDINFO_QUAN_LEN,ADDINFO_NO_BLOCKS_LEN,ADDINFO_NO_BLOCKS_LEN);

	/* Construct additional_input */
	sprintf(output, format,
		ALGORITHM_ID,
		temp_cus,
		temp_batch,
		temp_quantity,
		temp_num_blocks);

	if(strlen(output) != SIZE_OF_ADDINFO)
	{
		Error("Generate_Add_Info: additional_input not of the right length.");
		return 1;
	}

	strncpy(additional_input,output,SIZE_OF_ADDINFO+1);

	return 0;

}

/*****************************************************
Run_KAREVARA_Inst:
Runs the KAREVARA PRNG data generation algorithm using MQ_DRBG

Input:		IN		Handle - KAREVARA_Run_Inst Handle defining instance
			OUT		psuedorandom_data - output data stream
			IN		num_bytes - number of bytes to produce

Return: 	0 Success
			1 Failure

*****************************************************/

int Run_KAREVARA_Inst (KAREVARA_Run_Inst *Handle, uint8_t psuedorandom_data[], uint32_t num_bytes)
{
	if((Handle == NULL) || (psuedorandom_data == NULL))
	{
		Error("Run_KAREVARA_Inst: NULL Input.");
		return 1;
	}

	if(!Handle->init)
	{
		Error("Run_KAREVARA_Inst: KAREVARA not initialised.");
		return 1;
	}

	/* Is the bit requirement too great? */
	if(num_bytes*8 + Handle->algorithm_parameters.reseed_counter >= Handle->algorithm_parameters.reseed_interval)
	{
		Error("Run_KAREVARA_Inst: Data requirement too great - reinitialise KAREVARA.");
		return 1;
	}

	/* Generate the data */
	/* Need to convert byte requirement to bits */
	if(MQ_DRBG(&(Handle->algorithm_parameters),
		128,
		(num_bytes*8),
		Handle->fresh_add_input?Handle->additional_input:NULL,
		psuedorandom_data))
	{
		Error("Run_KAREVARA_Inst: MQ_DRBG failed.");
		return 1;
	}

	/* Record that we've used add_input */
	Handle->fresh_add_input = 0;

	return 0;
}

/*****************************************************
Log_KAREVARA_Run:
Logs what's in KAREVARA_Run

Input:		IN		Run - what to log

Return: 	void
*****************************************************/
void Log_KAREVARA_Run(KAREVARA_Run_Inst *Run)
{
	char temp_ms[33];

	if(Run != NULL)
	{
		Log("Masked_Master_Key:      %s",State_to_String(Mask_State(Run->master_key),temp_ms));
		Log("Additional_Input:       %s",Run->additional_input);
	}
}

static MQ_State Mask_State(MQ_State Input)
{
	MQ_State ret;
	ret.a = Input.a ^ MQ_STATE_MASK;
	ret.b = Input.b ^ MQ_STATE_MASK;
	return ret;
}
