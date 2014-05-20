/*
 * file        : bitslice.c
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * Constant declarations for bitsliced AES s-box
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */
#include "bitsliceaes.h"


MYALIGN const unsigned int _BS0[] = {0x55555555, 0x55555555, 0x55555555, 0x55555555};
MYALIGN const unsigned int _BS1[] = {0x33333333, 0x33333333, 0x33333333, 0x33333333};
MYALIGN const unsigned int _BS2[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
MYALIGN const unsigned int _ONE[] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

