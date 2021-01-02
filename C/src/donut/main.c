#include "../../fusion-c/header/msx_fusion.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../lib/fixed/fixed.h"
#include "../lib/trig/trig.h"

#define cos cosf
#define sin sinf

#define itype  int
#define xtype  fixed
#define i2x    int2fixed
#define f2x    float2fixed
#define x2i    fixed2int
#define x2f    fixed2float
#define x_mul  fixed_mul
#define lx_mul lfixed_mul
#define x_div  fixed_div
#define lx_div lfixed_div

xtype c, d, e, f, g, h, D, l, m, n, t;
xtype _1, _2, _5, _8, _15, _30;
xtype fg, fe, dg, de, dn, eh, gh, mh, nh;

int i, j, k, i_max, j_max, w=0;
int x, y, o, N;

float A, B, A_inc, B_inc;
float fi, fj, j_lim, j_inc, i_lim, i_inc;
float fElapsedTime=0.0f, fVDPHz = 60.0f;

xtype lt_cos_i[322], lt_cos_j[91];
xtype lt_sin_i[322], lt_sin_j[91];
xtype z[1760];

char b[1760];

void PrintFloat(float value, char round) // max round == 4
{
    char buffer[8];
    Itoa((int)value, buffer, 10);
    StrConcat(buffer, ".");
    
    char buffer2[5]; // max of (round+1)
    Itoa((int)((value-(int)value)*powf(10.0f, round)), buffer2, 10);
    if (StrLen(buffer2) < round) 
    {
        char buffer3[4]; // max of (round-StrLen(buffer2)+1)
        memset(buffer3,'0', round-StrLen(buffer2));
        buffer3[round-StrLen(buffer2)] = '\0';
        StrConcat(buffer, buffer3);
    }
    StrConcat(buffer, buffer2);

    Print(buffer);
}

int main() {
    Screen(0);
    Width(80);
    Locate(0,0);
    SetColors(15,1,1);

    _1 = i2x(1);   _2 = i2x(2);
    _5 = i2x(5);   _8 = i2x(8);    
    _15 = i2x(15); _30 = i2x(30);

    j_lim = 6.28f; // f2x(6.28f);
    j_inc = 0.21f; // f2x(0.07f);
    i_lim = 6.28f; // f2x(6.28f);
    i_inc = 0.06f; // f2x(0.02f);

    A_inc = 0.04f; // f2x(0.04f);
    B_inc = 0.02f; // f2x(0.02f);
    A = 0; B = 0;
    

    Print("Loading...");
    i_max=0; j_max=0;
    for(fj=0; fj < j_lim; fj += j_inc) {
        lt_cos_j[j_max] = f2x(cos(fj));
        lt_sin_j[j_max] = f2x(sin(fj));
        j_max++;
    }
    for(fi=0; fi < i_lim; fi += i_inc) {
        lt_cos_i[i_max] = f2x(cos(fi));
        lt_sin_i[i_max] = f2x(sin(fi));
        i_max++;
    }
    Cls();
    //printf("i_max=%d", i_max);
    //printf("j_max=%d", j_max);

    while(1) {
        SetRealTimer(0);

        memset(b,32, sizeof b);
        memset(z,0,sizeof z);

        g = f2x(cos(A)); e = f2x(sin(A));
        m = f2x(cos(B)); n = f2x(sin(B));

        for(j=0; j < j_max; j++) {
            Locate(0, 22);
            putchar('\x01');
            if (w>1) w=0;            
            putchar("\x56\x57"[w++]);

            d = lt_cos_j[j]; f = lt_sin_j[j];

            h = d + _2;

            fg = x_mul(f, g); fe = x_mul(f, e);
            dg = x_mul(d, g); de = x_mul(d, e);
            dn = x_mul(d, n); eh = x_mul(e, h);
            gh = x_mul(g, h); mh = x_mul(m, h);
            nh = x_mul(n, h);

            for(i=0; i < i_max; i++) {
                c = lt_sin_i[i]; l = lt_cos_i[i];

                D = lx_div(_1, x_mul(c, eh) + fg + _5);
                t = x_mul(c, gh) - fe;
                x = 40 + x2i( x_mul(x_mul(D, x_mul(mh, l) - x_mul(t, n)), _30) );
                y = 12 + x2i( x_mul(x_mul(D, x_mul(nh, l) + x_mul(t, m)), _15) );
                o = x + 80 * y;
                if(22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    N = x2i( x_mul( x_mul(fe - x_mul(c, dg), m) - x_mul(c, de) - fg - x_mul(l, dn), _8 ));
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }
        Locate(0,0);
        for(k = 0; k < 1760; k++) {
            putchar(b[k]);
        }
        A += A_inc*5;
        B += B_inc*5;
            
        fElapsedTime = RealTimer()/fVDPHz;
        Locate(2, 22);
        PrintFloat(fElapsedTime, 2);
    }
    return 0;
}
