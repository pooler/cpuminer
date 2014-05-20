#include <stdlib.h>
#include <stdio.h>

#include "nist.h"
#include "vector.h"

#define PRINT_SOME 0



int SupportedLength(int hashbitlen) {
  if (hashbitlen <= 0 || hashbitlen > 512)
    return 0;
  else
    return 1;
}

int RequiredAlignment(void) {
  return 16;
}

static const union cv V128 = CV(128);
static const union cv V255 = CV(255);
static const union cv V257 = CV(257);
static const union cv8  V0 = CV(0);


/*
 * Reduce modulo 257; result is in [-127; 383]
 * REDUCE(x) := (x&255) - (x>>8)
 */
#define REDUCE(x)                               \
  v16_sub(v16_and(x, V255.v16), v16_shift_r (x, 8))

/*
 * Reduce from [-127; 383] to [-128; 128]
 * EXTRA_REDUCE_S(x) := x<=128 ? x : x-257
 */
#define EXTRA_REDUCE_S(x)                       \
  v16_sub(x, v16_and(V257.v16, v16_cmp(x, V128.v16)))
 
/*
 * Reduce modulo 257; result is in [-128; 128]
 */
#define REDUCE_FULL_S(x)                        \
  EXTRA_REDUCE_S(REDUCE(x))

#define DO_REDUCE(i)                            \
  X(i) = REDUCE(X(i))

#define DO_REDUCE_FULL_S(i)                     \
  do {                                          \
    X(i) = REDUCE(X(i));                        \
    X(i) = EXTRA_REDUCE_S(X(i));                \
  } while(0)

#define MAYBE_VOLATILE

