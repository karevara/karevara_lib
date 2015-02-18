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
#ifndef _KAREVARA_ENC_H
#define _KAREVARA_ENC_H

#include <stdlib.h>
#include <stdio.h>

#include "aes.h"
#include "lib_des.h"

#include "KAREVARA_Log_Util.h"


/* Defined Values */

#define MAX_ENC_OUTPUT 1000
#define MAX_TRANS_KEY_LEN 32
#define DES_TRANS_KEY_LEN 16
#define TRAN_KEY_MIN 0
#define TRAN_KEY_MAX 10000

/* Encrypt type */
#define NO_ENCRYPT 0
#define ENCRYPT_DES 1
#define ENCRYPT_TDES 2
#define ENCRYPT_AES 3


/* Structure for keeping encryption details */
typedef struct KAREVARA_encrypt_tag {

	uint8_t type;
	uint8_t transport_key[MAX_TRANS_KEY_LEN];
	uint8_t transport_key_len;
	aes_encrypt_ctx aes_ctx;
	gl_des_ctx des_ctx;
	gl_3des_ctx tdes_ctx;
	void *p_ctx;
	void (*encrypt_func)(void *, void *, void *);
	uint8_t blocklen;

} KAREVARA_Encrypt;

static char default_transport_key[DES_TRANS_KEY_LEN*2+1] =
	"4eaa2a74530ac5285a66e378c32eb490";

static const uint8_t default_enc_type = ENCRYPT_DES;

static const int32_t default_enc_tran_key_num = 0;

static const int32_t invalid_enc_tran_key_num = -1;

/* KAREVARA_Enc.c : Encryption Functions*/

void Init_KAREVARA_Encrypt(KAREVARA_Encrypt *Encrypt_Inst);

int Setup_Encryption(KAREVARA_Encrypt *Encrypt_Inst,
		uint8_t encrypt_type,
		char input_key_string[]);

int Load_KAREVARA_Encrypt(KAREVARA_Encrypt *Encrypt_Inst,
		uint8_t encrypt_type,
		char transport_key_string[]);

int KAREVARA_Enc_Output(char outstr[], uint32_t maxlen, KAREVARA_Encrypt *Encrypt_Inst);

void Log_KAREVARA_Encrypt(KAREVARA_Encrypt *Encrypt);

static void mod_aes_encrypt(void *cx, void *in, void *out);
static void mod_3des_ecb_encrypt (void * cx, void *in, void *out);
static void mod_des_ecb_encrypt (void * cx, void *in, void *out);


#endif
