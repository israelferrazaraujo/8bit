#include "../../fusion-c/header/msx_fusion.h"
#include "../../fusion-c/header/vdp_graph1.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/// Fixed point format
typedef int fixed;
typedef unsigned int ufixed;

#define FRACTIONAL_BITS 10

const unsigned int uScreenWidth  = 80u,    // Screen Resolution X (columns)
                   uScreenHeight = 24u,    // Screen Resolution Y (rows)
                   uWindowWidth  = 80u,	   // Window Screen Size X (columns)
                   uWindowHeight = 24u,    // Window Screen Size Y (rows)
                   uWindowX      = 0u,    // Left Window Border Position
                   uWindowY      = 0u ,    // Top Window Border Position
                   uMapX         = 0u ,	   // Left Map Border Position
                   uMapY         = 1u ,    // Top Map Border Position
                   uMapWidth     = 16u,    // World Dimensions
                   uMapHeight    = 8u ;
const int          nLogMapWidth  = 4;            // Log2(uMapWidth)
const float        fFOV          = PI / 4.0f,    // Field of View
                   fRotSpeed     = PI / 36.0f,    // Rotation Angle  
                   fVDPHz        = 50.0f;        // VDP freq. to calc. elapsed time
ufixed             uxDepth,      // Maximum rendering distance 
                   uxStepSize;   // Increment size for ray casting,
                                 // decrease to increase resolution
ufixed             uxPlayerX,    // Player Start Position
                   uxPlayerY;
fixed              xPlayerA;     // Player Start Angle
float              fPlayerA;     // Player Start Rotation

// Create Map of world space 0x0A = wall block, 0x01 = space
const char *map = "################"
	              "#......#.......#"
	              "#......#.......#"
	              "#......#.......#"
	              "#..............#"
	              "#......#########"
	              "#..............#"
	              "################";

int nRayAngle, 
    nAngles;

unsigned int uTest,
             uLastTest,
             uCeiling,
             uFloor,
             uPosX,
             x, y,      // Counters
             uScreenSize;

fixed *xSin,               // Trigonometric Lookup Table
      *xCos,               //
      *xRayAngles,         // Ray Angle Lookup Table
      xStepAngle,
      xEyeX, 
      xEyeY;
    

ufixed uxDistanceToWall,
       uxMinDistanceToWall,
       uxWindowHeight,
       uxHalfWindowHeight;

float fHalfFOV, 
      fWindowWidth, 
      fStepAngle, 
      fCurAngle, 
      fElapsedTime;

char cKeyPressed, 
     bInvalidKey, 
     sElapsedTime1[4],  // Data Display
     sElapsedTime2[4],  // Data Display
     sAngle[4],
     sBuffer[4],
     bBoundary=0;

char *screen,  // Screen Framebuffer
     c1;


inline unsigned int ufixed2uint(ufixed input)
{
    return input >> FRACTIONAL_BITS;
}
inline ufixed uint2ufixed(unsigned int input)
{
    return input << FRACTIONAL_BITS;
}
inline float ufixed2float(ufixed input)
{
    return (float)input / (float)(1u << FRACTIONAL_BITS);
}
inline ufixed float2ufixed(float input)
{
    return (ufixed)(input * (1u << FRACTIONAL_BITS));
}
inline ufixed ufixed_mul(ufixed mer, ufixed mcand)
{
    return (mer >> 1) * (mcand >> (FRACTIONAL_BITS-1));
}
inline ufixed ufixed_div(ufixed num, ufixed den)
{
    return (ufixed)( ( ((unsigned long)num) << FRACTIONAL_BITS ) / (unsigned long)den );
}


inline int fixed2int(fixed input)
{
    return input / (1 << FRACTIONAL_BITS);
}
inline fixed int2fixed(int input)
{
    return input * (1 << FRACTIONAL_BITS);
}
inline float fixed2float(fixed input)
{
    return (float)input / (float)(1 << FRACTIONAL_BITS);
}
inline fixed float2fixed(float input)
{
    return (fixed)(input * (1 << FRACTIONAL_BITS));
}
inline fixed fixed_mul(fixed mer, ufixed mcand)
{
    return (mer / 2)  * (mcand >> (FRACTIONAL_BITS-1));
}
inline fixed fixed_div(fixed num, fixed den)
{
    return (fixed)( ( (long)num * (1l << FRACTIONAL_BITS) ) / (long)den );
}


