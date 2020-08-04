#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph1.h"

int main()
{
    unsigned int i,j;
    
    char c;
    char p[10];
    SetColors(15,5,7);
    Screen(2);
    SetColors(15,5,7);

    SC2Rect(5,10,50,20,2,255);
    SC2Rect(5,10,50,20,11,0);
   
   SC2Rect(25,2,51,10,12,0);
   SC2Rect(25,3,51,5,12,2);
   SC2Rect(25,4,51,6,12,3);
   SC2Rect(25,5,51,7,10,2);


  for (j=1; j<3;j++)
  {
    for ( i = 1; i < 50; ++i)
    {
      SC2Pset(i,j,6);
    }
}


    
     // now Drawing same lines
      for ( i = 0; i < 51; ++i)
      {
         SC2Line(25,10,i,0,10);
         SC2Line(25,10,i,21,9);
      }
      for ( i = 0; i < 21; ++i)
      {
         SC2Line(25,10,0,i,6);
         SC2Line(25,10,51,i,12);
      }

    
    
    WaitKey();
    Screen(0);


    Exit(0);
}
