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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQ_DRBG.h"
#include "sha2.h"

/******************************************************************
MQ_DRBG ():
Input: integer (state_handle, requested_strength, requested_no_of_bits), string additional_input.
Output: string status where status = Success or a failure message, bitstring pseudorandom_bits.
******************************************************************/

int MQ_DRBG(MQ_DRBG_Inst *Inst_Handle,
					uint32_t requested_strength,
					uint64_t requested_no_of_bits,
					char ad_in[],
					OUT uint8_t psuedorandom_bits[])
{
	char *additional_input;
	MQ_Block Block;
	MQ_State S;
	uint32_t reseed_counter;
	uint64_t remainder_bits;
	uint64_t requested_bytes;
	uint64_t produced_bytes;
	uint8_t hash_ad_in[32];
	sha256_ctx ctx;
	MQ_State temp;
	uint32_t j;

	/* Check that we have a 64-bit integer */
	if(sizeof(uint64_t) != 8)
	{
		fprintf(stderr,"Error: MQ_DRBG: 64-bit integers not supported.\n");
		return 1;
	}

	/* Step 1: Check Inst_Handle */
	if(Inst_Handle == NULL)
	{
		fprintf(stderr,"Error: MQ_DRBG: No MQ_DRBG_Inst handle provided.\n");
		return 1;
	}

	/* Create additional_input pointer */
	additional_input = ad_in;


	/* Step 2: Load State & Reseed Counter*/
	S = Inst_Handle->S;
	reseed_counter = Inst_Handle->reseed_counter;

	/* Compute bit requirements */
	remainder_bits = requested_no_of_bits%8;
	requested_bytes = requested_no_of_bits/8;
	if(remainder_bits > 0) requested_bytes++;

	/* Step 3: Is the requested_strength acheivable? */
	if(requested_strength > Inst_Handle->strength)
	{
		fprintf(stderr,"Error: MQ_DRBG: Requested_stength greater than Instance strength.\n requested_strength: %d \n instance strength: %d \n",requested_strength,Inst_Handle->strength);
		return 1;
	}

	/* Loop implied in step 12 */
	produced_bytes = 0;
	while(produced_bytes < requested_bytes)
	{

		/* Step 5: Check Prediction Resistance Flag */
		if(Inst_Handle->prediction_resistance_flag == 1)
		{
			if(Reseed_MQ_DRBG(Inst_Handle, additional_input))
			{
				fprintf(stderr,"Error: MQ_DRBG: Prediction Resistance on and Reseed Failed.\n");
				return 1;
			}
			/* Used it. Do not reuse */
			additional_input = NULL;
		}

		/* Step 6: Check if we need to reseed */
		if(reseed_counter >= Inst_Handle->reseed_interval)
		{
			/* Reseed not implemented */
			if(Reseed_MQ_DRBG(Inst_Handle, additional_input))
			{
				fprintf(stderr,"Error: MQ_DRBG: Reseed counter exceeded and Reseed Failed.\n");
				return 1;
			}
			/* Used it. Do not reuse */
			additional_input = NULL;
		}

		/* Step 7: Produce the additional input */
		if((additional_input != NULL) && (strlen(additional_input) > 0))
		{
			/*printf("%s\n",additional_input);*/

			/* Perform sha_256 hash */
			/* The sha256 functions use a type 'sha256_ctx' to hold details of the current hash state and uses the following three calls:

       		void sha256_begin(sha256_ctx ctx[1])
       		void sha256_hash(const unsigned char data[],
                            unsigned long len, sha256_ctx ctx[1])
       		void sha_end1(unsigned char hval[], sha256_ctx ctx[1])
       	*/

			sha256_begin(&ctx);
			sha256_hash((uint8_t *)additional_input,strlen(additional_input),&ctx);
			sha256_end(hash_ad_in,&ctx);

			/*
			int i;
			for(i=0;i<32;i++) printf("%2.2x",hash_ad_in[i]);
			printf("\n");
			*/

			/* Convert to a state (leftmost 128-bits) */

			for(j=0;j<8;j++)
			{
				temp.a = (temp.a<<8) | hash_ad_in[j];
				temp.b = (temp.b<<8) | hash_ad_in[j+8];
				}

			/* XOR in hash of additional_input */
			S.a ^= temp.a;
			S.b ^= temp.b;

			/* Used it. Do not reuse */
			additional_input = NULL;
		}

/*printf("State    : %016lx%016lx\n",S.a, S.b);*/

		/* Step 8: Perform MQ_DRBG */
		Evaluate_MQ(STATE_LENGTH,BLOCK_LENGTH,Inst_Handle->P,S,&S,&Block);

/*printf("New State: %016lx%016lx\n",S.b, S.a);*/
/*printf("New Block: %016lx%016lx\n",Block.b, Block.a);*/

		/* Step 11: Update Reseed Counter */
		reseed_counter++;

		/* Step 9 & 13: Output block bits to Pseudorandom_bits */
		j = 0;
		while((produced_bytes < requested_bytes) && (j<16))
		{
			if(j<8)
			{
				psuedorandom_bits[produced_bytes] = (uint8_t) (Block.a >> ((7-j)*8));
			}
			else
			{
				psuedorandom_bits[produced_bytes] = (uint8_t) (Block.b >> ((15-j)*8));
			}

			/* Truncate function */
			if(produced_bytes == requested_bytes-1)
			{
				if(remainder_bits > 0)
				{
					psuedorandom_bits[produced_bytes] &= (255<<(8-remainder_bits))&255;
				}
			}
			produced_bytes++;
			j++;
		}


	}

	/* Step 14: Update the Instance of MQ_DRBG */
	Inst_Handle->S = S;
	Inst_Handle->reseed_counter = reseed_counter;

	/* Erase local sensitive data */
	Block.a = 0;
	Block.b = 0;

	return 0;
}

