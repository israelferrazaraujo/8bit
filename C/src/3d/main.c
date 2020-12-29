/*
OneLoneCoder.com - 3D Graphics Part #1 - Triangles & Projections
"Tredimensjonal Grafikk" - @Javidx9

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
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
3D Graphics is an interesting, visually pleasing suite of algorithms. This is the
first video in a series that will demonstrate the fundamentals required to 
build your own software based 3D graphics systems.

Video
~~~~~
https://youtu.be/ih20l3pJoeU

Author
~~~~~~
Twitter: @javidx9
Blog: http://www.onelonecoder.com
Discord: https://discord.gg/WhwHUMV


Last Updated: 14/07/2018
*/

#include "../../fusion-c/header/msx_fusion.h"
#include "../../fusion-c/header/vdp_graph1.h"
#include <math.h>

int ScreenHeight = 48;
int ScreenWidth = 64;
int screenMemSize = 32*48;
unsigned char *vbuffer;

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

void Pixel(unsigned int x, unsigned int y, unsigned char color) // x=coluna, y=linha
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

void Line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) // x=coluna, y=linha
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
        Pixel(h1, h2, color);
        h1+=dx;
        h2+=dy;
    } while (h--);
}

void Triangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,unsigned int x3, unsigned int y3, unsigned char color)
{
	Line(x1, y1, x2, y2, color);
	Line(x2, y2, x3, y3, color);
	Line(x3, y3, x1, y1, color);
}

typedef struct
{
	float x, y, z;
} vec3d;

typedef struct
{
	vec3d p[3];
} triangle;

typedef struct
{
	triangle tris[12][3];
} mesh;

typedef struct
{
	float m[4][4];
} mat4x4;


mesh meshCube;
mat4x4 matProj;

float fTheta;

void MultiplyMatrixVector(vec3d *i, vec3d *o, mat4x4 *m)
{
	(*o).x  = (*i).x * (*m).m[0][0] + (*i).y * (*m).m[1][0] + (*i).z * (*m).m[2][0] + (*m).m[3][0];
	(*o).y  = (*i).x * (*m).m[0][1] + (*i).y * (*m).m[1][1] + (*i).z * (*m).m[2][1] + (*m).m[3][1];
	(*o).z  = (*i).x * (*m).m[0][2] + (*i).y * (*m).m[1][2] + (*i).z * (*m).m[2][2] + (*m).m[3][2];
	float w = (*i).x * (*m).m[0][3] + (*i).y * (*m).m[1][3] + (*i).z * (*m).m[2][3] + (*m).m[3][3];

	if (w != 0.0f)
	{
		(*o).x /= w; (*o).y /= w; (*o).z /= w;
	}
}


void OnUserCreate()
{
	//meshCube.tris = {
    float x[][] = {
		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};
    for(int x=0; x<12; x++)
        for(int y=0; y<3; y++)
        {
            meshCube.tris[x][y] = x[x][y];
        }
    
	// Projection Matrix
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)ScreenHeight/ (float)ScreenWidth;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	MemFill(matProj.m, 0.0f, 4*4*sizeof(float));
	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

}

void OnUserUpdate()
{
	// Clear Screen
	MemFill(vbuffer, TRANSPARENT, screenMemSize);

	// Set up rotation matrices
	mat4x4 matRotZ, matRotX;
	fTheta += 0.2f;

	// Rotation Z
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	// Rotation X
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta * 0.5f);
	matRotX.m[1][2] = sinf(fTheta * 0.5f);
	matRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matRotX.m[2][2] = cosf(fTheta * 0.5f);
	matRotX.m[3][3] = 1;
	
	// Draw Triangles
	for (auto tri : meshCube.tris)
	{
		triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

		// Rotate in Z-Axis
		MultiplyMatrixVector(tri.&p[0], triRotatedZ.&p[0], matRotZ);
		MultiplyMatrixVector(tri.&p[1], triRotatedZ.&p[1], matRotZ);
		MultiplyMatrixVector(tri.&p[2], triRotatedZ.&p[2], matRotZ);

		// Rotate in X-Axis
		MultiplyMatrixVector(triRotatedZ.&p[0], triRotatedZX.&p[0], matRotX);
		MultiplyMatrixVector(triRotatedZ.&p[1], triRotatedZX.&p[1], matRotX);
		MultiplyMatrixVector(triRotatedZ.&p[2], triRotatedZX.&p[2], matRotX);

		// Offset into the screen
		triTranslated = triRotatedZX;
		triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
		triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
		triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

		// Project triangles from 3D --> 2D
		MultiplyMatrixVector(triTranslated.&p[0], triProjected.&p[0], matProj);
		MultiplyMatrixVector(triTranslated.&p[1], triProjected.&p[1], matProj);
		MultiplyMatrixVector(triTranslated.&p[2], triProjected.&p[2], matProj);

		// Scale into view
		triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
		triProjected.p[0].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[0].y *= 0.5f * (float)ScreenHeight;
		triProjected.p[1].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[1].y *= 0.5f * (float)ScreenHeight;
		triProjected.p[2].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[2].y *= 0.5f * (float)ScreenHeight;

		// Rasterize triangle
		Triangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			WHITE);

	}

	// Draw Screen
	CopyRamToVram(vbuffer, 0, screenMemSize);
}


int main()
{
	Screen(3);
	InitVideoBuffer(3);

	OnUserCreate();

	for (unsigned char x=0; x<10;x++)
	{
		OnUserUpdate();
	}

    return 0;
}

