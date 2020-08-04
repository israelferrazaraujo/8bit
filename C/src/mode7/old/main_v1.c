/*
OneLoneCoder.com - Programming Pseudo 3D planes, aka MODE7
"The SNES was too advanced for its time" - @Javidx9

License
~~~~~~~
Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:

https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9

GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE

Cheers!

Background
~~~~~~~~~~
Pseudo 3D was a technique made popular by the Super Nintendo
in games such as Super Mario Kart. This code demonstrates the
effect of that hardware mode.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/ybLZyY655iY

Last Updated: 15/04/2018
*/

#include "../../fusion-c/header/msx_fusion.h"
#include "../../fusion-c/header/vdp_graph1.h"
#include "../lib/fixed/fixed.h"
#include "../lib/trig/trig.h"
#include <math.h>

#define TRANSPARENT 0x00
#define BLACK 0x01
#define MEDIUM_GREEN 0x02
#define LIGHT_GREEN 0x03 
#define DARK_BLUE 0x04
#define LIGHT_BLUE 0x05
#define DARK_RED 0x06
#define CYAN 0x07
#define MEDIUM_RED 0x08
#define LIGHT_RED 0x09 
#define DARK_YELLOW 0x0A
#define LIGHT_YELLOW 0x0B
#define DARK_GREEN 0x0C
#define MAGENTA 0x0D
#define GRAY 0x0E
#define WHITE 0x0F
#define C 0x0A
#define B 0x01
#define W 0x0F
#define G 0x0E
#define R 0x06
#define L 0x04
#define T 0x00

float fWorldX = 0.0f; // 16.0f; // 1000.0f
float fWorldY = 0.0f; // 16.0f; // 1000.0f
float fWorldA = 0.1f; // PI / 2.0f;
int nNear = 0; // 0.005f * 100.0f;
int nFar = 8;// 0.3f; // 0.03f*100.0f;
float fFoVHalf = PI / 4.0f;
const float fRotSpeed = PI / 18.0f;

int nMapSize = 16; // 1024

unsigned int uScreenWidth = 64u;
unsigned int uScreenHeight = 48u;

char cKeyPressed, 
     bInvalidKey;

fixed 	xSinPlus, 
		xCosPlus,
		xSinMinus, 
		xCosMinus;
unsigned int uHalfScreenHeight;
fixed 	xFarX1,
 		xFarY1,
 		xNearX1,
 		xNearY1,
 		xFarX2,
 		xFarY2,
 		xNearX2,
 		xNearY2,
		xWorldA,
		xFoVHalf;
ulfixed ulxSampleDepth,
		ulxSampleWidth;
ulfixed *ulxSampleDepthY,
	    *ulxSampleWidthX;
lfixed 	lxStartX,
       	lxStartY,
	   	lxEndX,
	   	lxEndY;
lfixed 	lxWorldX,
       	lxWorldY;
int 	nSampleX,
	 	nSampleY;
unsigned int y,x;
float fElapsedTime;

unsigned char *vbuffer;
//unsigned char *sprGround;
const unsigned char sprGround[] = 
							{ C,C,B,B,B,C,C,B, B,B,C,C,C,C,C,B,
							  B,C,C,C,C,C,B,B, B,C,B,B,C,B,B,C,
							  C,C,C,C,C,C,C,B, B,C,B,B,C,B,B,C,
							  C,B,C,C,C,B,C,B, B,C,C,C,C,C,C,C,
							  C,C,C,C,C,C,C,B, B,C,C,B,B,B,C,C,
							  C,C,B,B,B,C,C,B, B,C,C,C,C,C,C,C,
							  B,C,C,C,C,C,B,B, B,C,B,C,B,C,B,C,
							  B,B,B,B,B,B,B,B, B,B,B,B,B,B,B,B,

							  B,B,B,C,C,B,B,B, T,R,R,R,R,R,R,T,
							  B,C,B,C,C,B,B,B, T,R,W,W,W,W,R,T,
							  C,B,B,C,B,B,B,B, T,R,W,B,W,B,R,T,
							  C,C,C,C,C,C,C,B, T,R,W,W,W,W,R,R,
							  B,B,C,C,C,B,B,C, T,B,B,B,B,B,R,T,
							  B,B,C,C,C,B,B,B, T,W,B,B,B,W,R,R,
							  B,B,C,B,C,B,B,B, T,T,G,G,G,T,T,T,
							  B,C,C,B,C,C,B,B, T,T,G,T,G,T,T,T };
