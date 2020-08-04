#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph1.h"
#include <math.h>

const unsigned int nScreenWidth = 64;		  // Console Screen Size X (columns)
const unsigned int nScreenHeight = 48;	      // Console Screen Size Y (rows)
const unsigned int nMapWidth = 16;			  // World Dimensions
const unsigned int nMapHeight = 8;
const unsigned int VDPHz = 50;                // VDP freq. to calc. elapsed time

float fPlayerX = 6.7f;			              // Player Start Position
float fPlayerY = 5.09f;
float fPlayerA = 0.0f;			              // Player Start Rotation
const float fFOV = 3.14159f / 3.0f;           // Field of View
const float fDepth = 16.0f;          		  // Maximum rendering distance
const float fSpeed = 1.0f;          		  // Walking Speed
const float fRotation = 3.14159f / 4.0f;	  // Rotation Angle

const float fBound = 0.005f;

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

float fScreenWidth;
float fHalfScreenHeight;

unsigned int nElapsedTime;

float fRayAngle;
float fStepSize;
float fDistanceToWall;
float fEyeX;
float fEyeY;
char bHitWall;
char bBoundary;

int nTestX;
int nTestY;

float ds[4];
float dots[4];
unsigned int index;
unsigned int max;

float vy;
float vx;
float d;
float dot;

unsigned int nCeiling;
unsigned int nFloor;

int odd_x;
unsigned char offset;
unsigned char curr_color;
unsigned int vaddress;

unsigned char nShade;

float b;
char bInvalidKey;

unsigned int x;
unsigned int y;

void pset(unsigned int x, unsigned int y, unsigned char color)
{
    unsigned int vaddress = (y/8)*248 + y ; // importante dividir antes de 
    unsigned char offset;                   // multiplicar, para obter a 
                                            // parte inteira da divisão
                                            // 248 = 8 * (32-1)
    if (x % 2) // odd
    {
        vaddress += (x-1)*4;
        Vpoke(vaddress, nShade + Vpeek(vaddress)); // complementar a cor com bits
    }                                              // menos significativos.
    else       // even
    {
        vaddress += x*4; // o x está multiplicado por 2, por isso "*4" e não "*8"
        Vpoke(vaddress, 16*nShade); // 16=offset cor. 4 bits mais significativos
    }                                      
    
}

