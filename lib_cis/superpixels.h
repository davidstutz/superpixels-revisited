#ifndef SUPERPIXELS_H
#define	SUPERPIXELS_H

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

void check_error(int boolE, const char *error_message);
void PlaceSeeds(image<uchar> *I,int width,int height,
        int num_pixels, vector<int> &Seeds,int *numSeeds, int PATCH_SIZE);
void MoveSeedsFromEdges(image<uchar> *I,int width,int height,int num_pixels,
        vector<int> &Seeds, int numSeeds, int PATCH_SIZE);
int computeEnergy(vector<int> &labeling,int width,int height,int num_pixels,
        vector<Value> &horizWeights,vector<Value> &vertWeights,
        vector<Value> &diag1Weights,vector<Value> &diag2Weights,
        vector<int> &Seeds, image<uchar> *I,int TYPE);
void getBounds(int width,int height,vector<int> &Seeds,int *seedX,int *seedY,
        int *startX,int *startY,int *endX,int *endY,int label, int PATCH_SIZE);
void expandOnLabel(int label,int width,int height,int num_pixels,
        vector<int> &Seeds,int numSeeds, vector<int> &labeling,
        vector<Value> &horizWeights,vector<Value> &vertWeights,Value lambda,
        vector<Value> &diag1Weights,vector<Value> &diag2Weights,int PATCH_SIZE,
        vector<int> &changeMask, vector<int> &changeMaskNew,image<uchar> *I,
        int TYPE,float variance);
void initializeLabeling(vector<int> &labeling,int width,int height,
        vector<int> &Seeds,int numSeeds, int PATCH_SIZE);
float  computeImageVariance(image<uchar> *I,int width,int height);
void loadEdges(vector<Value> &weights,int num_pixels,int width,int height,
               Value lambda, char *name);
void computeWeights(vector<Value> &weights,int num_pixels,int width,
                    int height,Value lambda, float variance,
                    int incrX,int incrY, image<uchar> *I,
                    int TYPE);
int  saveSegmentationColor(vector<int> &labeling,int width,int height,int num_pixels,
                           image<uchar> *I, int numSeeds, char *name);
void purturbSeeds(vector<int> &order,int numSeeds);
void check_input_arguments(int argc);

#endif	/* SUPERPIXELS_H */