//char *sprSky;


/// Framebuffer screen 3 pixel set
void Point(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = ((y>>3)<<8)-((y>>3)<<3) + y ; // importante dividir antes de 
                                              // multiplicar, para obter a 
                                              // parte inteira da divisão
                                              // 248 = 8 * (32-1)
    if (x & 1u) // odd
    {
        vaddress += (x-1u)<<2;
        vbuffer[vaddress] = color | (vbuffer[vaddress] & 0b11110000);
                                                  // clean lower 4 bits
                                                  // e complementa com os 4 bits
    }                                             // menos significativos
    else       // even
    {
        vaddress += x<<2; // o x está multiplicado por 2, por isso "*4" e não "*8"
        vbuffer[vaddress] = (color<<4) | (vbuffer[vaddress] & 0b00001111);
                                                  // clean upper 4 bits
                                                  // e complementa com os 4 bits 
                                                  // mais significativos
    }                                      
    
}

void InitVideoBuffer(unsigned char mode)
{
	unsigned int screenMemSize = 40*24; // mode 0 default
	if (mode == 2)
		screenMemSize = 32*24;
	else if (mode == 3)
		screenMemSize = 32*48;

	vbuffer = MMalloc(screenMemSize);
	MemFill(vbuffer, TRANSPARENT, screenMemSize);

	if (mode == 2)
		CopyRamToVram(vbuffer, 6144, screenMemSize);
	else
		CopyRamToVram(vbuffer, 0, screenMemSize);
}
/*
void CreateSprites()
{
	// Create a large sprite and fill it with horizontal and vertical lines
	sprGround = MMalloc(nMapSize*nMapSize);
	MemFill(sprGround, BLACK, nMapSize*nMapSize);

	for (int x = 0; x < nMapSize; x += 16) // x += 32
	{
		for (int y = 0; y < nMapSize; y++)
		{
			sprGround[nMapSize*y+x] = MAGENTA;
			sprGround[nMapSize*y+x+1] = MAGENTA;
			//sprGround[nMapSize*(y+1)+x] = DARK_YELLOW;
			sprGround[nMapSize*x+y] = DARK_BLUE;
			sprGround[nMapSize*(x+1)+y] = DARK_BLUE;
		}
	}

	// Simply load very large sprites from file
	//sprGround = new olcSprite(L"mariokart.spr");
	//sprSky = new olcSprite(L"sky1.spr");

}
*/

int abs(int j)
{
	return (j < 0) ? -j : j;
}
unsigned int print(unsigned int x, unsigned int y, char *str)
{
    unsigned int len = StrLen(str);
    CopyRamToVram(str, y*40u+x, len);
    return len;
}
unsigned int printInt(unsigned int x, unsigned int y, int value)
{
    char buffer[5];
    return print(x, y, Itoa(value, buffer, 10) );
}
unsigned int printFloat(unsigned int x, unsigned int y, float value)
{
    char buffer[8];
    Itoa((int)value, buffer, 10);
    StrConcat(buffer, ".");
    
    char buffer2[3];
    Itoa(abs((int)((value-(int)value)*100.0f)), buffer2, 10);
    if (StrLen(buffer2) == 1) 
        StrConcat(buffer, "0");
    StrConcat(buffer, buffer2);

    return print(x, y, buffer);
}

