#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph1.h"
#include <math.h>
#include <string.h>

#define nFIXED 100
#define uFIXED 100u
#define lFIXED 100l
#define fFIXED 100.0f

const unsigned int uScreenWidth = 64u,	\   // Screen Resolution X (columns)
                   uScreenHeight = 48u, \    // Screen Resolution Y (rows)
                   uWindowWidth = 40u,	\   // Window Screen Size X (columns)
                   uWindowHeight = 24u, \    // Window Screen Size Y (rows)
                   uWindowX = 17u,	\       // Left Window Border Position
                   uWindowY = 0u, \          // Top Window Border Position
                   uMapX = 0u,	\           // Left Map Border Position
                   uMapY = 0u, \             // Top Map Border Position
                   uMapWidth = 16u, \        // World Dimensions
                   uMapHeight = 8u;

const int nDepth = 160, \                   // Maximum rendering distance 
          nStepSize = 5;                    // Increment size for ray casting,
                                            // decrease to increase resolution
const float fFOV = PI / 3.0f, \             // Field of View
            fRotSpeed = PI / 9.0f, \        // Rotation Angle  
            fVDPHz = 50.0f;                 // VDP freq. to calc. elapsed time
float fPlayerA = 0.0f;                      // Player Start Rotation
unsigned int uPlayerX = (unsigned int)(5.09f * fFIXED), \ // Player Start Position
             uPlayerY = (unsigned int)(6.7f * fFIXED);
long         lPlayerA = 0l;
// Create Map of world space 0x0A = wall block, 0x01 = space
const unsigned char map[] = \
{ 0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, \
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, \
  0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
  0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A };

unsigned char TRANSPARENT = 0x00, \
              BLACK = 0x01, \
              MEDIUM_GREEN = 0x02, \
              LIGHT_GREEN = 0x03, \
              DARK_BLUE = 0x04, \
              LIGHT_BLUE = 0x05, \
              DARK_RED = 0x06, \
              CYAN = 0x07, \
              MEDIUM_RED = 0x08, \
              LIGHT_RED = 0x09, \
              DARK_YELLOW = 0x0A, \
              LIGHT_YELLOW = 0x0B, \
              DARK_GREEN = 0x0C, \
              MAGENTA = 0x0D, \
              GRAY = 0x0E, \
              WHITE = 0x0F;

int nEyeX, \
    nEyeY, \
    nRayAngle, \
    nAngles, \
    *nSin, \        // Trigonometric Lookup Table
    *nCos;        //
    
long *nRayAngles, \     // Ray Angle Lookup Table
      nStepAngle;

unsigned int uTest, \
             uCeiling, \
             uFloor, \
             uPosX, \
             x, y;      // Counters

int nDistanceToWall;

float fHalfFOV, \
      fWindowWidth, \
      fHalfWindowHeight, \
      fStepAngle, \
      fCurAngle, \
      fElapsedTime, \
      *fRayAngles; // Ray Angle Lookup Table

char cKeyPressed, \
     bInvalidKey, \
     sElapsedTime1[4], \   // Data Display
     sElapsedTime2[4], \   // Data Display
     sAngle[4];

unsigned char *screen, \  // Screen Framebuffer
              cColor1;

