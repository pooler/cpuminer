#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "compat.h"

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)

/******************************* 
 * Using GCC vector extensions * 
 *******************************/

#if   defined(__SSE2__)

//typedef unsigned char v16qi __attribute__ ((vector_size (16)));
typedef char          v16qi __attribute__ ((vector_size (16)));
typedef short          v8hi __attribute__ ((vector_size (16)));
typedef int            v4si __attribute__ ((vector_size (16)));
typedef float          v4sf __attribute__ ((vector_size (16)));
typedef long long int  v2di __attribute__ ((vector_size (16)));

typedef short          v4hi __attribute__ ((vector_size (8)));
typedef unsigned char  v8qi __attribute__ ((vector_size (8)));

typedef v16qi v8;
typedef v8hi v16;
typedef v4si v32;
#define V16_SIZE 8

union cv {
  unsigned short u16[8];
  v16 v16;
};

union cv8 {
  unsigned char u8[16];
  v8 v8;
};

union u32 {
  u32 u[4];
  v32 v;
};

#define V3216(x) ((v16) (x))
#define V1632(x) ((v32) (x))
#define  V168(x) ( (v8) (x))
#define  V816(x) ((v16) (x))

#if 0
/* These instruction are shorter than the PAND/POR/... that GCC uses */

#define vec_and(x,y)  ({v16 a = (v16) x; v16 b = (v16) y;  __builtin_ia32_andps ((v4sf) a, (v4sf) b);})
#define vec_or(x,y)   ({v16 a = (v16) x; v16 b = (v16) y;  __builtin_ia32_orps ((v4sf) a, (v4sf) b);})
#define vec_xor(x,y)  ({v16 a = (v16) x; v16 b = (v16) y;  __builtin_ia32_xorps ((v4sf) a, (v4sf) b);})
#define vec_andn(x,y) ({v16 a = (v16) x; v16 b = (v16) y;  __builtin_ia32_andnps ((v4sf) a, (v4sf) b);})

#define v16_and(x,y)  ((v16) vec_and ((x), (y)))
#define v16_or(x,y)   ((v16) vec_or  ((x), (y)))
#define v16_xor(x,y)  ((v16) vec_xor ((x), (y)))
#define v16_andn(x,y) ((v16) vec_andn((x), (y)))

#define v32_and(x,y)  ((v32) vec_and ((x), (y)))
#define v32_or(x,y)   ((v32) vec_or  ((x), (y)))
#define v32_xor(x,y)  ((v32) vec_xor ((x), (y)))
#define v32_andn(x,y) ((v32) vec_andn((x), (y)))
#endif

#define vec_and(x,y) ((x)&(y))
#define vec_or(x,y)  ((x)|(y))
#define vec_xor(x,y) ((x)^(y))

#define v16_and vec_and
#define v16_or  vec_or
#define v16_xor vec_xor

#define v32_and vec_and
#define v32_or  vec_or
#define v32_xor vec_xor

#define vec_andn(x,y) __builtin_ia32_pandn128 ((v2di) x, (v2di) y)
#define v16_andn(x,y) ((v16) vec_andn(x,y))
#define v32_andn(x,y) ((v32) vec_andn(x,y))

#define v32_add(x,y) ((x)+(y))

#define v16_add(x,y) ((x)+(y))
#define v16_sub(x,y) ((x)-(y))
#define v16_mul(x,y) ((x)*(y))
#define v16_neg(x)   (-(x))
#define v16_shift_l  __builtin_ia32_psllwi128
#define v16_shift_r  __builtin_ia32_psrawi128
#define v16_cmp      __builtin_ia32_pcmpgtw128

#define v16_interleavel   __builtin_ia32_punpcklwd128
#define v16_interleaveh   __builtin_ia32_punpckhwd128

#define v16_mergel(a,b)   V1632(__builtin_ia32_punpcklwd128(a,b))
#define v16_mergeh(a,b)   V1632(__builtin_ia32_punpckhwd128(a,b))

#define v8_mergel(a,b) V816(__builtin_ia32_punpcklbw128(a,b))
#define v8_mergeh(a,b) V816(__builtin_ia32_punpckhbw128(a,b))

#define v32_shift_l  __builtin_ia32_pslldi128
#define v32_shift_r  __builtin_ia32_psrldi128

#define v32_rotate(x,n)                                 \
  v32_or(v32_shift_l(x,n), v32_shift_r(x,32-(n)))

#define v32_shuf __builtin_ia32_pshufd

#define SHUFXOR_1 0xb1          /* 0b10110001 */
#define SHUFXOR_2 0x4e          /* 0b01001110 */
#define SHUFXOR_3 0x1b          /* 0b00011011 */

#define CAT(x, y) x##y
#define XCAT(x,y) CAT(x,y)

#define v32_shufxor(x,s) v32_shuf(x,XCAT(SHUFXOR_,s))

