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
#ifndef _KAREVARA_LIB_H_
#define _KAREVARA_LIB_H_

#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
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

extern __declspec(dllexport)
int KAREVARA_lib_Create_OUT( IN char master_secret_string[],
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
extern __declspec(dllexport)
int KAREVARA_lib_Create_OUT_fm_INP(	IN char master_secret[],
							IN char inpstr[],
							IN char transport_key_str[],
							IN char trankey_csv_str[],
							OUT char outstr[], IN uint32_t maxoutlen,
						    OUT char csvstr[], IN uint32_t maxcsvlen,
							char logstr[], uint32_t loglength);

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
extern __declspec(dllexport)
int KAREVARA_lib_Create_INP( IN char master_secret[],
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
extern __declspec(dllexport)
int KAREVARA_lib_Load_INP(IN char inpstr[],
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
					IN char log[],
					IN uint32_t loglength);

/*****************************************************
KAREVARA_Create_INP_fm_Inst:
Function for creating an INP file from a KAREVARA Inst. Does not use KAREVARA algorithm.

Input:		IN		Main_Inst			- KAREVARA Inst
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
extern __declspec(dllexport)
int KAREVARA_lib_Create_INP_fm_Inst(KAREVARA_Inst *Main_Inst,
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
extern __declspec(dllexport)
int KAREVARA_lib_Create_Inst_fm_INP(OUT KAREVARA_Inst *Main_Inst,
								IN char master_secret[],
								IN char inpstr[],
								IN char transport_key_str[],
								IN char trankey_csv_str[],
								char logstr[], uint32_t loglength);

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
extern __declspec(dllexport)
int KAREVARA_lib_Create_CSV ( IN char master_secret_string[],
						 IN char customer[],
						 IN char batch[],
						 IN uint32_t quantity,
						 IN char format_string[],
						 IN uint8_t encrypt,
						 IN char transport_key_string[],
						 OUT char outcsv[], IN uint32_t outcsvlen,
						 OUT char encoutcsv[], IN uint32_t encoutcsvlen,
						 OUT char log[], IN uint32_t loglen);

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
extern __declspec(dllexport)
int KAREVARA_lib_Create_CSV_fm_INP(	IN char master_secret[],
							IN char inpstr[],
							IN char transport_key_str[],
							IN char trankey_csv_str[],
							OUT char csvstr[], IN uint32_t maxcsvlen,
						    OUT char unenccsvstr[], IN uint32_t maxunenccsvlen,
							char logstr[], uint32_t loglength);

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
extern __declspec(dllexport)
int KAREVARA_lib_Get_Tran_Key_fm_CSV(char tran_key[],
						IN uint32_t tran_key_num, IN char trancsv[],
						char logstr[], uint32_t loglength);

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
extern __declspec(dllexport)
int KAREVARA_lib_Run (KAREVARA_Inst *Main_Inst,
		char res[],
		uint32_t reslength,
		char resdelim[],
		char encres[],
		uint32_t encreslength,
		char encresdelim[],
		char logstr[],
		uint32_t loglength);

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
extern __declspec(dllexport)
int KAREVARA_lib_Create_Inst(char master_secret_string[],
		char customer[],
		char batch[],
		uint32_t quantity,
		char format_string[],
		uint8_t encrypt,
		char transport_key_string[],
		OUT KAREVARA_Inst *Main_Inst,
		char logstr[],
		uint32_t loglength);


#endif