#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph1.h"
#include <math.h>
#include <string.h>

const unsigned short nScreenWidth = 64,	\     // Screen Resolution X (columns)
                     nScreenHeight = 48, \    // Screen Resolution Y (rows)
                     nWindowWidth = 40,	\     // Window Screen Size X (columns)
                     nWindowHeight = 24, \    // Window Screen Size Y (rows)
                     nWindowX = 17,	\         // Left Window Border Position
                     nWindowY = 0, \          // Top Window Border Position
                     nMapX = 0,	\             // Left Map Border Position
                     nMapY = 0, \             // Top Map Border Position
                     nMapWidth = 16, \        // World Dimensions
                     nMapHeight = 8, \
                     VDPHz = 50;              // VDP freq. to calc. elapsed time
const float fFOV = PI / 3.0f, \               // Field of View
            fDepth = 16.0f, \          		  // Maximum rendering distance
            fRotSpeed = PI / 6.0f, \          // Rotation Angle
            fStepSize = 0.5f;                 // Increment size for ray casting,
                                              // decrease to increase resolution
float fPlayerX = 5.09f,	\		              // Player Start Position
      fPlayerY = 6.7f, \
      fPlayerA = 0.0f;                        // Player Start Rotation

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

float fHalfFOV, \
      fWindowWidth, \
      fHalfWindowHeight, \
      step_angle, \
      cur_angle, \
      fDistanceToWall, \
      fEyeX, \
      fEyeY, \
      *sin, \        // Trigonometric Lookup Table.
      *cos;          //


unsigned short nTestX, \
               nTestY, \
               nCeiling, \
               nFloor, \
               nPosX, \
               x, y;

unsigned char *screen, \  // Screen Framebuffer
              nColor1;

char key_press, \
     bInvalidKey, \
     sElapsedTime[4], \ // Data Display
     sAngle[4];

short nRayAngle;

int angles;              

