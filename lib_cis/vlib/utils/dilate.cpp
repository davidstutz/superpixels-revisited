#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "pnmfile.h"
#include "imconv.h"
#include "dt.h"

using namespace vlib;

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "usage: %s input(pbm) r output(pbm)\n", argv[0]);
    return 1;
  }

  char *input_name = argv[1];
  float r = square(atof(argv[2]));
  char *output_name = argv[3];

  // load input
  image<uchar> *input = loadPBM(input_name);

  // dilate
  image<float> *d = dt(input);
  image<uchar> *out = new image<uchar>(d->width(), d->height());
  for (int y = 0; y < out->height(); y++) {
    for (int x = 0; x < out->width(); x++) {
      imRef(out, x, y) = imRef(d, x, y) <= r;
    }
  }

  // save output
  savePBM(out, output_name);

  delete input;
  delete out;
  delete d;
}
