# karevara_lib
The KAREVARA Library

The KAREVARA Library is designed to provide the functionality needed to generate pseudo-random data using the KAREVARA algorithm. Beyond this, the library provides the means to format the output of the KAREVARA algorithm into fields, encrypt those fields with a transport key if required, and place those fields into an ‘.OUT’ file or ‘CSV’ file.

All library functions return 0 when successful and 1 if an error occurs. Details of the functionality that occurred (including any errors) are returned within log strings.

To aid portability, the library is written in C and is as self-contained as possible. In general the library does not interact with the system while running (the only exception to this rule being to obtain the time for recording in the log and output files). As a result, the library does not perform any file functionality and instead will return results as strings so that the calling function can best decide on how the results should be provided to the user. Similarly, files passed to the library are expected to be converted into strings before being passed to the library’s functions.


Main Library Functions:

KAREVARA_Create_Inst
The purpose of this function is to create a KAREVARA_Inst from a list of inputs. If required the function will also provide a string containing a log of the process.

KAREVARA
The purpose of this function is use a KAREVARA_Inst to produce a string of puesdo random data, formatted into a list of output parameters with transport encryption applied. If required, the function can also produce the string of output parameters without transport encryption applied and it can produce a string containing a log of the process.

KAREVARA_Create_OUT
The purpose of this function is to provide a string ‘outstr’ containing the output parameters formatted into an ‘OUT’ file given a list of inputs. If required the function will also provide a string containing a .csv file containing the parameters unencrypted and a string containing a log of the process.

KAREVARA_Create_OUT_frm_INP
The purpose of this function is to provide a string ‘outstr’ containing a OUT file given an input string ‘inpstr’ containing a .inp file and a list of inputs. If required the function will also provide a string containing a .csv file containing the parameters unencrypted and a string containing a log of the process.

KAREVARA_Create_INP
The purpose of this function is to provide a string ‘outstr’ containing a  INP file given a list of inputs. If required the function will also provide a string containing a log of the process.

KAREVARA_Create_CSV
The purpose of this function is to create a string containing the output parameters as a CSV file given a list of inputs. If required the function will also provide the same string but without transport encryption applied. The function can also generate a string containing a log of the process.

KAREVARA_Create_CSV_fm_INP
The purpose of this function is to create a string containing the output parameters as a CSV file given an input file. If required the function will also provide the same string but without transport encryption applied. The function can also generate a string containing a log of the process.

KAREVARA_Load_INP
The purpose of this function is to extract parameters from a .inp file. If required the function will also provide a string containing a log of the process.

KAREVARA_Create_Inst_fm_INP
The purpose of this function is to create a KAREVARA_Inst from an INP file and list of inputs. If required the function will also provide a string containing a log of the process.

Get_Tran_Key_fm_CSV
This function provides a transport key given a transport key number and a CSV string containing a list of ‘key_no, transport_key’ pairs. If required the function will also provide a string containing a log of the process.


KAREVARA Library Files:

KAREVARA_IO.c, KAREVARA_IO.h
These files contain the functions which format the output file or extract the input for the KAREVARA algorithm.

KAREVARA.c, KAREVARA.h
These files contain the functions designed to run the KAREVARA algorithm, including producing the pseudo-random data, formatting the data into fields and applying transport encryption.

KAREVARA_Format.h, KAREVARA_Format.c
These files contain functions by which the output ‘fields’ (e.g. Ki, PIN, PUK, OTA) can be defined and applied to pseudo-random data.

KAREVARA_Enc.c, KAREVARA_Enc.h
These files contain the functions for defining and applying transport encryption to the output fields.

KAREVARA_Run.c, KAREVARA_Format.h
These files provide a wrapper for the MQ_DRBG function and generate the ‘Additional_Input’ for this function.

KAREVARA_Log_Util.c, KAREVARA_Log_Util.h
These files contain logging functions, string functions and other utility functions used by the rest of the code base.

KAREVARA_System_Para.h
This contains the KAREVARA system parameters (definition of the multivariate quadratics) for use by the MQ_DRBG function.

MQ_DRBG.c, MQ_DRBG.h
These files contain the MQ_DRBG function as defined in ISO/IEC FDIS 18031.

Supporting files:

AES implementation (Brian Gladman)
aes.h
aesopt.h
aestab.h
aescrypt.c 
aeskey.c
aestab.c
brg_endian.h
brg_types.h

SHA2 implementation (Brian Gladman)
sha2.c
sha2.h
brg_endian.h
brg_types.h

DES/TDES implementation (extracted from GNULIB, Simon Josefsson)
lib_des.c
lib_des.h

Standard header files
stdbool.h
stdint.h
