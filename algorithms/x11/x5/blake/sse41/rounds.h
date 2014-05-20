
#ifndef __BLAKE512_ROUNDS_H__
#define __BLAKE512_ROUNDS_H__

#ifndef HAVE_XOP
	#define BSWAP64(x) _mm_shuffle_epi8((x), u8to64)

	#define _mm_roti_epi64(x, c) \
	(-(c) == 32) ? _mm_shuffle_epi32((x), _MM_SHUFFLE(2,3,0,1))  \
	: (-(c) == 16) ? _mm_shuffle_epi8((x), r16) \
		: _mm_xor_si128(_mm_srli_epi64((x), -(c)), _mm_slli_epi64((x), 64-(-c))) 
#else
	#define BSWAP64(x) _mm_perm_epi8((x),(x),u8to64)
#endif


#define LOAD_MSG_0_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m0, m1); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x13198A2E03707344ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m2, m3); \
t3 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0xBE5466CF34E90C6CULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_0_2(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m0, m1); \
t1 = _mm_set_epi64x(0xA4093822299F31D0ULL, 0x243F6A8885A308D3ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m2, m3); \
t3 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_0_3(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m4, m5); \
t1 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0xD1310BA698DFB5ACULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m6, m7); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0x24A19947B3916CF7ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_0_4(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m4, m5); \
t1 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0x9216D5D98979FB1BULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m6, m7); \
t3 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xBA7C9045F12C7F99ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_1_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m7, m2); \
t1 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x2FFD72DBD01ADFB7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m4, m6); \
t3 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x636920D871574E69ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_1_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m5, m4); \
t1 = _mm_set_epi64x(0x452821E638D01377ULL, 0x801F2E2858EFC16ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m3, m7, 8); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0xD1310BA698DFB5ACULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_1_3(b0, b1) \
do \
{ \
t0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(1,0,3,2)); \
t1 = _mm_set_epi64x(0xA4093822299F31D0ULL, 0xBA7C9045F12C7F99ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m5, m2); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x3F84D5B5B5470917ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_1_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m6, m1); \
t1 = _mm_set_epi64x(0x243F6A8885A308D3ULL, 0x13198A2E03707344ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m3, m1); \
t3 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xB8E1AFED6A267E96ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_2_1(b0, b1) \
do \
{ \
t0 = _mm_alignr_epi8(m6, m5, 8); \
t1 = _mm_set_epi64x(0x243F6A8885A308D3ULL, 0x9216D5D98979FB1BULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m2, m7); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0xA4093822299F31D0ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_2_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m4, m0); \
t1 = _mm_set_epi64x(0xBA7C9045F12C7F99ULL, 0xB8E1AFED6A267E96ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m1, m6, 0xF0); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0xBE5466CF34E90C6CULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_2_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m5, m1, 0xF0); \
t1 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x801F2E2858EFC16ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m3, m4); \
t3 = _mm_set_epi64x(0x452821E638D01377ULL, 0x13198A2E03707344ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_2_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m7, m3); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x2FFD72DBD01ADFB7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m2, m0, 8); \
t3 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x3F84D5B5B5470917ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_3_1(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m3, m1); \
t1 = _mm_set_epi64x(0x13198A2E03707344ULL, 0xD1310BA698DFB5ACULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m6, m5); \
t3 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xBA7C9045F12C7F99ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_3_2(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m4, m0); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x3F84D5B5B5470917ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m6, m7); \
t3 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0x24A19947B3916CF7ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_3_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m1, m2, 0xF0); \
t1 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xC0AC29B7C97C50DDULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m2, m7, 0xF0); \
t3 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x243F6A8885A308D3ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_3_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m3, m5); \
t1 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xA4093822299F31D0ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m0, m4); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_4_1(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m4, m2); \
t1 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0x243F6A8885A308D3ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m1, m5); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_4_2(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m0, m3, 0xF0); \
t1 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xD1310BA698DFB5ACULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m2, m7, 0xF0); \
t3 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xA4093822299F31D0ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_4_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m7, m5, 0xF0); \
t1 = _mm_set_epi64x(0xBA7C9045F12C7F99ULL, 0x13198A2E03707344ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m3, m1, 0xF0); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0x9216D5D98979FB1BULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_4_4(b0, b1) \
do \
{ \
t0 = _mm_alignr_epi8(m6, m0, 8); \
t1 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0x801F2E2858EFC16ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m4, m6, 0xF0); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0xC0AC29B7C97C50DDULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_5_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m1, m3); \
t1 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xBA7C9045F12C7F99ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m0, m4); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0xB8E1AFED6A267E96ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_5_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m6, m5); \
t1 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0xA4093822299F31D0ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m5, m1); \
t3 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x243F6A8885A308D3ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_5_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m2, m3, 0xF0); \
t1 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0x24A19947B3916CF7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m7, m0); \
t3 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x801F2E2858EFC16ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_5_4(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m6, m2); \
t1 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0x452821E638D01377ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m7, m4, 0xF0); \
t3 = _mm_set_epi64x(0x13198A2E03707344ULL, 0x636920D871574E69ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_6_1(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m6, m0, 0xF0); \
t1 = _mm_set_epi64x(0x636920D871574E69ULL, 0xBE5466CF34E90C6CULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m7, m2); \
t3 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0x24A19947B3916CF7ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_6_2(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m2, m7); \
t1 = _mm_set_epi64x(0x13198A2E03707344ULL, 0xBA7C9045F12C7F99ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m5, m6, 8); \
t3 = _mm_set_epi64x(0x452821E638D01377ULL, 0x801F2E2858EFC16ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_6_3(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m0, m3); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x3F84D5B5B5470917ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_shuffle_epi32(m4, _MM_SHUFFLE(1,0,3,2)); \
t3 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0xA4093822299F31D0ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_6_4(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m3, m1); \
t1 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x243F6A8885A308D3ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m1, m5, 0xF0); \
t3 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0xD1310BA698DFB5ACULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_7_1(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m6, m3); \
t1 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xB8E1AFED6A267E96ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m6, m1, 0xF0); \
t3 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x13198A2E03707344ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_7_2(b0, b1) \
do \
{ \
t0 = _mm_alignr_epi8(m7, m5, 8); \
t1 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0x24A19947B3916CF7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m0, m4); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0xBA7C9045F12C7F99ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_7_3(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m2, m7); \
t1 = _mm_set_epi64x(0x452821E638D01377ULL, 0x243F6A8885A308D3ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m4, m1); \
t3 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xC0AC29B7C97C50DDULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_7_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m0, m2); \
t1 = _mm_set_epi64x(0x636920D871574E69ULL, 0xBE5466CF34E90C6CULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m3, m5); \
t3 = _mm_set_epi64x(0xA4093822299F31D0ULL, 0x9216D5D98979FB1BULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_8_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m3, m7); \
t1 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x636920D871574E69ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m0, m5, 8); \
t3 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x82EFA98EC4E6C89ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_8_2(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m7, m4); \
t1 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xC0AC29B7C97C50DDULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m4, m1, 8); \
t3 = _mm_set_epi64x(0x243F6A8885A308D3ULL, 0xB8E1AFED6A267E96ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_8_3(b0, b1) \
do \
{ \
t0 = m6; \
t1 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0xA4093822299F31D0ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m5, m0, 8); \
t3 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_8_4(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m1, m3, 0xF0); \
t1 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0xBA7C9045F12C7F99ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = m2; \
t3 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0x13198A2E03707344ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_9_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m5, m4); \
t1 = _mm_set_epi64x(0x452821E638D01377ULL, 0xA4093822299F31D0ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m3, m0); \
t3 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xC0AC29B7C97C50DDULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_9_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m1, m2); \
t1 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x2FFD72DBD01ADFB7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m3, m2, 0xF0); \
t3 = _mm_set_epi64x(0x13198A2E03707344ULL, 0x3F84D5B5B5470917ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_9_3(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m7, m4); \
t1 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xB8E1AFED6A267E96ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m1, m6); \
t3 = _mm_set_epi64x(0x243F6A8885A308D3ULL, 0xBA7C9045F12C7F99ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_9_4(b0, b1) \
do \
{ \
t0 = _mm_alignr_epi8(m7, m5, 8); \
t1 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x636920D871574E69ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m6, m0); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0x82EFA98EC4E6C89ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_10_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m0, m1); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x13198A2E03707344ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m2, m3); \
t3 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0xBE5466CF34E90C6CULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_10_2(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m0, m1); \
t1 = _mm_set_epi64x(0xA4093822299F31D0ULL, 0x243F6A8885A308D3ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m2, m3); \
t3 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_10_3(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m4, m5); \
t1 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0xD1310BA698DFB5ACULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m6, m7); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0x24A19947B3916CF7ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_10_4(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m4, m5); \
t1 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0x9216D5D98979FB1BULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m6, m7); \
t3 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xBA7C9045F12C7F99ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_11_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m7, m2); \
t1 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x2FFD72DBD01ADFB7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m4, m6); \
t3 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x636920D871574E69ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_11_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m5, m4); \
t1 = _mm_set_epi64x(0x452821E638D01377ULL, 0x801F2E2858EFC16ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m3, m7, 8); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0xD1310BA698DFB5ACULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_11_3(b0, b1) \
do \
{ \
t0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(1,0,3,2)); \
t1 = _mm_set_epi64x(0xA4093822299F31D0ULL, 0xBA7C9045F12C7F99ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m5, m2); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x3F84D5B5B5470917ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_11_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m6, m1); \
t1 = _mm_set_epi64x(0x243F6A8885A308D3ULL, 0x13198A2E03707344ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m3, m1); \
t3 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xB8E1AFED6A267E96ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_12_1(b0, b1) \
do \
{ \
t0 = _mm_alignr_epi8(m6, m5, 8); \
t1 = _mm_set_epi64x(0x243F6A8885A308D3ULL, 0x9216D5D98979FB1BULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m2, m7); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0xA4093822299F31D0ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_12_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m4, m0); \
t1 = _mm_set_epi64x(0xBA7C9045F12C7F99ULL, 0xB8E1AFED6A267E96ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m1, m6, 0xF0); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0xBE5466CF34E90C6CULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_12_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m5, m1, 0xF0); \
t1 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0x801F2E2858EFC16ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m3, m4); \
t3 = _mm_set_epi64x(0x452821E638D01377ULL, 0x13198A2E03707344ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_12_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m7, m3); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x2FFD72DBD01ADFB7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_alignr_epi8(m2, m0, 8); \
t3 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x3F84D5B5B5470917ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_13_1(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m3, m1); \
t1 = _mm_set_epi64x(0x13198A2E03707344ULL, 0xD1310BA698DFB5ACULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m6, m5); \
t3 = _mm_set_epi64x(0x801F2E2858EFC16ULL, 0xBA7C9045F12C7F99ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_13_2(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m4, m0); \
t1 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0x3F84D5B5B5470917ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m6, m7); \
t3 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0x24A19947B3916CF7ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_13_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m1, m2, 0xF0); \
t1 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xC0AC29B7C97C50DDULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m2, m7, 0xF0); \
t3 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x243F6A8885A308D3ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_13_4(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m3, m5); \
t1 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xA4093822299F31D0ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m0, m4); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_14_1(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m4, m2); \
t1 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0x243F6A8885A308D3ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m1, m5); \
t3 = _mm_set_epi64x(0x636920D871574E69ULL, 0x452821E638D01377ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_14_2(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m0, m3, 0xF0); \
t1 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0xD1310BA698DFB5ACULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m2, m7, 0xF0); \
t3 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xA4093822299F31D0ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_14_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m7, m5, 0xF0); \
t1 = _mm_set_epi64x(0xBA7C9045F12C7F99ULL, 0x13198A2E03707344ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m3, m1, 0xF0); \
t3 = _mm_set_epi64x(0x24A19947B3916CF7ULL, 0x9216D5D98979FB1BULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_14_4(b0, b1) \
do \
{ \
t0 = _mm_alignr_epi8(m6, m0, 8); \
t1 = _mm_set_epi64x(0xB8E1AFED6A267E96ULL, 0x801F2E2858EFC16ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m4, m6, 0xF0); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0xC0AC29B7C97C50DDULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_15_1(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m1, m3); \
t1 = _mm_set_epi64x(0x2FFD72DBD01ADFB7ULL, 0xBA7C9045F12C7F99ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpacklo_epi64(m0, m4); \
t3 = _mm_set_epi64x(0x82EFA98EC4E6C89ULL, 0xB8E1AFED6A267E96ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_15_2(b0, b1) \
do \
{ \
t0 = _mm_unpacklo_epi64(m6, m5); \
t1 = _mm_set_epi64x(0xC0AC29B7C97C50DDULL, 0xA4093822299F31D0ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m5, m1); \
t3 = _mm_set_epi64x(0x9216D5D98979FB1BULL, 0x243F6A8885A308D3ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_15_3(b0, b1) \
do \
{ \
t0 = _mm_blend_epi16(m2, m3, 0xF0); \
t1 = _mm_set_epi64x(0xBE5466CF34E90C6CULL, 0x24A19947B3916CF7ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_unpackhi_epi64(m7, m0); \
t3 = _mm_set_epi64x(0xD1310BA698DFB5ACULL, 0x801F2E2858EFC16ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 


#define LOAD_MSG_15_4(b0, b1) \
do \
{ \
t0 = _mm_unpackhi_epi64(m6, m2); \
t1 = _mm_set_epi64x(0x3F84D5B5B5470917ULL, 0x452821E638D01377ULL); \
b0 = _mm_xor_si128(t0, t1); \
t2 = _mm_blend_epi16(m7, m4, 0xF0); \
t3 = _mm_set_epi64x(0x13198A2E03707344ULL, 0x636920D871574E69ULL); \
b1 = _mm_xor_si128(t2, t3); \
} while(0) 






#define G1(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1) \
  row1l = _mm_add_epi64(_mm_add_epi64(row1l, b0), row2l); \
  row1h = _mm_add_epi64(_mm_add_epi64(row1h, b1), row2h); \
  \
  row4l = _mm_xor_si128(row4l, row1l); \
  row4h = _mm_xor_si128(row4h, row1h); \
  \
  row4l = _mm_roti_epi64(row4l, -32); \
  row4h = _mm_roti_epi64(row4h, -32); \
  \
  row3l = _mm_add_epi64(row3l, row4l); \
  row3h = _mm_add_epi64(row3h, row4h); \
  \
  row2l = _mm_xor_si128(row2l, row3l); \
  row2h = _mm_xor_si128(row2h, row3h); \
  \
  row2l = _mm_roti_epi64(row2l, -25); \
  row2h = _mm_roti_epi64(row2h, -25); \

#define G2(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1) \
  row1l = _mm_add_epi64(_mm_add_epi64(row1l, b0), row2l); \
  row1h = _mm_add_epi64(_mm_add_epi64(row1h, b1), row2h); \
  \
  row4l = _mm_xor_si128(row4l, row1l); \
  row4h = _mm_xor_si128(row4h, row1h); \
  \
  row4l = _mm_roti_epi64(row4l, -16); \
  row4h = _mm_roti_epi64(row4h, -16); \
  \
  row3l = _mm_add_epi64(row3l, row4l); \
  row3h = _mm_add_epi64(row3h, row4h); \
  \
  row2l = _mm_xor_si128(row2l, row3l); \
  row2h = _mm_xor_si128(row2h, row3h); \
  \
  row2l = _mm_roti_epi64(row2l, -11); \
  row2h = _mm_roti_epi64(row2h, -11); \


#define DIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h) \
	t0 = _mm_alignr_epi8(row2h, row2l, 8); \
	t1 = _mm_alignr_epi8(row2l, row2h, 8); \
	row2l = t0; \
	row2h = t1; \
	\
	t0 = row3l; \
	row3l = row3h; \
	row3h = t0;    \
	\
	t0 = _mm_alignr_epi8(row4h, row4l, 8); \
	t1 = _mm_alignr_epi8(row4l, row4h, 8); \
	row4l = t1; \
	row4h = t0; 

#define UNDIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h) \
	t0 = _mm_alignr_epi8(row2l, row2h, 8); \
	t1 = _mm_alignr_epi8(row2h, row2l, 8); \
	row2l = t0; \
	row2h = t1; \
	\
	t0 = row3l; \
	row3l = row3h; \
	row3h = t0; \
	\
	t0 = _mm_alignr_epi8(row4l, row4h, 8); \
	t1 = _mm_alignr_epi8(row4h, row4l, 8); \
	row4l = t1; \
	row4h = t0; 

#define ROUND(r) \
  LOAD_MSG_ ##r ##_1(b0, b1); \
  G1(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  LOAD_MSG_ ##r ##_2(b0, b1); \
  G2(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  DIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h); \
  LOAD_MSG_ ##r ##_3(b0, b1); \
  G1(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  LOAD_MSG_ ##r ##_4(b0, b1); \
  G2(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  UNDIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h);

#endif

