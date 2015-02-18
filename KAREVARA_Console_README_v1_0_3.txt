KAREVARA CONSOLE README
-----------------------

Introduction
------------

The KAREVARA console application is designed to allow the KAREVARA programme to be run from the command line or as part of a script. This README file explains how to use the KAREVARA programme to generate output files.

A Simple Example
----------------

To begin with an example, we will run the following command:

KAREVARA_Console.exe -s 000000000000000 -q 10

The –s argument specifies that the ‘Starting IMSI’ should be ‘0’. The ‘-q’ argument specifies that the size of the batch should be 10. As no output or log file is specified, the output is sent to stdout

There are a number of important arguments left out here, so the log file begins by sending warnings to stderr stating that defaults will be used:

stdout:

-----------------------LOG-START------------------------
************  KAREVARA_Create_OUT Start  ************

Warning: Check_Inputs: MASTER KEY not provided. Using Default - should only be used for testing.
Warning: Check_Inputs: No Customer Provided. Loading Default.
Warning: Check_Inputs: Batch not provided. Loading Default.
Warning: String_To_KAREVARA_Format: Format string not provided. Loading Default.
Job_ID:                 9DF7FCD850A948AF867C9FC79761C0AF
GMT_Time:               Tue_Dec_18_16:12:31_2012

KAREVARA Inputs: 
Customer:               UKN
Batch:                  0000000
Quantity:               10

Masked_Master_Key:      C01F29F1A9EE58031E3CDBB4A675C198
Additional_Input:       KAREVARA_UKN_00000_00000010_0001

Random_Blocks_per_IMSI: 1
Num_Format_Records:     2
Format_String:          /IMSI+000000000000000/Ki:32x/

No Encryption Required

Outputting: 10 Records using 10 Blocks.
Output completed.
************  KAREVARA_Create_OUT Complete  ************
=========================LOG=END=======================

Output File:

******************************************
*       OUT PROCESS DESCRIPTION *
******************************************
*
JOB_ID:          9DF7FCD850A948AF867C9FC79761C0AF
TIME_ID:         Tue_Dec_18_16:12:31_2012
*
******************************************
*       HEADER DESCRIPTION      *
******************************************
*
CUSTOMER:        UKN
QUANTITY:        00010
BATCH:           0000000
*
TRANSPORT_ALG:   NO_TRANSPORT_ENCRYPTION
******************************************
*       INPUT VARIABLES DESCRIPTION     *
******************************************
*
Var_In_List:
IMSI:           000000000000000
*
******************************************
*       OUTPUT VARIABLES DESCRIPTION    *
******************************************
*
VAR_OUT:        /IMSI/Ki/
FORMAT_OUT:     /IMSI+000000000000000/Ki:32x/
*
******************************************
*       DATA SECTION    *
******************************************
*
000000000000000 28F827D8EB3AAA4A573F2D1EBBA6FFC2 
000000000000001 34BE358616B2C01BC9B403939F1DE14E
000000000000002 7A0B14CC3C84D5D855CF2EDE568FF375
000000000000003 41B4B94F0FAB7CD24336DDCED6D01BC1
000000000000004 5C0E9294A5F783012FD2B84D4938E2F4
000000000000005 40CAD03A9A0DF6524A3C7D5B43D81111
000000000000006 97AAAE8FFE46944BD2257CC68D9A0179
000000000000007 3FE747FA3A53E633189E1D06CE173A07
000000000000008 B5984F47FFC0869342ED508559C2F107
000000000000009 1AC4493D6B6B52948C9C435C5F3FA984

The five inputs listed have been used in combination with the default Master Key to produce data for 10 subscribers. This has been formated in the default way into a Ki field, each Ki corresponding to an IMSIs incrementing from 0.


A Second Example
----------------

The program can also be used to convert an input file to an output file. Assume we have the following file ‘test.inp’ to process: 

test.inp:

******************************************
* 	INP PROCESS DESCRIPTION	*
******************************************
*
JOB_ID:          D30119CE755CD46140958CC80BB2FFDD
TIME_ID:         Tue_Dec_18_16:40:07_2012
*
******************************************
* 	HEADER DESCRIPTION	*
******************************************
*
CUSTOMER:        EXAMPLE
QUANTITY:        00010
PROFILE:         01.01
BATCH:           0012345
*
TRANSPORT_KEY:   001
TRANSPORT_ALG:   DES
******************************************
* 	INPUT VARIABLES DESCRIPTION	*
******************************************
*
Var_In_List:
IMSI:           000000000000000
Ser_nb:         11111111110000000000
*
******************************************
* 	OUTPUT VARIABLES DESCRIPTION	*
******************************************
*
VAR_OUT: 	/ICCID/IMSI/Ki/
FORMAT_OUT: 	/ICCID+11111111110000000000/IMSI+000000000000000/Ki:32x^/
*

