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
#ifndef _KAREVARA_RUN_H
#define _KAREVARA_RUN_H

#include "MQ_DRBG.h"
#include "KAREVARA_System_Para.h"
#include "KAREVARA_Log_Util.h"


/* Defined Values */

#define SIZE_OF_ADDINFO 32
#define ADDINFO_ALG_LEN 8
#define ADDINFO_CUS_LEN 3
#define ADDINFO_BATCH_LEN 5
#define ADDINFO_QUAN_LEN 8
#define ADDINFO_NO_BLOCKS_LEN 4

#define ADDINFO_BATCH_MAX 100000
#define ADDINFO_MAX_BLOCKS 10000
#define ADDINFO_MAX_QUANTITY 100000000

#define MQ_STATE_MASK 0xAAAAAAAAAAAAAAAA

static const char ALGORITHM_ID[9] = "KAREVARA";

/*KAREVARA 'Run' structure */
typedef struct KAREVARA_run_inst_tag {

	MQ_State master_key;
	MQ_DRBG_Inst algorithm_parameters;
	char additional_input[SIZE_OF_ADDINFO+2];
	uint64_t bytes_req;
	uint8_t fresh_add_input;
	uint8_t init;

} KAREVARA_Run_Inst;


/* MQ_DRBG Default Inst*/
static const MQ_DRBG_Inst Default_Parameters =
{
	{0x6ab5835b0344f2a9, 0xb496711e0cdf6b32},
	&KAREVARA_System_Para,
	0,
	RESEED_INTERVAL,
	STATE_LENGTH,
	BLOCK_LENGTH,
	FIELD_SIZE,
	STRENGTH,
	PREDICTION_RESISTANCE_FLAG
};

/* KAREVARA_Run.c */



void Init_KAREVARA_Run_Inst(KAREVARA_Run_Inst *Handle);
void Copy_KAREVARA_Run_Inst(OUT KAREVARA_Run_Inst *Dest, IN KAREVARA_Run_Inst *Source);

int Load_KAREVARA_Run (char master_key_string[],
		char customer[],
		char batch[],
		uint32_t quantity,
		uint32_t no_blocks,
		OUT KAREVARA_Run_Inst *Handle);

static int Generate_Add_Info (char customer[],
		char batch[],
		uint32_t quantity,
		uint32_t no_blocks,
		OUT char additional_input[]);

int Run_KAREVARA_Inst (KAREVARA_Run_Inst *Handle, uint8_t psuedorandom_data[],
	uint32_t  num_bytes);

void Log_KAREVARA_Run(KAREVARA_Run_Inst *Run);

static MQ_State Mask_State(MQ_State Input);

#endif