/// Framebuffer screen 3 pixel set
void pixel(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = (y>>3)*248 + y ; // importante dividir antes de 
                                             // multiplicar, para obter a 
                                             // parte inteira da divisão
                                             // 248 = 8 * (32-1)
    if (x & 1) // odd
    {
        vaddress += (x-1)<<2;
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
    for (unsigned int i = 0; i < width; i++)
		for (unsigned int j = 0; j < height; j++)
            pixel(i+x, j+y, buffer[(j*width)+i]);
}

int main()
{
    // Create Screen Buffer
	screen = MMalloc((nScreenWidth>>1)*nScreenHeight);

    Screen(3);
    PutText(4*4, 4*16, "Init...", 0);

    // Create Trigonometric Lookup Table
    angles = (int)(TWO_PI*(nWindowWidth/fFOV)+1.0f);
    step_angle = TWO_PI/angles;
    cur_angle = 0.0f;
    cos = MMalloc(4*angles);                // 4 bytes per float, 2pi/FOV is the
    sin = MMalloc(4*angles);                // number of FOVs and Width is the
    for (unsigned int l=0; l<angles; l++)   // number of angles per FOV
    {
        cos[l]=cosf(cur_angle);
        sin[l]=sinf(cur_angle);
        cur_angle+=step_angle;
    }

    fHalfFOV = fFOV/2.0f;                // Optimization
    fWindowWidth = (float)nWindowWidth;       
    fHalfWindowHeight = nWindowHeight/2.0f;

	while (1)
	{
		SetRealTimer(0);

        MemFill(screen, 0, (nScreenWidth>>1)*nScreenHeight);

        x = nWindowWidth;
        while (x--)
		{
			// For each column, calc. the projected ray angle into world space
			nRayAngle = (short)((fPlayerA - fHalfFOV + \
                              (x/fWindowWidth)*fFOV) / step_angle);
                     
            if (nRayAngle < 0)           nRayAngle+=angles;
            else if (nRayAngle > angles) nRayAngle-=angles;
            
			// Find distance to wall
			fDistanceToWall = 0.0f; 

			fEyeX = cos[nRayAngle]; // Unit vector for ray in player space
            fEyeY = sin[nRayAngle];

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
            do {
                fDistanceToWall += fStepSize;
				nTestX = (unsigned short)(fPlayerX + fEyeX * fDistanceToWall);
				nTestY = (unsigned short)(fPlayerY + fEyeY * fDistanceToWall);
            } while (map[(nMapWidth * nTestY) + nTestX] != DARK_YELLOW);
		    
            // Shader walls based on distance
            if      (fDistanceToWall <= fDepth / 5.0f) {nColor1 = LIGHT_GREEN;} // Very close	
	        else if (fDistanceToWall <  fDepth / 3.0f) {nColor1 = MEDIUM_GREEN;}
	        else if (fDistanceToWall <  fDepth)		   {nColor1 = DARK_GREEN;}
	        else									   {nColor1 = BLACK;}       // Too far away

            // Calculate distance to ceiling and floor
			nCeiling = ((unsigned short)(fHalfWindowHeight - nWindowHeight/fDistanceToWall)) + nWindowY;
			nFloor = nWindowHeight - nCeiling - 2 + nWindowY;
            nPosX = x + nWindowX;

            line(nPosX, nWindowY, nPosX, nCeiling, BLACK);
            line(nPosX, nFloor+1, nPosX, nWindowHeight-1, GRAY);
            line(nPosX, nCeiling+1, nPosX, nFloor, nColor1);
            
		}

		// Display Map
		bitmap(map, nMapX, nMapY, nMapWidth, nMapHeight);
        pixel(((unsigned int)fPlayerX)+nMapX, ((unsigned int)fPlayerY)+nMapY, LIGHT_RED);

        // Display Frame
		CopyRamToVram(screen, 0, (nScreenWidth>>1)*nScreenHeight);

        // Display Stats
        Itoa((int)(fPlayerA*180.0f/PI), sAngle, 10);
        Itoa((int)(RealTimer()/VDPHz), sElapsedTime, 10);

        PutText(0, 4*26, "ang", 0);
        PutText(0, 4*36, "spf", 0);
        PutText(26*4, 4*26, sAngle, 0);
        PutText(26*4, 4*36, sElapsedTime, 0);

        bInvalidKey = 1;
        while (bInvalidKey)
        {
            bInvalidKey = 0;

            Beep();
            KillKeyBuffer();
            key_press = WaitKey();

            if (key_press == 27)         // Exit program
            {
                Screen(0);
                Cls();
                return 0;
            }
		    else if (key_press == 29)    // Handle CCW Rotation
            {
                fPlayerA -= fRotSpeed;
                if (fPlayerA < 0.0f) fPlayerA = TWO_PI-fRotSpeed;
            }
		    else if (key_press == 28)	 // Handle CW Rotation
            {
                fPlayerA += fRotSpeed;
                if (fPlayerA >= TWO_PI) fPlayerA = 0;
            }
		    else if (key_press == 30)	 // Handle Forwards movement & collision
		    {
                nRayAngle = (unsigned int)(fPlayerA / step_angle);
			    fPlayerX += cos[nRayAngle];
			    fPlayerY += sin[nRayAngle];
			    if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == 0x0A)
			    {
				    fPlayerX -= cos[nRayAngle];
				    fPlayerY -= sin[nRayAngle];
                    bInvalidKey = 1;
			    }			
		    }
		    else if (key_press == 31)	 // Handle backwards movement & collision
		    {   
                nRayAngle = (unsigned int)(fPlayerA / step_angle);
			    fPlayerX -= cos[nRayAngle];
			    fPlayerY -= sin[nRayAngle];
			    if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == 0x0A)
			    {
				    fPlayerX += cos[nRayAngle];
				    fPlayerY += sin[nRayAngle];
                    bInvalidKey = 1;
			    }
		    }
            else
                bInvalidKey = 1;
        }
	}

}