The following command line will read this input file and produce the output file ‘test.out’. As transport encryption is required, a transport key is provided. In this case the key is: ‘0xDEADBEEFDEADBEEF’

KAREVARA_Console.exe –i test.inp –o test.out –k DEADBEEFDEADBEEF

test.out:

******************************************
* 	OUT PROCESS DESCRIPTION	*
******************************************
*
JOB_ID:          582282E99D58A4B121F0F2F47E861275
TIME_ID:         Tue_Dec_18_16:50:43_2012
*
******************************************
* 	INP PROCESS DESCRIPTION	*
******************************************
*
JOB_ID:          D30119CE755CD46140958CC80BB2FFDD
TIME_ID:         Tue_Dec_18_16:40:07_2012
*
******************************************
* 	HEADER DESCRIPTION	*
******************************************
*
CUSTOMER:        EXAMPLE
QUANTITY:        00010
PROFILE:         01.01
BATCH:           0012345
*
TRANSPORT_KEY:   001
TRANSPORT_ALG:   DES
******************************************
* 	INPUT VARIABLES DESCRIPTION	*
******************************************
*
Var_In_List:
IMSI:           000000000000000
Ser_nb:         11111111110000000000
*
******************************************
* 	OUTPUT VARIABLES DESCRIPTION	*
******************************************
*
VAR_OUT: 	/ICCID/IMSI/Ki/
FORMAT_OUT: 	/ICCID+11111111110000000000/IMSI+000000000000000/Ki:32x^/
*
*
******************************************
* 	DATA SECTION	*
******************************************
*
11111111110000000000 000000000000000 D001F7797516FE79BE86F5C8199C81D9 
11111111110000000001 000000000000001 3AD36FFB155954F0675A0BB0BDF2F55C 
11111111110000000002 000000000000002 DDEAFA919458720B64597A32148239DF 
11111111110000000003 000000000000003 5D9BEC52644304397A3A155D60076FAF 
11111111110000000004 000000000000004 54E6874A3BDFAA2CBB25EC90F38999AA 
11111111110000000005 000000000000005 8212D1CD4152D7F48764AB4680184E36 
11111111110000000006 000000000000006 D61503CE82E7C4E8F464AF9614C5E70D 
11111111110000000007 000000000000007 47659F6D0BB945A652F8A2A6BD0F09ED 
11111111110000000008 000000000000008 186D9ED49E42DFD27C323C2F9CA421AE 
11111111110000000009 000000000000009 6789E1B156F4997DB3A5D4CA0D767B29


A Third Example
---------------

In addition to the second example, the program can read in transport keys from a csv file, using the transport key number from the input .inp file. In this case the file is called ‘transport_key_lookup.csv’ . 

transport_key_lookup.csv:

key_num,transport_key
001,DEADBEEFDEADBEEF
002,00112233445566778899AABBCCDDEEFF
003,0000111122223333
100,FEDCBA9876543210FEDCBA9876543210

As ‘test.inp’ specifies that transport key number ‘001’, the application will use the transport key: ‘DEADBEEFDEADBEEF’.

Along with outputting parameters to the .OUT file, the application can also output the parameters to a csv file (unencrypted). In this case we call the file ‘perso.csv’. This feature is useful for archive or for post-processing (such as personalisation).

KAREVARA_Console.exe –i test.inp –o test.out –t transport_key_lookup.csv –u perso.csv

As the tranpsort key is the same, this produces the same ‘test.out’ file as in the previous example along with the post-processing file ‘perso.csv’:

perso.csv:

JOB_ID: 582282E99D58A4B121F0F2F47E861275, TIME_ID: Tue_Dec_18_17:02:46_2012

ICCID,IMSI,Ki,
11111111110000000000,000000000000000,EACDBD684A33FDD35777C076F33EE990,
11111111110000000001,000000000000001,DF752911BA439A233C50350FACAABB5B,
11111111110000000002,000000000000002,0F04CAC82161A2D18922C1A1A9DDB24E,
11111111110000000003,000000000000003,9B13F6F40C39BE92910BBF53811F8495,
11111111110000000004,000000000000004,A13D71794CF45FD4E43114EC40B7E2BE,
11111111110000000005,000000000000005,2B737B876A74F7D2C021E704A75F8F5E,
11111111110000000006,000000000000006,9863C4EDF0386D99A18C0211C331CF7C,
11111111110000000007,000000000000007,5DC837198946EF86994E66BC62E6566E,
11111111110000000008,000000000000008,F024B78E6667D6542634114317FDDEF6,
11111111110000000009,000000000000009,AA0D053546B4F759F9E4C0C2DC85C30A,