/******************************************************************
Evaluate_MQ ():
Input: integer (state_length, block_length, field_size), bitstring (P, x).
Output: bitstring (y, z).
******************************************************************/

static int Evaluate_MQ(uint32_t state_length,
					uint32_t block_length,
					const MQ_System_Para *P,
					MQ_State x,
					OUT MQ_State *y,
					OUT MQ_Block *z)
{
	uint32_t j,k,last;
	uint8_t Offset[128];
	uint64_t xa;
	uint64_t xb;
	uint8_t bcnt;
	MQ_Pair *Ptr;
	uint32_t totaloffset;
	uint32_t diff;
	uint32_t next;

	if((P == NULL) || (y == NULL) || (z == NULL))
	{
		fprintf(stderr,"Error: Evaluate_MQ: NULL parameters provided.\n");
		return 1;
	}

	if((state_length != STATE_LENGTH) && (block_length != BLOCK_LENGTH))
	{
		fprintf(stderr,"Error: Evaluate_MQ: The state_length and block_length is not supported.\n");
		return 1;
	}

	/* Step 1: No need to extract P or x based upon storage form */

	/* Step 2 - 7 Optimisation: The 'i' loop is not required as
	we will compute all 128 for both y and z at once */

	/* Step 3 - 7 Optimisation: This section is time critical. */

	/*
	Initialise y and z is not necessary as first step will set
	to constant term. Rather than stepping through all multiplications,
	we're going to convert the x bitstring to a list of offsets.

	These offsets will pick the 128-bit values to add in from the
	parameters. Because we know the offsets, we can skip over values
	which we will not add. This should be more efficient.
	*/

	xa = x.a;
	xb = x.b;
	bcnt = 0;
	last = 0;

	j = 0;
	while(j < 64)
	{
		if(xa & 1)
		{
			Offset[bcnt] = j - last;
			last = j;

			/*printf("Offset %d: %d\n", bcnt, Offset[bcnt]);*/

			bcnt++;
		}
		xa >>= 1;
		j++;
	}

	while(xb != 0)
	{
		if(xb & 1)
		{
			Offset[bcnt] = j - last;
			last = j;

			/*printf("Offset %d: %d\n", bcnt, Offset[bcnt]);*/

			bcnt++;
		}
		xb >>= 1;
		j++;
	}

	/* Use the offsets to add in the right parameters */
	Ptr = P->P_Vec;

	/*printf("Const: %8.8x: %8.8x\n",Ptr[0].a,Ptr[1].a);*/

	/* Initialise to constant terms */

	y->a = Ptr->S.a;
	y->b = Ptr->S.b;
	z->a = Ptr->B.a;
	z->b = Ptr->B.b;

	/*printf("Add: %016lx%016lx %016lx%016lx\n",Ptr->S.b, Ptr->S.a, Ptr->B.b, Ptr->B.a);*/

	/*printf("Out: %016lx%016lx %016lx%016lx\n",y->b, y->a, z->b, z->a);*/

	/* Add in the quadratic terms */

	Ptr++;

	/*printf("First: %8.8x: %8.8x\n",Ptr[0].a,Ptr[1].a);*/

	totaloffset = 0;
	diff=0;

	/* x_j . x_k */
	for(j=0;j<bcnt;j++)
	{
		/* Move Ptr to next j */
		/* Remember each set is one less than the last 128, 127, ...*/
		next = (Offset[j]*(257-(totaloffset*2)-Offset[j])/2) - diff;
		Ptr += next;

/*printf("Offset: %d, Next: %d\n",Offset[j],next);*/


		/* x_j.x_j term */
		y->a ^= Ptr->S.a;
		y->b ^= Ptr->S.b;
		z->a ^= Ptr->B.a;
		z->b ^= Ptr->B.b;

/*printf("Add: %016lx%016lx %016lx%016lx\n",Ptr->S.b, Ptr->S.a, Ptr->B.b, Ptr->B.a);*/

/*printf("Out: %016lx%016lx %016lx%016lx\n",y->b, y->a, z->b, z->a);*/


		diff = 0;
		/* Add all within the 'j' */
		/* x_j . x_k terms */
		for(k=j+1;k<bcnt;k++)
		{
			Ptr += Offset[k];
			diff += Offset[k];

			y->a ^= Ptr->S.a;
			y->b ^= Ptr->S.b;
			z->a ^= Ptr->B.a;
			z->b ^= Ptr->B.b;

/*printf("Add: %016lx%016lx %016lx%016lx\n",tempPtr->S.b, tempPtr->S.a, tempPtr->B.b, tempPtr->B.a);*/

/*printf("Out: %016lx%016lx %016lx%016lx\n",y->b, y->a, z->b, z->a);*/

		}
		totaloffset += Offset[j];
	}

	/* Step 8: No need to convert state */

	/* Erase sensitive data */
	xa = 0;
	xb = 0;
	bcnt = 0;

	return 0;
}

