;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.0 #11528 (Linux)
;--------------------------------------------------------
	.module main
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _PrintFloat
	.globl _lfixed_div
	.globl _fixed_mul
	.globl _float2fixed
	.globl _int2fixed
	.globl _fixed2int
	.globl _powf
	.globl _cosf
	.globl _sinf
	.globl _putchar
	.globl _Itoa
	.globl _StrLen
	.globl _StrConcat
	.globl _Width
	.globl _SetColors
	.globl _RealTimer
	.globl _SetRealTimer
	.globl _Screen
	.globl _Cls
	.globl _Locate
	.globl _Print
	.globl _fVDPHz
	.globl _fElapsedTime
	.globl _w
	.globl _b
	.globl _z
	.globl _lt_sin_j
	.globl _lt_sin_i
	.globl _lt_cos_j
	.globl _lt_cos_i
	.globl _i_inc
	.globl _i_lim
	.globl _j_inc
	.globl _j_lim
	.globl _fj
	.globl _fi
	.globl _B_inc
	.globl _A_inc
	.globl _B
	.globl _A
	.globl _N
	.globl _o
	.globl _y
	.globl _x
	.globl _j_max
	.globl _i_max
	.globl _k
	.globl _j
	.globl _i
	.globl _nh
	.globl _mh
	.globl _gh
	.globl _eh
	.globl _dn
	.globl _de
	.globl _dg
	.globl _fe
	.globl _fg
	.globl __30
	.globl __15
	.globl __8
	.globl __5
	.globl __2
	.globl __1
	.globl _t
	.globl _n
	.globl _m
	.globl _l
	.globl _D
	.globl _h
	.globl _g
	.globl _f
	.globl _e
	.globl _d
	.globl _c
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_c::
	.ds 2
_d::
	.ds 2
_e::
	.ds 2
_f::
	.ds 2
_g::
	.ds 2
_h::
	.ds 2
_D::
	.ds 2
_l::
	.ds 2
_m::
	.ds 2
_n::
	.ds 2
_t::
	.ds 2
__1::
	.ds 2
__2::
	.ds 2
__5::
	.ds 2
__8::
	.ds 2
__15::
	.ds 2
__30::
	.ds 2
_fg::
	.ds 2
_fe::
	.ds 2
_dg::
	.ds 2
_de::
	.ds 2
_dn::
	.ds 2
_eh::
	.ds 2
_gh::
	.ds 2
_mh::
	.ds 2
_nh::
	.ds 2
_i::
	.ds 2
_j::
	.ds 2
_k::
	.ds 2
_i_max::
	.ds 2
_j_max::
	.ds 2
_x::
	.ds 2
_y::
	.ds 2
_o::
	.ds 2
_N::
	.ds 2
_A::
	.ds 4
_B::
	.ds 4
_A_inc::
	.ds 4
_B_inc::
	.ds 4
_fi::
	.ds 4
_fj::
	.ds 4
_j_lim::
	.ds 4
_j_inc::
	.ds 4
_i_lim::
	.ds 4
_i_inc::
	.ds 4
_lt_cos_i::
	.ds 644
_lt_cos_j::
	.ds 182
_lt_sin_i::
	.ds 644
_lt_sin_j::
	.ds 182
_z::
	.ds 3520
_b::
	.ds 1760
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
_w::
	.ds 2
_fElapsedTime::
	.ds 4
_fVDPHz::
	.ds 4
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;main.c:39: void PrintFloat(float value, char round) // max round == 4
;	---------------------------------
; Function PrintFloat
; ---------------------------------
_PrintFloat::
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl, #-27
	add	hl, sp
	ld	sp, hl
;main.c:42: Itoa((int)value, buffer, 10);
	ld	hl, #4
	add	hl, sp
	ld	-6 (ix), l
	ld	-5 (ix), h
	push	hl
	ld	l, 6 (ix)
	ld	h, 7 (ix)
	push	hl
	ld	l, 4 (ix)
	ld	h, 5 (ix)
	push	hl
	call	___fs2sint
	pop	af
	pop	af
	ex	de,hl
	pop	bc
	push	de
	ld	hl, #0x000a
	push	hl
	push	bc
	push	de
	call	_Itoa
	ld	hl, #6
	add	hl, sp
	ld	sp, hl
	pop	de
;main.c:43: StrConcat(buffer, ".");
	ld	hl, #___str_0+0
	ld	c, -6 (ix)
	ld	b, -5 (ix)
	push	de
	push	hl
	push	bc
	call	_StrConcat
	pop	af
	pop	af
	pop	de