/// Framebuffer screen 3 pixel set
void point(unsigned int x, unsigned int y, char c) // x=coluna, y=linha
{
    screen[y*uScreenWidth+x] = c;
}
/// VDP screen 3 vertical line draw
void vline(unsigned int x, unsigned int y1, unsigned int y2, char c) // x=coluna, y=linha
{
    unsigned int k = y2*uScreenWidth+x;
    for (unsigned int i=y1*uScreenWidth+x; i<=k; i+=uScreenWidth)
        screen[i] = c;
}
/// VDP screen 3 rectangle draw
void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, char c) // x=coluna, y=linha
{
    for (unsigned int i = x1; i <= x2; i++)
        vline(i, y1, y2, c);
}
/// VDP screen 3 bitmap display
void bitmap(char *buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    for (unsigned int i = 0u; i < width; i++)
		for (unsigned int j = 0u; j < height; j++)
            point(i+x, j+y, buffer[j*width+i]);
}
unsigned int print(unsigned int x, unsigned int y, char *str)
{
    unsigned int k = y*uScreenWidth+x;
    while (*str != '\0')
        screen[k++]=*(str++);
    
    return k-(y*uScreenWidth+x);
}
unsigned int printInt(unsigned int x, unsigned int y, int value)
{
    char buffer[5];
    return print(x, y, Itoa(value, buffer, 10) );
}
unsigned int printFloat(unsigned int x, unsigned int y, float value)
{
    char buffer[5];
    unsigned int l = 0;
    l  = print(x, y, Itoa((int)value, buffer, 10) );
    l += print(x+l, y, ".");
    Itoa((int)((value-(int)value)*100.0f), buffer, 10);
    if (strlen(buffer) == 1)
        l += print(x+l, y, "0");
    l += print(x+l, y, buffer);

    return l;
}