A Full Example
--------------

In addition to the previous examples, for operational use a master key should be specified (generated from a source of true random). In the case below, the master key is set to be ‘0123456789ABCDEF0123456789ABCDEF’. As a result of changing the master key, the output of the application will change.

Finally, we should add the output log to a logfile rather than ‘stdout’. Logs will be prepended to the logfile, so the file may be reused for multiple calls to the application.

KAREVARA_Console.exe –m 0123456789ABCDEF0123456789ABCDEF –i test.inp –o test2.out –t transport_key_lookup.csv –u perso2.csv –v karevara.log

This produces the same test.out file as in the previous example along with ‘perso.csv’:

test2.out

******************************************
* 	OUT PROCESS DESCRIPTION	*
******************************************
*
JOB_ID:          DFED0196B9C0B31209742CBD899A1021
TIME_ID:         Tue_Dec_18_17:12:11_2012
*
******************************************
* 	INP PROCESS DESCRIPTION	*
******************************************
*
JOB_ID:          D30119CE755CD46140958CC80BB2FFDD
TIME_ID:         Tue_Dec_18_16:40:07_2012
*
******************************************
* 	HEADER DESCRIPTION	*
******************************************
*
CUSTOMER:        EXAMPLE
QUANTITY:        00010
PROFILE:         01.01
BATCH:           0012345
*
TRANSPORT_KEY:   001
TRANSPORT_ALG:   DES
******************************************
* 	INPUT VARIABLES DESCRIPTION	*
******************************************
*
Var_In_List:
IMSI:           000000000000000
Ser_nb:         11111111110000000000
*
******************************************
* 	OUTPUT VARIABLES DESCRIPTION	*
******************************************
*
VAR_OUT: 	/ICCID/IMSI/Ki/
FORMAT_OUT: 	/ICCID+11111111110000000000/IMSI+000000000000000/Ki:32x^/
*
*
******************************************
* 	DATA SECTION	*
******************************************
*
11111111110000000000 000000000000000 7C07F22D3A387188F8AF2304FC1BDF60 
11111111110000000001 000000000000001 C915F9025754A1732D6B2F4607045B4F 
11111111110000000002 000000000000002 4D669F23879FD55A6AD8CEBCBDC8624C 
11111111110000000003 000000000000003 7B0831E2CD592CC86133796E24780AEB 
11111111110000000004 000000000000004 4FED87256D5012737F763E129B9BC4C2 
11111111110000000005 000000000000005 F4FFBA84A6714911E7D08F200E7F6718 
11111111110000000006 000000000000006 6AC71720FC874A8BFDAEE05951B13D1C 
11111111110000000007 000000000000007 2A1C3F28F4245E3CB788A3FBBFB99B9E 
11111111110000000008 000000000000008 4838322A0765362052E2DFA413F4050E 
11111111110000000009 000000000000009 162CA9A71BE4F9E39AFF13F05A5EF9F3


perso2.csv

JOB_ID: DFED0196B9C0B31209742CBD899A1021, TIME_ID: Tue_Dec_18_17:12:11_2012

ICCID,IMSI,Ki,
11111111110000000000,000000000000000,0BD67AD1D3C4FA14EAFA22E902F9A1F4,
11111111110000000001,000000000000001,E141DED888FD71F9769C04784BD1A994,
11111111110000000002,000000000000002,44D7910015954768C3B58B67CB31274B,
11111111110000000003,000000000000003,99BDF558ABD4C7927B854C7F4911DEA4,
11111111110000000004,000000000000004,B49955F3578B33B98C7A561514C7DA6C,
11111111110000000005,000000000000005,66594D24604D3C83346B115E9D1B8526,
11111111110000000006,000000000000006,54EB3F495A58E16CD2D300098F6DBE6D,
11111111110000000007,000000000000007,3D11F483569E4E820FE225DADB43866B,
11111111110000000008,000000000000008,6DFAFFA9743970F36EC0D6BFF5FF2737,
11111111110000000009,000000000000009,3E1159A0898B53C408B2100DB7BC8573,