MAYBE_INLINE void fft64(void *a) {

  v16* const A = a;

  register v16 X0, X1, X2, X3, X4, X5, X6, X7;

#if V16_SIZE == 8
#define X(i) A[i]
#elif V16_SIZE == 4
#define X(i) A[2*i]
#endif

#define X(i) X##i

  X0 = A[0];
  X1 = A[1];
  X2 = A[2];
  X3 = A[3];
  X4 = A[4];
  X5 = A[5];
  X6 = A[6];
  X7 = A[7];

#define DO_REDUCE(i)                            \
  X(i) = REDUCE(X(i))

  /*
   * Begin with 8 parallels DIF FFT_8
   *
   * FFT_8 using w=4 as 8th root of unity
   *  Unrolled decimation in frequency (DIF) radix-2 NTT.
   *  Output data is in revbin_permuted order.
   */

  static const int w[] = {0, 2, 4, 6};
  //  v16 *Twiddle = (v16*)FFT64_Twiddle;

#define BUTTERFLY(i,j,n)                                \
  do {                                                  \
    MAYBE_VOLATILE v16 v = X(j);                              \
    X(j) =  v16_add(X(i), X(j));                        \
    if (n)                                              \
      X(i) = v16_shift_l(v16_sub(X(i), v), w[n]);       \
    else                                                \
      X(i) = v16_sub(X(i), v);                          \
  } while(0)

  BUTTERFLY(0, 4, 0);
  BUTTERFLY(1, 5, 1);
  BUTTERFLY(2, 6, 2);
  BUTTERFLY(3, 7, 3);
  
  DO_REDUCE(2);
  DO_REDUCE(3);
  
  BUTTERFLY(0, 2, 0);
  BUTTERFLY(4, 6, 0);
  BUTTERFLY(1, 3, 2);
  BUTTERFLY(5, 7, 2);
  
  DO_REDUCE(1);
  
  BUTTERFLY(0, 1, 0);
  BUTTERFLY(2, 3, 0);
  BUTTERFLY(4, 5, 0);
  BUTTERFLY(6, 7, 0);
  
  /* We don't need to reduce X(7) */
  DO_REDUCE_FULL_S(0);
  DO_REDUCE_FULL_S(1);
  DO_REDUCE_FULL_S(2);
  DO_REDUCE_FULL_S(3);
  DO_REDUCE_FULL_S(4);
  DO_REDUCE_FULL_S(5);
  DO_REDUCE_FULL_S(6);
    
#undef BUTTERFLY

  /*
   * Multiply by twiddle factors
   */

  X(6) = v16_mul(X(6), FFT64_Twiddle[0].v16);
  X(5) = v16_mul(X(5), FFT64_Twiddle[1].v16);
  X(4) = v16_mul(X(4), FFT64_Twiddle[2].v16);
  X(3) = v16_mul(X(3), FFT64_Twiddle[3].v16);
  X(2) = v16_mul(X(2), FFT64_Twiddle[4].v16);
  X(1) = v16_mul(X(1), FFT64_Twiddle[5].v16);
  X(0) = v16_mul(X(0), FFT64_Twiddle[6].v16);

  /*
   * Transpose the FFT state with a revbin order permutation
   * on the rows and the column.
   * This will make the full FFT_64 in order.
   */

#define INTERLEAVE(i,j)                          \
  do {                                           \
    v16 t1= X(i);                                \
    v16 t2= X(j);                                \
    X(i) = v16_interleavel(t1, t2);              \
    X(j) = v16_interleaveh(t1, t2);              \
  } while(0)

  INTERLEAVE(1, 0);
  INTERLEAVE(3, 2);
  INTERLEAVE(5, 4);
  INTERLEAVE(7, 6);

  INTERLEAVE(2, 0);
  INTERLEAVE(3, 1);
  INTERLEAVE(6, 4);
  INTERLEAVE(7, 5);

  INTERLEAVE(4, 0);
  INTERLEAVE(5, 1);
  INTERLEAVE(6, 2);
  INTERLEAVE(7, 3);

#undef INTERLEAVE

  /*
   * Finish with 8 parallels DIT FFT_8
   *
   * FFT_8 using w=4 as 8th root of unity
   *  Unrolled decimation in time (DIT) radix-2 NTT.
   *  Intput data is in revbin_permuted order.
   */
  
#define BUTTERFLY(i,j,n)                                \
  do {                                                  \
    MAYBE_VOLATILE v16 u = X(j);                              \
    if (n)                                              \
      X(i) = v16_shift_l(X(i), w[n]);                   \
    X(j) = v16_sub(X(j), X(i));                         \
    X(i) = v16_add(u, X(i));                            \
  } while(0)

  DO_REDUCE(0);
  DO_REDUCE(1);
  DO_REDUCE(2);
  DO_REDUCE(3);
  DO_REDUCE(4);
  DO_REDUCE(5);
  DO_REDUCE(6);
  DO_REDUCE(7);
  
  BUTTERFLY(0, 1, 0);
  BUTTERFLY(2, 3, 0);
  BUTTERFLY(4, 5, 0);
  BUTTERFLY(6, 7, 0);
  
  BUTTERFLY(0, 2, 0);
  BUTTERFLY(4, 6, 0);
  BUTTERFLY(1, 3, 2);
  BUTTERFLY(5, 7, 2);
  
  DO_REDUCE(3);
  
  BUTTERFLY(0, 4, 0);
  BUTTERFLY(1, 5, 1);
  BUTTERFLY(2, 6, 2);
  BUTTERFLY(3, 7, 3);
  
  DO_REDUCE_FULL_S(0);
  DO_REDUCE_FULL_S(1);
  DO_REDUCE_FULL_S(2);
  DO_REDUCE_FULL_S(3);
  DO_REDUCE_FULL_S(4);
  DO_REDUCE_FULL_S(5);
  DO_REDUCE_FULL_S(6);
  DO_REDUCE_FULL_S(7);
  
#undef BUTTERFLY

  A[0] = X0;
  A[1] = X1;
  A[2] = X2;
  A[3] = X3;
  A[4] = X4;
  A[5] = X5;
  A[6] = X6;
  A[7] = X7;

#undef X

}


MAYBE_INLINE void fft128(void *a) {

  int i;

  // Temp space to help for interleaving in the end
  v16 B[8];

  v16 *A = (v16*) a;
  //  v16 *Twiddle = (v16*)FFT128_Twiddle;

  /* Size-2 butterflies */

  for (i = 0; i<8; i++) {
    B[i]   = v16_add(A[i], A[i+8]);
    B[i]   = REDUCE_FULL_S(B[i]);
    A[i+8] = v16_sub(A[i], A[i+8]);
    A[i+8] = REDUCE_FULL_S(A[i+8]);
    A[i+8] = v16_mul(A[i+8], FFT128_Twiddle[i].v16);
    A[i+8] = REDUCE_FULL_S(A[i+8]);
  }

  fft64(B);
  fft64(A+8);

  /* Transpose (i.e. interleave) */

  for (i=0; i<8; i++) {
    A[2*i]   = v16_interleavel (B[i], A[i+8]);
    A[2*i+1] = v16_interleaveh (B[i], A[i+8]);
  }
}

#ifdef v16_broadcast
/* Compute the FFT using a table
 * The function works if the value of the message is smaller 
 * than 2^14.
 */
