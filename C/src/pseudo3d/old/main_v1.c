#include "fusion-c/header/msx_fusion.h"
#include <math.h>

const unsigned int nScreenWidth = 40;		  // Console Screen Size X (columns)
const unsigned int nScreenHeight = 24;	      // Console Screen Size Y (rows)
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

unsigned int nSumScreenWidth;
char nShade;

float b;
char bInvalidKey;

int main()
{
	// Create Screen Buffer
	char *screen = MMalloc(nScreenWidth*nScreenHeight);

	// Create Map of world space # = wall block, . = space
	const char *map = "################"
	                  "#......#.......#"
	                  "#......#.......#"
	                  "#......#.......#"
	                  "#..............#"
	                  "#......#########"
	                  "#..............#"
	                  "################\0";

	Screen(0);
    Width(nScreenWidth);
    SetColors(15,1,1);

    fScreenWidth = nScreenWidth/1.0f;       // Optimization
    fHalfScreenHeight = nScreenHeight/2.0f; //

    char key_press;
	while (1)
	{
		SetRealTimer(0);

        Locate(0, 0);
        for (unsigned int k = 0; k < 2*nScreenWidth; k++)
            PrintChar(' ');
        Locate(0, 0);
        Print("Ray tracing");
        Locate(0, 1);

		for (unsigned int x = 0; x < nScreenWidth; x++)
		{
            Print("."); // Progress indicator

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

						// To highlight tile boundaries, cast a ray from each corner
						// of the tile, to the player. The more coincident this ray
						// is to the rendering ray, the closer we are to a tile 
						// boundary, which we'll shade to add detail to the walls
                        
						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
                        /*
                        max = 0;
                        for (unsigned int tx = 0; tx < 2; tx++)
							for (unsigned int ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								vy = nTestY + ty - fPlayerY;
								vx = nTestX + tx - fPlayerX;
								d = sqrtf(vx*vx + vy*vy); 
        						dot = (fEyeX * vx / d) + (fEyeY * vy / d);

                                index = tx*2+ty;
                                ds[index] = d;
                                dots[index] = dot;                                
                                if(d > ds[max])
                                    max = index;
							}

                        for (unsigned int j = 0; j < 4; j++)
                            if(j != max && acosf(dots[j]) < fBound)
                            {
                                bBoundary = 1;
                                j = 4;
                            }
                        */
					}
                    
				}
			}
		
			// Calculate distance to ceiling and floor
			nCeiling = (unsigned int)(fHalfScreenHeight - nScreenHeight/fDistanceToWall);
			nFloor = nScreenHeight - nCeiling;

            // Shader walls based on distance
            if (bBoundary)                             nShade = 124; // Corner			
            else if (fDistanceToWall <= fDepth / 5.0f) nShade = 219; // Very close	
	        else if (fDistanceToWall < fDepth / 3.0f)  nShade = 199;
	        else if (fDistanceToWall < fDepth / 2.0f)  nShade = 215;
	        else if (fDistanceToWall < fDepth)		   nShade = 203;
	        else									   nShade = ' '; // Too far away

            nSumScreenWidth = 0;
			for (unsigned int y = 0; y < nScreenHeight; y++)
			{
                // Each Row
				if(y <= nCeiling)
					screen[nSumScreenWidth + x] = ' ';
				else if(y <= nFloor)
    				screen[nSumScreenWidth + x] = nShade;
				else // Floor
				{				
					// Shade floor based on distance
					b = y/fHalfScreenHeight;
					if (b > 1.75f)		nShade = '#';
					else if (b > 1.5f)	nShade = 'x';
					else if (b > 1.25f)	nShade = '.';
					else if (b > 1.0f)	nShade = '-';
					else				nShade = ' ';
					screen[nSumScreenWidth + x] = nShade;
				}
                nSumScreenWidth += nScreenWidth;
			}
		}

		// Display Map
		for (unsigned int ny = 0; ny < nMapWidth; ny++)
			for (unsigned int nx = 0; nx < nMapHeight; nx++)
				screen[(nx+1)*nScreenWidth + ny] = map[nx * nMapWidth + ny];

		screen[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = 'P';

		// Display Frame
		CopyRamToVram(screen, 0, nScreenWidth * nScreenHeight);

        // Display Stats
        nElapsedTime = RealTimer();
        Locate(0, 0);
		Print("X=");     PrintDec(fPlayerX);
        Print(", Y=");   PrintDec(fPlayerY);
        Print(", A=");   PrintDec(fPlayerA*10);
        Print(", SPF="); PrintDec(nElapsedTime/50);

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
                Cls();
                return 0;
            }
            else
                bInvalidKey = 1;
        }
	}

}