karevara.log

-----------------------LOG-START------------------------
************  KAREVARA_Create_OUT_fm_INP Started  ************

Loaded Format String: /ICCID/IMSI/Ki/
Loaded Customer: EXAMPLE
Loaded Quantity: 10
Loaded Batch: 0012345
Loaded Transport Key Num: 1
Loaded Encryption: DES
Loaded IMSI: 000000000000000
Loaded ICCID: 11111111110000000000
Reading transport key '1'.
Loaded Transport Key Num: 1

Job_ID:                 DFED0196B9C0B31209742CBD899A1021
GMT_Time:               Tue_Dec_18_17:12:11_2012

KAREVARA Inputs: 
Customer:               EXAMPLE
Batch:                  0012345
Quantity:               10

Masked_Master_Key:      AB89EFCD23016745AB89EFCD23016745
Additional_Input:       KAREVARA_EXA_12345_00000010_0001

Random_Blocks_per_IMSI: 1
Num_Format_Records:     3
Format_String:          /ICCID+11111111110000000000/IMSI+000000000000000/Ki:32x^/

Encryption Algorithm:  DES
Transport Block Length: 8

TRANSPORT_KEY:          DEADBEEFDEADBEEF
Transport Key Length:   8


Outputting: 10 Records using 10 Blocks.
Output completed.
************  KAREVARA_Create_OUT_fm_INP Successful  ************

=========================LOG=END=======================

Parameters
----------

Basic Parameters:
-s [number]       Starting IMSI
-q [number]       Quantity (number of subscribers)
-b [number]       Batch No
-c [name]         Customer Name
-m [hex_field]    Master Key

Input/Output:
-o [filename]     Output (.out) Filename
-i [filename]     Input (.inp) Filename
-x                No data is generated (.inp file)
-u [filename]     Output file. For parameters unencrypted and in CSV format.
-v [filename]     Log File

Transport Encryption:
-a [DES/TDES/AES] Transport Encryption Algorithm
-n [number]       Transport Key Number
-k [hex_field]    Transport Key
-t [filename]     CSV File containing ‘key_no, transport key’ fields

Additional:
-y [name]         Type
-p [name]         Profile
-g [name]         Graph_Ref
-h [number]       MSISDN 
-d [number]       ICCID 

Detailed Parameter Infomation
-----------------------------

Basic Parameters:
-----------------
-q: Quantity
The quantity is a required parameter and must be provided in the arguments or within the input file. The quantity can be any value up to 8 digits long (a hundred million). For example:
-q 10000

Be aware that the algorithm can generate up to 20000 Ki each second, so 1 million will take approximately 1 minute.

-b: Batch
The batch number is specified using the ‘b’ argument. A number at most 7 digits long is expected. Shorter numbers will be padded on the left with zeros, the least significant digits of longer numbers will be ignored. For example:
-b 0010305

-c: Customer
The ‘c’ argument is used to specify the customer. Any name up to 100 characters long is accepted. The customer name cannot contain any spaces. For example:
-c TEST_CUSTOMER_1

-s: The Starting IMSI
The value following –s must be numeric, and may be at most 16 digits long. If the value is less than 15 digits long, it will be padded on the left with 0s to make a value 15 digits long. For example:
-s 000001234567890
The IMSI will always increment from this starting position for each subscriber. Hence the final IMSI produced will be equal to: Starting IMSI + Quantity - 1. The Starting IMSI may be provided within an input file rather than specified within the console.

-m: Master Secret
This parameter changes the master secret from the default master secret. Hence this parameter must be provided during any operational use. The value provided should be 32 hexadecimal characters generated from a hardware entropy source. For example:
-m 6ab5835b0344f2a9b496711e0cdf6b32

-f: Specifying the Output Format
The program has in-built formats to apply to certain names. For example if the following is specified:

-f /IMSI/Ki/PIN/PUK/OTA/foo/bar/

The program will: 
* Recognize IMSI (starting at the value specified by –s). 
* Recognize ‘K’ or ‘Ki’ and assume that a 32 character, transport encrypted, random hex field is required. 
* Recognize ‘PIN’ and assume that a 4 digit decimal field is required.
* Recognize ‘PUK’ and assume that a 8 digit decimal field is required.
* Assume that all other fields require a 16 character hex field.

Specifying the Format Exactly
Clearly the above defaults may not meet every user’s requirements. Hence a more detailed format is available. The following sections provide details of how to use this format.