;main.c:46: Itoa((int)((value-(int)value)*powf(10.0f, round)), buffer2, 10);
	ld	hl, #12
	add	hl, sp
	ld	-4 (ix), l
	ld	-3 (ix), h
	push	hl
	push	de
	call	___sint2fs
	pop	af
	push	de
	push	hl
	ld	l, 6 (ix)
	ld	h, 7 (ix)
	push	hl
	ld	l, 4 (ix)
	ld	h, 5 (ix)
	push	hl
	call	___fssub
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-27 (ix), l
	ld	-26 (ix), h
	ld	-25 (ix), e
	ld	-24 (ix), d
	ld	a, 8 (ix)
	push	af
	inc	sp
	call	___uchar2fs
	inc	sp
	push	de
	push	hl
	ld	hl, #0x4120
	push	hl
	ld	hl, #0x0000
	push	hl
	call	_powf
	pop	af
	pop	af
	pop	af
	pop	af
	push	de
	push	hl
	ld	l, -25 (ix)
	ld	h, -24 (ix)
	push	hl
	ld	l, -27 (ix)
	ld	h, -26 (ix)
	push	hl
	call	___fsmul
	pop	af
	pop	af
	pop	af
	pop	af
	push	de
	push	hl
	call	___fs2sint
	pop	af
	pop	af
	pop	bc
	ld	de, #0x000a
	push	de
	push	bc
	push	hl
	call	_Itoa
	ld	hl, #6
	add	hl, sp
	ld	sp, hl
;main.c:47: if (StrLen(buffer2) < round) 
	ld	c, -4 (ix)
	ld	b, -3 (ix)
	push	bc
	call	_StrLen
	pop	af
	ld	c, 8 (ix)
	ld	b, #0x00
	ld	a, l
	sub	a, c
	ld	a, h
	sbc	a, b
	jp	PO, 00110$
	xor	a, #0x80
00110$:
	jp	P, 00102$
;main.c:50: memset(buffer3,'0', round-StrLen(buffer2));
	ld	hl, #17
	add	hl, sp
	ld	-2 (ix), l
	ld	-1 (ix), h
	ex	de,hl
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	bc
	push	de
	push	hl
	call	_StrLen
	pop	af
	pop	de
	pop	bc
	ld	a, c
	sub	a, l
	ld	c, a
	ld	a, b
	sbc	a, h
	ld	b, a
	push	bc
	ld	hl, #0x0030
	push	hl
	push	de
	call	_memset
	ld	hl, #6
	add	hl, sp
	ld	sp, hl
;main.c:51: buffer3[round-StrLen(buffer2)] = '\0';
	ld	c, -4 (ix)
	ld	b, -3 (ix)
	push	bc
	call	_StrLen
	pop	af
	ld	a, 8 (ix)
	sub	a, l
	ld	c, a
	rla
	sbc	a, a
	ld	b, a
	ld	l, -2 (ix)
	ld	h, -1 (ix)
	add	hl, bc
	ld	(hl), #0x00
;main.c:52: StrConcat(buffer, buffer3);
	ld	e, -2 (ix)
	ld	d, -1 (ix)
	ld	c, -6 (ix)
	ld	b, -5 (ix)
	push	de
	push	bc
	call	_StrConcat
	pop	af
	pop	af
00102$:
;main.c:54: StrConcat(buffer, buffer2);
	ld	e, -4 (ix)
	ld	d, -3 (ix)
	ld	c, -6 (ix)
	ld	b, -5 (ix)
	push	de
	push	bc
	call	_StrConcat
	pop	af
	pop	af
;main.c:56: Print(buffer);
	ld	c, -6 (ix)
	ld	b, -5 (ix)
	push	bc
	call	_Print
;main.c:57: }
	ld	sp,ix
	pop	ix
	ret
_Done_Version:
	.ascii "Made with FUSION-C 1.2 (ebsoft)"
	.db 0x00
___str_0:
	.ascii "."
	.db 0x00
