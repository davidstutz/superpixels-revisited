/* chanfer dt */

#include <algorithm>
#include "image.h"
#include "misc.h"

namespace vlib {

#define DSCALE  100

/* Some values, scaled by DSCALE */
#define	ROOT_2	141
#define	ROOT_5	224
#define	ONE	100

static image<long> *map(image<uchar> *im, uchar on) {
  int width = im->width();
  int height = im->height();
  image<long> *out = new image<long>(width, height, false);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (imRef(im, x, y) == on)
	imRef(out, x, y) = 0;
      else
	imRef(out, x, y) = 1 << 30;
    }
  }

  return out;
}

/* generalized L1 distance transform
 *
 * The transform is done using a mask which is
 *			  ONE  
 *		    ONE	   0	ONE
 *			  ONE 
 */
static void gdtL1(image<long> *dt) {
  int width = dt->width();
  int height = dt->height();

  /* forward pass */
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      long val = imRef(dt, x, y);
      if (y-1 >= 0)
	val = std::min(val, imRef(dt, x, y-1) + ONE);
      if (x-1 >= 0)
	val = std::min(val, imRef(dt, x-1, y) + ONE);
      imRef(dt, x, y) = val;
    }
  }

  /* reverse pass */
  for (int y = height-1; y >= 0; y--) {
    for (int x = width-1; x >= 0; x--) {
      long val = imRef(dt, x, y);
      if (y+1 < height)
	val = std::min(val, imRef(dt, x, y+1) + ONE);
      if (x+1 < width)
	val = std::min(val, imRef(dt, x+1, y) + ONE);
      imRef(dt, x, y) = val;
    }
  }
}

/* generalized L2 distance transform
 *
 * The transform is done using a mask which is
 *			ROOT_5		ROOT_5
 *		ROOT_5	ROOT_2	  ONE	ROOT_2	ROOT_5
 *			ONE	   0	ONE
 *		ROOT_5	ROOT_2	  ONE	ROOT_2	ROOT_5
 *			ROOT_5		ROOT_5
 */
static void gdtL2(image<long> *dt) {
  int width = dt->width();
  int height = dt->height();

  /* forward pass */
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      long val = imRef(dt, x, y);
      if ((x-1 >= 0) && (y-2 >= 0))
	val = std::min(val, imRef(dt, x-1, y-2) + ROOT_5);
      if ((x+1 < width) && (y-2 >= 0))
	val = std::min(val, imRef(dt, x+1, y-2) + ROOT_5);
      if ((x-2 >= 0) && (y-1 >= 0))
	val = std::min(val, imRef(dt, x-2, y-1) + ROOT_5);
      if ((x+2 < width) && (y-1 >= 0))
	val = std::min(val, imRef(dt, x+2, y-1) + ROOT_5);
      if ((x-1 >= 0) && (y-1 >= 0))
	val = std::min(val, imRef(dt, x-1, y-1) + ROOT_2);
      if ((x+1 < width) && (y-1 >= 0))
	val = std::min(val, imRef(dt, x+1, y-1) + ROOT_2);
      if (y-1 >= 0)
	val = std::min(val, imRef(dt, x, y-1) + ONE);
      if (x-1 >= 0)
	val = std::min(val, imRef(dt, x-1, y) + ONE);
      imRef(dt, x, y) = val;
    }
  }

  /* reverse pass */
  for (int y = height-1; y >= 0; y--) {
    for (int x = width-1; x >= 0; x--) {
      long val = imRef(dt, x, y);
      if ((x-1 >= 0) && (y+2 < height))
	val = std::min(val, imRef(dt, x-1, y+2) + ROOT_5);
      if ((x+1 < width) && (y+2 < height))
	val = std::min(val, imRef(dt, x+1, y+2) + ROOT_5);
      if ((x-2 >= 0) && (y+1 < height))
	val = std::min(val, imRef(dt, x-2, y+1) + ROOT_5);
      if ((x+2 < width) && (y+1 < height))
	val = std::min(val, imRef(dt, x+2, y+1) + ROOT_5);
      if ((x-1 >= 0) && (y+1 < height))
	val = std::min(val, imRef(dt, x-1, y+1) + ROOT_2);
      if ((x+1 < width) && (y+1 < height))
	val = std::min(val, imRef(dt, x+1, y+1) + ROOT_2);
      if (y+1 < height)
	val = std::min(val, imRef(dt, x, y+1) + ONE);
      if (x+1 < width)
	val = std::min(val, imRef(dt, x+1, y) + ONE);
      imRef(dt, x, y) = val;
    }
  }
}

/* L1 distance transform */
static image<long> *dtL1(image<uchar> *im, uchar on = 1) {
  image<long> *dt = map(im, on);
  gdtL1(dt);
  return dt;
}

/* L2 distance transform */
static image<long> *dtL2(image<uchar> *im, uchar on = 1) {
  image<long> *dt = map(im, on);
  gdtL2(dt);
  return dt;
}

}
