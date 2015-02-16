/* image morphology */

#ifndef MORPH_H
#define MORPH_H

#include "misc.h"
#include "image.h"
#include "chamfer.h"

namespace vlib {

static void dilate(image<uchar> *im, float r) {
  image<long> *dt = dtL2(im);
  int width = im->width();
  int height = im->height();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(im, x, y) = imRef(dt, x, y) <= r*DSCALE;
    }
  }
  delete dt;
}

static void erode(image<uchar> *im, float r) {
  image<long> *dt = dtL2(im, 0);
  int width = im->width();
  int height = im->height();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(im, x, y) = imRef(dt, x, y) > r*DSCALE;
    }
  }
  delete dt;
}

}

#endif
