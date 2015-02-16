#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <climits>
#include "pnmfile.h"
#include "pyramid.h"
#include "features_.h"
#include "filter.h"

using namespace vlib;

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "usage: %s input(ppm) output(ppm) levels\n", argv[0]);
    return 1;
  }

  char *input_name = argv[1];
  char *output_name = argv[2];
  int levels = atoi(argv[3]);

  image<rgb> *input = loadPPM(input_name);
  image<rgb_float> **P = pyramid(input, levels);
  output_pyramid(P, levels, output_name);

  for (int i = 0; i < levels; i++) {
    image<rgb_float> *s = smooth(P[i], 1);
    image<feature> *f = features(s);
    image<float> *im = draw_features(f, 10);
    image<uchar> *tmp = imageFLOATtoUCHAR(im);
    char name[256];
    sprintf(name, "features%02d.pgm", i);
    savePGM(tmp, name);
    delete f;
    delete im;
    delete tmp;
  }

  return 0;
}
 
