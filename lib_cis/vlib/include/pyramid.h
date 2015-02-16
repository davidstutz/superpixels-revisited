/* 
   
  Gaussian pyramid.

  Implementation of the method described in the paper:
  The Laplacian Pyramid as a Compact Image Code (Burt, Adelson).

*/

#ifndef PYRAMID_H
#define PYRAMID_H

#include "image.h"
#include "misc.h"
#include "imconv.h"

namespace vlib {

  /* Fill in weights in convolution mask. */
  static void pyramid_mask(float a, float *mask) {
    mask[0] = a;
    mask[1] = 0.25F;
    mask[2] = 0.25F - a/2.0F;
  }

  /* Reduce source along horizontal direction using mask.
     Output is flipped (transposed)! */
  template<class T>
  static image<T> *reduce_helper(image<T> *src, float *mask, int len) {
    int width = src->width();
    int height = src->height();
    
    int new_width = ((width - 1) >> 1) + 1;
    image<T> *dst = new image<T>(height, new_width, false);
    
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < new_width; x++) {
	int xp = x << 1;
	T sum = mask[0] * imRef(src, xp, y);
	for (int i = 1; i < len; i++) {
	  sum = sum + mask[i] * 
	    (imRef(src, std::max(xp-i,0), y) + 
	     imRef(src, std::min(xp+i, width-1), y));
	}
	imRef(dst, y, x) = sum;
      }
    }
    
    return dst;
  }

  /* Smooth and subsample image. */
  template <class T>
  image<T> *reduce(image<T> *input) {
    float mask[3];
    pyramid_mask(0.5F, mask);
    image<T> *tmp = reduce_helper(input, mask, 3);
    image<T> *output = reduce_helper(tmp, mask, 3);
    delete tmp;
    return output;
  }

  /* Compute gaussian pyramid. */
  template <class T>
  image<T> **pyramid(image<T> *input, int levels) {
    assert(levels >= 1);
    image<T> **P = new image<T> *[levels];
    P[0] = input->copy();
    for (int i = 1; i < levels; i++) {
      P[i] = reduce(P[i-1]);
    }
    return P;
  }

  /* Compute gaussian pyramid of uchar image. */
  image<float> **pyramid(image<uchar> *input, int levels) {
    image<float> *tmp = imageUCHARtoFLOAT(input);
    image<float> **P = pyramid(tmp, levels);
    delete tmp;
    return P;
  }

  /* Compute gaussian pyramid of rgb image. */
  image<rgb_float> **pyramid(image<rgb> *input, int levels) {
    image<rgb_float> *tmp = imageRGBtoFLOAT(input);
    image<rgb_float> **P = pyramid(tmp, levels);
    delete tmp;
    return P;
  }

  /* Save picture of pyramid */
  void output_pyramid(image<float> **P, int levels, char *name) {
    int width = 0;
    int height = P[0]->height();
    for (int i = 0; i < levels; i++)
      width += P[i]->width();

    image<uchar> *out = new image<uchar>(width, height);
    int pos = 0;
    for (int i = 0; i < levels; i++) {
      for (int y = 0; y < P[i]->height(); y++) {
	for (int x = 0; x < P[i]->width(); x++) {
	  imRef(out, x+pos, y) = (uchar)(imRef(P[i], x, y));
	}
      }
      pos += P[i]->width();
    }

    savePGM(out, name);
    delete out;
  }

  /* Save picture of pyramid */
  void output_pyramid(image<rgb_float> **P, int levels, char *name) {
    int width = 0;
    int height = P[0]->height();
    for (int i = 0; i < levels; i++)
      width += P[i]->width();

    image<rgb> *out = new image<rgb>(width, height);
    int pos = 0;
    for (int i = 0; i < levels; i++) {
      for (int y = 0; y < P[i]->height(); y++) {
	for (int x = 0; x < P[i]->width(); x++) {
	  imRef(out, x+pos, y).r = (uchar)(imRef(P[i], x, y).r);
	  imRef(out, x+pos, y).g = (uchar)(imRef(P[i], x, y).g);
	  imRef(out, x+pos, y).b = (uchar)(imRef(P[i], x, y).b);
	}
      }
      pos += P[i]->width();
    }

    savePPM(out, name);
    delete out;
  }

}

#endif