/******************************************************************
Reseed_MQ_DRBG_Instantiation ():
Input: integer state_handle, string additional_input.
Output: string status, where status = Success or a failure message.
******************************************************************/

int Reseed_MQ_DRBG(MQ_DRBG_Inst *Inst_Handle, char additional_input[])
{
	/* Not implemented */
	fprintf(stderr,"Error: Reseeding not implemented\n");
	return 1;
}

/******************************************************************
Instantiate_MQ_DRBG ():
Input: integer (requested_strength, prediction_resistance_flag, requested_block_length), string personalisation_string.
Output: string status, integer state_handle.
******************************************************************/

int Instantiate_MQ_DRBG(uint32_t requested_strength,
					uint32_t prediction_resistance_flag,
					uint32_t requested_block_length,
					char personalisation_string[])
{
	/* Not implemented: Defined in KAREVARA.h */
	fprintf(stderr,"Error: Instantiate_MQ_DRBG: Instantiate not implemented\n");
	return 1;
}

/*****************************************************
Make_MQ_State ():
Additional Internal Function for converting a 32 character hex string into the 128-bit MQ_State comprising 64-bit values 'a' and 'b'
*****************************************************/
int Make_MQ_State (const char string[], OUT MQ_State *state)
{
	uint64_t w=0, x = 0;

	/* Check that we have a 64-bit integer */
	if(sizeof(uint64_t) != 8)
	{
		fprintf(stderr,"Error: Make_MQ_State: 64-bit integers not supported.\n");
		return 1;
	}

	if(strlen(string) < 32)
	{
		fprintf(stderr, "Error: Make_MQ_State: string too short: %s\n",string);
		return 1;
	}

	if(sscanf(string, "%016llx%016llx",&w,&x) == 2)
	{
		state->a = w;
		state->b = x;
	}
	else
	{
		fprintf(stderr, "Error: Make_MQ_State: sscanf failed\n",string);
		return 1;
	}

	return 0;
}

/*****************************************************
Init_MQ_State ():
Sets state to zero
*****************************************************/
void Init_MQ_State(MQ_State* State)
{
	State->a = 0;
	State->b = 0;
}

/*****************************************************
Copy_MQ_State ():
Copies State
*****************************************************/
void Copy_MQ_State(MQ_State* Dest,MQ_State* Source)
{
	Dest->a = Source->a;
	Dest->b = Source->b;
}

/*****************************************************
State_to_String():
Creates a String containing the state. String must be able to hold 32 hex characters
*****************************************************/
char* State_to_String(MQ_State S,char string[])
{
	sprintf(string,"%016llX%016llX",S.a, S.b);
	return string;
}







