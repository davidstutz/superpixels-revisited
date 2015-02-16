////////////////////////////////////////////////////////
////////// First version of faster iterations///////////
////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <image.h>
#include <misc.h>
#include <pnmfile.h>
#include <matrix.h>
#include <misc.h>
#include <draw.h>
#include <errno.h>
#include "graph.h"
#include "energy.h"

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using namespace vlib;

#define NUM_COLORS 255
#define MULTIPLIER_VAR 1.5
#define sq(x) ((x)*(x))

typedef int Value;
typedef int TotalValue;
typedef int Var;


///////////////////////////////////////////////////////////////////////////////////
// terminates if error has occured
void check_error( int boolE, const char *error_message)
{ 
   if  (boolE) 
   {
      printf("\n %s \n", error_message);
      exit(1);
   }
}


///////////////////////////////////////////////////////////////////////////////////

void PlaceSeeds(image<uchar> *I,int width,int height,int num_pixels,
				vector<int> &Seeds,int *numSeeds, int PATCH_SIZE)
{

	int bSize = PATCH_SIZE/2;
	for  ( int y = bSize; y < height; y = y + bSize){
		for ( int x = bSize; x < width; x = x + bSize){
			Seeds[*numSeeds] = x+y*width;
			(*numSeeds)++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////

void MoveSeedsFromEdges(image<uchar> *I,int width,int height,int num_pixels,
				        vector<int> &Seeds, int numSeeds, int PATCH_SIZE)
{

	for ( int i = 0; i < numSeeds; i++ ){
		int nextY  = Seeds[i]/width;
		int nextX = Seeds[i]-nextY*width;
		

		int delta = PATCH_SIZE/4-1;
		int bestX = nextX;
		int bestY = nextY;

		int startX = nextX-delta;
		int endX = nextX+delta;
		int startY = nextY-delta;
		int endY = nextY+delta;
		if ( startX < 1 ) startX = 1;
		if ( endX > width-2 ) endX = width-2;
		if ( startY < 1 ) startY = 1;
		if (endY > height-2) endY = height-2;

		int bestScore = 255*sq(2*delta+1);
		//int bestScore = 0;

		for ( int y = startY; y <= endY; y++ )
			for ( int x = startX; x <= endX; x++ ){
				int currScore = abs(imRef(I,x,y)-imRef(I,x-1,y-1))+
					            abs(imRef(I,x,y)-imRef(I,x-1,y))+
								abs(imRef(I,x,y)-imRef(I,x-1,y+1))+
								abs(imRef(I,x,y)-imRef(I,x,y-1))+
								abs(imRef(I,x,y)-imRef(I,x,y+1))+
								abs(imRef(I,x,y)-imRef(I,x+1,y-1))+
								abs(imRef(I,x,y)-imRef(I,x+1,y))+
								abs(imRef(I,x,y)-imRef(I,x+1,y+1));
				if (currScore < bestScore ){
					bestScore = currScore;
					bestX = x;
					bestY = y;
				}
				
			}
		Seeds[i] = bestX+bestY*width;
	}
}


///////////////////////////////////////////////////////////////////////////////////


Value computeEnergy(vector<int> &labeling,int width,int height,int num_pixels,
					vector<Value> &horizWeights,vector<Value> &vertWeights,
					vector<Value> &diag1Weights,vector<Value> &diag2Weights,
					vector<int> &Seeds, image<uchar> *I,int TYPE)
{
	TotalValue engSmooth = 0,engData = 0;
	float SQRT_2= 1/sqrt(2.0);


	if ( TYPE == 1 )
	{
		for ( int y = 0; y < height; y++ )
			for ( int x = 0; x < width; x++){
				int label = labeling[x+y*width];
				int seedY = Seeds[label]/width;
				int seedX = Seeds[label] - seedY*width;
				int color = imRef(I,seedX,seedY);

				int diff = abs(imRef(I,x,y)-color);
				int maxD = 15;
				if ( diff > maxD ) diff = maxD;
				engData = engData+diff;
			}
	}

	for ( int y = 1; y < height; y++ ){
		for (int  x = 0; x < width; x++ )
			if ( labeling[x+y*width] != labeling[x+(y-1)*width] )
				engSmooth = engSmooth + vertWeights[x+(y-1)*width];
		}

	for (int  y = 0; y < height; y++ ){
		for (int  x = 1; x < width; x++ )
			if ( labeling[x+y*width] != labeling[(x-1)+y*width] ){
				engSmooth = engSmooth + horizWeights[(x-1)+y*width];
			}
		}

	for ( int y = 1; y < height; y++ ){
		for (int  x = 1; x < width; x++ )
			if ( labeling[x+y*width] != labeling[x-1+(y-1)*width] )
				engSmooth = engSmooth + SQRT_2*diag1Weights[x-1+(y-1)*width];
		}

	for ( int y = 1; y < height; y++ ){
		for (int  x = 0; x < width-1; x++ )
			if ( labeling[x+y*width] != labeling[x+1+(y-1)*width] )
				engSmooth = engSmooth + SQRT_2*diag2Weights[x+1+(y-1)*width];
	}


	//printf("\nDeng %d ",engSmooth);
	return(engSmooth+engData);
}


///////////////////////////////////////////////////////////////////////////////////

void getBounds(int width,int height,vector<int> &Seeds,int *seedX,int *seedY,
			   int *startX,int *startY,int *endX,int *endY,int label, int PATCH_SIZE)
{
	*seedY = Seeds[label]/width;
	*seedX = Seeds[label]-(*seedY)*width;

	*startX = *seedX-PATCH_SIZE;
	*endX   = *seedX+PATCH_SIZE;

	*startY = *seedY-PATCH_SIZE;
	*endY   = *seedY+PATCH_SIZE;

	if ( *startX < 0 )     *startX  = 0;
	if ( *startY < 0 )     *startY  = 0;
	if ( *endX >= width )  *endX    = width-1;
	if ( *endY >= height ) *endY    = height-1;

}

///////////////////////////////////////////////////////////////////////////////////
// variable 0 corresponds to the old label, variable 1 to the new label, which
// is the first input parameter into the procedure
// does alpha-expansion in a block of size BLOCK_SIZE by BLOCK_SIZE
// the border stays fixed to the old label values

void expandOnLabel(int label,int width,int height,int num_pixels,
				   vector<int> &Seeds,int numSeeds, vector<int> &labeling,
				   vector<Value> &horizWeights,vector<Value> &vertWeights,Value lambda,
				   vector<Value> &diag1Weights,vector<Value> &diag2Weights,int PATCH_SIZE,
				   vector<int> &changeMask, vector<int> &changeMaskNew,image<uchar> *I,
				   int TYPE,float variance)
{
	int seedX,seedY,startX,startY,endX,endY,numVars,blockWidth;
	getBounds(width,height,Seeds,&seedX,&seedY,&startX,&startY,&endX,&endY,label,PATCH_SIZE);

	int somethingChanged = 0;

	for ( int y = startY; y <= endY; y++ )
		for ( int x = startX; x <= endX; x++ )
			if ( changeMask[x+y*width] == 1 )
			{
				somethingChanged = 1;
				break;
			}

	
	if ( somethingChanged == 0)
		return;
		
	blockWidth = endX-startX+1;
	numVars   = (endY-startY+1)*blockWidth;
	

	vector<Var> variables(numVars);
	Energy<int,int,int> *e = new Energy<int,int,int>(numVars,numVars*3);

	for ( int i = 0; i < numVars; i++ )
		variables[i] = e->add_variable();

	Value LARGE_WEIGHT = lambda*NUM_COLORS*8;

	// First fix the border to old labels, except the edges of the image
	for ( int y = startY; y <= endY; y++ ){
		if ( startX != 0 )
			e->add_term1(variables[(y-startY)*blockWidth],0,LARGE_WEIGHT);
		else if ( y == startY || y == endY)
			e->add_term1(variables[(y-startY)*blockWidth],0,LARGE_WEIGHT);

		if( endX != width -1 )
			e->add_term1(variables[(endX-startX)+(y-startY)*blockWidth],0,LARGE_WEIGHT);
		else if ( y == startY || y == endY)
			e->add_term1(variables[(endX-startX)+(y-startY)*blockWidth],0,LARGE_WEIGHT);
	}

	for ( int x = startX+1; x < endX; x++){
		if ( startY != 0 )
			e->add_term1(variables[(x-startX)],0,LARGE_WEIGHT);
		if ( endY != height - 1)
			e->add_term1(variables[(x-startX)+(endY-startY)*blockWidth],0,LARGE_WEIGHT);
	}

	// add links to center of the patch for color constant superpixels
	if ( TYPE == 1 )
	{
		int color = imRef(I,seedX,seedY);

		for ( int y = startY+1; y < endY; y++ )
			for ( int x = startX+1; x < endX; x++){
				Value E00=0,E01=0,E10=LARGE_WEIGHT,E11=0;

				if (seedX != x && seedY != y)
					e->add_term2(variables[(x-startX)+(y-startY)*blockWidth],
					         variables[(seedX-startX)+(seedY-startY)*blockWidth],E00,E01,E10,E11);

				int diff = abs(imRef(I,x,y)-color);
				int maxD = (int) variance*MULTIPLIER_VAR;
				if ( diff > maxD ) diff = maxD;

				int oldLabel = labeling[x+y*width];
				int oldY = Seeds[oldLabel]/width;
				int oldX = Seeds[oldLabel]-oldY*width;
				int oldColor = imRef(I,oldX,oldY);
				int oldDiff = abs(imRef(I,x,y)-oldColor);
				if ( oldDiff > maxD ) oldDiff = maxD;

				if ( oldDiff > diff) 
					 e->add_term1(variables[(x-startX)+(y-startY)*blockWidth],oldDiff-diff,0);
				else e->add_term1(variables[(x-startX)+(y-startY)*blockWidth],0,diff-oldDiff);
			}
	}


	// First set up horizontal links 
	for ( int y = startY; y <= endY; y++ )
		for ( int x = startX+1; x <=endX; x++){
			int oldLabelPix       = labeling[x+y*width];
			int oldLabelNeighbPix = labeling[x-1+y*width];
			Value E00,E01,E10,E11=0;

			if ( oldLabelPix != oldLabelNeighbPix ) 
				E00 = horizWeights[x-1+y*width];
			else E00 = 0;
			if ( oldLabelNeighbPix != label ) 
				E01 = horizWeights[x-1+y*width];
			else E01 = 0;
			if ( label != oldLabelPix )
				E10 = horizWeights[x-1+y*width];
			else E10 = 0;
			
			e->add_term2(variables[(x-startX)-1+(y-startY)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
		}

	// Next set up vertical links
	for ( int y = startY+1; y <= endY; y++ )
		for ( int x = startX; x <=endX; x++){
			int oldLabelPix       = labeling[x+y*width];
			int oldLabelNeighbPix = labeling[x+(y-1)*width];
			Value E00,E01,E10,E11=0;

			if ( oldLabelPix != oldLabelNeighbPix ) 
				E00 = vertWeights[x+(y-1)*width];
			else E00 = 0;
			if ( oldLabelNeighbPix != label ) 
				E01 = vertWeights[x+(y-1)*width];
			else E01 = 0;
			if ( label != oldLabelPix )
				E10 = vertWeights[x+(y-1)*width];
			else E10 = 0;

			
			e->add_term2(variables[(x-startX)+(y-startY-1)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
		}

	// Next set up diagonal links 
	float SQRT_2= 1/sqrt(2.0);
	for ( int y = startY+1; y <= endY; y++ )
		for ( int x = startX+1; x <=endX; x++){
			int oldLabelPix       = labeling[x+y*width];
			int oldLabelNeighbPix = labeling[x-1+(y-1)*width];
			Value E00,E01,E10,E11=0;
			
			if ( oldLabelPix != oldLabelNeighbPix ) 
				E00 = SQRT_2*diag1Weights[x-1+(y-1)*width];
			else E00 = 0;
			if ( oldLabelNeighbPix != label ) 
				E01 = SQRT_2*diag1Weights[x-1+(y-1)*width];
			else E01 = 0;
			if ( label != oldLabelPix )
				E10 = SQRT_2*diag1Weights[x-1+(y-1)*width];
			else E10 = 0;
			
			e->add_term2(variables[(x-startX)-1+(y-startY-1)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
		}
	
	// More diagonal links
	for ( int y = startY+1; y <= endY; y++ )
		for ( int x = startX; x <=endX-1; x++){
			int oldLabelPix       = labeling[x+y*width];
			int oldLabelNeighbPix = labeling[(x+1)+(y-1)*width];
			Value E00,E01,E10,E11=0;

			if ( oldLabelPix != oldLabelNeighbPix ) 
				E00 = SQRT_2*diag2Weights[(x+1)+(y-1)*width];
			else E00 = 0;
			if ( oldLabelNeighbPix != label ) 
				E01 = SQRT_2*diag2Weights[(x+1)+(y-1)*width];
			else E01 = 0;
			if ( label != oldLabelPix )
				E10 = SQRT_2*diag2Weights[(x+1)+(y-1)*width];
			else E10 = 0;

			e->add_term2(variables[(x-startX+1)+(y-startY-1)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
		}


	e->minimize();

	for ( int y = startY; y <= endY; y++ )
		for ( int x = startX; x <= endX; x++){
			if ( e->get_var(variables[(x-startX)+(y-startY)*blockWidth]) != 0 )
			{
				if ( labeling[x+y*width] != label ){
					labeling[x+y*width] = label; 
					changeMaskNew[x+y*width] = 1;
					changeMask[x+y*width] = 1;
				}
			}
		}


	delete e;
}

///////////////////////////////////////////////////////////////////////////////////

void initializeLabeling(vector<int> &labeling,int width,int height,
						vector<int> &Seeds,int numSeeds, int PATCH_SIZE)
{
	for ( int i = 0; i < numSeeds; i++ ){

		int seedX,seedY,startX,startY,endX,endY;

		seedY = Seeds[i]/width;
		seedX = Seeds[i]-seedY*width;

		startX = seedX-PATCH_SIZE/2-1;
		endX   = seedX+PATCH_SIZE/2+1;

		startY = seedY-PATCH_SIZE/2-1;
		endY   = seedY+PATCH_SIZE/2+1;

		if ( startX < 0 )     startX  = 0;
		if ( startY < 0 )     startY  = 0;
		if ( endX >= width )  endX    = width-1;
		if ( endY >= height ) endY    = height-1;

		for ( int y = startY; y <= endY; y++ )
			for ( int x = startX; x <= endX; x++ )
				labeling[x+y*width] = i;
	}

}

///////////////////////////////////////////////////////////////////////////////////

float  computeImageVariance(image<uchar> *I,int width,int height)
{
	float v = (float) 0.0;
	int total = 0;

	for ( int y = 1; y < height; y++ )
		for ( int x = 1; x < width ; x++ ){
			v = v + abs(imRef(I,x,y)-imRef(I,x-1,y))+abs(imRef(I,x,y)-imRef(I,x,y-1));
			total = total + 2;
	}
    
	return( v/total);
}



///////////////////////////////////////////////////////////////////////////////////

void loadEdges(vector<Value> &weights,int num_pixels,int width,int height,
			   Value lambda, char *name)
{

	image<uchar> *edges = loadPGM(name);

	for ( int y = 0; y < height; y++ )
		for ( int x = 0; x < width; x++){
			weights[x+y*width] = (Value) lambda*imRef(edges,x,y);
		}
}

///////////////////////////////////////////////////////////////////////////////////

void computeWeights(vector<Value> &weights,int num_pixels,int width,
					int height,Value lambda, float variance,
					int incrX,int incrY, image<uchar> *I,
					int TYPE)
{

	float sigma = 2.0f;

	int startX=0, startY=0;

	if ( incrX != 0 ) startX  = abs(incrX);

	if (incrY != 0 ) startY = abs(incrY);

	Value smallPenalty;
	if ( TYPE == 1 )
		smallPenalty = (MULTIPLIER_VAR*variance)/8+1;
	else smallPenalty  = 1;


	for ( int y = startY; y < height; y++ )
		for ( int x = startX; x < width; x++){
			int difference = sq((imRef(I,x,y)-imRef(I,x+incrX,y+incrY)));
				weights[(x+incrX)+(y+incrY)*width] = (Value) (lambda*exp((-difference/(sigma*sq(variance))))+smallPenalty);
		}

	
	//image<uchar> *e = new image<uchar>(width,height);
		
	//for ( int y = 0; y < height; y++ )
	//	for (int  x = 0; x < width; x++ ){
	//		imRef(e,x,y) = weights[x+y*width];
	//	}

	//savePGM(e,name);  

}

///////////////////////////////////////////////////////////////////////////////////

int  saveSegmentationColor(vector<int> &labeling,int width,int height,int num_pixels,
					       image<uchar> *I, int numSeeds, char *name)
{

	image<rgb> *out = new image<rgb>(width,height);


	vector<rgb> colorLookup(numSeeds);
	vector<int> counts(numSeeds,0);

	for ( int i = 0; i < numSeeds; i++ ){
		colorLookup[i].r = rand()%NUM_COLORS;
		colorLookup[i].g = rand()%NUM_COLORS;
		colorLookup[i].b = rand()%NUM_COLORS;
	}

	for ( int y = 0; y < height; y++ )
		for (int  x = 0; x < width; x++ ){
			imRef(out,x,y) = colorLookup[labeling[x+y*width]];
			counts[labeling[x+y*width]]++;
		}

	savePPM(out,name);

/*    
	image<uchar> *e = new image<uchar>(width,height);
	e->init(0);
	for ( int y = 1; y < height; y++ )
		for (int  x = 1; x < width; x++ ){
			if ( labeling[x+y*width] != labeling[x-1+y*width])
				imRef(e,x-1,y) = 255;
			if ( labeling[x+y*width] != labeling[x+(y-1)*width])
				imRef(e,x,y-1) = 255;
		}

	savePGM(e,"edges.pgm");  
*/
	int num_superpixels = 0;

	for ( int i = 0; i < numSeeds; i++ )
		if (counts[i] > 0 )
			num_superpixels++;
	return(num_superpixels);

}

///////////////////////////////////////////////////////////////////////////////////

void purturbSeeds(vector<int> &order,int numSeeds)
{
	for (int i = 0; i < 3*numSeeds; i++ )
	{
		int first  = (rand()*rand())%numSeeds;
		int second = (rand()*rand())%numSeeds;
		int temp = order[first];
		order[first] = order[second];
		order[second] = temp;
	}
}

///////////////////////////////////////////////////////////////////////////////////

void check_input_arguments(int argc)
{
	check_error( ( argc < 4 ),"superpixels input.pgm output.ppm patch_size [NUM_ITERATIONS] [TYPE] [lambda]  [h_edges.pgm v_edges.pgm d_b_edges.pgm d_f_edges.pgm]");
	check_error( ( argc > 7 && argc < 11 ),"If providing edge images, provide all 4 of them");

}
////////////////////////////////////////////////////////////////////////////////////////////////////


/*int main(int argc, char **argv)
{
	
	check_input_arguments(argc);
	
	int numIter = 2;
	if ( argc >= 5) 
		numIter = (Value) atoi( argv[4]);
	
	
	Value lambda = 10;
	if ( argc >= 7) 
		lambda = (Value) atoi( argv[6]);
	
	int TYPE;

	if (argc >= 6 )
		TYPE = atoi( argv[5]);
	else TYPE  = 0;

		
	int PATCH_SIZE = atoi(argv[3]);
	image<uchar> *I = loadPGM(argv[1]);
	check_error(  I == 0, "Cannot load input image");
	srand(clock());
    
	
	int width  = I->width();
	int height = I->height();
    int num_pixels = width*height; 

	float variance = computeImageVariance(I,width,height);
	
	// Initialize and place seeds
	vector<int> Seeds(num_pixels);
	int numSeeds = 0;
	PlaceSeeds(I,width,height,num_pixels,Seeds,&numSeeds,PATCH_SIZE);
	MoveSeedsFromEdges(I,width,height,num_pixels,Seeds,numSeeds,PATCH_SIZE);


	vector<int> horizWeights(num_pixels,lambda);
	vector<int> vertWeights(num_pixels,lambda);
	vector<int> diag1Weights(num_pixels,lambda);
	vector<int> diag2Weights(num_pixels,lambda);

	if (argc <= 7 )
	{
		computeWeights(horizWeights,num_pixels,width,height,lambda,variance,-1,0,I,TYPE);
		computeWeights(vertWeights,num_pixels,width,height,lambda,variance,0,-1,I,TYPE);
		computeWeights(diag1Weights,num_pixels,width,height,lambda,variance,-1,-1,I,TYPE);
		computeWeights(diag2Weights,num_pixels,width,height,lambda,variance,1,-1,I,TYPE);
	}
	else{
		loadEdges(horizWeights,num_pixels,width,height,lambda,argv[7]);
		loadEdges(vertWeights,num_pixels,width,height,lambda,argv[8]);
		loadEdges(diag1Weights,num_pixels,width,height,lambda,argv[9]);
		loadEdges(diag2Weights,num_pixels,width,height,lambda,argv[10]);
	}

	vector<int> labeling(num_pixels);

	initializeLabeling(labeling,width,height,Seeds,numSeeds,PATCH_SIZE);
		
	Value oldEnergy,newEnergy;

	vector<int> changeMask(num_pixels,1);
	vector<int> changeMaskNew(num_pixels,0);

	vector<int> order(numSeeds);
	for ( int i = 0; i < numSeeds; i++ )
		order[i] = i;

	int j = 0;
	//purturbSeeds(order,numSeeds);
	

	while ( 1 )	{
		newEnergy = computeEnergy(labeling,width,height,num_pixels,horizWeights,vertWeights,
			                      diag1Weights,diag2Weights,Seeds, I,TYPE);

		if ( j == 0 ){
			oldEnergy = newEnergy+1;
			printf("Initial Energy: %d,  %f sec", newEnergy, ((float)clock())/CLOCKS_PER_SEC );
		}
		//else {
		//	printf("\nAfter iteration %d: ", j-1);
		//	printf("Energy: %d,  %f sec", newEnergy, ((float)clock())/CLOCKS_PER_SEC );
		//}

		if ( newEnergy == oldEnergy || j >= numIter) 
			break;

		oldEnergy = newEnergy;
	
		
		for ( int i = 0; i < numSeeds; i++ ){
			expandOnLabel(order[i],width,height,num_pixels,Seeds,numSeeds,labeling,horizWeights,
				         vertWeights,lambda,diag1Weights,diag2Weights,PATCH_SIZE,changeMask,
						 changeMaskNew,I,TYPE,variance);
		}
		for (int i = 0; i < num_pixels; i++){
			changeMask[i] = changeMaskNew[i];
			changeMaskNew[i] = 0;
		}

		//purturbSeeds(order,numSeeds);
		j++;
	}
	
	printf("\nFinal Energy:   %d,",newEnergy);

	int numSegm = saveSegmentationColor(labeling,width,height,num_pixels,I,numSeeds,argv[2]);

	printf("  %f sec\nNumber of superpixels is %d ",((float)clock())/CLOCKS_PER_SEC,numSegm );
	delete I;
  return 0;
}*/
