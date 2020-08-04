#include "../../fusion-c/header/msx_fusion.h"
#include "../../fusion-c/header/vdp_graph1.h"
#include <math.h>
#include <string.h>

/// Fixed point format
typedef int fixed;
typedef unsigned int ufixed;

#define FRACTIONAL_BITS 10

const unsigned int uScreenWidth  = 64u,    // Screen Resolution X (columns)
                   uScreenHeight = 48u,    // Screen Resolution Y (rows)
                   uWindowWidth  = 40u,	   // Window Screen Size X (columns)
                   uWindowHeight = 24u,    // Window Screen Size Y (rows)
                   uWindowX      = 23u,    // Left Window Border Position
                   uWindowY      = 0u ,    // Top Window Border Position
                   uMapX         = 0u ,	   // Left Map Border Position
                   uMapY         = 0u ,    // Top Map Border Position
                   uMapWidth     = 16u,    // World Dimensions
                   uMapHeight    = 16u ;
const int          nLogMapWidth  = 4;            // Log2(uMapWidth)
const float        fFOV          = PI / 4.0f,    // Field of View
                   fRotSpeed     = PI / 18.0f,    // Rotation Angle  
                   fVDPHz        = 50.0f;        // VDP freq. to calc. elapsed time
ufixed             uxDepth,      // Maximum rendering distance 
                   uxStepSize;   // Increment size for ray casting,
                                 // decrease to increase resolution
ufixed             uxPlayerX,    // Player Start Position
                   uxPlayerY;
fixed              xPlayerA;     // Player Start Angle
float              fPlayerA;     // Player Start Rotation

// Create Map of world space 0x0A = wall block, 0x01 = space
const unsigned char map[] = \
{ 0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x0A,0x0A,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x0A,0x0A,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, 
  0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A };

unsigned char TRANSPARENT = 0x00, 
              BLACK = 0x01,
              MEDIUM_GREEN = 0x02, 
              LIGHT_GREEN = 0x03, 
              DARK_BLUE = 0x04, 
              LIGHT_BLUE = 0x05, 
              DARK_RED = 0x06, 
              CYAN = 0x07, 
              MEDIUM_RED = 0x08, 
              LIGHT_RED = 0x09, 
              DARK_YELLOW = 0x0A, 
              LIGHT_YELLOW = 0x0B, 
              DARK_GREEN = 0x0C, 
              MAGENTA = 0x0D, 
              GRAY = 0x0E, 
              WHITE = 0x0F;

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
     bBoundary;

unsigned char *screen,  // Screen Framebuffer
              cColor1;


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
void pixel(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = ((y>>3)<<8)-((y>>3)<<3) + y ; // importante dividir antes de 
                                              // multiplicar, para obter a 
                                              // parte inteira da divisão
                                              // 248 = 8 * (32-1)
    if (x & 1u) // odd
    {
        vaddress += (x-1u)<<2;
        screen[vaddress] = color | (screen[vaddress] & 0b11110000);
                                                  // clean lower 4 bits
                                                  // e complementa com os 4 bits
    }                                             // menos significativos
    else       // even
    {
        vaddress += x<<2; // o x está multiplicado por 2, por isso "*4" e não "*8"
        screen[vaddress] = (color<<4) | (screen[vaddress] & 0b00001111);
                                                  // clean upper 4 bits
                                                  // e complementa com os 4 bits 
                                                  // mais significativos
    }                                      
    
}
/// VDP screen 3 vertical line draw
void vline(unsigned int x, unsigned int y1, unsigned int y2, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress;
    if (x & 1u) // odd
    {
        for (unsigned int y=y1; y<=y2; y++)
        {
            vaddress = ((y>>3)<<8)-((y>>3)<<3) + y + ((x-1u)<<2);
            screen[vaddress] = color | (screen[vaddress] & 0b11110000);
        }
    }
    else
    {   
        for (unsigned int y=y1; y<=y2; y++)
        {
            vaddress = ((y>>3)<<8)-((y>>3)<<3) + y + (x<<2);
            screen[vaddress] = (color<<4) | (screen[vaddress] & 0b00001111);
        }
    }
}
/// VDP screen 3 rectangle draw
void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) // x=coluna, y=linha
{
    for (unsigned int i = x1; i <=x2; i++)
        vline(i, y1, y2, color);
}
/// VDP screen 3 bitmap display
void bitmap(unsigned char *buffer, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    for (unsigned int i = 0u; i < width; i++)
		for (unsigned int j = 0u; j < height; j++)
            pixel(i+x, j+y, buffer[(j*width)+i]);
}



