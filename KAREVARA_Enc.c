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

#include "KAREVARA_Enc.h"


/*****************************************************
Init_KAREVARA_Encrypt:
Initialisation function

Input:		IN	Instance to Init

Return: 	void
*****************************************************/
void Init_KAREVARA_Encrypt(KAREVARA_Encrypt *Encrypt_Inst)
{
	uint32_t i;
	Encrypt_Inst->type = NO_ENCRYPT;
	for(i=0;i<MAX_TRANS_KEY_LEN;i++) Encrypt_Inst->transport_key[0] = 0;
	Encrypt_Inst->blocklen = 0;
}

/*****************************************************
Setup_Encryption:
Sets up keys and parameters for encryption of output data

Input:	OUT	encrypt vars - struct for holding encryption parameters
			IN		encrypt_type - algorithm specifier e.g. ENCRYPT_DES
			IN		input_key_string - transport key char string

Return: 	0 Success
			1 Failure
*****************************************************/
int Setup_Encryption(KAREVARA_Encrypt *Encrypt_Inst,
		uint8_t encrypt_type,
		char input_key_string[])
{
	char tran_key_str[MAX_TRANS_KEY_LEN] = "";
	char *ptran_key = tran_key_str;

	if(encrypt_type != NO_ENCRYPT)
	{
		/* First check if key is provided  */
		if((input_key_string != NULL) && (strlen(input_key_string) != 0))
		{
			/* We will use this key */
			ptran_key = input_key_string;
		}

		/* No file: Load default */
		else
		{
			Error("Setup_Encryption: No Transport Key or file provided.");
			return 1;
			/* ptran_key = default_transport_key; */
		}

		/* Loaded, now check transport key */
		/* Not there */
		if(strlen(ptran_key) == 0)
		{
			Warning("Setup_Encryption: No transport_key found.");
			return 1;
			/* ptran_key = default_transport_key; */
		}
		/* Wrong format? */
		else if(strspn(ptran_key,MASKHEX) != strlen(ptran_key))
		{
			Warning("Setup_Encryption: transport_key invalid.");
			return 1;
			/* ptran_key = default_transport_key; */
		}
		/* Too long? */
		if(strlen(ptran_key) > MAX_TRANS_KEY_LEN)
		{
			Warning("Setup_Encryption: Transport key too long. Setting to 32 hex digits.");
			ptran_key[MAX_TRANS_KEY_LEN] = '\0';
		}
	}
	Load_KAREVARA_Encrypt(Encrypt_Inst,encrypt_type,ptran_key);
	return 0;
}

/*****************************************************
Load_KAREVARA_Encrypt:
Loads parameters into KAREVARA_Encrypt

Input:	OUT	encrypt vars - struct for holding encryption parameters
			IN		encrypt_type - algorithm specifier e.g. ENCRYPT_DES
			IN		input_key_string - transport key char string

Return: 	0 Success
			1 Failure
*****************************************************/
int Load_KAREVARA_Encrypt(KAREVARA_Encrypt *Encrypt_Inst,
		uint8_t encrypt_type,
		char transport_key_string[])
{
	uint32_t j,i;
	uint32_t len;

	/* Initialise T key */

	for(j=0;j<16;j++) Encrypt_Inst->transport_key[j] = 0;

	/* Check to see if there are any values to encrypt */
	if(encrypt_type != NO_ENCRYPT)
	{
		/* Load paras */
		if(transport_key_string == NULL)
		{
			Error("Setup_Encryption: Transport Key is NULL");
			return 1;
		}

		Encrypt_Inst->type = encrypt_type;
		len = strlen(transport_key_string)/2;

		/* Check it will fit */
		if(len > 16)
		{
			char temp[100];
			sprintf(temp,"Error: Setup_Encryption: Transport Key too long. Length: %d.",len);
			Error(temp);
			return 1;
		}

		/* Load transport key into memory */

		for(i=0;i<len;i++)
		{
			if(sscanf(transport_key_string+2*i,"%02hhx",
				Encrypt_Inst->transport_key+i) != 1)
			{
				Error("Setup_Encryption: scanf failed to read key byte.");
				return 1;
			}
		}


		/* Setup DES key */
		if(Encrypt_Inst->type == ENCRYPT_DES)
		{
			Encrypt_Inst->transport_key_len = 8;
			gl_des_setkey(&(Encrypt_Inst->des_ctx), (char *)Encrypt_Inst->transport_key);
			Encrypt_Inst->p_ctx = &(Encrypt_Inst->des_ctx);
			/* Function(&context, plaintext, ciphertext)*/
			Encrypt_Inst->encrypt_func = mod_des_ecb_encrypt;
			Encrypt_Inst->blocklen = 8;

		}
		/* Setup Triple DES key */
		else if(Encrypt_Inst->type == ENCRYPT_TDES)
		{
			Encrypt_Inst->transport_key_len = 16;
			gl_3des_set2keys(&(Encrypt_Inst->tdes_ctx),
				(char *) Encrypt_Inst->transport_key,
				(char *) Encrypt_Inst->transport_key+8);
			Encrypt_Inst->p_ctx = &(Encrypt_Inst->tdes_ctx);
			/* Function(&context, plaintext, ciphertext)*/
			Encrypt_Inst->encrypt_func = mod_3des_ecb_encrypt;
			Encrypt_Inst->blocklen = 8;
		}
		/* Setup AES key */
		else if(Encrypt_Inst->type == ENCRYPT_AES)
		{
			Encrypt_Inst->transport_key_len = 16;
			aes_encrypt_key128(Encrypt_Inst->transport_key, &(Encrypt_Inst->aes_ctx));
			Encrypt_Inst->p_ctx = &(Encrypt_Inst->aes_ctx);
			/* Function(&context, plaintext, ciphertext)*/
			Encrypt_Inst->encrypt_func = mod_aes_encrypt;
			Encrypt_Inst->blocklen = 16;
		}
		else
		{
			Error("Setup_Encryption: Encryption Function not supported.");
			return 1;
		}
	}
	/* No values to encrypt */
	else
	{
		Encrypt_Inst->type = NO_ENCRYPT;
	}
	return 0;
}



