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
#ifndef _KAREVARA_LOG_UTIL_H
#define _KAREVARA_LOG_UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include "stdint.h"

#define IN
#define OUT

/* Defined Values */
#define ALLOC_SIZE 100000

#define MASKALPHA "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"
#define MASKALPHANUM "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"
#define MASKALPHANUMS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/_.,"
#define MASKHEX "ABCDEFabcdef0123456789"
#define MASKNUM "0123456789"
#define MASKWHITESPACE " "

/* Defined Values */
#define MAX_LINE 1000
#define BUF_INC_SIZE 10000

#define MAX_LOG_LEN 100000

#define STARTLOG "\n-----------------------LOG-START------------------------"
#define ENDLOG   "=========================LOG=END=======================\n\n"
#define LOGEND 50
#define FULLLOG "FULL LOG\n"

#define TOPHEADER "******************************************\n* \t"
#define BOTHEADER "\t*\n******************************************\n*\n"

/* Log file */

/* Global value to decide whether we will be P_LOGGING */
uint8_t P_Logging;

/* Global filepointer for logs */
char *P_log;
uint32_t P_loglen;
uint32_t P_maxlen;

/* KAREVARA_Log.c : Log Functions*/

void Start_Log(char logstr[], uint32_t len);
void Clean_Log();
void Close_Log();
int Log(char message[], ...);
int LogHeader(char message[]);
void Error(char message[], ...);
void Warning(char message[], ...);


/* KAREVARA_Util.c : Utility Functions*/

int hextoascii(uint8_t *data,char string[],uint32_t num_char, uint32_t max_bytes);
int hextodigits(uint8_t *data,char *string,uint32_t num_char, uint32_t max_bytes);
void leftpad(char c,char str[],uint32_t minlen);
int inc_str(char str[]);
void trimcstr(char str[],char charlist[]);
void trimstr(char str[],char charlist[]);
int openfile(FILE **pfp,char filename[], char para[], uint8_t *is_open);
void closefile(FILE *fp, uint8_t *is_open);
int filetostr(char filename[], OUT char **strout, OUT uint32_t *maxlen);
void addslash(char str[],int maxlen);
char* spacetouscore(char str[]);
void toupperstr(char str[]);
int printheader(char out[], char str[], uint32_t maxlen);
int movelastlogtofront(char logfile[]);


#endif
