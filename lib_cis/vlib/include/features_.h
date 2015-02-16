/*

  Low-level features.

*/

#ifndef FEATURES_H
#define FEATURES_H

#include <cmath>
#include "image.h"
#include "misc.h"
#include "draw.h"

namespace vlib {

  /* First order features have an orientation and a magnitude */
  struct feature {
    float theta, mag;
  };

  /* Gray images */
  image<feature> *features(image<float> *im) {
    int width = im->width();
    int height = im->height();
    image<feature> *f = new image<feature>(width, height);

    for (int y = 1; y < height-1; y++) {
      for (int x = 1; x < width-1; x++) {
	float dx = imRef(im, x+1, y) - imRef(im, x-1, y);
	float dy = imRef(im, x, y+1) - imRef(im, x, y-1);
	imRef(f, x, y).theta = atan2f(-dy, dx);
	imRef(f, x, y).mag = sqrtf(dx*dx + dy*dy);
      }
    }
    return f;
  }

  /* Color images */
  image<feature> *features(image<rgb_float> *im) {
    int width = im->width();
    int height = im->height();
    image<feature> *f = new image<feature>(width, height);

    for (int y = 1; y < height-1; y++) {
      for (int x = 1; x < width-1; x++) {
	rgb_float dx = imRef(im, x+1, y) - imRef(im, x-1, y);
	rgb_float dy = imRef(im, x, y+1) - imRef(im, x, y-1);
	rgb_float mag = dx*dx + dy*dy;
	if ((mag.r >= mag.g) && (mag.r >= mag.b)) {
	  imRef(f, x, y).theta = atan2f(-dy.r, dx.r);
	  imRef(f, x, y).mag = sqrt(mag.r);
	} else if (mag.g >= mag.b) {
	  imRef(f, x, y).theta = atan2f(-dy.g, dx.g);
	  imRef(f, x, y).mag = sqrt(mag.g);
	} else {
	  imRef(f, x, y).theta = atan2f(-dy.b, dx.b);
	  imRef(f, x, y).mag = sqrt(mag.b);
	}
      }
    }    

    return f;
  }

  /* Visualize features */
  image<float> *draw_features(image<feature> *f, int SCALE) {
    int width = f->width();
    int height = f->height();
    image<float> *im = new image<float>(width*SCALE, height*SCALE);
    
    for (int y = 1; y < height-1; y++) {
      for (int x = 1; x < width-1; x++) {
	float theta = imRef(f, x, y).theta;
	float mag = imRef(f, x, y).mag;
	int dx = -(int)rintf(SCALE*sinf(theta)/2.0F);
	int dy = -(int)rintf(SCALE*cosf(theta)/2.0F);
	point<int> p(x*SCALE, y*SCALE);
	point<int> shift(dx, dy);
	draw_line(im, mag, p-shift, p+shift);
      }
    }

    return im;
  }

}

#endif
