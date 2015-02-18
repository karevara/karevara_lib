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
#include "KAREVARA_Log_Util.h"

/*****************************************************
Setup_Logs:
Sets up the global log
*****************************************************/
void Start_Log(char logstr[], uint32_t maxlen)
{
	if((logstr != NULL) && (maxlen > 100))
	{
		Close_Log();
		P_maxlen = maxlen;
		P_log = logstr;
		Clean_Log();
		Log(STARTLOG);
	}
}

/*****************************************************
Clean_Logs:
Cleans the global log
*****************************************************/
void Clean_Log()
{
	P_log[0] = '\0';
	P_Logging = 1;
	P_loglen = P_maxlen;
}

/*****************************************************
Close_Log:
Closes the global log
*****************************************************/
void Close_Log()
{
	if(P_Logging && (strlen(ENDLOG) < P_loglen))
	{
		strcat(P_log,ENDLOG);
	}
	P_Logging = 0;
}

/*****************************************************
Log:
Logs a message. Treat like printf
*****************************************************/
int Log(char message[], ...)
{
	char temp[1000];
	uint32_t len;
	va_list args;
	va_start(args,message);
	vsnprintf(temp,1000,message,args);
	va_end(args);

	len = strlen(temp);
	if(P_Logging)
	{
		if(len + LOGEND > P_loglen)
		{
			if(P_loglen > LOGEND)
			{
				strcat(P_log,FULLLOG);
				P_loglen -= len+1;
			}
			/* Close Log */
			Close_Log();
			return 1;
		}
		else
		{
			strcat(P_log,temp);
			strcat(P_log,"\r\n");
			P_loglen -= len+2;
			return 0;
		}
	}
	return 1;
}

/*****************************************************
Error:
Logs an Error. Treat like printf
*****************************************************/
void Error(char message[], ...)
{
	char temp[1000] = "";
	char temp2[1100] = "Error: ";
	va_list args;
	va_start(args,message);
	vsnprintf(temp,1000,message,args);
	va_end(args);
	strncat(temp2, temp, 1000);
	if(Log(temp2))
	{
		/* Failed to log, use stderr */
		fprintf(stderr, temp2);
		fprintf(stderr, "\n");
	}
}

/*****************************************************
Warning:
Logs a Warning. Treat like printf
*****************************************************/
void Warning(char message[], ...)
{
	char temp[1000] = "";
	char temp2[1100] = "Warning: ";
	va_list args;
	va_start(args,message);
	vsnprintf(temp,1000,message,args);
	va_end(args);
	strncat(temp2, temp, 1000);
	if(Log(temp2))
	{
		/* Failed to log, use stderr */
		fprintf(stderr, temp2);
		fprintf(stderr, "\n");
	}
}

/*****************************************************
LogHeader:
Logs a Header.
*****************************************************/
int LogHeader(char message[])
{
	char temp[200];
	if(strlen(message)>100)
	{
		Error("HeaderLog: message too long.");
		return 1;
	}
	temp[0] = '\0';
	strcat(temp,"************  ");
	strcat(temp,message);
	strcat(temp,"  ************\n");
	return Log(temp);
}

/*****************************************************
hextoascii:
Data conversion function
*****************************************************/
int hextoascii(uint8_t *data,char string[],uint32_t num_char, uint32_t max_bytes)
{
	char *ptr_str = string;

	/* Do it nibble-based */
	uint8_t *ptrData8 = data;
	while(num_char>0)
	{
		/* Check the data exists */
		max_bytes--;
		if(max_bytes < 0)
		{
				Error("hextoascii: No data left.");
				return 1;
		}

		/* Perform the conversion */
		sprintf(ptr_str,"%01X",(*ptrData8)>>4);
		num_char--;
		ptr_str++;
		if(num_char > 0)
		{
			sprintf(ptr_str,"%01X",(*ptrData8)&15);
			num_char--;
			ptr_str++;
		}
		ptrData8++;
	}
	*ptr_str = '\0';
	return 0;
}

