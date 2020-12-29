#include "../../fusion-c/header/msx_fusion.h"
#include <stdio.h>
#include <string.h>
#include "../lib/fixed/fixed.h"
#include "../lib/trig/trig.h"
//#include <math.h>

#define cos xcos
#define sin xsin
#define i2x int2lfixed
#define f2x float2lfixed
#define x2i lfixed2int
#define x2f lfixed2float
#define x_mul lfixed_mul
#define x_div lfixed_div

lfixed A, B, c, d, e, f, g, h, D, l, m, n, t, i, j, j_lim, j_inc, i_lim, i_inc, A_inc, B_inc;
lfixed _1, _2, _5, _8, _15, _30;
int k, x, y, o, N;

lfixed z[1760];
char b[1760];

int main() {
    Screen(0);
    Width(80);
    Locate(0,0);
    SetColors(15,1,1);

    _1 = i2x(1);
    _2 = i2x(2);
    _5 = i2x(5);
    _8 = i2x(8);    
    _15 = i2x(15);
    _30 = i2x(30);

    j_lim = f2x(6.28f);
    j_inc = f2x(0.07f);
    i_lim = f2x(6.28f);
    i_inc = f2x(0.02f);

    A_inc = f2x(0.0004f);
    B_inc = f2x(0.0002f);
    A = 0; B = 0;
    
    while(1) {
        memset(b,32, sizeof b);
        memset(z,0,sizeof z);
        g = cos(A);
        e = sin(A);
        m = cos(B);
        n = sin(B);

        for(j=0; j < j_lim; j += j_inc) {
            d = cos(j);
            f = sin(j);
            for(i=0; i < i_lim; i += i_inc) {
                c = sin(i);
                l = cos(i);

                h = d + _2;
                D = x_div(_1, (x_mul(x_mul(c, h), e) + x_mul(f, g) + _5));
                //D = f2x(1.0f / x2f((x_mul(x_mul(c, h), e) + x_mul(f, g) + _5)));
                t = x_mul(x_mul(c, h), g) - x_mul(f, e);
                x = 40 + x2i( x_mul(_30, x_mul(D, (x_mul(l, x_mul(h, m)) - x_mul(t, n))) ));
                y = 12 + x2i( x_mul(_15, x_mul(D, (x_mul(l, x_mul(h, n)) + x_mul(t, m))) ));
                o = x + 80 * y;
                N = x2i( x_mul(_8, (x_mul((x_mul(f, e) - x_mul(c, x_mul(d, g))), m) - x_mul(c, x_mul(d, e)) - x_mul(f, g) - x_mul(l, x_mul(d, n)))));
                if(22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }
        Locate(0,0);
        //for(k = 0; k < 1761; k++) {
        for(k = 0; k < 1760; k++) {
            //putchar(k % 80 ? b[k] : 13);
            putchar(b[k]);
            A += A_inc;
            B += B_inc;
        }
        //usleep(30000);
    }
    return 0;
}