void CalcSampleDepths()
{
	ulfixed ulxTopScreenWidth = uint2ulfixed(uScreenWidth-1u);
	ulfixed ulxTopHalfScreenHeight = uint2ulfixed((uScreenHeight/2u)-1u);

	ulxSampleDepthY = MMalloc(uHalfScreenHeight*sizeof(ulfixed));
	for (y = 0; y < uHalfScreenHeight; y++)
	{
		ulxSampleDepthY[y] = ulfixed_div(uint2ulfixed(y), ulxTopHalfScreenHeight);	
	}

	ulxSampleWidthX = MMalloc(uScreenWidth*sizeof(ulfixed));
	for (x = 0; x < uScreenWidth; x++)
	{
		ulxSampleWidthX[x] = ulfixed_div(uint2ulfixed(x), ulxTopScreenWidth);
	}
}

int main()
{
	uHalfScreenHeight = uScreenHeight/2u;
	lxWorldX = float2lfixed(fWorldX);
	lxWorldY = float2lfixed(fWorldY);
	xWorldA = float2fixed(fWorldA);
	xFoVHalf = float2fixed(fFoVHalf);

	Screen(3);
	InitVideoBuffer(3);

	PutText(26*4, 4*36, "init", 0);

	//CreateSprites();
	CalcSampleDepths();

	while (1)
	{
		SetRealTimer(0);

		xSinPlus = xsin(xWorldA + xFoVHalf);
		xCosPlus = xcos(xWorldA + xFoVHalf);
		xSinMinus = xsin(xWorldA - xFoVHalf);
		xCosMinus = xcos(xWorldA - xFoVHalf);

		// Create Frustum corner points
		xFarX1 = xCosMinus * nFar;
		xFarY1 = xSinMinus * nFar;
		xFarX2 = xCosPlus * nFar;
		xFarY2 = xSinPlus * nFar;

		//fNearX1 = fixed_mul(cos_minus, fNear);
		//fNearY1 = fixed_mul(sin_minus, fNear);
		//fNearX2 = fixed_mul(cos_plus, fNear);
		//fNearY2 = fixed_mul(sin_plus, fNear);

		// Starting with furthest away line and work towards the camera point
		for (y = 0; y < uHalfScreenHeight; y++)
		{
			// Take a sample point for depth linearly related to rows down screen
			ulxSampleDepth = ulxSampleDepthY[y];
			
			// Use sample point in non-linear (1/x) way to enable perspective
			// and grab start and end points for lines across the screen
			lxStartX = lfixed_div(xFarX1, ulxSampleDepth);
			lxStartY = lfixed_div(xFarY1, ulxSampleDepth);
			lxEndX = lfixed_div(xFarX2, ulxSampleDepth);
			lxEndY = lfixed_div(xFarY2, ulxSampleDepth);

			// Linearly interpolate lines across the screen
			for (x = 0; x < uScreenWidth; x++)
			{
				ulxSampleWidth = ulxSampleWidthX[x];
			
				nSampleX = lfixed2int(lfixed_mul(lxEndX-lxStartX, ulxSampleWidth) + lxStartX + lxWorldX);
				nSampleY = lfixed2int(lfixed_mul(lxEndY-lxStartY, ulxSampleWidth) + lxStartY + lxWorldY);

				// Wrap sample coordinates to give "infinite" periodicity on maps
				//nSampleX = (int)(modff(fSampleX, &test)*nMapSize);
				//nSampleY = (int)(modff(fSampleY, &test)*nMapSize);
				//while (nSampleX<0) nSampleX+=nMapSize;
				//while (nSampleY<0) nSampleY+=nMapSize;
				//while (nSampleX>=nMapSize) nSampleX-=nMapSize;
				//while (nSampleY>=nMapSize) nSampleY-=nMapSize;
				
				// Sample symbol and colour from map sprite, and draw the
				// pixel to the screen
				if (nSampleY>=0 && nSampleX>=0 && nSampleY < nMapSize && nSampleX < nMapSize)
				{
					Point(x, y+uHalfScreenHeight, sprGround[nSampleY*nMapSize + nSampleX]);		
					//Point(x, uHalfScreenHeight-y, sprGround[nSampleY*nMapSize + nSampleX]);		
				}
				else
				{
					Point(x, y+uHalfScreenHeight, BLACK);		
					//Point(x, uHalfScreenHeight-y, TRANSPARENT);		
				}
				
			}
		}

		// Draw a blanking line to fill the gap between sky and ground
		//DrawLine(0, ScreenHeight() / 2, ScreenWidth(), ScreenHeight() / 2, PIXEL_SOLID, FG_CYAN);

		// Display Frame
		CopyRamToVram(vbuffer, 0, 32*48);

		fElapsedTime = RealTimer()/50;

		bInvalidKey = 1;
        KillKeyBuffer();
        while (bInvalidKey)
        {
            bInvalidKey = 0;
            
            cKeyPressed = Inkey();

            if (cKeyPressed == 27) 
            {
                // Exit program
                Screen(0);
                Cls();
                return 0;
            }
			else if (cKeyPressed == 0x20)
            {
                // Display Stats
                Screen(0);
                Width(40);
                x =print(0, 0, "X=");     x+=printFloat(x, 0, lfixed2float(lxWorldX));
                x+=print(x, 0, ", Y=");   x+=printFloat(x, 0, lfixed2float(lxWorldY));
                x+=print(x, 0, ", A=");   x+=printFloat(x, 0, fWorldA*180.0f/PI);
                x+=print(x, 0, ", SPF=");    printFloat(x, 0, fElapsedTime);

                while (!Inkey()) {}
                Screen(3);
            }
            //else if (cKeyPressed == 'Q' || cKeyPressed == 'q') { fNear    += 1.0f; }
			//else if (cKeyPressed == 'A' || cKeyPressed == 'a') { fNear    -= 1.0f; }
			else if (cKeyPressed == 'W' || cKeyPressed == 'w') { nFar     += 1; }
			else if (cKeyPressed == 'S' || cKeyPressed == 's') { nFar     -= 1; }
			else if (cKeyPressed == 'Z' || cKeyPressed == 'z') { xFoVHalf += float2fixed(PI/18.0f); }
			else if (cKeyPressed == 'X' || cKeyPressed == 'x') { xFoVHalf -= float2fixed(PI/18.0f); }
            else
            {
                cKeyPressed = JoystickRead(0);
            
                bInvalidKey = !cKeyPressed;    

                if (cKeyPressed == 7 || cKeyPressed == 6 || cKeyPressed == 8)
                {
                    // Handle CCW Rotation
                    fWorldA -= fRotSpeed;
					if (fWorldA < 0.0f) fWorldA += TWO_PI;
					xWorldA = float2fixed(fWorldA);
                }
                else if (cKeyPressed == 3 || cKeyPressed == 2 || cKeyPressed == 4)
                {
                    // Handle CW Rotation
                    fWorldA += fRotSpeed;
					if (fWorldA >= TWO_PI) fWorldA -= TWO_PI;
					xWorldA = float2fixed(fWorldA);
                }
                
                if (cKeyPressed == 1 || cKeyPressed == 2 || cKeyPressed == 8) 
                {
                    // Handle Forwards movement & collision
                    lxWorldX += xcos(xWorldA);
					lxWorldY += xsin(xWorldA);
                }
                else if (cKeyPressed == 5 || cKeyPressed == 4 || cKeyPressed == 6)
                {   
                    // Handle backwards movement & collision
                    lxWorldX -= xcos(xWorldA);
					lxWorldY -= xsin(xWorldA);
                }
            }
            
		    
            
        }

	}

}