#define v32_bswap(x) (x)

#define v16_broadcast(x) ({                     \
      union u32 u;                              \
      u32 xx = x;                               \
      u.u[0] = xx | (xx << 16);                 \
      V3216(v32_shuf(u.v,0)); })

#define CV(x) {{x, x, x, x, x, x, x, x}}

#elif defined(__ALTIVEC__)

#include <altivec.h>

typedef vector unsigned char  v8;
typedef vector signed   short v16;
typedef vector unsigned int   v32;

#define V3216(x) ((v16) (x))
#define V1632(x) ((v32) (x))
#define  V168(x) ( (v8) (x))
#define  V816(x) ((v16) (x))

#define V16_SIZE 8
#define print_vec print_sse

#define MAKE_VECT(x, ...) {{x, __VA_ARGS__}}

#define CV(x) MAKE_VECT(x, x, x, x, x, x, x, x)
#define CV16(x)  ((vector   signed short) {x,x,x,x,x,x,x,x})
#define CVU16(x) ((vector unsigned short) {x,x,x,x,x,x,x,x})
#define CV32(x)  ((vector unsigned int  ) {x,x,x,x})

union cv {
  unsigned short u16[8];
  v16 v16;
};

union cv8 {
  unsigned char u8[16];
  v8 v8;
};

union ucv {
  unsigned short u16[8];
  vector unsigned char v16;
};

// Nasty hack to avoid macro expansion madness


/* altivec.h is broken with Gcc 3.3 is C99 mode  */
#if defined __STDC__ && __STDC_VERSION__ >= 199901L
#define typeof __typeof
#endif

MAYBE_INLINE v16 vec_and_fun (v16 x, v16 y) {
  return vec_and (x, y);
}

MAYBE_INLINE v16 vec_or_fun (v16 x, v16 y) {
  return vec_or (x, y);
}

MAYBE_INLINE v16 vec_xor_fun (v16 x, v16 y) {
  return vec_xor (x, y);
}

#undef vec_and
#undef vec_or
#undef vec_xor

#define vec_and(x,y) ((__typeof(x)) vec_and_fun((v16) x, (v16) y))
#define vec_or(x,y)  ((__typeof(x)) vec_or_fun((v16) x, (v16) y))
#define vec_xor(x,y) ((__typeof(x)) vec_xor_fun((v16) x, (v16) y))


#define v16_and vec_and
#define v16_or  vec_or
#define v16_xor vec_xor

#define v32_and vec_and
#define v32_or  vec_or
#define v32_xor vec_xor


#define v32_add vec_add

#define v16_add vec_add
#define v16_sub vec_sub
#define v16_mul(a,b) vec_mladd(a,b,CV16(0))

vector unsigned   short ZZ = {0,0,0,0,0,0,0,0};

v16 v16_shift_l(v16 x,int s) {
  vector unsigned short shift = {s,s,s,s,s,s,s,s};
  v16 y = vec_sl (x, shift);
  return y;
}
#define v16_shift_l(x,s)  vec_sl (x,CVU16(s))
#define v16_shift_r(x,s)  vec_sra(x,CVU16(s))
#define v16_cmp      vec_cmpgt

#define v16_mergel(a,b)   V1632(vec_mergeh(b,a))
#define v16_mergeh(a,b)   V1632(vec_mergel(b,a))

#define v16_interleavel(a,b)   vec_mergeh(a,b)
#define v16_interleaveh(a,b)   vec_mergel(a,b)

#define v8_mergel(a,b) V816(vec_mergeh(b,a))
#define v8_mergeh(a,b) V816(vec_mergel(b,a))

#define v32_rotate(x,s)  vec_rl(x,CV32(s))

// #define v32_unpckl   vec_mergel
// #define v32_unpckh   vec_mergeh

#define vector_shuffle(x,s) vec_perm(x,x,s)

static const v8 SHUFXOR_1 = {4,5,6,7,0,1,2,3,12,13,14,15,8,9,10,11};
static const v8 SHUFXOR_2 = {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7};
static const v8 SHUFXOR_3 = {12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3};

#define v32_shufxor(x,s) vector_shuffle(x,SHUFXOR_##s)

//static const v8 SHUFSWAP = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
static const v8 SHUFSWAP = {3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12};

#define v32_bswap(x) vector_shuffle(x,SHUFSWAP)

#else

#error "I don't know how to vectorize on this architecture."

#endif

#else

/******************************** 
 * Using MSVC/ICC vector instrinsics * 
 ********************************/

#include <emmintrin.h>

typedef __m128i  v8;
typedef __m128i v16;
typedef __m128i v32;

#define V3216(x) (x)
#define V1632(x) (x)
#define  V168(x) (x)
#define  V816(x) (x)

#define V16_SIZE 8

union cv {
  unsigned short u16[8];
  v16 v16;
};