int main()
{
	// Create Screen Buffer
	//char *screen = MMalloc(nScreenWidth*nScreenHeight);

	// Create Map of world space # = wall block, . = space
	const char *map = "################"
	                  "#......#.......#"
	                  "#......#.......#"
	                  "#......#.......#"
	                  "#..............#"
	                  "#......#########"
	                  "#..............#"
	                  "################\0";

	Screen(3);
    //Width(nScreenWidth);
    //SetColors(15,1,1);

    fScreenWidth = nScreenWidth/1.0f;       // Optimization
    fHalfScreenHeight = nScreenHeight/2.0f; //

    char key_press;
	while (1)
	{
		//SetRealTimer(0);

        //Locate(0, 0);
        //for (unsigned int k = 0; k < 2*nScreenWidth; k++)
        //    PrintChar(' ');
        //Locate(0, 0);
        //Print("Ray tracing");
        //Locate(0, 1);

		for (x = 0; x < nScreenWidth; x++)
		{
            //Print("."); // Progress indicator

			// For each column, calc. the projected ray angle into world space
			fRayAngle = (fPlayerA - fFOV/2.0f) + (x/fScreenWidth) * fFOV;

			// Find distance to wall
			fStepSize = 0.5f;       // Increment size for ray casting, 										
			fDistanceToWall = 0.0f; // decrease to increase resolution

			bHitWall = 0;		// Set when ray hits wall block
			bBoundary = 0;		// Set when ray hits boundary between two blocks

			fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			fEyeY = cosf(fRayAngle);

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapHeight || \
                    nTestY < 0 || nTestY >= nMapWidth)
				{
					bHitWall = 1; // Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[nTestX * nMapWidth + nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = 1;

					}
                    
				}
			}
		
			// Calculate distance to ceiling and floor
			nCeiling = (unsigned int)(fHalfScreenHeight - nScreenHeight/fDistanceToWall);
			nFloor = nScreenHeight - nCeiling - 1;

            // Shader walls based on distance
            if (bBoundary)                             nShade = BLACK; // Corner			
            else if (fDistanceToWall <= fDepth / 5.0f) nShade = DARK_GREEN; // Very close	
	        else if (fDistanceToWall < fDepth / 4.0f)  nShade = MEDIUM_GREEN;
	        else if (fDistanceToWall < fDepth / 2.5f)  nShade = LIGHT_GREEN;
	        else if (fDistanceToWall < fDepth / 1.5f)  nShade = LIGHT_YELLOW;
	        else if (fDistanceToWall < fDepth)		   nShade = DARK_YELLOW;
	        else									   nShade = BLACK; // Too far away

            odd_x = x % 2;
            curr_color = 0;                        
            if (odd_x)
            {
                offset = 1;
                vaddress = (x-1)*4;
            }
            else
            {
                offset = 16;
                vaddress = x*4;
            }

			for (y = 0; y < nScreenHeight; y++)
			{
                if (y % 8 == 0 && y > 0)
                    vaddress += 31*8;

                if (odd_x)
                    curr_color = Vpeek(vaddress);

                // Each Row
				if(y <= nCeiling)
                    Vpoke(vaddress, BLACK*offset + curr_color);
				else if(y <= nFloor)
                    Vpoke(vaddress, nShade*offset + curr_color);
				else // Floor
				{				
					// Shade floor based on distance
                    /*					
                    b = y/fHalfScreenHeight;
					if (b > 1.75f)		nShade = DARK_BLUE;
					else if (b > 1.5f)	nShade = LIGHT_BLUE;
					else if (b > 1.25f)	nShade = CYAN;
					else if (b > 1.0f)	nShade = WHITE;
					else				nShade = BLACK;
                    Vpoke(vaddress, nShade*offset + curr_color);
                    */
                    Vpoke(vaddress, GRAY*offset + curr_color);					
				}

                vaddress++;
			}
		}

		// Display Map
        /*
		for (int ny = 0; ny < nMapWidth; ny++)
			for (int nx = 0; nx < nMapHeight; nx++)
            {
                if (map[nx * nMapWidth + ny]=='.')	nShade = BLACK;
				else                                nShade = DARK_YELLOW;
                SC2Pset(nx*4, ny, nShade);
                //PutText(nx, ny, &map[nx * nMapWidth + ny], 0);
				//screen[(nx+1)*nScreenWidth + ny] = map[nx * nMapWidth + ny];
            }
		//screen[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = 'P';
        //PutText((int)fPlayerX+1, (int)fPlayerY, 'P', 0);
        SC2Pset(((int)fPlayerX+1)*4, (int)fPlayerY, 8);
        */

		// Display Frame
		//CopyRamToVram(screen, 0, nScreenWidth * nScreenHeight);

        // Display Stats
        //nElapsedTime = RealTimer();
        /*        
        Locate(0, 0);
		Print("X=");     PrintDec(fPlayerX);
        Print(", Y=");   PrintDec(fPlayerY);
        Print(", A=");   PrintDec(fPlayerA*10);
        Print(", SPF="); PrintDec(nElapsedTime/50);
        */

        bInvalidKey = 1;
        while (bInvalidKey)
        {
            bInvalidKey = 0;

            Beep();
            KillKeyBuffer();
            key_press = WaitKey();

		    if (key_press == 29)         // Handle CCW Rotation
			    fPlayerA -= fRotation;
		    else if (key_press == 28)	 // Handle CW Rotation
			    fPlayerA += fRotation;
		    else if (key_press == 30)	 // Handle Forwards movement & collision
		    {
			    fPlayerX += sinf(fPlayerA) * fSpeed;
			    fPlayerY += cosf(fPlayerA) * fSpeed;
			    if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			    {
				    fPlayerX -= sinf(fPlayerA) * fSpeed;
				    fPlayerY -= cosf(fPlayerA) * fSpeed;
			    }			
		    }
		    else if (key_press == 31)	 // Handle backwards movement & collision
		    {
			    fPlayerX -= sinf(fPlayerA) * fSpeed;
			    fPlayerY -= cosf(fPlayerA) * fSpeed;
			    if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			    {
				    fPlayerX += sinf(fPlayerA) * fSpeed;
				    fPlayerY += cosf(fPlayerA) * fSpeed;
			    }
		    }
            else if (key_press == 27)    // Exit program
            {
                Screen(0);
                Cls();
                return 0;
            }
            else
                bInvalidKey = 1;
        }
	}

}

