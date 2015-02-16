#include <cstdio>
#include <cstdlib>
#include "misc.h"
#include "pnmfile.h"
#include "canny.h"
#include "filter.h"

using namespace vlib;

int main(int argc, char **argv) {
  if (argc != 5) {
    fprintf(stderr, "usage: %s input(pgm) output(pbm) sigma thresh\n", 
	    argv[0]);
    return 1;
  }

  char *input_name = argv[1];
  char *output_name = argv[2];
  double sigma = atof(argv[3]);
  double thresh = atof(argv[4]);

  // load input
  image<uchar> *input = loadPGM(input_name);

  // convolve image with gaussian
  image<float> *smoothed = smooth(input, sigma);

  // edge detect
  image<uchar> *edges = canny(smoothed, thresh);

  // save output
  savePBM(edges, output_name);

  delete input;
  delete smoothed;
  delete edges;
}
