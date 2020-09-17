#include <stdio.h>

int x,y,k,r,i,t;
main()
{
    for(y=-10; y<11;y++)
    {
        for(x=-22; x<10; x++)
        {
            r=0; i=0; k=0;
            do
            {
                t = r*r/10 - i*i/10 + x;
                i = 2*r*i/10+y;
                r = t; k++;
            } while ((r*r/10 + i*i/10 <= 40) && (k<200));
            (k==200 ? putchar('*') : putchar(' '));
        }
        putchar('\r'); putchar('\n');
    }
}