/*****************************************************
hextodigits:
Data conversion function
*****************************************************/
int hextodigits(uint8_t *data,char string[],uint32_t num_char, uint32_t max_bytes)
{
	int j;
	uint32_t val32;
	uint32_t *ptrData32 = (uint32_t *) data;
	char *ptr_str = string;
	while(num_char>0)
	{
		/* Check the data exists */
		max_bytes -= 4;
		if(max_bytes < 0)
		{
			Error("hextodigits: No data left.");
			return 1;
		}

		/* Perform the conversion */
		val32 = *ptrData32;
		j=0;
		while((j<4) && (num_char>0))
		{
			sprintf(ptr_str,"%01d",val32%10);
			val32 = val32/10;
			ptr_str++;
			num_char--;
			j++;
		}
		*ptrData32++;
	}
	*ptr_str = '\0';
	return 0;
}

/*****************************************************
inc_str:
Increments the string as an integer
*****************************************************/
int inc_str(char str[])
{
	int len = strlen(str);
	int last = len-9;
	int val;

	if(last < 0) last = 0;
	if(len > 0)
	{
		if(sscanf(&str[last],"%09d",&val) != 1)
		{
			Error("inc_str: sscanf failed.");
			return 1;
		}
	}
	else val = 0;
	val++;

	if(val == 1000000000)
	{
		str[last] = '\0';
		len = inc_str(str)+9;
		strcat(str,"000000000");
	}
	else if(last > 0)
	{
		sprintf(&str[last],"%09d",val);
	}
	else
	{
		sprintf(str,"%d",val);
		/* Pad on left back to original length */
		leftpad('0',str,len);
	}
	return strlen(str);
}

/*****************************************************
leftpad:
pads string 'str' with characters 'c' on the left
until str has length 'minlen'
*****************************************************/
void leftpad(char c,char str[],uint32_t minlen)
{
	uint32_t len;
	int i;
	int diff;
	if(strlen(str) < minlen)
	{
		len = strlen(str);
		diff = minlen-len;
		for(i=len;i>=0;i--)
		{
			str[diff+i] = str[i];
		}
		for(i=0;i<diff;i++)
		{
			str[i] = c;
		}
	}
	return;
}

/*****************************************************
openfile:
opens a file. If there isn't anything specified, we go to stdout
*****************************************************/
int openfile(FILE **pfp,char filename[], char para[], uint8_t *is_open)
{
	if(filename != NULL && strlen(filename) > 0 && para != NULL)
	{
		*pfp = fopen (filename, para);
		if(*pfp == NULL)
		{
			return 1;
		}
		else
		{
			if(is_open != NULL) *is_open = 1;
		}
	}
	else
	{
		if(strchr(para,'r') == NULL)
		{
			*pfp = stdout;
		}
		else *pfp = stdin;
	}
	return 0;
}

/*****************************************************
closefile:
closes a file
*****************************************************/
void closefile(FILE *fp, uint8_t *is_open)
{
	if((*is_open) && (fp != NULL))
	{
		fclose(fp);
		*is_open = 0;
	}
	return;
}

