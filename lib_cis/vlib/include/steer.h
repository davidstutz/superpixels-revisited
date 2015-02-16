/* steerable gradient filters */

#ifndef STEER_H
#define STEER_H

#include "filter.h"

namespace vlib {

#define STEER_G1(a) cos(a)
#define STEER_G2(a) sin(a)

MAKE_FILTER(f0, exp(-0.5*square(i/sigma)));
MAKE_FILTER(f1, -i * exp(-0.5*square(i/sigma)));

void steerable(image<float> *src, float sigma, 
	       image<float> **ret_G1, image<float> **ret_G2) {
  int width = src->width();
  int height = src->height();
  
  std::vector<float> mask0 = make_f0(sigma);
  normalize(mask0);
  std::vector<float> mask1 = make_f1(sigma);
  normalize(mask1);
  
  image<float> *tmp = new image<float>(height, width, false);
  image<float> *G1 = new image<float>(width, height, false);
  image<float> *G2 = new image<float>(width, height, false);
  
  convolve_odd(src, tmp, mask1);
  convolve_even(tmp, G1, mask0);
  convolve_even(src, tmp, mask0);
  convolve_odd(tmp, G2, mask1);

  *ret_G1 = G1;
  *ret_G2 = G2;
  delete tmp;
}


image<float> *steer(image<float> *G1, image<float> *G2, float angle) {
  int width = G1->width();
  int height = G1->height();
  image<float> *dst = new image<float>(width, height, false);  

  float a = STEER_G1(angle);
  float b = STEER_G2(angle);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(dst, x, y) = a * imRef(G1, x, y) + b * imRef(G2, x, y);
    }
  }

  return dst;
}

}

#endif
