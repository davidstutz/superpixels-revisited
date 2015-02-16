/* simple drawing functions */

#ifndef DRAW_H
#define DRAW_H

#include "image.h"
#include "point.h"

namespace vlib {

/* draw a point */ 
template <class T>
inline void draw_point(image<T> *im, const T &val, const point<int> &p) {
  if ((p.x >= 0) && (p.x < im->width()) &&
      (p.y >= 0) && (p.y < im->height()))
    imRef(im, p.x, p.y) = val;
}

/* draw a square */
template <class T>
void draw_square(image<T> *im, const T &val, const point<int> &p, int size) {
  point<int> l;

  for (l.y = p.y-size; l.y <= p.y+size; l.y++)
    for (l.x = p.x-size; l.x <= p.x+size; l.x++)
      draw_point(im, val, l);
}

/* draw a rectangle */
template <class T>
void draw_rect(image<T> *im, const T &val, 
	       const point<int> &a, const point<int> &b) {
  for (int x = a.x; x < b.x; x++) {
    draw_point(im, val, point<int>(x, a.y));
    draw_point(im, val, point<int>(x, b.y));
  }
  for (int y = a.y; y < b.y; y++) {
    draw_point(im, val, point<int>(a.x, y));
    draw_point(im, val, point<int>(b.x, y));
  }
}

/* draw a filled rectangle */
template <class T>
void draw_rectfill(image<T> *im, const T &val, 
		   const point<int> &a, const point<int> &b) {
  for (int y = a.y; y < b.y; y++) {
    for (int x = a.x; x < b.x; x++) {
      draw_point(im, val, point<int>(x, y));
    }
  }
}

/* draw a cross */
template <class T>
void draw_cross(image<T> *im, const T &val, const point<int> &p, int size) {
  for (int yp = p.y-size; yp <= p.y+size; yp++)
    draw_point(im, val, point<int>(p.x, yp));
  for (int xp = p.x-size; xp <= p.x+size; xp++)
    draw_point(im, val, point<int>(xp, p.y));
}

/* draw a line segment */
template <class T>
void draw_line(image<T> *im, const T &val, point<int> p1, point<int> p2) {
  point<int> v = p2-p1;
  point<int> a = abs(v);
  point<int> s = sign(v);
  a.x <<= 1;
  a.y <<= 1;

  draw_point(im, val, p1);
  if (a.x > a.y) {
    /* x dominant */
    int d = a.y - (a.x >> 1);
    while (p1.x != p2.x) {
      if (d > 0) {
	p1.y += s.y;
	d -= a.x;
      }
      p1.x += s.x;
      d += a.y;
      draw_point(im, val, p1);
    }
  } else {
    /* y dominant */
    int d = a.x - (a.y >> 1);
    while (p1.y != p2.y) {
      if (d > 0) {
	p1.x += s.x;
	d -= a.y;
      }
      p1.y += s.y;
      d += a.x;
      draw_point(im, val, p1);
    }
  }
}

/* draw a line 
   the line is given by (a,b,c) in ax + by + c = 0 */
template <class T>
void draw_line(image<T> *im, const T &val, double a, double b, double c) {
  if (abs(a) <= abs(b)) {
    for (int x = 0; x < im->width(); x++) {
      int y = (int)(-(a * x + c) / b + 0.5); 
      draw_point(im, val, point<int>(x, y));
    }
  } else {
    for (int y = 0; y < im->height(); y++) {
      int x = (int)(-(b * y + c) / a + 0.5);
      draw_point(im, val, point<int>(x, y));
    }
  }
}

/* draw a circle */
template <class T>
void draw_circle(image<T> *im, const T &val, point<int> p, int radius) {
  int r2 = square(radius);
  for (int x = 0; x <= radius; x++) {
    int y = (int)(sqrt(r2 - square(x)) + 0.5);
    draw_point(im, val, point<int>(p.x + x, p.y + y));
    draw_point(im, val, point<int>(p.x + x, p.y - y));
    draw_point(im, val, point<int>(p.x - x, p.y + y));
    draw_point(im, val, point<int>(p.x - x, p.y - y));
    draw_point(im, val, point<int>(p.x + y, p.y + x));
    draw_point(im, val, point<int>(p.x + y, p.y - x));
    draw_point(im, val, point<int>(p.x - y, p.y + x));
    draw_point(im, val, point<int>(p.x - y, p.y - x));
  }
}

/* draw a disc */
template <class T>
void draw_disc(image<T> *im, const T &val, point<int> p, int radius) {
  int r2 = square(radius);
  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      if (x*x + y*y <= radius*radius)
	draw_point(im, val, point<int>(p.x + x, p.y + y));
    }
  }
}

}

#endif