/*****************************************************
KAREVARA_Enc_Output:
Outputs the encryption parameters

Input:		OUT		outstr - output string
			IN		maxlen - maximum length of above string
			IN		Encrypt_Inst - encryption parameters

Return: 	0 Success
			1 Failure
*****************************************************/
int KAREVARA_Enc_Output(char outstr[], uint32_t maxlen, KAREVARA_Encrypt *Encrypt_Inst)
{
	char res[MAX_ENC_OUTPUT];
	res[0] = '\0';
	if(Encrypt_Inst == NULL)
	{
		Error("KAREVARA_Header_Output: NULL Encrypt_Inst ptr.");
		return 1;
	}
	if(Encrypt_Inst->type != NO_ENCRYPT)
	{
		printheader(res,"TRANSPORT ENCRYPTION",1000);
		if(Encrypt_Inst->type == ENCRYPT_DES) strcat(res,"TRANSPORT_ENCRYPTION_ALG: \tDES\n");
		if(Encrypt_Inst->type == ENCRYPT_TDES) strcat(res,"TRANSPORT_ENCRYPTION_ALG: \tTDES\n");
		if(Encrypt_Inst->type == ENCRYPT_AES) strcat(res,"TRANSPORT_ENCRYPTION_ALG: \tAES\n");
	}
	if(strlen(res) < maxlen) {strcat(outstr,res);}
	else
	{
		Error("KAREVARA_Header_Output: Not sufficent space in output string.");
		return 1;
	}
	return 0;
}

/*****************************************************
Log_KAREVARA_Encrypt:
Logs what's in KAREVARA_Encrypt

Input:		IN		Main_Inst - what to log

Return: 	void
*****************************************************/
void Log_KAREVARA_Encrypt(KAREVARA_Encrypt *Encrypt)
{
	uint32_t j;

	char hex[3];
	if(Encrypt != NULL)
	{
		if(Encrypt->type != NO_ENCRYPT)
		{
			if(Encrypt->type == ENCRYPT_DES) Log("Encryption Algorithm:  DES");
			else if(Encrypt->type == ENCRYPT_TDES) Log("Encryption Algorithm:  TDES");
			else if(Encrypt->type == ENCRYPT_AES) Log("Encryption Algorithm:  AES");
			else  Log("Encryption Algorithm:  UKN");
			Log("Transport Block Length: %d\n",Encrypt->blocklen);
			if(Encrypt->transport_key_len <= 128)
			{
				char temp[1000];
				sprintf(temp,"TRANSPORT_KEY:          ");
				for(j=0;j<Encrypt->transport_key_len;j++)
				{
						sprintf(hex,"%02X",Encrypt->transport_key[j]);
						strcat(temp,hex);
				}
				Log(temp);
				Log("Transport Key Length:   %d\n",Encrypt->transport_key_len);
			}
		}
		else { Log("No Encryption Required"); }
	}
}


/*****************************************************
mod functions for encryption:
	 void Function(&context, plaintext, ciphertext)
Used to make encryption function same for aes, des and tdes.
*****************************************************/
static void mod_aes_encrypt(void *cx, void *in, void *out)
{
	aes_encrypt((uint8_t *)in,(uint8_t *)out,(aes_encrypt_ctx *)cx);
}

static void mod_3des_ecb_encrypt (void *cx, void *in, void *out)
{
	gl_3des_ecb_encrypt((gl_3des_ctx*)cx,(char *)in,(char *)out);
}

static void mod_des_ecb_encrypt (void *cx, void *in, void *out)
{
	gl_des_ecb_encrypt((gl_des_ctx*)cx,(char *)in,(char *)out);
}
