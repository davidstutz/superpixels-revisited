#include <Elimination/Elimination.h>

#include <cstdio>

int main (int argc, char *argv[])
   {
   (void)argc;
   (void)argv;
	
   float a[5];
   float v[8];
	
   a[0] = 30.0f;
   a[1] = 0.0f;
   a[2] = -20.0f;
   a[3] = -20.0f;
   a[4] = -20.0f;

   Elimination< float > eliminator;
	
   eliminator.compute_coefficients( a, v );
	
   for ( int i = 0; i < 8; ++i )
      printf( "%f\n", v[i] );
	
   // Finished
   return 0;
   }
