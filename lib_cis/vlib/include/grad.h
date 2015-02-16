/* compute image gradients */

#ifndef GRAD_H
#define GRAD_H

#include "image.h"
#include "misc.h"

namespace vlib {

/* compute image derivatives */
static void gradient(image<float> *src, 
		     image<float> **ret_dx, image<float> **ret_dy) {
  int width = src->width();
  int height = src->height();
  image<float> *dx = new image<float>(width, height);
  image<float> *dy = new image<float>(width, height);

  for (int y = 1; y < height-1; y++) {
    for (int x = 1; x < width-1; x++) {
      imRef(dx, x, y) = imRef(src, x+1, y) - imRef(src, x-1, y);
      imRef(dy, x, y) = imRef(src, x, y+1) - imRef(src, x, y-1);
    }
  }

  *ret_dx = dx;
  *ret_dy = dy;
}

/* compute gradient magniture */
static void gradient(image<float> *src, image<float> **ret_mag) {
  int width = src->width();
  int height = src->height();
  image<float> *mag = new image<float>(width, height);
  
  for (int y = 1; y < height-1; y++) {
    for (int x = 1; x < width-1; x++) {
      float dx = imRef(src, x+1, y) - imRef(src, x-1, y);
      float dy = imRef(src, x, y+1) - imRef(src, x, y-1);
      imRef(mag, x, y) = square(dx) + square(dy);
    }
  }

  *ret_mag = mag;
}

/* compute derivatives and gradient magnitude */
static void gradient(image<float> *src, 
		     image<float> **ret_dx, image<float> **ret_dy,
		     image<float> **ret_mag) {
  int width = src->width();
  int height = src->height();
  image<float> *dx = new image<float>(width, height);
  image<float> *dy = new image<float>(width, height);
  image<float> *mag = new image<float>(width, height);
  
  for (int y = 1; y < height-1; y++) {
    for (int x = 1; x < width-1; x++) {
      imRef(dx, x, y) = imRef(src, x+1, y) - imRef(src, x-1, y);
      imRef(dy, x, y) = imRef(src, x, y+1) - imRef(src, x, y-1);
      imRef(mag, x, y) = square(imRef(dx, x, y)) + square(imRef(dy, x, y));
    }
  }

  *ret_dx = dx;
  *ret_dy = dy;
  *ret_mag = mag;
}

}

#endif