Format for Randomly generated data
The following field defines that the field will contain randomly generated data:
/ <name> : <length> <type char> <encrypt sym> /
Where:
<name> is any alphanumeric ASCII string, 
<length> is a number denoting the number of characters in the random output
<type char> is a character denoting the type of random output. Options are ‘x’ (denotes hexadecimal) and ‘d’ (denotes decimal).
<encrypt sym> is the optional ‘hat’ character ‘^’. If added, this denotes the field should have transport encryption applied to it. See the section on Encryption.

For example producing:
* The IMSI
* a 32 character (128-bit) randomly-generated, hexadecimal, encrypted Ki.
* a 4 digit random PIN
* an 8 digit random PUK
* a 16 character random hexadecimal OTA
may be achieved using:

-f /IMSI/Ki:32x^/PIN:4d/PUK:8d/OTA:16x/

NOTE: On some platforms it may be necessary to put in a pair of encrypt symbols. For example:

-f /IMSI/Ki:32x^^/PIN:4d/PUK:8d/OTA:16x/
Format for Non-random data
In some cases, non-random data will be required. While these will not need to interact with the DRBG, it is useful for the same process to be able to produce both. 

The following field is used to specify non-random data:

/ <name> <type sym> <detail> <encrypt sym> /

Where:
<name> is any alphanumeric ASCII string, 
<type sym> is a symbol denoting the type. For fixed outputs ‘=’ is used. For incrementing outputs ‘+’ is used. 
<detail> If fixed type, detail is the fixed output. If incrementing type, detail is the initial number to increment.
<encrypt sym> is the optional ‘hat’ character ‘^’. If added, this denotes the field should have transport encryption applied to it.

For example producing:
* an ICCID incrementing from 01234567890123456789
* a PIN fixed at 9999
may be achieved using:

-f /ICCID+01234567890123456789/PIN=9999/

Input/Output Parameters:
------------------------

-o: Output Filename 
Use this parameter to specify the name of the output (.out) file. If not specified, by default the data will be output to stdout. For example:
-o karevara.out

-x: Generate Input File
Along with generating data, the console can also be used to generate an input file for use by another party or program. This is performed using the –x attribute. For example the following will generate the input file ‘test.inp’ containing the specified parameters:
KAREVARA_Console.exe -x -s 000001234567890 -q 1000 -b 11111 -c CUSTOMER –n 101 -f /K:32x/PIN=9999/PUK:8d/OTA:16x/ -o test.inp

-i: Reading from an input file
Using the ‘i’ attribute, an input file can be specified which the program will aim to read parameters from to perform the generation. For example:
-i karevara.inp

The program expects lines within the file of the form:

NAME:PARAMETER

For example:

Batch : 10015

The program will ignore lines that do not meet this format and the program is not case-sensitive. The following table specifies the names that the program will recognize and the parameters these names are associated to:

Parameter         Recognized Names
-s Starting IMSI  IMSI, SIMSI, START_IMSI, STARTIMSI
-b Batch No       BATCH, BATCH_NO
-c Customer       CUSTOMER
-q Quantity       QUANTITY, SIZE, BATCH_SIZE
-f Format Field   VAR_OUT, FORMAT_OUT
Encryption Alg    TRANSPORT_ALG, TRANSPORT_ENCRYPTION TRANSPORT_ENCRYPTION_ALG, TRANSPORT_ALGORITHM, 
Transport key num TRANSPORT_KEY_NUM, TRANSPORT_KEY
ICCID             ICCID, SER_NB
MSISDN            MSISDN
Type              TYPE
Profile           PROFILE
Graph_Ref         GRAPH_REF, GRAPH
Address           ADDRESS
For parameters not recognized by this process, NAME:PARAMETER values will be stored and included in the output file for reference.

-u: Archive / Post Processing File
Using the ‘u’ attribute, an output filename can be specified for outputing the data as a CSV file with parameters unencrypted. The purpose of this functionality is to enable the archive or post-processing (e.g. for personalisation) of generated parameters. For example:
KAREVARA_Console.exe -s 000000000000000 -q 10 –f /IMSI/Ki/PIN/PUK/ –u perso.csv

Produces (perso.csv):
JOB_ID: 5E28049908EDADDBF2015D90A69FFACE, TIME_ID: Tue_Dec_18_17:23:09_2012

