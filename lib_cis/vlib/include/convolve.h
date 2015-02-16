/* convolution */

#ifndef CONVOLVE_H
#define CONVOLVE_H

#include <vector>
#include <algorithm>
#include <cmath>
#include "image.h"

namespace vlib {

/* convolve src with mask.  dst is flipped! */
template <class T>
static void convolve_even(image<T> *src, image<T> *dst, 
			  std::vector<float> &mask) {
  int width = src->width();
  int height = src->height();
  int len = mask.size();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      T sum = mask[0] * imRef(src, x, y);
      for (int i = 1; i < len; i++) {
	sum = sum + mask[i] * 
	  (imRef(src, std::max(x-i,0), y) + 
	   imRef(src, std::min(x+i, width-1), y));
      }
      imRef(dst, y, x) = sum;
    }
  }
}

/* convolve src with mask.  dst is flipped! */
template <class T>
static void convolve_odd(image<T> *src, image<T> *dst, 
			 std::vector<float> &mask) {
  int width = src->width();
  int height = src->height();
  int len = mask.size();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      T sum = mask[0] * imRef(src, x, y);
      for (int i = 1; i < len; i++) {
	sum = sum + mask[i] * 
	  (imRef(src, std::max(x-i,0), y) - 
	   imRef(src, std::min(x+i, width-1), y));
      }
      imRef(dst, y, x) = sum;
    }
  }
}

}

#endif