union cv8 {
  unsigned char u8[16];
  v8 v8;
};

#define CV(x) {{x, x, x, x, x, x, x, x}}

#define vec_and      _mm_and_si128
#define vec_or       _mm_or_si128
#define vec_xor      _mm_xor_si128

#define v16_and vec_and
#define v16_or  vec_or
#define v16_xor vec_xor

#define v32_and vec_and
#define v32_or  vec_or
#define v32_xor vec_xor

#define vector_shuffle(x,s) _mm_shuffle_epi8(x, s)

#define v32_add      _mm_add_epi32

#define v16_add      _mm_add_epi16
#define v16_sub      _mm_sub_epi16
#define v16_mul      _mm_mullo_epi16
#define v16_neg(x)   (-(x))
#define v16_shift_l  _mm_slli_epi16
#define v16_shift_r  _mm_srai_epi16
#define v16_cmp      _mm_cmpgt_epi16

#define v16_interleavel   _mm_unpacklo_epi16
#define v16_interleaveh   _mm_unpackhi_epi16

#define v16_mergel   _mm_unpacklo_epi16
#define v16_mergeh   _mm_unpackhi_epi16

#define v8_mergel    _mm_unpacklo_epi8
#define v8_mergeh    _mm_unpackhi_epi8

#define v32_shift_l  _mm_slli_epi32
#define v32_shift_r  _mm_srli_epi32

#define v32_rotate(x,n)                                 \
  vec_or(v32_shift_l(x,n), v32_shift_r(x,32-(n)))

#define v32_shuf     _mm_shuffle_epi32

#define SHUFXOR_1 0xb1          /* 0b10110001 */
#define SHUFXOR_2 0x4e          /* 0b01001110 */
#define SHUFXOR_3 0x1b          /* 0b00011011 */

#define CAT(x, y) x##y
#define XCAT(x,y) CAT(x,y)

//#define v32_shufxor(x,s) v32_shuf(x,SHUFXOR_##s)
#define v32_shufxor(x,s) v32_shuf(x,XCAT(SHUFXOR_,s))

#define v32_bswap(x) (x)

#endif

/* Twiddle tables */

  static const union cv FFT64_Twiddle[] = {
    {{1,    2,    4,    8,   16,   32,   64,  128}},
    {{1,   60,    2,  120,    4,  -17,    8,  -34}},
    {{1,  120,    8,  -68,   64,  -30,   -2,   17}},
    {{1,   46,   60,  -67,    2,   92,  120,  123}},
    {{1,   92,  -17,  -22,   32,  117,  -30,   67}},
    {{1,  -67,  120,  -73,    8,  -22,  -68,  -70}},
    {{1,  123,  -34,  -70,  128,   67,   17,   35}},
  };


  static const union cv FFT128_Twiddle[] =  {
    {{  1, -118,   46,  -31,   60,  116,  -67,  -61}},
    {{  2,   21,   92,  -62,  120,  -25,  123, -122}},
    {{  4,   42,  -73, -124,  -17,  -50,  -11,   13}},
    {{  8,   84,  111,    9,  -34, -100,  -22,   26}},
    {{ 16,  -89,  -35,   18,  -68,   57,  -44,   52}},
    {{ 32,   79,  -70,   36,  121,  114,  -88,  104}},
    {{ 64,  -99,  117,   72,  -15,  -29,   81,  -49}},
    {{128,   59,  -23, -113,  -30,  -58,  -95,  -98}},
  };


  static const union cv FFT256_Twiddle[] =  {
    {{   1,   41, -118,   45,   46,   87,  -31,   14}}, 
    {{  60, -110,  116, -127,  -67,   80,  -61,   69}}, 
    {{   2,   82,   21,   90,   92,  -83,  -62,   28}}, 
    {{ 120,   37,  -25,    3,  123,  -97, -122, -119}}, 
    {{   4,  -93,   42,  -77,  -73,   91, -124,   56}}, 
    {{ -17,   74,  -50,    6,  -11,   63,   13,   19}}, 
    {{   8,   71,   84,  103,  111,  -75,    9,  112}}, 
    {{ -34, -109, -100,   12,  -22,  126,   26,   38}}, 
    {{  16, -115,  -89,  -51,  -35,  107,   18,  -33}}, 
    {{ -68,   39,   57,   24,  -44,   -5,   52,   76}}, 
    {{  32,   27,   79, -102,  -70,  -43,   36,  -66}}, 
    {{ 121,   78,  114,   48,  -88,  -10,  104, -105}}, 
    {{  64,   54,  -99,   53,  117,  -86,   72,  125}}, 
    {{ -15, -101,  -29,   96,   81,  -20,  -49,   47}}, 
    {{ 128,  108,   59,  106,  -23,   85, -113,   -7}}, 
    {{ -30,   55,  -58,  -65,  -95,  -40,  -98,   94}}
  };




#endif
