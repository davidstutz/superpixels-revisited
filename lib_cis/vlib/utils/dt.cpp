#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "pnmfile.h"
#include "imconv.h"
#include "dt.h"

using namespace vlib;

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s input(pbm) output(pgm)\n", argv[0]);
    return 1;
  }

  char *input_name = argv[1];
  char *output_name = argv[2];

  // load input
  image<uchar> *input = loadPBM(input_name);

  // compute dt
  image<float> *out = dt(input);

  // take square roots
  for (int y = 0; y < out->height(); y++) {
    for (int x = 0; x < out->width(); x++) {
      imRef(out, x, y) = sqrt(imRef(out, x, y));
    }
  }

  // convert to grayscale
  image<uchar> *gray = imageFLOATtoUCHAR(out);

  // save output
  savePGM(gray, output_name);

  delete input;
  delete out;
  delete gray;
}
