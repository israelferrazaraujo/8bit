#include "../../fusion-c/header/msx_fusion.h"
#include "../../fusion-c/header/vdp_graph1.h"
#include "../../fusion-c/header/vdp_sprites.h"

#define TRANSPARENT 0x00u
#define BLACK 0x01u
#define MEDIUM_GREEN 0x02u
#define LIGHT_GREEN 0x03u
#define DARK_BLUE 0x04u
#define LIGHT_BLUE 0x05u
#define DARK_RED 0x06u
#define CYAN 0x07u
#define MEDIUM_RED 0x08u
#define LIGHT_RED 0x09u
#define DARK_YELLOW 0x0Au
#define LIGHT_YELLOW 0x0Bu
#define DARK_GREEN 0x0Cu
#define MAGENTA 0x0Du
#define GRAY 0x0Eu
#define WHITE 0x0Fu

const char spritePattern[] = 
							{ 1,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0 };

const char sc2_pattern[]  = 
	                      { 0b00000000,
						  	0b00000000,
						  	0b00000000,
						  	0b00000000,
						  	0b00000000,
						  	0b00000000,
						  	0b00000000,
						    0b00000000,
							0b11000110, 
							0b01111100,
							0b11111110,
							0b10111010,
							0b11111110,
							0b11000110,
							0b01111100,
							0b00000000
						  };
const char sc2_color[] = 
	                    { (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT, 
                          (TRANSPARENT<<4) | TRANSPARENT,
						  (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK, 
                          (DARK_YELLOW<<4) | BLACK
                        };

void sc3Pixel(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = ((y>>3)<<8)-((y>>3)<<3) + y ; // importante dividir antes de 
                                              // multiplicar, para obter a 
                                              // parte inteira da divisão
                                              // 248 = 8 * (32-1)
    if (x & 1u) // odd
    {
        vaddress += (x-1u)<<2;
        Vpoke(vaddress, color | (Vpeek(vaddress) & 0b11110000));
                                                  // clean lower 4 bits
                                                  // e complementa com os 4 bits
    }                                             // menos significativos
    else       // even
    {
        vaddress += x<<2; // o x está multiplicado por 2, por isso "*4" e não "*8"
        Vpoke(vaddress, (color<<4) | (Vpeek(vaddress) & 0b00001111));
                                                  // clean upper 4 bits
                                                  // e complementa com os 4 bits 
                                                  // mais significativos
    }                                      
    
}

// https://www.msx.org/wiki/BASE()
void init_sc0()
{
	FillVram(0, 0x30, 40);
}

void init_sc1()
{
	FillVram(6144, 0x30, 32);
}

void init_sc2()
{
	for (unsigned int i=0; i<3; i++)
	{
		FillVram(6144+32*8*i, 0, 32*8); // TRANSPARENT tile. i'th 1/3 screen.
		CopyRamToVram(sc2_pattern, 256*8*i, sizeof(sc2_pattern));
		CopyRamToVram(sc2_color, 8192+256*8*i, sizeof(sc2_color));
	}
    FillVram(6144+32*22, 1, 32*2); // last 8 lines.
}

void init_sc3()
{
	FillVram(0, 0, 32*48);
	/*
	for (unsigned int x=0; x<64; x++)
		for(unsigned int y=0; y<36; y++)
			if ((x+y) & 1u)
				sc3Pixel(x, y, BLACK);
			else
				sc3Pixel(x, y, WHITE);
	*/
	for (unsigned int x=0; x<64; x++)
		for(unsigned int y=8; y<39; y++)
			if ((x+y) & 1u)
				sc3Pixel(x, y, BLACK);
			else
				sc3Pixel(x, y, WHITE);
		
}

/*
void scrmode(unsigned char mode) __naked
{
	__asm

	ld    hl,#2     ;; /  Make HL point to SP+2
	add   hl,sp     ;; \  (as the first 2 bytes are the return address)
	ld    a,(hl)    ;; A = mode to be set (got from the stack)
	
	ld 		hl,#0xFCAF		;; Save current mode to FCAF 
	ld 		(hl),a
	
	push	ix
	ld		ix, #0x005f		;;	chgmod on bios
	ld		iy, (0xfcc0)	;;	iyh <= (0xfcc1) : EXPTBL(MAIN-ROM SLOT)
	call	0x001c			;;	CALSLT
	pop		ix
	
	ret 

	__endasm;
}
*/

void scr0() __naked
{
	__asm

	push	ix
	ld		ix, #0x0078		;;	SETTXT on bios
	ld		iy, (0xfcc0)	;;	iyh <= (0xfcc1) : EXPTBL(MAIN-ROM SLOT)
	call	0x001c			;;	CALSLT
	pop		ix
	
	ret

	__endasm;
}

void scr1() __naked
{
	__asm

	push	ix
	ld		ix, #0x007b		;;	SETT32 on bios
	ld		iy, (0xfcc0)	;;	iyh <= (0xfcc1) : EXPTBL(MAIN-ROM SLOT)
	call	0x001c			;;	CALSLT
	pop		ix
	
	ret

	__endasm;
}

void scr2() __naked
{
	__asm

	push	ix
	ld		ix, #0x007e		;;	SETGRP on bios
	ld		iy, (0xfcc0)	;;	iyh <= (0xfcc1) : EXPTBL(MAIN-ROM SLOT)
	call	0x001c			;;	CALSLT
	pop		ix
	
	ret

	__endasm;
}

void scr3() __naked
{
	__asm

	push	ix
	ld		ix, #0x0081		;;	SETMLT on bios
	ld		iy, (0xfcc0)	;;	iyh <= (0xfcc1) : EXPTBL(MAIN-ROM SLOT)
	call	0x001c			;;	CALSLT
	pop		ix

	ret

	__endasm;
}

int sprite = 0;
static char interrupt()
{
	if (IsVsync() == 1)
	{
		//scr3();
		sprite=0; // sync sprite collision detection.
		//sc3Pixel(10, 32, DARK_YELLOW);
	}
		
	return 1;	
}


int main()
{
	Screen(2);
	init_sc2();

	Screen(3);
	init_sc3();

	SpriteOn();
	Sprite8();
	SetSpritePattern(0, spritePattern, 8);
	PutSprite(0, 0, 0, 0, BLACK);
	PutSprite(1, 0, 0, 0, BLACK);
	//PutSprite(2, 0, 0, 32, BLACK);
	//PutSprite(3, 0, 0, 32, BLACK);
	PutSprite(4, 0, 0, 156, BLACK);
	PutSprite(5, 0, 0, 156, BLACK);
	//PutSprite(6, 0, 255, 191, BLACK);
	//PutSprite(7, 0, 255, 191, BLACK);

	InitInterruptHandler();
	SetInterruptHandler( interrupt );

	while (1) {
		char key = Inkey();
		if (key==0x1B) {break;}
		else if (key==0x30) {scr0();}
		else if (key==0x31) {scr1();}
		else if (key==0x32) {scr2();}
		else if (key==0x33) {scr3();}
		
		if (SpriteCollision() == 1)
		{
			if (sprite < 1)
			{
				//Vpoke(6144+32*22, 1);
				scr3();
			}
			else if (sprite < 2)
			{
				scr2();
				//Vpoke(6144+32*22, 0);
			}
			sprite++;
			if (sprite == 2)
			{
				sprite=0;
			}	
		}
	}

	EndInterruptHandler();

	Screen(0);
	return 0;
}
