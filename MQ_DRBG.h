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

 This is an implementation of MQ_DRBG optimised for 64-bit systems. The
 implementation only supports a STATE_LENGTH of 128-bits and a BLOCK_LENGTH
 of 128 bits. Hence a state is treated as two 64-bit unsigned integers.

 The MQ-DRBG function evaluates 128 multivariate quadratics to produce a
 new 128-bit state, and 128 multivariate quadratics to produce the 128-bit
 output block. Each binary quadratic contains 1 constant term, c, 128 linear
 terms, b_i, and 8128 bi-linear terms (a_ij). This means that a quadratic
 is defined by 8257 bits.

 Each bit of the input state is treated as an input variable to these
 quadratics. Hence evaluating the MQ-DRBG function applied to 128-bit state
 involves calculating the 8128 bilinear terms, x_i.x_j and adding the a_ij
 where these are 1, then viewing the 128 linear terms, x_i, and adding the
 b_i where these are 1 then adding c. As this process is the same for all
 128 quadratics, we can combine the a_ij, b_i and c into 8257, 128-bit
 vectors and compute all 128 multivariate quadratics at once.

 One can view the process as using the bits of the input state to select
 a subset of 8257, 128-bit constants to add (xor) together. Naturally,
 this must be performed twice, with two different sets of quadratics:
 One to update the state, one to produce output.

 This software implements the MQ_DRBG function:

 int MQ_DRBG(	IN MQ_DRBG_Inst *Inst_Handle,
				IN uint32_t requested_strength,
				IN uint64_t requested_no_of_bits,
				IN char ad_in[],
				OUT uint8_t psuedorandom_bits[])

 Inst_Handle should be a pointer to a MQ_DRBG_Inst defined as:

	typedef struct mq_drbg_inst_tag
	{
		MQ_State S;
		const MQ_System_Para *P;
		uint32_t reseed_counter;
		uint32_t reseed_interval;
		uint32_t state_length;
		uint32_t block_length;
		uint32_t field_size;
		uint32_t strength;
		uint32_t prediction_resistance_flag;
	} MQ_DRBG_Inst;

 Where the state S is the initial state. The MQ_System_Para contains a pointer
 to the 8127 pairs of 128-bit constants defining the quadratics:

	 typedef struct mq_system_para_tag {
		int system_length;
		int field_size;
		MQ_Pair *P_Vec;

	} MQ_System_Para;

*/

#ifndef _MQ_DRBG_H
#define _MQ_DRBG_H




#include "stdint.h"

#define HAVE_STDINT_H 1
#include "sha2.h"

#define OUT


/*Programmed based upon the following system: */

#define STATE_LENGTH 128
#define BLOCK_LENGTH 128
#define FIELD_SIZE 1
#define STRENGTH 128
#define RESEED_INTERVAL 268435456
#define PREDICTION_RESISTANCE_FLAG 0
#define SYSTEM_LENGTH 2113792

/* MQ_DRBG Structures */

/* Struct for MQ_State - uses two 64-bit values */
typedef struct mq_state_tag {
	uint64_t a;
	uint64_t b;
} MQ_State;

typedef MQ_State MQ_Block;

/* Struct for MQ_Pair */
typedef struct mq_pair_tag {
	MQ_State S;
	MQ_Block B;
} MQ_Pair;

/* Struct for MQ_DRBG Parameters */
typedef struct mq_system_para_tag {
	int system_length;
	int field_size;
	MQ_Pair *P_Vec;

} MQ_System_Para;

/* Struct for MQ_DRBG Inst */
typedef struct mq_drbg_inst_tag
{
	MQ_State S;
	const MQ_System_Para *P;
	uint32_t reseed_counter;
	uint32_t reseed_interval;
	uint32_t state_length;
	uint32_t block_length;
	uint32_t field_size;
	uint32_t strength;
	uint32_t prediction_resistance_flag;
} MQ_DRBG_Inst;


/* External Functions */

int MQ_DRBG(MQ_DRBG_Inst *Inst_Handle,
					uint32_t requested_strength,
					uint64_t requested_no_of_bits,
					char additional_input[],
					OUT uint8_t psuedorandom_bits[]);

int Reseed_MQ_DRBG(MQ_DRBG_Inst *Inst_Handle, char additional_input[]);

int Instantiate_MQ_DRBG(uint32_t requested_strength,
					uint32_t prediction_resistance_flag,
					uint32_t requested_block_length,
					char personalisation_string[]);

int Make_MQ_State (const char string[], OUT MQ_State *state);

void Init_MQ_State(MQ_State* State);

void Copy_MQ_State(MQ_State* Dest,MQ_State* Source);

char* State_to_String(MQ_State S,char string[]);

/* Internal Functions */

static int Evaluate_MQ(uint32_t state_length,
					uint32_t block_length,
					const MQ_System_Para *P,
					MQ_State x,
					OUT MQ_State *y,
					OUT MQ_State *z);


#endif











