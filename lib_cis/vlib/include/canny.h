/* canny */

#ifndef CANNY_H
#define CANNY_H

#include <cmath>
#include "grad.h"

namespace vlib {

static int offset1[5];
static int offset2[5];

/* non-maximum suppression */
static inline int NMS(float *magn, float dx, float dy, float &theta) {
  float mag, m1, m2;
  int n, o1, o2;
  float interp1, interp2;

  /* map [-pi/2 .. pi/2] to [0 .. 4] */
  if (dx == 0)
    theta = 0;
  else
    theta = 4.0*(0.5 + atan(dy/dx)/M_PI);

  n = (int)theta;
  interp1 = theta - (float)n;
  interp2 = 1.0 - interp1;

  o1 = offset1[n];
  o2 = offset2[n];

  mag = *magn;
  m1 = *(magn + o1)*interp2 + *(magn + o2)*interp1;
  m2 = *(magn - o1)*interp2 + *(magn - o2)*interp1;
  return (mag>=m1 && mag>=m2 && m1!=m2);     /* return 1 iff passes NMS */
}

/* canny edge detection */
static image<uchar> *canny(image<float> *src, float t) {
  image<float> *dx, *dy, *mag;
  float theta;

  t = square(t);
  int width = src->width();
  int height = src->height();

  /* used for NMS */
  offset1[0] = -width;   offset2[0] = -width+1;
  offset1[1] = -width+1; offset2[1] = 1;
  offset1[2] = 1;        offset2[2] = width+1;
  offset1[3] = width+1;  offset2[3] = width;
  offset1[4] = width;    offset2[4] = width-1;  

  gradient(src, &dx, &dy, &mag);
  image<uchar> *edges = new image<uchar>(width, height);

  /* threshold and do NMS */
  for (int y = 1; y < height-1; y++) {
    for (int x = 1; x < width-1; x++) {
      if (imRef(mag, x, y) > t)
	imRef(edges, x, y) = NMS(imPtr(mag, x, y), imRef(dx, x, y), 
				 imRef(dy, x, y), theta);
    }
  }

  delete dx;
  delete dy;
  delete mag;
  return edges;
}

}

#endif
