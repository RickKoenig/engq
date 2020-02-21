//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//

//------------------------------------------
//  related cmp
//------------------------------------------
#define CMP_MODE_EQ         0
#define CMP_MODE_NE         1
#define CMP_MODE_LT         2
#define CMP_MODE_LE         3
#define CMP_MODE_GT         4
#define CMP_MODE_GE         5

//------------------------------------------
//  related condition
//------------------------------------------
#define COND_MODE_OR        0
#define COND_MODE_AND       1
#define COND_MODE_STA0      2
#define COND_MODE_STA1      3

//------------------------------------------
//  related status reg
//------------------------------------------
#define STAT0_0             0
#define STAT0_1             1
#define STAT1_0             0
#define STAT1_1             1

// Constant registers map
#define dmp_projection_matrix           c0
#define dmp_local_to_view_matrix        c4
#define dmp_local_to_world_matrix       c8

// default input registers
#define dmp_position                    v0
#define dmp_normal                      v1
#define dmp_tangent                     v2

// default output registers
#define gl_Position                     o0
#define dmp_lrQuat                      o1
#define dmp_lrView                      o2
#define gl_TexCoord0                    o3

#define EQ                  0
#define NEQ                 1
#define LESS                2
#define LEQ                 3
#define GT                  4
#define GEQ                 5

// Temporary registers map
#define TEMP0               r0
#define TEMP1               r1
#define TEMP2               r2
#define TEMP3               r3
#define TEMP4               r4

#define TEMP_TEX0           r10
#define TEMP_TEX1           r11
#define TEMP_TEX2           r12
#define TEMP_TEX3           r13
#define TEMP_TANG           r13
#define TEMP_NORM           r14
#define TEMP_VIEW           r15
#define TEMP_POS            r12

// Constant registers
#define CONSTANT            c80 // sincos uses c93,94,95

def     CONSTANT[0], 0.0, 1.0, 2.0, 3.0
def     CONSTANT[1], 0.125, 0.00390625, 0.5, 0.25

#define CONST_0                 CONSTANT[0].x
#define CONST_1                 CONSTANT[0].y
#define CONST_2                 CONSTANT[0].z
#define CONST_3                 CONSTANT[0].w
#define CONST_HALF              CONSTANT[1].z
#define CONST_QUARTER           CONSTANT[1].w
#define CONST_1_0               CONSTANT[0].yx
#define CONST_1__4              CONSTANT[1].w
#define CONST_1__8              CONSTANT[1].x
#define CONST_1__256            CONSTANT[1].y

