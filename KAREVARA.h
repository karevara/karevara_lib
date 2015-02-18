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
#ifndef _KAREVARA_H
#define _KAREVARA_H

#include "KAREVARA_Run.h"
#include "KAREVARA_System_Para.h"
#include "KAREVARA_Log_Util.h"
#include "KAREVARA_Enc.h"
#include "KAREVARA_Format.h"


/* Defined Values */

#define DATA_BLOCK_SIZE 9999
#define MAX_OUTPUT_BUFFER_LEN 10000
#define CUS_LEN 100
#define TIME_LEN 100
#define JOB_LEN 100
#define ADD_STR_LEN 10000

#define MASTER_KEY_LEN 32

#define BATCH_LEN 7
#define MAX_NUM_BLOCKS 9999
#define MAX_QUANTITY 99999999

/* Internal defaults */

static char default_customer[CUS_LEN+1] = "UKN";
static char default_batch[BATCH_LEN+1] = "00000";
static char default_master_key[MASTER_KEY_LEN+1] = "6ab5835b0344f2a9b496711e0cdf6b32";

/*KAREVARA 'Inst' structure */
typedef struct KAREVARA_inst_tag {

	uint8_t ready;
	char job[JOB_LEN+1];
	char time[TIME_LEN+1];
	char customer[CUS_LEN+1];
	char batch[BATCH_LEN+1];
	uint32_t quantity;
	KAREVARA_Run_Inst Run_Inst;
	KAREVARA_Format Format_Inst;
	KAREVARA_Encrypt Encrypt_Inst;

} KAREVARA_Inst;

/* KAREVARA_Main.c : Main KAREVARA Functions

/* External Functions */

int KAREVARA (KAREVARA_Inst *Main_Inst,
		char res[],
		uint32_t reslength,
		char resdelim[],
		char unencres[],
		uint32_t unencreslength,
		char unencresdelim[],
		char logstr[],
		uint32_t loglength);

int KAREVARA_Create_Inst(char master_key_string[],
		char customer[],
		char batch[],
		uint32_t quantity,
		char format_string[],
		uint8_t encrypt,
		char transport_key_string[],
		OUT KAREVARA_Inst *Main_Inst,
		char logstr[],
		uint32_t loglength);

/* Internal functions */

int Check_KAREVARA_Inst(KAREVARA_Inst *Main_Inst);

static int Load_Inputs(char master_key_string[],
		char customer[],
		char batch[],
		uint32_t quantity,
		char format_string[],
		uint8_t encrypt,
		char transport_key_string[],
		OUT KAREVARA_Inst *Main_Inst);

static int Check_Inputs(char **pmaster_key,
		char **pcustomer,
		char **pbatch,
		uint32_t *pquantity,
		uint8_t *pencrypt,
		char *format_string);

static int CreateTimeJob(KAREVARA_Inst *Main_Inst);


/* Output */

static int KAREVARA_Run_Output(KAREVARA_Inst *Main_Inst,
		char res[],
		uint32_t reslength,
		char resdelim[],
		char encres[],
		uint32_t encreslength,
		char encresdelim[]);

static int KAREVARA_Output_Block(char res[],
		uint32_t reslength,
		char resdelim[],
		uint8_t Data[],
		KAREVARA_Format *Format,
		KAREVARA_Encrypt *Encrypt);

static void Init_KAREVARA_Inst(KAREVARA_Inst *Main_Inst);

void Log_KAREVARA(KAREVARA_Inst *Main_Inst);

#endif
