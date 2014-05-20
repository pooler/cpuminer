/* Modified (October 2010) by Eli Biham and Orr Dunkelman    * 
 * (applying the SHAvite-3 tweak) from:                      */

/*                     compress.h                            */

/*************************************************************
 * Source for Intel AES-NI assembly implementation/emulation *
 * of the compression function of SHAvite-3 512              *
 *                                                           *
 * Authors:  Ryad Benadjila -- Orange Labs                   *
 *           Olivier Billet -- Orange Labs                   *
 *                                                           *
 * June, 2009                                                *
 *************************************************************/


#define tos(a)    #a
#define tostr(a)  tos(a)

#define T8(x) ((x) & 0xff)

#define tos(a)    #a
#define tostr(a)  tos(a)

#define T8(x) ((x) & 0xff)

#define rev_reg_0321(j){\
        /* asm ("pshufb xmm"tostr(j)", [SHAVITE512_REVERSE]");\ */\
        asm ("shufps xmm"tostr(j)", xmm"tostr(j)", 0x39"); \
}

#define replace_aes(i, j){\
        asm ("aesenc xmm"tostr(i)", xmm"tostr(j)"");\
}

/* SHAvite-3 definition */

typedef struct {
   unsigned char chaining_value[64]; /* An array containing the chaining value */
   unsigned char buffer[128];        /* A buffer storing bytes until they are  */
				   /* compressed			     */
   unsigned long long CNT;
   unsigned long long  bitcount;           /* The number of bits compressed so far   */
   unsigned char partial_byte;       /* A byte to store a fraction of a byte   */
				   /* in case the input is not fully byte    */
				   /* aligned				     */
   unsigned char salt[64];           /* The salt used in the hash function     */ 
   int DigestSize;		   /* The requested digest size              */
   int BlockSize;		   /* The message block size                 */
} hashState;


/* Encrypts the plaintext pt[] using the key message[], salt[],      */
/* and counter[], to produce the ciphertext ct[]                     */
__attribute__ ((aligned (16))) unsigned int SHAVITE512_MESS[8*14];
__attribute__ ((aligned (16))) unsigned char SHAVITE512_PTXT[8*16];
__attribute__ ((aligned (16))) unsigned int SHAVITE512_CNTS[4] = {0,0,0,0}; 
__attribute__ ((aligned (16))) unsigned int SHAVITE512_REVERSE[4] = {0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x03020100 };
__attribute__ ((aligned (16))) unsigned int SHAVITE512_XOR[4] = {0x0, 0x0, 0x0, 0xFFFFFFFF};
__attribute__ ((aligned (16))) unsigned int SHAVITE512_NXOR[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0 };


#define seven_plus(a,b,c) do {\
   asm("movdqu  xmm5,  xmm"tostr(a)"");\
   asm("movdqu  xmm6,  xmm"tostr(b)"");\
   asm("psrldq  xmm5,  4");\
   asm("pslldq  xmm6,  12");\
   asm("pxor    xmm"tostr(c)",  xmm5");\
   asm("pxor    xmm"tostr(c)",  xmm6");\
\
} while(0);


#define key_mixing() do {\
  seven_plus(14,15,8); \
  seven_plus(15,8, 9); \
  seven_plus(8, 9, 10); \
  seven_plus(9, 10,11); \
  seven_plus(10,11,12); \
  seven_plus(11,12,13); \
  seven_plus(12,13,14); \
  seven_plus(13,14,15); \
} while(0);

#define key_nonlin_pre() do {\
   rev_reg_0321(8);	     \
   rev_reg_0321(9);	     \
   rev_reg_0321(10);	     \
   rev_reg_0321(11);	     \
   rev_reg_0321(12);	     \
   rev_reg_0321(13);	     \
   rev_reg_0321(14);	     \
   rev_reg_0321(15);				\
   replace_aes(8, 4);\
   replace_aes(9, 4);\
   replace_aes(10,4);\
   replace_aes(11,4);\
   replace_aes(12,4);\
   replace_aes(13,4);\
   replace_aes(14,4);\
   replace_aes(15,4);\
} while(0);

#define key_nonlin_post() do {\
   asm ("pxor   xmm8,  xmm7"); \
   asm ("pxor   xmm9,  xmm8");\
   asm ("pxor   xmm10, xmm9");\
   asm ("pxor   xmm11, xmm10");\
   asm ("pxor   xmm12, xmm11");\
   asm ("pxor   xmm13, xmm12");\
   asm ("pxor   xmm14, xmm13");\
   asm ("pxor   xmm15, xmm14");\
} while(0);

#define round(L,A,B,R) do {\
   asm ("movdqu xmm5, xmm"tostr(A)""); \
   asm ("movdqu xmm6, xmm"tostr(R)""); \
   asm ("pxor xmm"tostr(A)",  xmm8"); \
   asm ("pxor xmm"tostr(R)",  xmm12"); \
   replace_aes(A,9); \
   replace_aes(R,13); \
   replace_aes(A,10); \
   replace_aes(R,14); \
   replace_aes(A,11); \
   replace_aes(R,15); \
   replace_aes(A,4); \
   replace_aes(R,4); \
   asm ("pxor   xmm"tostr(A)",  xmm"tostr(L)""); \
   asm ("pxor   xmm"tostr(R)",  xmm"tostr(B)""); \
   asm ("movdqu xmm"tostr(L)",  xmm6"); \
   asm ("movdqu xmm"tostr(B)",  xmm5"); \
} while(0);