;main.c:59: int main() {
;	---------------------------------
; Function main
; ---------------------------------
_main::
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl, #-8
	add	hl, sp
	ld	sp, hl
;main.c:60: Screen(0);
	xor	a, a
	push	af
	inc	sp
	call	_Screen
	inc	sp
;main.c:61: Width(80);
	ld	a, #0x50
	push	af
	inc	sp
	call	_Width
	inc	sp
;main.c:62: Locate(0,0);
	xor	a, a
	push	af
	inc	sp
	xor	a, a
	push	af
	inc	sp
	call	_Locate
	pop	af
;main.c:63: SetColors(15,1,1);
	ld	de, #0x0101
	push	de
	ld	a, #0x0f
	push	af
	inc	sp
	call	_SetColors
;main.c:65: _1 = i2x(1);   _2 = i2x(2);
	inc	sp
	ld	hl,#0x0001
	ex	(sp),hl
	call	_int2fixed
	pop	af
	ld	(__1), hl
	ld	hl, #0x0002
	push	hl
	call	_int2fixed
	pop	af
	ld	(__2), hl
;main.c:66: _5 = i2x(5);   _8 = i2x(8);    
	ld	hl, #0x0005
	push	hl
	call	_int2fixed
	pop	af
	ld	(__5), hl
	ld	hl, #0x0008
	push	hl
	call	_int2fixed
	pop	af
	ld	(__8), hl
;main.c:67: _15 = i2x(15); _30 = i2x(30);
	ld	hl, #0x000f
	push	hl
	call	_int2fixed
	pop	af
	ld	(__15), hl
	ld	hl, #0x001e
	push	hl
	call	_int2fixed
	pop	af
	ld	(__30), hl
;main.c:69: j_lim = 6.28f; // f2x(6.28f);
	ld	iy, #_j_lim
	ld	0 (iy), #0xc3
	ld	1 (iy), #0xf5
	ld	2 (iy), #0xc8
	ld	3 (iy), #0x40
;main.c:70: j_inc = 0.21f; // f2x(0.07f);
	ld	iy, #_j_inc
	ld	0 (iy), #0x3d
	ld	1 (iy), #0x0a
	ld	2 (iy), #0x57
	ld	3 (iy), #0x3e
;main.c:71: i_lim = 6.28f; // f2x(6.28f);
	ld	iy, #_i_lim
	ld	0 (iy), #0xc3
	ld	1 (iy), #0xf5
	ld	2 (iy), #0xc8
	ld	3 (iy), #0x40
;main.c:72: i_inc = 0.06f; // f2x(0.02f);
	ld	iy, #_i_inc
	ld	0 (iy), #0x8f
	ld	1 (iy), #0xc2
	ld	2 (iy), #0x75
	ld	3 (iy), #0x3d
;main.c:74: A_inc = 0.04f; // f2x(0.04f);
	ld	iy, #_A_inc
	ld	0 (iy), #0x0a
	ld	1 (iy), #0xd7
	ld	2 (iy), #0x23
	ld	3 (iy), #0x3d
;main.c:75: B_inc = 0.02f; // f2x(0.02f);
	ld	iy, #_B_inc
	ld	0 (iy), #0x0a
	ld	1 (iy), #0xd7
	ld	2 (iy), #0xa3
	ld	3 (iy), #0x3c
;main.c:76: A = 0; B = 0;
	ld	iy, #_A
	ld	0 (iy), #0x00
	ld	1 (iy), #0x00
	ld	2 (iy), #0x00
	ld	3 (iy), #0x00
	ld	iy, #_B
	ld	0 (iy), #0x00
	ld	1 (iy), #0x00
	ld	2 (iy), #0x00
	ld	3 (iy), #0x00
;main.c:79: Print("Loading...");
	ld	hl, #___str_2
	push	hl
	call	_Print
	pop	af
;main.c:80: i_max=0; j_max=0;
	ld	hl, #0x0000
	ld	(_i_max), hl
	ld	l, #0x00
	ld	(_j_max), hl
;main.c:81: for(fj=0; fj < j_lim; fj += j_inc) {
	ld	iy, #_fj
	ld	0 (iy), #0x00
	ld	1 (iy), #0x00
	ld	2 (iy), #0x00
	ld	3 (iy), #0x00
00118$:
	ld	hl, (_j_lim + 2)
	push	hl
	ld	hl, (_j_lim)
	push	hl
	ld	hl, (_fj + 2)
	push	hl
	ld	hl, (_fj)
	push	hl
	call	___fslt
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-1 (ix), l
	ld	a, l
	or	a, a
	jp	Z, 00101$
;main.c:82: lt_cos_j[j_max] = f2x(cos(fj));
	ld	hl, (_j_max)
	add	hl, hl
	ld	a, #<(_lt_cos_j)
	add	a, l
	ld	-4 (ix), a
	ld	a, #>(_lt_cos_j)
	adc	a, h
	ld	-3 (ix), a
	ld	hl, (_fj + 2)
	push	hl
	ld	hl, (_fj)
	push	hl
	call	_cosf
	pop	af
	pop	af
	ld	-8 (ix), l
	ld	-7 (ix), h
	ld	-6 (ix), e
	ld	-5 (ix), d
	push	de
	ld	l, -8 (ix)
	ld	h, -7 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	ld	a, -2 (ix)
	ld	(hl), a
	inc	hl
	ld	a, -1 (ix)
	ld	(hl), a
;main.c:83: lt_sin_j[j_max] = f2x(sin(fj));
	ld	hl, (_j_max)
	add	hl, hl
	ld	a, #<(_lt_sin_j)
	add	a, l
	ld	-4 (ix), a
	ld	a, #>(_lt_sin_j)
	adc	a, h
	ld	-3 (ix), a
	ld	hl, (_fj + 2)
	push	hl
	ld	hl, (_fj)
	push	hl
	call	_sinf
	pop	af
	pop	af
	ld	-8 (ix), l
	ld	-7 (ix), h
	ld	-6 (ix), e
	ld	-5 (ix), d
	push	de
	ld	l, -8 (ix)
	ld	h, -7 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	ld	a, -2 (ix)
	ld	(hl), a
	inc	hl
	ld	a, -1 (ix)
	ld	(hl), a
;main.c:84: j_max++;
	ld	hl, (_j_max)
	inc	hl
	ld	(_j_max), hl
;main.c:81: for(fj=0; fj < j_lim; fj += j_inc) {
	ld	hl, (_j_inc + 2)
	push	hl
	ld	hl, (_j_inc)
	push	hl
	ld	hl, (_fj + 2)
	push	hl
	ld	hl, (_fj)
	push	hl
	call	___fsadd
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	ld	de, #_fj
	ld	hl, #4
	add	hl, sp
	ld	bc, #4
	ldir
	jp	00118$
00101$:
;main.c:86: for(fi=0; fi < i_lim; fi += i_inc) {
	ld	iy, #_fi
	ld	0 (iy), #0x00
	ld	1 (iy), #0x00
	ld	2 (iy), #0x00
	ld	3 (iy), #0x00
00121$:
	ld	hl, (_i_lim + 2)
	push	hl
	ld	hl, (_i_lim)
	push	hl
	ld	hl, (_fi + 2)
	push	hl
	ld	hl, (_fi)
	push	hl
	call	___fslt
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-1 (ix), l
	ld	a, l
	or	a, a
	jp	Z, 00102$
;main.c:87: lt_cos_i[i_max] = f2x(cos(fi));
	ld	iy, #_i_max
	ld	a, 0 (iy)
	ld	-2 (ix), a
	ld	a, 1 (iy)
	ld	-1 (ix), a
	sla	-2 (ix)
	rl	-1 (ix)
	ld	a, -2 (ix)
	add	a, #<(_lt_cos_i)
	ld	-4 (ix), a
	ld	a, -1 (ix)
	adc	a, #>(_lt_cos_i)
	ld	-3 (ix), a
	ld	hl, (_fi + 2)
	push	hl
	ld	hl, (_fi)
	push	hl
	call	_cosf
	pop	af
	pop	af
	ld	-8 (ix), l
	ld	-7 (ix), h
	ld	-6 (ix), e
	ld	-5 (ix), d
	push	de
	ld	l, -8 (ix)
	ld	h, -7 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	ld	a, -2 (ix)
	ld	(hl), a
	inc	hl
	ld	a, -1 (ix)
	ld	(hl), a
;main.c:88: lt_sin_i[i_max] = f2x(sin(fi));
	ld	hl, (_i_max)
	add	hl, hl
	ld	a, #<(_lt_sin_i)
	add	a, l
	ld	-4 (ix), a
	ld	a, #>(_lt_sin_i)
	adc	a, h
	ld	-3 (ix), a
	ld	hl, (_fi + 2)
	push	hl
	ld	hl, (_fi)
	push	hl
	call	_sinf
	pop	af
	pop	af
	ld	-8 (ix), l
	ld	-7 (ix), h
	ld	-6 (ix), e
	ld	-5 (ix), d
	push	de
	ld	l, -8 (ix)
	ld	h, -7 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	ld	a, -2 (ix)
	ld	(hl), a
	inc	hl
	ld	a, -1 (ix)
	ld	(hl), a
;main.c:89: i_max++;
	ld	hl, (_i_max)
	inc	hl
	ld	(_i_max), hl
;main.c:86: for(fi=0; fi < i_lim; fi += i_inc) {
	ld	hl, (_i_inc + 2)
	push	hl
	ld	hl, (_i_inc)
	push	hl
	ld	hl, (_fi + 2)
	push	hl
	ld	hl, (_fi)
	push	hl
	call	___fsadd
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	ld	de, #_fi
	ld	hl, #4
	add	hl, sp
	ld	bc, #4
	ldir
	jp	00121$
00102$:
;main.c:91: Cls();
	call	_Cls
;main.c:95: while(1) {
00115$:
;main.c:96: SetRealTimer(0);
	ld	hl, #0x0000
	push	hl
	call	_SetRealTimer
	pop	af
;main.c:98: memset(b,32, sizeof b);
	ld	hl, #_b
	ld	(hl), #0x20
	ld	e, l
	ld	d, h
	inc	de
	ld	bc, #0x06df
	ldir
;main.c:99: memset(z,0,sizeof z);
	ld	hl, #_z
	ld	(hl), #0x00
	ld	e, l
	ld	d, h
	inc	de
	ld	bc, #0x0dbf
	ldir
;main.c:101: g = f2x(cos(A)); e = f2x(sin(A));
	ld	hl, (_A + 2)
	push	hl
	ld	hl, (_A)
	push	hl
	call	_cosf
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	push	de
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	(_g), hl
	ld	hl, (_A + 2)
	push	hl
	ld	hl, (_A)
	push	hl
	call	_sinf
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	push	de
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	(_e), hl
;main.c:102: m = f2x(cos(B)); n = f2x(sin(B));
	ld	hl, (_B + 2)
	push	hl
	ld	hl, (_B)
	push	hl
	call	_cosf
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	push	de
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	(_m), hl
	ld	hl, (_B + 2)
	push	hl
	ld	hl, (_B)
	push	hl
	call	_sinf
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	push	de
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	hl
	call	_float2fixed
	pop	af
	pop	af
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	(_n), hl
;main.c:104: for(j=0; j < j_max; j++) {
	ld	hl, #0x0000
	ld	(_j), hl
00127$:
	ld	hl, #_j_max
	ld	iy, #_j
	ld	a, 0 (iy)
	sub	a, (hl)
	ld	a, 1 (iy)
	inc	hl
	sbc	a, (hl)
	jp	PO, 00227$
	xor	a, #0x80
00227$:
	jp	P, 00112$
;main.c:105: Locate(0, 22);
	ld	a, #0x16
	push	af
	inc	sp
	xor	a, a
	push	af
	inc	sp
	call	_Locate
;main.c:106: putchar('\x01');
	ld	hl, #0x0001
	ex	(sp),hl
	call	_putchar
	pop	af
;main.c:107: if (w>1) w=0;            
	ld	a, #0x01
	ld	iy, #_w
	cp	a, 0 (iy)
	ld	a, #0x00
	sbc	a, 1 (iy)
	jp	PO, 00228$
	xor	a, #0x80
00228$:
	jp	P, 00104$
	ld	hl, #0x0000
	ld	(_w), hl
00104$:
;main.c:108: putchar("\x56\x57"[w++]);
	ld	bc, (_w)
	ld	hl, (_w)
	inc	hl
	ld	(_w), hl
	ld	hl, #___str_3
	add	hl, bc
	ld	-2 (ix), l
	ld	-1 (ix), h
	ld	a, (hl)
	ld	-1 (ix), a
	ld	-2 (ix), a
	xor	a, a
	ld	-1 (ix), a
	ld	l, -2 (ix)
	ld	h, -1 (ix)
	push	hl
	call	_putchar
	pop	af
;main.c:110: d = lt_cos_j[j]; f = lt_sin_j[j];
	ld	bc, (_j)
	sla	c
	rl	b
	ld	hl, #_lt_cos_j
	add	hl, bc
	ld	a, (hl)
	ld	(_d+0), a
	inc	hl
	ld	a, (hl)
	ld	(_d+1), a
	ld	hl, #_lt_sin_j
	add	hl, bc
	ld	a, (hl)
	ld	(_f+0), a
	inc	hl
	ld	a, (hl)
	ld	(_f+1), a
;main.c:112: h = d + _2;
	ld	hl, #__2
	push	de
	ld	de,#_h
	ld	iy,#_d
	ld	a, 0 (iy)
	add	a, (hl)
	ld	(de), a
	ld	a, 1 (iy)
	inc	hl
	adc	a, (hl)
	inc	de
	ld	(de), a
	pop	de
;main.c:114: fg = x_mul(f, g); fe = x_mul(f, e);
	ld	hl, (_g)
	push	hl
	ld	hl, (_f)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_fg), hl
	ld	hl, (_e)
	push	hl
	ld	hl, (_f)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_fe), hl
;main.c:115: dg = x_mul(d, g); de = x_mul(d, e);
	ld	hl, (_g)
	push	hl
	ld	hl, (_d)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_dg), hl
	ld	hl, (_e)
	push	hl
	ld	hl, (_d)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_de), hl
;main.c:116: dn = x_mul(d, n); eh = x_mul(e, h);
	ld	hl, (_n)
	push	hl
	ld	hl, (_d)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_dn), hl
	ld	hl, (_h)
	push	hl
	ld	hl, (_e)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_eh), hl
;main.c:117: gh = x_mul(g, h); mh = x_mul(m, h);
	ld	hl, (_h)
	push	hl
	ld	hl, (_g)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_gh), hl
	ld	hl, (_h)
	push	hl
	ld	hl, (_m)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_mh), hl