void fft128_msg_final(short *a, const unsigned char *x) {

  static const union cv FFT128_Final_Table[] = {
    {{   1, -211,   60,  -67,    2,   92, -137,  123}},
    {{   2,  118,   45,  111,   97,  -46,   49, -106}},
    {{   4,  -73,  -17,  -11,    8,  111,  -34,  -22}},
    {{ -68,   -4,   76,  -25,   96,  -96,  -68,   -9}},
    {{  16,  -35,  -68,  -44,   32,  -70, -136,  -88}},
    {{   0, -124,   17,   12,   -6,   57,   47,   -8}},
    {{  64,  117,  -15,   81,  128,  -23,  -30,  -95}},
    {{ -68,  -53,  -52,  -70,  -10, -117,   77,   21}},
    {{  -1,  -46,  -60,   67,   -2,  -92, -120, -123}},
    {{  -2, -118,  -45, -111,  -97,   46,  -49,  106}},
    {{  -4,   73,   17,   11,   -8, -111,   34,   22}},
    {{  68,    4,  -76,   25,  -96,   96,   68,    9}},
    {{ -16, -222,   68,   44,  -32,   70, -121,   88}},
    {{   0,  124,  -17,  -12,    6,  -57,  -47,    8}},
    {{ -64, -117,   15,  -81, -128, -234,   30,   95}},
    {{  68,   53,   52,   70,   10,  117,  -77,  -21}},
    {{-118,  -31,  116,  -61,   21,  -62,  -25, -122}},
    {{-101,  107,  -45,  -95,   -8,    3,  101,  -34}},
    {{  42, -124,  -50,   13,   84,    9, -100, -231}},
    {{ -79,  -53,   82,   65,  -81,   47,   61,  107}},
    {{ -89, -239,   57, -205, -178,   36, -143,  104}},
    {{-126,  113,   33,  111,  103, -109,   65, -114}},
    {{ -99,   72,  -29,  -49, -198, -113,  -58,  -98}},
    {{   8,  -27, -106,  -30,  111,    6,   10, -108}},
    {{-139,   31, -116, -196,  -21,   62,   25, -135}},
    {{ 101, -107,   45,   95,    8,   -3, -101,   34}},
    {{ -42, -133,   50,  -13,  -84,   -9,  100,  -26}},
    {{  79,   53,  -82,  -65,   81,  -47,  -61, -107}},
    {{-168,  -18,  -57,  -52,  -79,  -36, -114, -104}},
    {{ 126, -113,  -33, -111, -103,  109,  -65,  114}},
    {{  99,  -72, -228,   49,  -59,  113,   58, -159}},
    {{  -8,   27,  106,   30, -111,   -6,  -10,  108}}
  };

  //  v16 *Table = (v16*)FFT128_Final_Table;
  v16 *A = (v16*) a;
  int i;

  v16 msg1 = v16_broadcast(x[0]>128?x[0]-257:x[0]);
  v16 msg2 = v16_broadcast(x[1]>128?x[1]-257:x[1]);
  // v16 msg2 = v16_broadcast(x[1]);

#if 0

  for (i=0; i<16; i++) {
    v16 tmp = v16_mul(FFT128_Final_Table[2*i].v16  , msg2);
    v16 sum = v16_add(FFT128_Final_Table[2*i+1].v16, msg1);
    sum = v16_add(sum, tmp);
    A[i] = REDUCE_FULL_S(sum);
  }

#else

#define FFT_FINAL(i)                                           \
  v16 tmp##i = v16_mul(FFT128_Final_Table[2*i].v16, msg2);     \
  v16 sum##i = v16_add(FFT128_Final_Table[2*i+1].v16, msg1);   \
  sum##i = v16_add(sum##i, tmp##i);                            \
  A[i] = REDUCE_FULL_S(sum##i);

  FFT_FINAL(0)
  FFT_FINAL(1)
  FFT_FINAL(2)
  FFT_FINAL(3)
  FFT_FINAL(4)
  FFT_FINAL(5)
  FFT_FINAL(6)
  FFT_FINAL(7)
  FFT_FINAL(8)
  FFT_FINAL(9)
  FFT_FINAL(10)
  FFT_FINAL(11)
  FFT_FINAL(12)
  FFT_FINAL(13)
  FFT_FINAL(14)
  FFT_FINAL(15)

#endif

}
#endif

void fft128_msg(short *a, const unsigned char *x, int final) {

  static const union cv Tweak =
    {{0,0,0,0,0,0,0,1}};
  static const union cv FinalTweak =
    {{0,0,0,0,0,1,0,1}};


  v8  *X = (v8*)  x;
  v16 *A = (v16*) a;
  //  v16 *Twiddle = (v16*)FFT128_Twiddle;

#define UNPACK(i)                                      \
  do {                                                 \
    v8 t = X[i];                                       \
    A[2*i]   = v8_mergel(t, V0.v8);                    \
    A[2*i+8] = v16_mul(A[2*i], FFT128_Twiddle[2*i].v16);          \
    A[2*i+8] = REDUCE(A[2*i+8]);                       \
    A[2*i+1] = v8_mergeh(t, V0.v8);                    \
    A[2*i+9] = v16_mul(A[2*i+1], FFT128_Twiddle[2*i+1].v16);      \
    A[2*i+9] = REDUCE(A[2*i+9]);                       \
  } while(0)


  /* 
   * This allows to tweak the last butterflies to introduce X^127
   */
#define UNPACK_TWEAK(i,tw)                             \
  do {                                                 \
    v8 t = X[i];                                       \
    v16 tmp;                                           \
    A[2*i]   = v8_mergel(t, V0.v8);                    \
    A[2*i+8] = v16_mul(A[2*i], FFT128_Twiddle[2*i].v16);          \
    A[2*i+8] = REDUCE(A[2*i+8]);                       \
    tmp      = v8_mergeh(t, V0.v8);                    \
    A[2*i+1] = v16_add(tmp, tw);                               \
    A[2*i+9] = v16_mul(v16_sub(tmp, tw), FFT128_Twiddle[2*i+1].v16);      \
    A[2*i+9] = REDUCE(A[2*i+9]);                       \
  } while(0)

  UNPACK(0);
  UNPACK(1);
  UNPACK(2);
  if (final)
    UNPACK_TWEAK(3, FinalTweak.v16);
  else
    UNPACK_TWEAK(3, Tweak.v16);

#undef UNPACK
#undef UNPACK_TWEAK

  fft64(a);
  fft64(a+64);
}

#if 0
void fft128_msg(short *a, const unsigned char *x, int final) {

  for (int i=0; i<64; i++)
    a[i] = x[i];

  for (int i=64; i<128; i++)
    a[i] = 0;

  a[127] = 1;
  a[125] = final? 1: 0;

  fft128(a);
}
#endif

void fft256_msg(short *a, const unsigned char *x, int final) {

  static const union cv Tweak =
    {{0,0,0,0,0,0,0,1}};
  static const union cv FinalTweak =
    {{0,0,0,0,0,1,0,1}};


  v8  *X = (v8*)  x;
  v16 *A = (v16*) a;
  //  v16 *Twiddle = (v16*)FFT256_Twiddle;

#define UNPACK(i)                                       \
  do {                                                  \
    v8 t      = X[i];                                   \
    A[2*i]    = v8_mergel(t, V0.v8);                    \
    A[2*i+16] = v16_mul(A[2*i], FFT256_Twiddle[2*i].v16);          \
    A[2*i+16] = REDUCE(A[2*i+16]);                      \
    A[2*i+1]  = v8_mergeh(t, V0.v8);                    \
    A[2*i+17] = v16_mul(A[2*i+1], FFT256_Twiddle[2*i+1].v16);      \
    A[2*i+17] = REDUCE(A[2*i+17]);                       \
  } while(0)


  /* 
   * This allows to tweak the last butterflies to introduce X^127
   */
#define UNPACK_TWEAK(i,tw)                              \
  do {                                                  \
    v8 t = X[i];                                        \
    v16 tmp;                                            \
    A[2*i]    = v8_mergel(t, V0.v8);                    \
    A[2*i+16] = v16_mul(A[2*i], FFT256_Twiddle[2*i].v16);          \
    A[2*i+16] = REDUCE(A[2*i+16]);                       \
    tmp       = v8_mergeh(t, V0.v8);                    \
    A[2*i+1]  = v16_add(tmp, tw);                               \
    A[2*i+17] = v16_mul(v16_sub(tmp, tw), FFT256_Twiddle[2*i+1].v16);      \
    A[2*i+17] = REDUCE(A[2*i+17]);                      \
  } while(0)

  UNPACK(0);
  UNPACK(1);
  UNPACK(2);
  UNPACK(3);
  UNPACK(4);
  UNPACK(5);
  UNPACK(6);
  if (final)
    UNPACK_TWEAK(7, FinalTweak.v16);
  else
    UNPACK_TWEAK(7, Tweak.v16);

#undef UNPACK
#undef UNPACK_TWEAK

  fft128(a);
  fft128(a+128);
}


void rounds(u32* state, const unsigned char* msg, short* fft) {
  
  v32* S = (v32*) state;
  const v32* M = (v32*)msg;
  volatile v16* W = (v16*)fft;

  register v32 S0, S1, S2, S3;
  static const union cv code[] = { CV(185), CV(233) };

  S0 = v32_xor(S[0], v32_bswap(M[0]));
  S1 = v32_xor(S[1], v32_bswap(M[1]));
  S2 = v32_xor(S[2], v32_bswap(M[2]));
  S3 = v32_xor(S[3], v32_bswap(M[3]));

#define S(i) S##i


/* #define F_0(B, C, D)     ((((C) ^ (D)) & (B)) ^ (D)) */
/* #define F_1(B, C, D)     (((D) & (C)) | (((D) | (C)) & (B))) */

#define F_0(B, C, D)     v32_xor(v32_and(v32_xor(C,D), B), D)
#define F_1(B, C, D)     v32_or(v32_and(D, C), v32_and( v32_or(D,C), B))

#define F(a,b,c,fun) F_##fun (a,b,c)

  /*
   * We split the round function in two halfes
   * so as to insert some independent computations in between
   */

#define SUM3_00 1
#define SUM3_01 2
#define SUM3_02 3
#define SUM3_10 2
#define SUM3_11 3
#define SUM3_12 1
#define SUM3_20 3
#define SUM3_21 1
#define SUM3_22 2

#define STEP_1(a,b,c,d,w,fun,r,s,z)                             \
  do {                                                          \
    if (PRINT_SOME) {                                           \
      int j;                                                    \
      v32 ww=w, aa=a, bb=b, cc=c, dd=d;                         \
      u32 *WW = (void*)&ww;                                     \
      u32 *AA = (void*)&aa;                                     \
      u32 *BB = (void*)&bb;                                     \
      u32 *CC = (void*)&cc;                                     \
      u32 *DD = (void*)&dd;                                     \
      for (j=0; j<4; j++) {                                     \
        printf ("%08x/%2i/%2i[%i]: %08x %08x %08x %08x\n",      \
                WW[j], r, s, SUM3_##z,                          \
                AA[j], BB[j], CC[j], DD[j]);                    \
      }                                                         \
    }                                                           \
    TT = F(a,b,c,fun);                                          \
    a = v32_rotate(a,r);                                        \
    w = v32_add(w, d);                                          \
    TT = v32_add(TT, w);                                        \
    TT = v32_rotate(TT,s);                                      \
    d = v32_shufxor(a,SUM3_##z);                                \
  } while(0)

#define STEP_2(a,b,c,d,w,fun,r,s)                               \
  do {                                                          \
    d = v32_add(d, TT);                                         \
  } while(0)

#define STEP(a,b,c,d,w,fun,r,s,z)               \
  do {                                          \
    register v32 TT;                            \
    STEP_1(a,b,c,d,w,fun,r,s,z);                \
    STEP_2(a,b,c,d,w,fun,r,s);                  \
  } while(0);


#define ROUND(h0,l0,u0,h1,l1,u1,h2,l2,u2,h3,l3,u3,        \
              fun,r,s,t,u,z,r0)                           \
  do {                                                    \
    register v32 W0, W1, W2, W3, TT;                      \
    W0 = v16_merge##u0(W[h0], W[l0]);                     \
    W0 = V1632(v16_mul(V3216(W0), code[z].v16));          \
    STEP_1(S(0), S(1), S(2), S(3), W0, fun, r, s, r0##0); \
    W1 = v16_merge##u1(W[h1], W[l1]);                     \
    W1 = V1632(v16_mul(V3216(W1), code[z].v16));          \
    STEP_2(S(0), S(1), S(2), S(3), W0, fun, r, s);        \
    STEP_1(S(3), S(0), S(1), S(2), W1, fun, s, t, r0##1); \
    W2 = v16_merge##u2(W[h2], W[l2]);                     \
    W2 = V1632(v16_mul(V3216(W2), code[z].v16));          \
    STEP_2(S(3), S(0), S(1), S(2), W1, fun, s, t);        \
    STEP_1(S(2), S(3), S(0), S(1), W2, fun, t, u, r0##2); \
    W3 = v16_merge##u3(W[h3], W[l3]);                     \
    W3 = V1632(v16_mul(V3216(W3), code[z].v16));          \
    STEP_2(S(2), S(3), S(0), S(1), W2, fun, t, u);        \
    STEP_1(S(1), S(2), S(3), S(0), W3, fun, u, r, r0##0); \
    STEP_2(S(1), S(2), S(3), S(0), W3, fun, u, r);        \
  } while(0)


  /*
   * 4 rounds with code 185
   */
  ROUND(  2, 10, l,  3, 11, l,  0,  8, l,  1,  9, l, 0, 3,  23, 17, 27, 0, 0);
  ROUND(  3, 11, h,  2, 10, h,  1,  9, h,  0,  8, h, 1, 3,  23, 17, 27, 0, 1);
  ROUND(  7, 15, h,  5, 13, h,  6, 14, l,  4, 12, l, 0, 28, 19, 22,  7, 0, 2);
  ROUND(  4, 12, h,  6, 14, h,  5, 13, l,  7, 15, l, 1, 28, 19, 22,  7, 0, 0);

  /*
   * 4 rounds with code 233
   */
  ROUND(  0,  4, h,  1,  5, l,  3,  7, h,  2,  6, l, 0, 29,  9, 15,  5, 1, 1);
  ROUND(  3,  7, l,  2,  6, h,  0,  4, l,  1,  5, h, 1, 29,  9, 15,  5, 1, 2);
  ROUND( 11, 15, l,  8, 12, l,  8, 12, h, 11, 15, h, 0,  4, 13, 10, 25, 1, 0);
  ROUND(  9, 13, h, 10, 14, h, 10, 14, l,  9, 13, l, 1,  4, 13, 10, 25, 1, 1);


  /*
   * 1 round as feed-forward
   */
  STEP(S(0), S(1), S(2), S(3), S[0], 0,  4, 13, 20);
  STEP(S(3), S(0), S(1), S(2), S[1], 0, 13, 10, 21);
  STEP(S(2), S(3), S(0), S(1), S[2], 0, 10, 25, 22);
  STEP(S(1), S(2), S(3), S(0), S[3], 0, 25,  4, 20);

  S[0] = S(0);  S[1] = S(1);  S[2] = S(2);  S[3] = S(3);
}


void rounds512(u32* state, const unsigned char* msg, short* fft) {
  
  v32* S = (v32*) state;
  v32* M = (v32*) msg;
  v16* W = (v16*) fft;

  register v32 S0l, S1l, S2l, S3l;
  register v32 S0h, S1h, S2h, S3h;
  static const union cv code[] = { CV(185), CV(233) };

  S0l = v32_xor(S[0], v32_bswap(M[0]));
  S0h = v32_xor(S[1], v32_bswap(M[1]));
  S1l = v32_xor(S[2], v32_bswap(M[2]));
  S1h = v32_xor(S[3], v32_bswap(M[3]));
  S2l = v32_xor(S[4], v32_bswap(M[4]));
  S2h = v32_xor(S[5], v32_bswap(M[5]));
  S3l = v32_xor(S[6], v32_bswap(M[6]));
  S3h = v32_xor(S[7], v32_bswap(M[7]));

#define S(i) S##i


/* #define F_0(B, C, D)     ((((C) ^ (D)) & (B)) ^ (D)) */
/* #define F_1(B, C, D)     (((D) & (C)) | (((D) | (C)) & (B))) */

#define F_0(B, C, D)     v32_xor(v32_and(v32_xor(C,D), B), D)
#define F_1(B, C, D)     v32_or(v32_and(D, C), v32_and( v32_or(D,C), B))

#define Fl(a,b,c,fun) F_##fun (a##l,b##l,c##l)
#define Fh(a,b,c,fun) F_##fun (a##h,b##h,c##h)

  /*
   * We split the round function in two halfes
   * so as to insert some independent computations in between
   */

#define SUM7_00 0
#define SUM7_01 1
#define SUM7_02 2
#define SUM7_03 3
#define SUM7_04 4
#define SUM7_05 5
#define SUM7_06 6

#define SUM7_10 1
#define SUM7_11 2
#define SUM7_12 3
#define SUM7_13 4
#define SUM7_14 5
#define SUM7_15 6
#define SUM7_16 0
                
#define SUM7_20 2
#define SUM7_21 3
#define SUM7_22 4
#define SUM7_23 5
#define SUM7_24 6
#define SUM7_25 0
#define SUM7_26 1
                
#define SUM7_30 3
#define SUM7_31 4
#define SUM7_32 5
#define SUM7_33 6
#define SUM7_34 0
#define SUM7_35 1
#define SUM7_36 2
                
#define SUM7_40 4
#define SUM7_41 5
#define SUM7_42 6
#define SUM7_43 0
#define SUM7_44 1
#define SUM7_45 2
#define SUM7_46 3
                
#define SUM7_50 5
#define SUM7_51 6
#define SUM7_52 0
#define SUM7_53 1
#define SUM7_54 2
#define SUM7_55 3
#define SUM7_56 4

#define SUM7_60 6
#define SUM7_61 0
#define SUM7_62 1
#define SUM7_63 2
#define SUM7_64 3
#define SUM7_65 4
#define SUM7_66 5

#define PERM(z,d,a) XCAT(PERM_,XCAT(SUM7_##z,PERM_START))(d,a)

#define PERM_0(d,a) /* XOR 1 */           \
  do {                                    \
    d##l = v32_shufxor(a##l,1);           \
    d##h = v32_shufxor(a##h,1);           \
  } while(0)

#define PERM_1(d,a) /* XOR 6 */           \
  do {                                    \
    d##l = v32_shufxor(a##h,2);           \
    d##h = v32_shufxor(a##l,2);           \
  } while(0)

#define PERM_2(d,a) /* XOR 2 */           \
  do {                                    \
    d##l = v32_shufxor(a##l,2);           \
    d##h = v32_shufxor(a##h,2);           \
  } while(0)

#define PERM_3(d,a) /* XOR 3 */           \
  do {                                    \
    d##l = v32_shufxor(a##l,3);           \
    d##h = v32_shufxor(a##h,3);           \
  } while(0)

#define PERM_4(d,a) /* XOR 5 */           \
  do {                                    \
    d##l = v32_shufxor(a##h,1);           \
    d##h = v32_shufxor(a##l,1);           \
  } while(0)

#define PERM_5(d,a) /* XOR 7 */           \
  do {                                    \
    d##l = v32_shufxor(a##h,3);           \
    d##h = v32_shufxor(a##l,3);           \
  } while(0)

#define PERM_6(d,a) /* XOR 4 */           \
  do {                                    \
    d##l = a##h;                          \
    d##h = a##l;                          \
  } while(0)

#define STEP_1_(a,b,c,d,w,fun,r,s,z)                            \
  do {                                                          \
    if (PRINT_SOME) {                                           \
      int j;                                                    \
      v32 ww=w##l, aa=a##l, bb=b##l, cc=c##l, dd=d##l;          \
      u32 *WW = (void*)&ww;                                     \
      u32 *AA = (void*)&aa;                                     \
      u32 *BB = (void*)&bb;                                     \
      u32 *CC = (void*)&cc;                                     \
      u32 *DD = (void*)&dd;                                     \
      for (j=0; j<4; j++) {                                     \
        printf ("%08x/%2i/%2i: %08x %08x %08x %08x\n",          \
                WW[j], r, s,                                    \
                AA[j], BB[j], CC[j], DD[j]);                    \
      }                                                         \
    }                                                           \
    TTl = Fl(a,b,c,fun);                                        \
    TTh = Fh(a,b,c,fun);                                        \
    a##l = v32_rotate(a##l,r);                                  \
    a##h = v32_rotate(a##h,r);                                  \
    w##l  = v32_add(w##l, d##l);                                \
    w##h  = v32_add(w##h, d##h);                                \
    TTl = v32_add(TTl, w##l);                                   \
    TTh = v32_add(TTh, w##h);                                   \
    TTl = v32_rotate(TTl,s);                                    \
    TTh = v32_rotate(TTh,s);                                    \
    PERM(z,d,a);                                                \
  } while(0)

#define STEP_1(a,b,c,d,w,fun,r,s,z)             \
  STEP_1_(a,b,c,d,w,fun,r,s,z)

#define STEP_2_(a,b,c,d,w,fun,r,s)                               \
  do {                                                          \
    d##l = v32_add(d##l, TTl);                                  \
    d##h = v32_add(d##h, TTh);                                  \
  } while(0)

#define STEP_2(a,b,c,d,w,fun,r,s)              \
  STEP_2_(a,b,c,d,w,fun,r,s)
  
#define STEP(a,b,c,d,w1,w2,fun,r,s,z)           \
  do {                                          \
    register v32 TTl, TTh, Wl=w1, Wh=w2;        \
    STEP_1(a,b,c,d,W,fun,r,s,z);                \
    STEP_2(a,b,c,d,W,fun,r,s);                  \
  } while(0);


#define MSG_l(x) (2*(x))
#define MSG_h(x) (2*(x)+1)

#define MSG(w,hh,ll,u,z)                                \
  do {                                                  \
    int a = MSG_##u(hh);                                \
    int b = MSG_##u(ll);                                \
    w##l = v16_mergel(W[a], W[b]);                      \
    w##l = V1632(v16_mul(V3216(w##l), code[z].v16));    \
    w##h = v16_mergeh(W[a], W[b]);                      \
    w##h = V1632(v16_mul(V3216(w##h), code[z].v16));    \
  } while(0)
  
#define ROUND(h0,l0,u0,h1,l1,u1,h2,l2,u2,h3,l3,u3,        \
              fun,r,s,t,u,z)                              \
  do {                                                    \
    register v32 W0l, W1l, W2l, W3l, TTl;                 \
    register v32 W0h, W1h, W2h, W3h, TTh;                 \
    MSG(W0,h0,l0,u0,z);                                   \
    STEP_1(S(0), S(1), S(2), S(3), W0, fun, r, s, 0);     \
    MSG(W1,h1,l1,u1,z);                                   \
    STEP_2(S(0), S(1), S(2), S(3), W0, fun, r, s);        \
    STEP_1(S(3), S(0), S(1), S(2), W1, fun, s, t, 1);     \
    MSG(W2,h2,l2,u2,z);                                   \
    STEP_2(S(3), S(0), S(1), S(2), W1, fun, s, t);        \
    STEP_1(S(2), S(3), S(0), S(1), W2, fun, t, u, 2);     \
    MSG(W3,h3,l3,u3,z);                                   \
    STEP_2(S(2), S(3), S(0), S(1), W2, fun, t, u);        \
    STEP_1(S(1), S(2), S(3), S(0), W3, fun, u, r, 3);     \
    STEP_2(S(1), S(2), S(3), S(0), W3, fun, u, r);        \
  } while(0)


  /*
   * 4 rounds with code 185
   */
#define PERM_START 0
  ROUND(  2, 10, l,  3, 11, l,  0,  8, l,  1,  9, l, 0, 3,  23, 17, 27, 0);
#define PERM_START 4
  ROUND(  3, 11, h,  2, 10, h,  1,  9, h,  0,  8, h, 1, 3,  23, 17, 27, 0);
#define PERM_START 1
  ROUND(  7, 15, h,  5, 13, h,  6, 14, l,  4, 12, l, 0, 28, 19, 22, 7,  0);
#define PERM_START 5
  ROUND(  4, 12, h,  6, 14, h,  5, 13, l,  7, 15, l, 1, 28, 19, 22, 7,  0);

  /*
   * 4 rounds with code 233
   */
#define PERM_START 2
  ROUND(  0,  4, h,  1,  5, l,  3,  7, h,  2,  6, l, 0, 29,  9, 15,  5, 1);
#define PERM_START 6
  ROUND(  3,  7, l,  2,  6, h,  0,  4, l,  1,  5, h, 1, 29,  9, 15,  5, 1);
#define PERM_START 3
  ROUND( 11, 15, l,  8, 12, l,  8, 12, h, 11, 15, h, 0,  4, 13, 10, 25, 1);
#define PERM_START 0
  ROUND(  9, 13, h, 10, 14, h, 10, 14, l,  9, 13, l, 1,  4, 13, 10, 25, 1);


  /*
   * 1 round as feed-forward
   */
#define PERM_START 4
  STEP(S(0), S(1), S(2), S(3), S[0], S[1], 0,  4, 13, 0);
  STEP(S(3), S(0), S(1), S(2), S[2], S[3], 0, 13, 10, 1);
  STEP(S(2), S(3), S(0), S(1), S[4], S[5], 0, 10, 25, 2);
  STEP(S(1), S(2), S(3), S(0), S[6], S[7], 0, 25,  4, 3);

  S[0] = S0l;  S[1] = S0h;  S[2] = S1l;  S[3] = S1h;
  S[4] = S2l;  S[5] = S2h;  S[6] = S3l;  S[7] = S3h;
}

void SIMD_Compress(hashState_sd * state, const unsigned char *m, int final) {
  if (state->hashbitlen <= 256) {
    union cv Y[16];
    short* y = (short*) Y[0].u16;

#ifdef v16_broadcast
    if (final == 2) {
      fft128_msg_final(y, m);
      rounds(state->A, m, y);
    } else {
      fft128_msg(y, m, final);
      rounds(state->A, m, y);
    }
#else
    fft128_msg(y, m, final);
    rounds(state->A, m, y);
#endif
  } else {
    union cv Y[32];
    short* y = (short*) Y[0].u16;
    
    fft256_msg(y, m, final);
    rounds512(state->A, m, y);
  }
}

/* 
 * Give the FFT output in the regular order for consitancy checks
 */
void fft128_natural(fft_t *x, unsigned char *a) {
  union cv Y[16];
  short* y = (short*) Y[0].u16;
  int i;

  fft128_msg(y, a, 0);

  for(i=0; i<64; i++) {
    x[2*i]   = y[i];
    x[2*i+1] = y[i+64];
  }
}