void E512()
{
   asm (".intel_syntax noprefix");

   /* (L,A,B,R) = (xmm0,xmm1,xmm2,xmm3) */
   asm ("movaps xmm0, [SHAVITE512_PTXT]");
   asm ("movaps xmm1, [SHAVITE512_PTXT+16]");
   asm ("movaps xmm2, [SHAVITE512_PTXT+32]");
   asm ("movaps xmm3, [SHAVITE512_PTXT+48]");

   /* init key schedule */
   asm ("movaps xmm8,  [SHAVITE512_MESS]");
   asm ("movaps xmm9,  [SHAVITE512_MESS+16]");
   asm ("movaps xmm10, [SHAVITE512_MESS+32]");
   asm ("movaps xmm11, [SHAVITE512_MESS+48]");
   asm ("movaps xmm12, [SHAVITE512_MESS+64]");
   asm ("movaps xmm13, [SHAVITE512_MESS+80]");
   asm ("movaps xmm14, [SHAVITE512_MESS+96]");
   asm ("movaps xmm15, [SHAVITE512_MESS+112]");
   asm ("movaps xmm7,  [SHAVITE512_MESS+112]");

   /* load counter and zero key for AES */
   asm ("pxor   xmm4,  xmm4");

   round(0,1,2,3); // First Round
   key_nonlin_pre();
   asm ("pxor   xmm8,  [SHAVITE512_CNTS]");
   asm ("pxor   xmm8,  [SHAVITE512_XOR]");
   key_nonlin_post();
   round(0,1,2,3); // Second Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Third Round
   key_nonlin_pre();
   asm ("pxor   xmm12,  [SHAVITE512_CNTS]");
   asm ("pxor   xmm12,  [SHAVITE512_NXOR]");
   key_nonlin_post();
   round(0,1,2,3); // Fourth Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Fifth Round
   key_nonlin_pre();
   asm ("pshufd xmm5,  [SHAVITE512_CNTS], 27");
   asm ("pxor   xmm9,  [SHAVITE512_XOR]");
   asm ("pxor   xmm9,  xmm5");
   asm ("movaps [SHAVITE512_CNTS], xmm5");
   key_nonlin_post();
   round(0,1,2,3); // Sixth Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Seventh Round
   key_nonlin_pre();
   asm ("pxor   xmm13,  [SHAVITE512_CNTS]");
   asm ("pxor   xmm13,  [SHAVITE512_NXOR]");
   key_nonlin_post();
   round(0,1,2,3); // Eighth Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Ninth Round
   key_nonlin_pre();
   asm ("pshufd xmm5,  [SHAVITE512_CNTS], 177");
   asm ("pxor   xmm15,  [SHAVITE512_XOR]");
   asm ("pxor   xmm15,  xmm5");
   asm ("movaps [SHAVITE512_CNTS], xmm5");
   key_nonlin_post();
   round(0,1,2,3); // Tenth Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Eleventh Round
   key_nonlin_pre();
   asm ("pxor   xmm11,  [SHAVITE512_CNTS]");
   asm ("pxor   xmm11,  [SHAVITE512_NXOR]");
   key_nonlin_post();
   round(0,1,2,3); // Twelfth Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Thirteenth Round
   key_nonlin_pre();
   asm ("pshufd xmm5,  [SHAVITE512_CNTS], 27");
   asm ("pxor   xmm14,  [SHAVITE512_XOR]");
   asm ("pxor   xmm14, xmm5");
   asm ("movaps [SHAVITE512_CNTS], xmm5");
   key_nonlin_post();
   round(0,1,2,3); // Fourteenth Round
   key_mixing();

   asm ("movdqu xmm7,  xmm15");
   round(0,1,2,3); // Fifteenth Round
   key_nonlin_pre();
   asm ("pshufd xmm5,  [SHAVITE512_CNTS], 177");
   asm ("pxor   xmm10,  [SHAVITE512_NXOR]");
   asm ("pxor   xmm10,  [SHAVITE512_CNTS]");
   asm ("movaps [SHAVITE512_CNTS], xmm5");

   key_nonlin_post();
   round(0,1,2,3); // Sixteenth Round

   /* feedforward */
   asm ("pxor   xmm0,  [SHAVITE512_PTXT]");
   asm ("pxor   xmm1,  [SHAVITE512_PTXT+16]");
   asm ("pxor   xmm2,  [SHAVITE512_PTXT+32]");
   asm ("pxor   xmm3,  [SHAVITE512_PTXT+48]");
   asm ("movaps [SHAVITE512_PTXT],    xmm0");
   asm ("movaps [SHAVITE512_PTXT+16], xmm1");
   asm ("movaps [SHAVITE512_PTXT+32], xmm2");
   asm ("movaps [SHAVITE512_PTXT+48], xmm3");
   asm (".att_syntax noprefix");

   return;
}



void Compress512(const unsigned char *message_block, unsigned char *chaining_value, unsigned long long counter, const unsigned char salt[64])
{    
   int i;

   for (i=0;i<16*4;i++)
      SHAVITE512_PTXT[i]=chaining_value[i];

   for (i=0;i<32;i++)
      SHAVITE512_MESS[i]= *((unsigned int*)(message_block+4*i));

   SHAVITE512_CNTS[0]=(unsigned int)(counter & 0xFFFFFFFFULL);
   SHAVITE512_CNTS[1]=(unsigned int)(counter>>32);

   E512();

   for(i=0; i<16*4; i++)
       chaining_value[i]=SHAVITE512_PTXT[i]; 

   return;
}