int main()
{
    Screen(0);
    Width(uScreenWidth);
    SetColors(15,1,1);
    KeySound(0);

    printf("Init...");

    uxDepth       = float2ufixed(16.0f);   // Maximum rendering distance 
    uxStepSize    = float2ufixed(0.5f);    // Increment size for ray casting,
    uxPlayerX     = float2ufixed(5.09f);   // Player Start Position
    uxPlayerY     = float2ufixed(6.7f) ;
    xPlayerA      = float2fixed(0.0f)  ;   // Player Start Angle
    fPlayerA      = 0.0f;
    uxMinDistanceToWall = float2ufixed(2.0f);

    // Create Screen Buffer
    uScreenSize = uScreenWidth*uScreenHeight;
	screen = MMalloc(uScreenSize);
    MemFill(screen, ' ', uScreenSize);

    fHalfFOV = fFOV/2.0f;                // Optimization
    fWindowWidth = (float)uWindowWidth;       
    uxHalfWindowHeight = uint2ufixed(uWindowHeight/2u);
    uxWindowHeight = uint2ufixed(uWindowHeight);
    
    // Create Trigonometric Lookup Table
    nAngles = (int)(TWO_PI*(uWindowWidth/fFOV));
    fStepAngle = TWO_PI/nAngles;
    fCurAngle = 0.0f;
    xCos = MMalloc(2*nAngles);                // 2 bytes per int, 2pi/FOV is the
    xSin = MMalloc(2*nAngles);                // number of FOVs and Width is the
    for (int l=0; l<nAngles; l++)             // number of angles per FOV
    {
        xCos[l] = float2fixed(cosf(fCurAngle));
        xSin[l] = float2fixed(sinf(fCurAngle));
        fCurAngle+=fStepAngle;
    }

    xStepAngle = float2fixed(fStepAngle);
    // Create Ray Angle Lookup Table
    xRayAngles = MMalloc(uWindowWidth<<1);
    for (int l=0; l<=uWindowWidth; l++)           
    {
        xRayAngles[l] = float2fixed(fHalfFOV - (l/fWindowWidth)*fFOV);
    }
    
    while (1)
	{
		SetRealTimer(0);

        uLastTest = 32768u;
        xPlayerA = float2fixed(fPlayerA);
        x = uWindowWidth;
        while (x--)
		{
			// For each column, calc. the projected ray angle into world space
            nRayAngle = (xPlayerA - xRayAngles[x]) / xStepAngle;

            if (nRayAngle < 0)             nRayAngle+=nAngles;
            else if (nRayAngle >= nAngles) nRayAngle-=nAngles;
            
			// Find distance to wall
			uxDistanceToWall = uxStepSize; 
			xEyeX = xCos[nRayAngle]; // Unit vector for ray in player space
            xEyeY = xSin[nRayAngle];
            
			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
            do {
                uxDistanceToWall += uxStepSize;
				uTest =   ufixed2uint(uxPlayerX + fixed_mul(xEyeX, uxDistanceToWall)) + \
				        ((ufixed2uint(uxPlayerY + fixed_mul(xEyeY, uxDistanceToWall)))<<nLogMapWidth);
            } while (map[uTest] == '.');
            
            bBoundary = (uTest != uLastTest);
            uLastTest = uTest;
            
            if (uxDistanceToWall>=uxMinDistanceToWall)
            {             
                uCeiling = ufixed2uint(uxHalfWindowHeight - ufixed_div(uxWindowHeight, uxDistanceToWall)) + uWindowY;
                uFloor = uWindowHeight - 1u - uCeiling + uWindowY;

                // Shader walls based on distance
                if      (bBoundary)                        c1 = '|'; //124; 
                else if (uxDistanceToWall <= (uxDepth>>3)) c1 = '*'; //219; // Very close	
                else if (uxDistanceToWall <= (uxDepth>>2)) c1 = 'X'; //199; 
                else if (uxDistanceToWall <  (uxDepth>>1)) c1 = 'x'; //215; 
                else if (uxDistanceToWall <   uxDepth)	   c1 = '~'; //203;
                else									   c1 = ' '; // Too far away

                uPosX = x + uWindowX;
                vline(uPosX, uCeiling, uFloor, c1);
                if (uCeiling > uWindowY)
                {
                    vline(uPosX, uWindowY, uCeiling-1u, ' ');

                    //vline(uPosX, uFloor+1u, uWindowHeight-1u, '.');
                                        
                    for (y=uWindowHeight-1u; y>=uFloor+1u; y--)
                    {
                        if (y>=uWindowHeight-(uWindowHeight>>2)) c1 = '-';
                        else                                     c1 = '=';
                        point(uPosX, y, c1);
                    }
                    
                }
            }
            else
            {
                vline(x + uWindowX, uWindowY, uWindowHeight-1u, '*'); // 219);
            }

		}

		// Display Map
		bitmap(map, uMapX, uMapY, uMapWidth, uMapHeight);
        point(ufixed2uint(uxPlayerX)+uMapX, ufixed2uint(uxPlayerY)+uMapY, 'P');

        // Display Stats
        fElapsedTime = RealTimer()/fVDPHz;
        x =print(0, 0, "X=");     x+=printFloat(x, 0, ufixed2float(uxPlayerX));
        x+=print(x, 0, ", Y=");   x+=printFloat(x, 0, ufixed2float(uxPlayerY));
        x+=print(x, 0, ", A=");   x+=printFloat(x, 0, fPlayerA*180.0f/PI);
        x+=print(x, 0, ", SPF=");    printFloat(x, 0, fElapsedTime);
        
        // Display Frame
		CopyRamToVram(screen, 0, uScreenSize);

        bInvalidKey = 1;
        KillKeyBuffer();
        while (bInvalidKey)
        {
            bInvalidKey = 0;

            cKeyPressed = Inkey();

            if (cKeyPressed == 27)         // Exit program
            {
                Screen(0);
                Cls();
                return 0;
            }
		    else if (cKeyPressed == 29)    // Handle CCW Rotation
            {
                fPlayerA -= fRotSpeed;
                if (fPlayerA < 0.0f) fPlayerA += TWO_PI;
            }
		    else if (cKeyPressed == 28)	 // Handle CW Rotation
            {
                fPlayerA += fRotSpeed;
                if (fPlayerA >= TWO_PI) fPlayerA -= TWO_PI;
            }
		    
            else if (cKeyPressed == 30)	 // Handle Forwards movement & collision
		    {
                nRayAngle = (int)(fPlayerA / fStepAngle);
			    uxPlayerX += xCos[nRayAngle]>>1;
			    uxPlayerY += xSin[nRayAngle]>>1;
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == '#')
			    {
				    uxPlayerX -= xCos[nRayAngle]>>1;
				    uxPlayerY -= xSin[nRayAngle]>>1;
                    bInvalidKey = 1;
			    }			
		    }
		    else if (cKeyPressed == 31)	 // Handle backwards movement & collision
		    {   
                nRayAngle = (int)(fPlayerA / fStepAngle);
			    uxPlayerX -= xCos[nRayAngle]>>1;
			    uxPlayerY -= xSin[nRayAngle]>>1;
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == '#')
			    {
				    uxPlayerX += xCos[nRayAngle]>>1;
				    uxPlayerY += xSin[nRayAngle]>>1;
                    bInvalidKey = 1;
			    }
		    }

            else if (cKeyPressed == 18)	 // Handle Left movement & collision
		    {
                nRayAngle = (int)( (fPlayerA-HALF_PI) / fStepAngle);
			    uxPlayerX += xCos[nRayAngle];
			    uxPlayerY += xSin[nRayAngle];
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == '#')
			    {
				    uxPlayerX -= xCos[nRayAngle];
				    uxPlayerY -= xSin[nRayAngle];
                    bInvalidKey = 1;
			    }			
		    }
		    else if (cKeyPressed == 11)	 // Handle Right movement & collision
		    {   
                nRayAngle = (int)((fPlayerA+HALF_PI) / fStepAngle);
			    uxPlayerX -= xCos[nRayAngle];
			    uxPlayerY -= xSin[nRayAngle];
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == '#')
			    {
				    uxPlayerX += xCos[nRayAngle];
				    uxPlayerY += xSin[nRayAngle];
                    bInvalidKey = 1;
			    }
		    }
            else
            {
                bInvalidKey = 1;
            }
        }
	}

}