IMSI,Ki,PIN,PUK,
000000000000000,7968D00CDF4FF96CF8B6B952E317C09A,0000,98746749,
000000000000001,B6437649F98E01A0696254D743D7AA81,0000,80981250,
000000000000002,29FBD0A2A3D410AD2BF6A2A48BD172F8,0000,62025812,
000000000000003,EAF8FCE25EA748B9B1EEE89FB6CF4F03,0000,36420802,
000000000000004,FB289ABF4447C5EA459581C57ACDAEC9,0000,29598366,
000000000000005,04D8DC70C57FA041F4F74CCFBBB8E1BC,0000,38361463,
000000000000006,DFC28F7670517045FCF6BD7B4943FCFF,0000,70013156,
000000000000007,33933E1C49E55231EA5B2BE96E785667,0000,40578276,
000000000000008,F6DD38C4237B776AF32CDBDA4298C626,0000,15488128,
000000000000009,5ADF8B0C0FFD890155CE14735033E6A6,0000,42268956,


-v: Log File Output
The KAREVARA log file records the actions taken during the running of the program. It also contains all the information needed to reproduce a batch. As a result if produced, the file should be suitably protected.
If a log file is specified, the program will add a log of the program to the top of the file. For example the command:
KAREVARA_Console.exe -s 0 -q 10 -v KAREVARA.log  

This produces:
-----------------------LOG-START------------------------
************  KAREVARA_Create_OUT Start  ************

Warning: Check_Inputs: MASTER KEY not provided. Using Default - should only be used for testing.
Warning: Check_Inputs: No Customer Provided. Loading Default.
Warning: Check_Inputs: Batch not provided. Loading Default.
Warning: String_To_KAREVARA_Format: Format string not provided. Loading Default.
Job_ID:                 FEB46D66B225EA91DDA6D1CEED2904AD
GMT_Time:               Tue_Dec_18_16:12:31_2012

KAREVARA Inputs: 
Customer:               UKN
Batch:                  0000000
Start_IMSI:             000000000000000
Quantity:               10

Masked_Master_Key:      C01F29F1A9EE58031E3CDBB4A675C198
Additional_Input:       KAREVARA_UKN_00000_00000010_0001

Random_Blocks_per_IMSI: 1
Num_Format_Records:     2
Format_String:          /IMSI+000000000000000/Ki:32x/

No Encryption Required

Outputting: 10 Records using 10 Blocks.
Output completed.
************  KAREVARA_Create_OUT Complete  ************
=========================LOG=END=======================


Encryption Parameters:
----------------------

If the intention is to provide the output file to another party (e.g. operator or SIM manufacturer) then it may be necessary to apply transport encryption to values within the file. As has been previously stated, this can be specified using the ‘^’ symbol. For example:

-f /Ki:32x^/

will produce transport encrypted Ki. The following sections specify additional arguements that alter how transport encryption is applied.
-a Encryption algorithm
The default algorithm is DES, but Triple DES (TDES) and AES are also supported. For example:
-a TDES
If using DES, a 16 character hexadecimal key should be used. If using TDES or AES, a 32 character hexadecimal key should be used.

-n Transport Key Number
There may be benefit in specifying the transport key in the input or output file. This may be done using the –n argument. The maximum the transport key number can be is 9999. For example:
-n 128

-k Transport Key
The transport key can be specified using the –k parameter. If using DES, a 16 character hexadecimal key should be used. If using TDES or AES, a 32 character hexadecimal key should be used. For example:
-a DES –k 0123456789ABCDEF

-t Transport Key File
To avoid entering the transport key each time, the program is capable of reading the key out of a CSV file using the transport key number to lookup the required key. The transport key file should be of the form ‘key_number, key’ and can contain a mixture of 64-bit and 128-bit keys.  For example:

KEYS.csv:

001,0123456789ABCEDF
002,00001111222233334444555566667777
003,8899AABBCCDDEEFF

This feature is used by specifying the transport key number (either using the -n argument or within an input file) and specifying the transport key file using the ‘-t’ argument. For example:
-n 3 –t KEYS.csv

Will use the transport key ‘8899AABBCCDDEEFF’.

Additional Parameters:
----------------------

-y: Type
The Type field is specified using the ‘y’ argument. For example
-y 01.01

-p: Profile
The Profile field is specified using the ‘p’ argument. For example
-p 01.01

-g: Graph_Ref
The Graph_Ref field is specified using the ‘g’ argument. For example
-g 01.01

-h: MSISDN
The MSISDN field is specified using the ‘h’ argument. This will add an MSISDN field to the output (if not already specified). For example
-h 0011223344

-d: ICCID
The ICCID field is specified using the ‘d’ argument. This will add an ICCID field to the output (if not already specified). For example
-d 01234567890123456789