int main()
{
    Screen(3);
    PutText(4*4, 4*16, "Init...", 0);

    uxDepth       = float2ufixed(16.0f);   // Maximum rendering distance 
    uxStepSize    = float2ufixed(0.2f);    // Increment size for ray casting,
    uxPlayerX     = float2ufixed(5.09f);   // Player Start Position
    uxPlayerY     = float2ufixed(6.7f) ;
    xPlayerA      = float2fixed(0.0f)  ;   // Player Start Angle
    fPlayerA      = 0.0f;
    uxMinDistanceToWall = float2ufixed(2.0f);

    // Create Screen Buffer
    uScreenSize = (uScreenWidth>>1)*(uScreenHeight);
	screen = MMalloc(uScreenSize);
    MemFill(screen, 0, uScreenSize);

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
			uxDistanceToWall = uxMinDistanceToWall-(uxStepSize<<1); 
			xEyeX = xCos[nRayAngle]; // Unit vector for ray in player space
            xEyeY = xSin[nRayAngle];
            
			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
            do {
                uxDistanceToWall += uxStepSize;
				uTest =   ufixed2uint(uxPlayerX + fixed_mul(xEyeX, uxDistanceToWall)) + \
				        ((ufixed2uint(uxPlayerY + fixed_mul(xEyeY, uxDistanceToWall)))<<nLogMapWidth);
            } while (map[uTest] == BLACK);
            
            bBoundary = (uTest != uLastTest);
            uLastTest = uTest;
            
            if (uxDistanceToWall>=uxMinDistanceToWall)
            {             
                uCeiling = ufixed2uint(uxHalfWindowHeight - ufixed_div(uxWindowHeight, uxDistanceToWall)) + uWindowY;
                uFloor = uWindowHeight - 1u - uCeiling + uWindowY;

                // Shader walls based on distance
                if      (bBoundary)                       cColor1 = DARK_GREEN; 
                else if (uxDistanceToWall < (uxDepth>>2)) cColor1 = LIGHT_GREEN; // Very close	
                else if (uxDistanceToWall < (uxDepth>>1)) cColor1 = MEDIUM_GREEN;
                else if (uxDistanceToWall <  uxDepth)	  cColor1 = DARK_GREEN;
                else									  cColor1 = BLACK;       // Too far away

                uPosX = x + uWindowX;
                vline(uPosX, uCeiling, uFloor, cColor1);
                if (uCeiling > uWindowY)
                {
                    vline(uPosX, uWindowY, uCeiling-1u, GRAY);
                    //vline(uPosX, uFloor+1u, uWindowHeight-1u, GRAY);
                    
                    for (y=uWindowHeight-1u; y>=uFloor+1u; y--)
                    {
                        if      (y>uWindowHeight-4u) cColor1 = LIGHT_RED;
                        else if (y>uWindowHeight-8u) cColor1 = MEDIUM_RED;
                        else                         cColor1 = DARK_RED;
                        pixel(uPosX, y, cColor1);
                    }
                    
                }
            }
            else
            {
                vline(x + uWindowX, uWindowY, uWindowHeight-1u, LIGHT_GREEN);
            }

		}

		// Display Map
		bitmap(map, uMapX, uMapY, uMapWidth, uMapHeight);
        pixel(ufixed2uint(uxPlayerX)+uMapX, ufixed2uint(uxPlayerY)+uMapY, LIGHT_RED);

        // Display Frame
		CopyRamToVram(screen, 0, uScreenSize);

        // Display Stats
        fElapsedTime = RealTimer()/fVDPHz;
        //Itoa((int)(fPlayerA*180.0f/PI), sAngle, 10);
        Itoa((int)fElapsedTime, sElapsedTime1, 10);
        Itoa((int)((fElapsedTime-(int)fElapsedTime)*100), sElapsedTime2, 10);

        //rect(26,26,43,42,LIGHT_BLUE);
        //PutText(0, 4*26, "ang", 0);
        //PutText(0, 4*36, "spf", 0);
        //PutText(26*4, 4*26, sAngle, 0);
        PutText(26*4, 4*36, sElapsedTime1, 0);
        PutText(31*4, 4*36, ".", 0);
        PutText(35*4, 4*36, sElapsedTime2, 0);

        bInvalidKey = 1;
        while (bInvalidKey)
        {
            bInvalidKey = 0;

            Beep();
            KillKeyBuffer();
            cKeyPressed = WaitKey();

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
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == 0x0A)
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
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == 0x0A)
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
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == 0x0A)
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
			    if (map[ufixed2uint(uxPlayerY) * uMapWidth + ufixed2uint(uxPlayerX)] == 0x0A)
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

