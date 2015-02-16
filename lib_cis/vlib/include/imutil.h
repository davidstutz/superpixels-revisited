/* some image utilities */

#ifndef IMUTIL_H
#define IMUTIL_H

#include "image.h"
#include "misc.h"

namespace vlib {

/* compute minimum and maximum value in an image */
template <class T>
void min_max(image<T> *im, T *ret_min, T *ret_max) {
  int width = im->width();
  int height = im->height();
  
  T min = imRef(im, 0, 0);
  T max = imRef(im, 0, 0);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      T val = imRef(im, x, y);
      if (min > val)
	min = val;
      if (max < val)
	max = val;
    }
  }

  *ret_min = min;
  *ret_max = max;
} 

/* threshold image */
template <class T>
image<uchar> *threshold(image<T> *src, T t) {
  int width = src->width();
  int height = src->height();
  image<uchar> *dst = new image<uchar>(width, height);
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(dst, x, y) = (imRef(src, x, y) >= t);
    }
  }

  return dst;
}

}

#endif

