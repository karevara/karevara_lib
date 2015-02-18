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
#ifndef _KAREVARA_FORMAT_H
#define _KAREVARA_FORMAT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "KAREVARA_Log_Util.h"
#include "KAREVARA_Enc.h"


/* Defined Values */

#define OUTTYPE_SPACE 0
#define OUTTYPE_CSV 1

#define MAX_FORMAT_RECORDS 128
#define MAX_FORMAT_STRING_LEN 10000
#define MAX_FORMAT_REC_LEN 100
#define MAX_FORMAT_NAME_LEN 100
#define MAX_FORMAT_DETAILS_LEN 100
#define MAX_FORMAT_REC_OUT_LEN 1000
#define MAX_FORMAT_HEADER 20000

#define MIN_IMSI_LEN 15
#define MAX_IMSI_LEN 16
#define DATA_BLOCK_SIZE 9999


/* Format record for converted format string*/
/* e.g. /Ki:32x/PIN=0000/PUK:8d/ADM:16x/  */
typedef struct KAREVARA_format_rec_tag {

	char name[MAX_FORMAT_NAME_LEN];
	char details[MAX_FORMAT_DETAILS_LEN];
	uint32_t length;
	char type;
	uint32_t byteref;
	uint32_t bytelength;
	uint8_t encrypt;

} KAREVARA_Format_Rec;

typedef struct KAREVARA_format_tag {

	KAREVARA_Format_Rec Record[MAX_FORMAT_RECORDS];
	uint32_t num_rec;
	uint32_t num_blk;

} KAREVARA_Format;

#define HEX_TYPE 'x'
#define DEC_TYPE 'd'
#define FIX_TYPE 'f'
#define INC_TYPE 'i'
#define NUL_TYPE 'n'

#define PUR_NULL 0
#define PUR_RUN_KAREVARA 1
#define PUR_GEN_INP 2
#define PUR_LOAD_INP 3
#define PUR_KAREVARA_CSV 4


/* Internal defaults */

static char default_format_string[20] = "/IMSI/Ki:32x/";



/* KAREVARA_Format.c : Format String Functions*/

int Init_KAREVARA_Format(KAREVARA_Format *Format);

static void Init_KAREVARA_Format_Rec(KAREVARA_Format_Rec *Record);

int Add_Record_To_KAREVARA_Format(KAREVARA_Format *thisFormat, KAREVARA_Format_Rec *thisRec);

int String_To_KAREVARA_Format(char format_str[], OUT KAREVARA_Format *Format);

int KAREVARA_CSV_OUT(KAREVARA_Format *thisFormat, char out_str[], uint32_t maxlen);
int KAREVARA_VAR_OUT(KAREVARA_Format *thisFormat, char out_str[], uint32_t maxlen);
int KAREVARA_FORMAT_OUT(KAREVARA_Format *thisFormat, char out_str[], uint32_t maxlen);

static int Add_Rec_String_To_KAREVARA_Format(KAREVARA_Format *thisFormat, char str[]);
int Add_Inc_To_KAREVARA_Format(KAREVARA_Format *thisFormat,char name[], char val[]);
int Add_Fix_To_KAREVARA_Format(KAREVARA_Format *thisFormat,char name[], char val[]);
int Add_Rand_To_KAREVARA_Format(KAREVARA_Format *thisFormat,char name[], uint32_t len, char type);

int KAREVARA_Format_To_String(IN KAREVARA_Format *thisFormat, OUT char format_str[],
	uint32_t maxlen);

int KAREVARA_Var_To_String(IN KAREVARA_Format *thisFormat, OUT char var_str[],IN char var_delimit[],uint32_t maxlen);

int KAREVARA_Format_Rec_To_String(IN KAREVARA_Format_Rec *Record, OUT char format_str[],
	uint32_t maxlen);

int KAREVARA_Format_Output(char outstr[], uint32_t outlen, KAREVARA_Format *thisFormat, uint8_t encrypt);

void KAREVARA_Format_Check_Encrypt(IN KAREVARA_Format *Format, uint8_t *encrypt);

int Apply_KAREVARA_Format(IN uint8_t Data[],
						  IN KAREVARA_Format *Format,
						  IN KAREVARA_Encrypt *enc_vars,
						  OUT char out_str[],
						  uint32_t outlen,
						  char delimit[]);

void Inc_KAREVARA_Format_Rec(KAREVARA_Format *Format);

void Log_KAREVARA_Format(KAREVARA_Format *Format);

/* Internals */

static int String_To_KAREVARA_Format_Rec(IN char record_str[],
	OUT KAREVARA_Format_Rec *Record);

static int Compute_Byte_Req(IN KAREVARA_Format *Format);

static void Update_Record(KAREVARA_Format_Rec *Dest, KAREVARA_Format_Rec *Source);

static void KAREVARA_Format_Rec_Load_Default(KAREVARA_Format_Rec *Record);

#endif
