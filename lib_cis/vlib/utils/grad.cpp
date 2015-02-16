#include <cstdio>
#include <cstdlib>
#include <climits>
#include "pnmfile.h"
#include "filter.h"
#include "grad.h"

using namespace vlib;

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "usage: %s input(pgm) output(pgm) sigma\n", argv[0]);
    return 1;
  }

  char *input_name = argv[1];
  char *output_name = argv[2];
  double sigma = atof(argv[3]);

  // load input
  image<uchar> *input = loadPGM(input_name);

  // convolve image with gaussian
  image<float> *smoothed = smooth(input, sigma);

  // compute gradient magnitude
  image<float> *mag;
  gradient(smoothed, &mag);
  for (int y = 0; y < mag->height(); y++) {
    for (int x = 0; x < mag->width(); x++) {
      imRef(mag, x, y) = sqrt(imRef(mag, x, y));
    }
  }

  // convert to gray
  image<uchar> *output = imageFLOATtoUCHAR(mag);

  // save output
  savePGM(output, output_name);

  delete input;
  delete smoothed;
  delete output;
}