/// Framebuffer screen 3 pixel set
void pixel(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = (y>>3)*248u + y ; // importante dividir antes de 
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
/// VDP screen 3 line draw
void line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) // x=coluna, y=linha
{
    int dx = x2-x1, \
        dy = y2-y1;
    
    unsigned int h1 = (unsigned int)dx, \
                 h2 = (unsigned int)dy;

    unsigned int h = ((h2>h1)?h2:h1);

    dx = dx/h;
    dy = dy/h;

    h1=x1; h2=y1;
    do {
        pixel(h1, h2, color);
        h1+=dx;
        h2+=dy;
    } while (h--);
}
/// VDP screen 3 rectangle draw
void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) // x=coluna, y=linha
{
    for (unsigned int i = x1; i <=x2; i++)
        line(i, y1, i, y2, color);
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
    // Create Screen Buffer
	screen = MMalloc((uScreenWidth>>1)*uScreenHeight);

    Screen(3);
    PutText(4*4, 4*16, "Init...", 0);

    fHalfFOV = fFOV/2.0f;                // Optimization
    fWindowWidth = (float)uWindowWidth;       
    fHalfWindowHeight = uWindowHeight/2.0f;

    // Create Trigonometric Lookup Table
    nAngles = (int)(TWO_PI*(uWindowWidth/fFOV)+1.0f);
    fStepAngle = TWO_PI/nAngles;
    fCurAngle = 0.0f;
    nCos = MMalloc(2*nAngles);                // 2 bytes per int, 2pi/FOV is the
    nSin = MMalloc(2*nAngles);                // number of FOVs and Width is the
    for (int l=0; l<nAngles; l++)             // number of angles per FOV
    {
        nCos[l] = (int)(cosf(fCurAngle)*fFIXED);
        nSin[l] = (int)(sinf(fCurAngle)*fFIXED);
        fCurAngle+=fStepAngle;
    }

    nStepAngle = (long)(fStepAngle*fFIXED*fFIXED*fFIXED);
    // Create Ray Angle Lookup Table
    nRayAngles = MMalloc(4*uWindowWidth);    
    for (int l=0; l<=uWindowWidth; l++)           
    {
        nRayAngles[l] = (long)((fHalfFOV - (l/fWindowWidth)*fFOV)*fFIXED*fFIXED*fFIXED);
    }
    
	while (1)
	{
		SetRealTimer(0);

        MemFill(screen, 0, (uScreenWidth>>1)*uScreenHeight);

        lPlayerA = (long)(fPlayerA*fFIXED*fFIXED*fFIXED);
        x = uWindowWidth;
        while (x--)
		{
            //Cls();
            //Itoa(x, sAngle, 10);
            //PutText(4*4, 0, sAngle, 0);
            
			// For each column, calc. the projected ray angle into world space
			nRayAngle = (int)(((lPlayerA - nRayAngles[x])/nStepAngle));

            if (nRayAngle < 0)             nRayAngle+=nAngles;
            else if (nRayAngle >= nAngles) nRayAngle-=nAngles;
            
			// Find distance to wall
			nDistanceToWall = 0; 

			nEyeX = nCos[nRayAngle]; // Unit vector for ray in player space
            nEyeY = nSin[nRayAngle];
            //PutText(4*4, 4*30, "1", 0);
			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
            do {
                nDistanceToWall += nStepSize;
				uTest = (((uPlayerX + (unsigned int)((nEyeX * nDistanceToWall)/10))/uFIXED) + \
				         ((uPlayerY + (unsigned int)((nEyeY * nDistanceToWall)/10))/uFIXED) * uMapWidth);
            } while (map[uTest] != DARK_YELLOW);
            //PutText(4*12, 4*30, "2", 0);
            // Shader walls based on distance
            if      (nDistanceToWall <= nDepth / 5) {cColor1 = LIGHT_GREEN;} // Very close	
	        else if (nDistanceToWall <  nDepth / 3) {cColor1 = MEDIUM_GREEN;}
	        else if (nDistanceToWall <  nDepth)		{cColor1 = DARK_GREEN;}
	        else									{cColor1 = BLACK;}       // Too far away
            //PutText(4*20, 4*30, "3", 0);
            // Calculate distance to ceiling and floor
			uCeiling = (unsigned int)(fHalfWindowHeight - uWindowHeight/(nDistanceToWall/10.0f)) + uWindowY;
			uFloor = uWindowHeight - uCeiling - 2u + uWindowY;
            uPosX = x + uWindowX;
            //PutText(4*28, 4*30, "4", 0);
            line(uPosX, uWindowY, uPosX, uCeiling, BLACK);
            line(uPosX, uFloor+1u, uPosX, uWindowHeight-1u, GRAY);
            line(uPosX, uCeiling+1u, uPosX, uFloor, cColor1);
            //PutText(4*36, 4*30, "5", 0);
		}

		// Display Map
		bitmap(map, uMapX, uMapY, uMapWidth, uMapHeight);
        pixel((uPlayerX/nFIXED)+uMapX, (uPlayerY/nFIXED)+uMapY, LIGHT_RED);

        // Display Frame
		CopyRamToVram(screen, 0, (uScreenWidth>>1)*uScreenHeight);

        // Display Stats
        //fElapsedTime = RealTimer()/fVDPHz;
        //Itoa((int)(fPlayerA*180.0f/PI), sAngle, 10);
        //Itoa((int)fElapsedTime, sElapsedTime1, 10);
        //Itoa((int)((fElapsedTime-(int)fElapsedTime)*100), sElapsedTime2, 10);

        //PutText(0, 4*26, "ang", 0);
        //PutText(0, 4*36, "spf", 0);
        //PutText(26*4, 4*26, sAngle, 0);
        //PutText(26*4, 4*36, sElapsedTime1, 0);
        //PutText(34*4, 4*36, ".", 0);
        //PutText(42*4, 4*36, sElapsedTime2, 0);

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
                if (fPlayerA < 0.0f) fPlayerA = TWO_PI+fPlayerA;
            }
		    else if (cKeyPressed == 28)	 // Handle CW Rotation
            {
                fPlayerA += fRotSpeed;
                if (fPlayerA >= TWO_PI) fPlayerA = fPlayerA-TWO_PI;
            }
		    else if (cKeyPressed == 30)	 // Handle Forwards movement & collision
		    {
                nRayAngle = (int)(fPlayerA / fStepAngle);
			    uPlayerX += nCos[nRayAngle];
			    uPlayerY += nSin[nRayAngle];
			    if (map[(uPlayerY/uFIXED) * uMapWidth + (uPlayerX/uFIXED)] == 0x0A)
			    {
				    uPlayerX -= nCos[nRayAngle];
				    uPlayerY -= nSin[nRayAngle];
                    bInvalidKey = 1;
			    }			
		    }
		    else if (cKeyPressed == 31)	 // Handle backwards movement & collision
		    {   
                nRayAngle = (int)(fPlayerA / fStepAngle);
			    uPlayerX -= nCos[nRayAngle];
			    uPlayerY -= nSin[nRayAngle];
			    if (map[(uPlayerY/uFIXED) * uMapWidth + (uPlayerX/uFIXED)] == 0x0A)
			    {
				    uPlayerX += nCos[nRayAngle];
				    uPlayerY += nSin[nRayAngle];
                    bInvalidKey = 1;
			    }
		    }
            else
                bInvalidKey = 1;
        }
	}

}

