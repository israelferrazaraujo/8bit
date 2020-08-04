#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph1.h"
#include <math.h>

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

float fPlayerX = 5.09f,	\		              // Player Start Position
      fPlayerY = 6.7f, \
      fPlayerA = 0.0f;                        // Player Start Rotation

const float fFOV = 3.14159f / 3.0f, \         // Field of View
            fDepth = 16.0f, \          		  // Maximum rendering distance
            fSpeed = 1.0f, \          		  // Walking Speed
            fRotation = 3.14159f / 4.0f, \	  // Rotation Angle
            fBound = 0.005f;

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

float fWindowWidth;
float fHalfWindowHeight;

unsigned int nElapsedTime;

float fRayAngle;
float fStepSize;
float fDistanceToWall;
float fEyeX;
float fEyeY;
char bHitWall;

short nTestX;
short nTestY;

unsigned short nCeiling;
unsigned short nFloor;
unsigned short nPosX;

unsigned char nColor1;

char key_press;
char bInvalidKey;

unsigned short x, y;

/// VDP screen 3 pixel set
void pixel(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = ((y>>3)<<3)*31 + y ; // importante dividir antes de 
                                                 // multiplicar, para obter a 
                                                 // parte inteira da divisão
                                                 // 248 = 8 * (32-1)
    if (x & 1) // odd x%2==x&(2-1)
    {
        vaddress += (x-1)<<2;
        Vpoke(vaddress, color | (Vpeek(vaddress) & 0b11110000));
                                                  // clean lower 4 bits
                                                  // e complementar com os 4 bits
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
/// VDP screen 3 line draw
void line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) // x=coluna, y=linha
{
    int dx = x2-x1, \
        dy = y2-y1;
    
    unsigned int h1 = (unsigned int)((dx<0)?-dx:dx), \
                 h2 = (unsigned int)((dy<0)?-dy:dy);

    h1 = ((h2>h1)?h2:h1);

    dx = dx/h1;
    dy = dy/h1;

    unsigned int x=x1, y=y1;
    for (unsigned short i=0; i<=h1; i++)
    {
        pixel(x, y, color);
        x+=dx;
        y+=dy;
    }
}
/// VDP screen 3 bitmap display
void bitmap(unsigned char *buffer, unsigned int x, unsigned int y, unsigned short width, unsigned short height)
{
    for (unsigned short i = 0; i < width; i++)
		for (unsigned short j = 0; j < height; j++)
            pixel(i+x, j+y, buffer[(j*width)+i]);
}

/// Framebuffer screen 3 pixel set
void fbpixel(unsigned char *buffer, unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
{
    unsigned int vaddress = ((y>>3)<<3)*31 + y ; // importante dividir antes de 
                                                 // multiplicar, para obter a 
                                                 // parte inteira da divisão
                                                 // 248 = 8 * (32-1)
    if (x & 1) // odd
    {
        vaddress += (x-1)<<2;
        buffer[vaddress] = color | (buffer[vaddress] & 0b11110000);
                                                  // clean lower 4 bits
                                                  // e complementa com os 4 bits
    }                                             // menos significativos
    else       // even
    {
        vaddress += x<<2; // o x está multiplicado por 2, por isso "*4" e não "*8"
        buffer[vaddress] = (color<<4) | (buffer[vaddress] & 0b00001111);
                                                  // clean upper 4 bits
                                                  // e complementa com os 4 bits 
                                                  // mais significativos
    }                                      
    
}
void fbline(unsigned char *buffer, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) // x=coluna, y=linha
{
    int dx = x2-x1, \
        dy = y2-y1;
    
    unsigned int h1 = (unsigned int)((dx<0)?-dx:dx), \
                 h2 = (unsigned int)((dy<0)?-dy:dy);

    h1 = ((h2>h1)?h2:h1);

    dx = dx/h1;
    dy = dy/h1;

    unsigned int x=x1, y=y1;
    for (unsigned short i=0; i<=h1; i++)
    {
        fbpixel(buffer, x, y, color);
        x+=dx;
        y+=dy;
    }
}

int main()
{
    // Create Screen Buffer
	//unsigned char *screen = MMalloc((nScreenWidth>>1)*nScreenHeight);

	// Create Map of world space # = wall block, . = space
    const unsigned char map[] = \
   { 0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, \
     0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
     0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
     0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
     0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
     0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A, \
     0x0A,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x0A, \
     0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A };

	Screen(3);

    fWindowWidth = nWindowWidth/1.0f;       // Optimization
    fHalfWindowHeight = nWindowHeight/2.0f; //

	while (1)
	{
		//SetRealTimer(0);

        //PutText(16, 40, "tracing", 0);

        //MemFill(screen, 0, (nScreenWidth>>1)*nScreenHeight);

		for (x = 0; x < nWindowWidth; x++)
		{
			// For each column, calc. the projected ray angle into world space
			fRayAngle = (fPlayerA - fFOV/2.0f) + (x/fWindowWidth) * fFOV;

			// Find distance to wall
			fStepSize = 0.5f;       // Increment size for ray casting, 										
			fDistanceToWall = 0.0f; // decrease to increase resolution

			bHitWall = 0;		// Set when ray hits wall block

			fEyeX = cosf(fRayAngle); // Unit vector for ray in player space
            fEyeY = sinf(fRayAngle);

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				nTestX = (short)(fPlayerX + fEyeX * fDistanceToWall);
				nTestY = (short)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || \
                    nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = 1; // Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
                    bHitWall = map[(nMapWidth * nTestY) + nTestX] == 0x0A;                    
				}
			}
		
            // Shader walls based on distance
            if      (fDistanceToWall <= fDepth / 5.0f) {nColor1 = LIGHT_GREEN;} // Very close	
	        else if (fDistanceToWall <  fDepth / 3.0f) {nColor1 = MEDIUM_GREEN;}
	        else if (fDistanceToWall <  fDepth)		   {nColor1 = DARK_GREEN;}
	        else									   {nColor1 = BLACK;}      // Too far away

            // Calculate distance to ceiling and floor
			nCeiling = ((unsigned short)(fHalfWindowHeight - nWindowHeight/fDistanceToWall)) + nWindowY;
			nFloor = nWindowHeight - nCeiling - 2 + nWindowY;
            nPosX = x + nWindowX;

            //fbline(screen, x, nWindowY, x, nCeiling, BLACK);
            //fbline(screen, x, nFloor+1, x, nWindowHeight-1, GRAY);
            //fbline(screen, x, nCeiling+1, x, nFloor, nColor1);
            line(nPosX, nWindowY, nPosX, nCeiling, BLACK);
            line(nPosX, nFloor+1, nPosX, nWindowHeight-1, GRAY);
            line(nPosX, nCeiling+1, nPosX, nFloor, nColor1);
            
		}

		// Display Map
		bitmap(map, nMapX, nMapY, nMapWidth, nMapHeight);
        pixel(((unsigned int)fPlayerX)+nMapX, ((unsigned int)fPlayerY)+nMapY, LIGHT_RED);
        //fbpixel(screen, (unsigned int)fPlayerX, (unsigned int)fPlayerY+1, LIGHT_RED);        

		// Display Frame
		//CopyRamToVram(screen, 0, (nScreenWidth>>1)*nScreenHeight);

        // Display Stats
        //nElapsedTime = RealTimer();
        /*        
        Locate(0, 0);
		Print("X=");     PrintDec(fPlayerX);
        Print(", Y=");   PrintDec(fPlayerY);
        Print(", A=");   PrintDec(fPlayerA*10);
        Print(", SPF="); PrintDec(nElapsedTime/50);
        */
        //char *teste = "teste";
        //Locate(0, 0);
        //PutText(0, 0, teste, 0);

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
			    fPlayerA -= fRotation;
		    else if (key_press == 28)	 // Handle CW Rotation
			    fPlayerA += fRotation;
		    else if (key_press == 30)	 // Handle Forwards movement & collision
		    {
			    fPlayerX += cosf(fPlayerA) * fSpeed;
			    fPlayerY += sinf(fPlayerA) * fSpeed;
			    if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == 0x0A)
			    {
				    fPlayerX -= cosf(fPlayerA) * fSpeed;
				    fPlayerY -= sinf(fPlayerA) * fSpeed;
                    bInvalidKey = 1;
			    }			
		    }
		    else if (key_press == 31)	 // Handle backwards movement & collision
		    {
			    fPlayerX -= cosf(fPlayerA) * fSpeed;
			    fPlayerY -= sinf(fPlayerA) * fSpeed;
			    if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == 0x0A)
			    {
				    fPlayerX += cosf(fPlayerA) * fSpeed;
				    fPlayerY += sinf(fPlayerA) * fSpeed;
                    bInvalidKey = 1;
			    }
		    }
            else
                bInvalidKey = 1;
        }
	}

}