;main.c:118: nh = x_mul(n, h);
	ld	hl, (_h)
	push	hl
	ld	hl, (_n)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	(_nh), hl
;main.c:120: for(i=0; i < i_max; i++) {
	ld	hl, #0x0000
	ld	(_i), hl
00124$:
	ld	hl, #_i_max
	ld	iy, #_i
	ld	a, 0 (iy)
	sub	a, (hl)
	ld	a, 1 (iy)
	inc	hl
	sbc	a, (hl)
	jp	PO, 00231$
	xor	a, #0x80
00231$:
	jp	P, 00128$
;main.c:121: c = lt_sin_i[i]; l = lt_cos_i[i];
	ld	bc, (_i)
	sla	c
	rl	b
	ld	hl, #_lt_sin_i
	add	hl, bc
	ld	a, (hl)
	ld	(_c+0), a
	inc	hl
	ld	a, (hl)
	ld	(_c+1), a
	ld	hl, #_lt_cos_i
	add	hl, bc
	ld	a, (hl)
	ld	(_l+0), a
	inc	hl
	ld	a, (hl)
	ld	(_l+1), a
;main.c:123: D = lx_div(_1, x_mul(c, eh) + fg + _5);
	ld	hl, (_eh)
	push	hl
	ld	hl, (_c)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	c, l
	ld	b, h
	ld	a, c
	ld	hl, #_fg
	add	a, (hl)
	ld	c, a
	ld	a, b
	inc	hl
	adc	a, (hl)
	ld	b, a
	ld	hl, (__5)
	add	hl, bc
	ld	-4 (ix), l
	ld	a, h
	ld	-3 (ix), a
	rla
	sbc	a, a
	ld	-2 (ix), a
	ld	-1 (ix), a
	ld	hl,#__1
	ld	c, (hl)
	inc	hl
	ld	a, (hl)
	ld	b, a
	rla
	sbc	a, a
	ld	e, a
	ld	d, a
	ld	l, -2 (ix)
	ld	h, -1 (ix)
	push	hl
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	hl
	push	de
	push	bc
	call	_lfixed_div
	pop	af
	pop	af
	pop	af
	pop	af
	ld	iy, #_D
	ld	0 (iy), l
	ld	1 (iy), h
;main.c:124: t = x_mul(c, gh) - fe;
	ld	hl, (_gh)
	push	hl
	ld	hl, (_c)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	c, l
	ld	b, h
	ld	hl, #_fe
	ld	iy, #_t
	ld	a, c
	sub	a, (hl)
	ld	0 (iy), a
	ld	a, b
	inc	hl
	sbc	a, (hl)
	inc	iy
	ld	0 (iy), a
;main.c:125: x = 40 + x2i( x_mul(x_mul(D, x_mul(mh, l) - x_mul(t, n)), _30) );
	ld	hl, (_l)
	push	hl
	ld	hl, (_mh)
	push	hl
	call	_fixed_mul
	pop	af
	ex	(sp),hl
	ld	hl, (_n)
	push	hl
	ld	hl, (_t)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	pop	bc
	ld	a, c
	sub	a, l
	ld	c, a
	ld	a, b
	sbc	a, h
	ld	b, a
	push	bc
	ld	hl, (_D)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	c, l
	ld	b, h
	ld	hl, (__30)
	push	hl
	push	bc
	call	_fixed_mul
	pop	af
	ex	(sp),hl
	call	_fixed2int
	pop	af
	ld	bc,#0x0028
	add	hl,bc
	ld	(_x), hl
;main.c:126: y = 12 + x2i( x_mul(x_mul(D, x_mul(nh, l) + x_mul(t, m)), _15) );
	ld	hl, (_l)
	push	hl
	ld	hl, (_nh)
	push	hl
	call	_fixed_mul
	pop	af
	ex	(sp),hl
	ld	hl, (_m)
	push	hl
	ld	hl, (_t)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	pop	bc
	add	hl, bc
	push	hl
	ld	hl, (_D)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	c, l
	ld	b, h
	ld	hl, (__15)
	push	hl
	push	bc
	call	_fixed_mul
	pop	af
	ex	(sp),hl
	call	_fixed2int
	pop	af
	ld	bc,#0x000c
	add	hl,bc
	ld	(_y), hl
;main.c:127: o = x + 80 * y;
	ld	bc, (_y)
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, hl
	add	hl, hl
	add	hl, hl
	ex	de, hl
	ld	hl, #_o
	ld	iy, #_x
	ld	a, 0 (iy)
	add	a, e
	ld	(hl), a
	ld	a, 1 (iy)
	adc	a, d
	inc	hl
	ld	(hl), a
;main.c:128: if(22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
	ld	hl, #_y
	ld	a, (hl)
	sub	a, #0x16
	inc	hl
	ld	a, (hl)
	rla
	ccf
	rra
	sbc	a, #0x80
	jp	NC, 00125$
	xor	a, a
	ld	iy, #_y
	cp	a, 0 (iy)
	sbc	a, 1 (iy)
	jp	PO, 00234$
	xor	a, #0x80
00234$:
	jp	P, 00125$
	xor	a, a
	ld	iy, #_x
	cp	a, 0 (iy)
	sbc	a, 1 (iy)
	jp	PO, 00235$
	xor	a, #0x80
00235$:
	jp	P, 00125$
	ld	hl, #_x
	ld	a, (hl)
	sub	a, #0x50
	inc	hl
	ld	a, (hl)
	rla
	ccf
	rra
	sbc	a, #0x80
	jp	NC, 00125$
	ld	hl, (_o)
	add	hl, hl
	ld	de, #_z
	add	hl, de
	ld	c, (hl)
	inc	hl
	ld	b, (hl)
	ld	a, c
	ld	iy, #_D
	sub	a, 0 (iy)
	ld	a, b
	sbc	a, 1 (iy)
	jp	PO, 00236$
	xor	a, #0x80
00236$:
	jp	P, 00125$
;main.c:129: N = x2i( x_mul( x_mul(fe - x_mul(c, dg), m) - x_mul(c, de) - fg - x_mul(l, dn), _8 ));
	ld	hl, (_dg)
	push	hl
	ld	hl, (_c)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	ld	c, l
	ld	b, h
	ld	iy, #_fe
	ld	a, 0 (iy)
	sub	a, c
	ld	c, a
	ld	a, 1 (iy)
	sbc	a, b
	ld	b, a
	ld	hl, (_m)
	push	hl
	push	bc
	call	_fixed_mul
	pop	af
	pop	af
	ld	c, l
	ld	b, h
	push	bc
	ld	hl, (_de)
	push	hl
	ld	hl, (_c)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	pop	bc
	ld	a, c
	sub	a, l
	ld	c, a
	ld	a, b
	sbc	a, h
	ld	b, a
	ld	hl, #_fg
	ld	a, c
	sub	a, (hl)
	ld	c, a
	ld	a, b
	inc	hl
	sbc	a, (hl)
	ld	b, a
	push	bc
	ld	hl, (_dn)
	push	hl
	ld	hl, (_l)
	push	hl
	call	_fixed_mul
	pop	af
	pop	af
	pop	bc
	ld	a, c
	sub	a, l
	ld	c, a
	ld	a, b
	sbc	a, h
	ld	b, a
	ld	hl, (__8)
	push	hl
	push	bc
	call	_fixed_mul
	pop	af
	ex	(sp),hl
	call	_fixed2int
	pop	af
	ld	(_N), hl
;main.c:130: z[o] = D;
	ld	hl, (_o)
	add	hl, hl
	ld	de, #_z
	add	hl, de
	ld	iy, #_D
	ld	a, 0 (iy)
	ld	(hl), a
	inc	hl
	ld	a, 1 (iy)
	ld	(hl), a
;main.c:131: b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
	ld	a, #<(_b)
	ld	hl, #_o
	add	a, (hl)
	ld	c, a
	ld	a, #>(_b)
	inc	hl
	adc	a, (hl)
	ld	b, a
	xor	a, a
	ld	iy, #_N
	cp	a, 0 (iy)
	sbc	a, 1 (iy)
	jp	PO, 00237$
	xor	a, #0x80
00237$:
	jp	P, 00133$
	ld	hl, (_N)
	jr	00134$
00133$:
	ld	hl, #0x0000
00134$:
	ld	de, #___str_4
	add	hl, de
	ld	a, (hl)
	ld	(bc), a
00125$:
;main.c:120: for(i=0; i < i_max; i++) {
	ld	hl, (_i)
	inc	hl
	ld	(_i), hl
	jp	00124$
00128$:
;main.c:104: for(j=0; j < j_max; j++) {
	ld	hl, (_j)
	inc	hl
	ld	(_j), hl
	jp	00127$
00112$:
;main.c:135: Locate(0,0);
	xor	a, a
	push	af
	inc	sp
	xor	a, a
	push	af
	inc	sp
	call	_Locate
	pop	af
;main.c:136: for(k = 0; k < 1760; k++) {
	ld	hl, #0x0000
	ld	(_k), hl
00129$:
;main.c:137: putchar(b[k]);
	ld	a, #<(_b)
	ld	hl, #_k
	add	a, (hl)
	ld	-2 (ix), a
	ld	a, #>(_b)
	inc	hl
	adc	a, (hl)
	ld	-1 (ix), a
	ld	l, -2 (ix)
	ld	h, -1 (ix)
	ld	a, (hl)
	ld	-1 (ix), a
	ld	-2 (ix), a
	xor	a, a
	ld	-1 (ix), a
	ld	l, -2 (ix)
	ld	h, -1 (ix)
	push	hl
	call	_putchar
	pop	af
;main.c:136: for(k = 0; k < 1760; k++) {
	ld	hl, (_k)
	inc	hl
	ld	(_k), hl
	ld	iy, #_k
	ld	a, 0 (iy)
	sub	a, #0xe0
	ld	a, 1 (iy)
	rla
	ccf
	rra
	sbc	a, #0x86
	jr	C,00129$
;main.c:139: A += A_inc*5;
	ld	hl, (_A_inc + 2)
	push	hl
	ld	hl, (_A_inc)
	push	hl
	ld	hl, #0x40a0
	push	hl
	ld	hl, #0x0000
	push	hl
	call	___fsmul
	pop	af
	pop	af
	pop	af
	pop	af
	push	de
	push	hl
	ld	hl, (_A + 2)
	push	hl
	ld	hl, (_A)
	push	hl
	call	___fsadd
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	ld	de, #_A
	ld	hl, #4
	add	hl, sp
	ld	bc, #4
	ldir
;main.c:140: B += B_inc*5;
	ld	hl, (_B_inc + 2)
	push	hl
	ld	hl, (_B_inc)
	push	hl
	ld	hl, #0x40a0
	push	hl
	ld	hl, #0x0000
	push	hl
	call	___fsmul
	pop	af
	pop	af
	pop	af
	pop	af
	push	de
	push	hl
	ld	hl, (_B + 2)
	push	hl
	ld	hl, (_B)
	push	hl
	call	___fsadd
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	ld	de, #_B
	ld	hl, #4
	add	hl, sp
	ld	bc, #4
	ldir
;main.c:142: fElapsedTime = RealTimer()/fVDPHz;
	call	_RealTimer
	ld	-2 (ix), l
	ld	-1 (ix), h
	push	hl
	call	___uint2fs
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	ld	hl, (_fVDPHz + 2)
	push	hl
	ld	hl, (_fVDPHz)
	push	hl
	ld	l, -2 (ix)
	ld	h, -1 (ix)
	push	hl
	ld	l, -4 (ix)
	ld	h, -3 (ix)
	push	hl
	call	___fsdiv
	pop	af
	pop	af
	pop	af
	pop	af
	ld	-4 (ix), l
	ld	-3 (ix), h
	ld	-2 (ix), e
	ld	-1 (ix), d
	ld	de, #_fElapsedTime
	ld	hl, #4
	add	hl, sp
	ld	bc, #4
	ldir
;main.c:143: Locate(2, 22);
	ld	de, #0x1602
	push	de
	call	_Locate
;main.c:144: PrintFloat(fElapsedTime, 2);
	ld	h,#0x02
	ex	(sp),hl
	inc	sp
	ld	hl, (_fElapsedTime + 2)
	push	hl
	ld	hl, (_fElapsedTime)
	push	hl
	call	_PrintFloat
	pop	af
	pop	af
	inc	sp
;main.c:146: return 0;
;main.c:147: }
	jp	00115$
___str_2:
	.ascii "Loading..."
	.db 0x00
___str_3:
	.ascii "VW"
	.db 0x00
___str_4:
	.ascii ".,-~:;=!*#$@"
	.db 0x00
	.area _CODE
	.area _INITIALIZER
__xinit__w:
	.dw #0x0000
__xinit__fElapsedTime:
	.byte #0x00, #0x00, #0x00, #0x00	;  0.000000e+00
__xinit__fVDPHz:
	.byte #0x00, #0x00, #0x70, #0x42	;  6.000000e+01
	.area _CABS (ABS)
