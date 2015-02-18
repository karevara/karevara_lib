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
#ifndef _KAREVARA_IO_H
#define _KAREVARA_IO_H

#include "KAREVARA_Run.h"
#include "KAREVARA_System_Para.h"
#include "KAREVARA_Log_Util.h"
#include "KAREVARA_Enc.h"
#include "KAREVARA_Format.h"
#include "KAREVARA.h"

#define MAX_INPUT_HEADER_LEN 10000
#define TYPE_LEN 8
#define PROFILE_LEN 8
#define GRAPH_REF_LEN 8
#define ADDRESS_LEN 1000
#define ICCID_LEN 20
#define MSISDN_LEN 15


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
						 OUT char logstr[], IN uint32_t loglength);

int KAREVARA_Create_OUT_fm_INP(	IN char master_key[],
							IN char inpstr[],
							IN char transport_key_str[],
							IN char trankey_csv_str[],
							OUT char outstr[], IN uint32_t maxoutlen,
						    OUT char csvstr[], IN uint32_t maxcsvlen,
							char logstr[], uint32_t loglength);

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
						 OUT char logstr[], IN uint32_t loglength);

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
					IN char logstr[],
					IN uint32_t logstrlen);


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
						char logstr[], uint32_t loglength);

int KAREVARA_Create_Inst_fm_INP(OUT KAREVARA_Inst *Main_Inst,
								IN char master_key[],
								IN char inpstr[],
								IN char transport_key_str[],
								IN char trankey_csv_str[],
								char logstr[], uint32_t loglength);

int KAREVARA_Create_CSV ( IN char master_key_string[],
						 IN char customer[],
						 IN char batch[],
						 IN uint32_t quantity,
						 IN char format_string[],
						 IN uint8_t encrypt,
						 IN char transport_key_string[],
						 OUT char outcsv[], IN uint32_t outcsvlen,
						 OUT char encoutcsv[], IN uint32_t encoutcsvlen,
						 OUT char log[], IN uint32_t loglen);

int KAREVARA_Create_CSV_fm_INP ( IN char master_key[],
							IN char inpstr[],
							IN char transport_key_str[],
							IN char trankey_csv_str[],
							OUT char outcsv[], IN uint32_t outcsvlen,
						    OUT char unencoutcsv[], IN uint32_t unencoutcsvlen,
							char logstr[], uint32_t loglength);

int Get_Tran_Key_fm_CSV(char tran_key[],
						IN uint32_t tran_key_num, IN char trancsv[],
						char logstr[], uint32_t loglength);

static int Read_TK_from_CSV(IN char csvstr[],
					 IN int32_t tran_key_num,
					 OUT char tran_key_str[]);

static int KAREVARA_CSV_Header(IN KAREVARA_Inst *Main_Inst,
					OUT char out[], IN uint32_t outlen);

static int KAREVARA_Process_Header(IN char instr[], IN KAREVARA_Inst *Main_Inst,
					OUT char out[], IN uint32_t outlen);





#endif