/*****************************************************
filetostr:
extracts the file as a string

MUST free 'strout' outside this function!
*****************************************************/
int filetostr(char filename[], OUT char **strout, OUT uint32_t *maxlen)
{
	uint32_t size = BUF_INC_SIZE;
	char *str = NULL;
	char *str2 = NULL;
	FILE *fp;
	uint8_t fp_open;

	openfile(&fp, filename, "r", &fp_open);

	if(fp != NULL)
	{
		FILE *fp2 = fp;
		uint32_t i=0;
		str = (char *) malloc(size);
		if(str != NULL)
		{
			while(1)
			{
				for(i=0;i<size;i++) str[i] = '\0';
				fread(str,sizeof(char),size-1,fp2);
				if(!feof(fp2))
				{
					size += BUF_INC_SIZE;
					str = realloc(str,size);
					if(str == NULL)
					{
						closefile(fp,&fp_open);
						*maxlen = 0;
						*strout = NULL;
						return 1;
					}
					for(i=0;i<size;i++) str[i] = '\0';
					rewind(fp2);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			closefile(fp,&fp_open);
			*maxlen = 0;
			*strout = NULL;
			return 1;
		}
	}
	else
	{
		return 1;
	}
	if(maxlen != NULL) *maxlen = size;
	*strout = str;
	//closefile(fp,&fp_open);
	return 0;
	/* strout must be freed elsewhere*/
}

/*****************************************************
trimstr:
trims characters not in charlist from the string str
*****************************************************/
void trimstr(char str[],char charlist[])
{
	uint32_t len = strspn(str,charlist);
	while(len < strlen(str))
	{
		str[len] = '\0';
		strcat(str,&str[len+1]);
		len = strspn(str,charlist);
	}
	return;
}

/*****************************************************
trimcstr:
trims characters in charlist from the string str
*****************************************************/
void trimcstr(char str[],char charlist[])
{
	uint32_t len = strcspn(str,charlist);
	while(len < strlen(str))
	{
		str[len] = '\0';
		strcat(str,&str[len+1]);
		len = strcspn(str,charlist);
	}
	return;
}

/*****************************************************
toupperstr:
Makes all characters in string uppercase
*****************************************************/
void toupperstr(char str[])
{
	uint32_t i;
	for(i=0;i<strlen(str);i++)
	{
		str[i] = toupper(str[i]);
	}
}

/*****************************************************
addslash:
Adds '/' to front and back of string
*****************************************************/
void addslash(char str[],int maxlen)
{
	int i, len;
	if(str[0] != '/')
	{
		len = strlen(str);
		if(len+1 < maxlen)
		{
			for(i=len;i>=0;i--)
			{
				str[i+1] = str[i];
			}
			str[0] = '/';
		}
	}
	len = strlen(str);
	if(str[len-1] != '/')
	{
		if(len+1 < maxlen)
		{
			str[len] = '/';
			str[len+1] = '\0';
		}
	}
}

/*****************************************************
spacetouscore:
Replaces spaces ' ' with underscores '_'
*****************************************************/
char* spacetouscore(char str[])
{
	char * pstr = str;
	while(*pstr != '\0')
	{
		if(*pstr == ' ')
		{
			*pstr = '_';
		}
		pstr++;
	}
	return str;
}

/*****************************************************
printheader:
Prints a header (like this one)
*****************************************************/
int printheader(char out[], char str[], uint32_t maxlen)
{
	if(maxlen > strlen(out) + strlen(TOPHEADER) + strlen(BOTHEADER) + strlen(str))
	{
		strcat(out,TOPHEADER);
		strcat(out,str);
		strcat(out,BOTHEADER);
		return 0;
	}
	else
	{
		Error("printheader: Not enough space to print header.\n");
		return 1;
	}

}

/*****************************************************
movelastlogtofront:
Prints a header (like this one)
*****************************************************/
int movelastlogtofront(char logfile[])
{
	uint32_t alloc = ALLOC_SIZE;
	char *read_log = (char*) malloc(sizeof(char) * alloc);
	char *p_start, *p_next, *p_end, *new_log;
	uint32_t used_data = 0;

	FILE *fp = fopen(logfile,"r");

	if(read_log == NULL || fp == NULL)
	{
		Error("movelastlogtofront: NULL ptr.");
		return 1;
	}

	/* Read everything in */
	p_start = read_log;
	used_data = fread(read_log,sizeof(char),alloc,fp);
	while(used_data == alloc)
	{
		alloc += ALLOC_SIZE;
		free(read_log);
		read_log = malloc(sizeof(char) * alloc);
		if(read_log == NULL)
		{
			Error("movelastlogtofront: realloc failed.");
			return 1;
		}
		rewind(fp);
		used_data = fread(read_log,sizeof(char),alloc,fp);
	}
	fclose(fp);

	/* Find the last log */
	p_start = read_log;
	p_next = strstr(p_start,STARTLOG);

	while(p_next != NULL)
	{
		p_start = p_next;
		p_next = strstr( p_next + strlen(STARTLOG),STARTLOG);
	}

	if(p_start == read_log)
	{
		/* No log found: Done */
		free(read_log);
		return 0;
	}

	/* re-open file and get another block of data */
	fp = fopen(logfile,"w");
	new_log = (char*) malloc (sizeof(char) * alloc * 2);
	if(new_log == NULL || fp == NULL)
	{
		Error("movelastlogtofront: NULL ptr.");
		return 1;
	}

	/* lets not add any unnecessary whitespace */
	p_end = strstr(p_start,ENDLOG) + strlen(ENDLOG);
	p_end[0] = '\0';

	/* Shift stuff around */
	new_log[0] = '\0';
	strcpy(new_log,p_start);
	strcat(new_log,"\n\n");
	p_start[0] = '\0';
	strcat(new_log,read_log);

	/* Write data back */
	if(fwrite(new_log,sizeof(char),strlen(new_log),fp) != strlen(new_log))
	{
		Error("movelastlogtofront: Write Error.");
		return 1;
	}
	fclose(fp);
	free(read_log);
	free(new_log);
	return 0;
}


