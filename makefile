KAREVARA_Console: KAREVARA_Console.c KAREVARA_lib.h libkarevara.a
	gcc -O3 -static KAREVARA_Console.c -L. -lkarevara -o KAREVARA_Console 
#
libkarevara.a: KAREVARA_IO.o KAREVARA.o KAREVARA_Run.o KAREVARA_Format.o KAREVARA_Enc.o KAREVARA_Log_Util.o MQ_DRBG.o sha2.o aescrypt.o aeskey.o aestab.o des.o
	ar rcs libkarevara.a KAREVARA_IO.o KAREVARA.o KAREVARA_Run.o KAREVARA_Format.o KAREVARA_Enc.o KAREVARA_Log_Util.o MQ_DRBG.o sha2.o aescrypt.o aeskey.o aestab.o des.o
#
KAREVARA_IO.o: KAREVARA_IO.c KAREVARA_IO.h KAREVARA.o 
	gcc -O3 -c KAREVARA_IO.c -o KAREVARA_IO.o 
#
KAREVARA.o: KAREVARA.c KAREVARA.h KAREVARA_System_Para.h KAREVARA_Run.o KAREVARA_Format.o KAREVARA_Enc.o KAREVARA_Log_Util.o
	gcc -O3 -c KAREVARA.c -o KAREVARA.o 
#
KAREVARA_Run.o: KAREVARA_Run.c KAREVARA_Run.h MQ_DRBG.o KAREVARA_Log_Util.o KAREVARA_System_Para.h 
	gcc -O3 -c KAREVARA_Run.c -o KAREVARA_Run.o 
#
KAREVARA_Format.o: KAREVARA_Format.c KAREVARA_Format.h KAREVARA_Enc.o KAREVARA_Log_Util.o 
	gcc -O3 -c KAREVARA_Format.c -o KAREVARA_Format.o 
#
KAREVARA_Enc.o: KAREVARA_Enc.c KAREVARA_Enc.h lib_des.h aes.h KAREVARA_Log_Util.o
	gcc -O3 -c KAREVARA_Enc.c -o KAREVARA_Enc.o 
#
KAREVARA_Log_Util.o: KAREVARA_Log_Util.c KAREVARA_Log_Util.h
	gcc -O3 -c KAREVARA_Log_Util.c -o KAREVARA_Log_Util.o 
#
MQ_DRBG.o: MQ_DRBG.c MQ_DRBG.h sha2.h 
	gcc -O3 -c MQ_DRBG.c -o MQ_DRBG.o 
#	
sha2.o: sha2.c sha2.h
	gcc -O3 -c sha2.c -o sha2.o 
#
aescrypt.o: aescrypt.c aes.h
	gcc -O3 -c aescrypt.c -o aescrypt.o 
#
aeskey.o: aeskey.c
	gcc -O3 -c aeskey.c -o aeskey.o 
#
aestab.o: aestab.c aestab.h
	gcc -O3 -c aestab.c -o aestab.o 
#
des.o: lib_des.c lib_des.h
	gcc -O3 -c lib_des.c -o des.o 
#
clean:
	rm ${target} *.o