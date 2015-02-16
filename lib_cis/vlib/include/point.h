/* 2d point */

#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <iostream>
#include "misc.h"

namespace vlib {

template <class T>
class point {
public:
  typedef T type;
  T x, y;
  
  point(const T &a = 0, const T &b = 0) : x(a), y(b) {}
  template <class C> point(const point<C> &a) : x(a.x), y(a.y) {}

  point<T> &operator+=(const point<T> &a) { x += a.x; y += a.y; return *this; }
  point<T> &operator-=(const point<T> &a) { x -= a.x; y -= a.y; return *this; }
  point<T> &operator*=(const T &a) { x *= a; y *= a; return *this; }
  point<T> &operator/=(const T &a) { x /= a; y /= a; return *this; }
};

template <class T>
inline point<int> round(const point<T> &a) {
  return point<int>((int)(a.x + (T)0.5), (int)(a.y + (T)0.5));
}

template <class T>
inline bool operator==(const point<T> &a, const point<T> &b) {
  return ((a.x == b.x) && (a.y == b.y));
}

template <class T>
inline point<T> operator+(const point<T> &a, const point<T> &b) {
  return point<T>(a.x + b.x, a.y + b.y);
}

template <class T>
inline point<T> operator-(const point<T> &a, const point<T> &b) {
  return point<T>(a.x - b.x, a.y - b.y);
}

template <class T>
inline point<T> operator*(const point<T> &a, const T &b) {
  return point<T>(a.x * b, a.y * b);
}

template <class T>
inline point<T> operator/(const point<T> &a, const T &b) {
  return point<T>(a.x / b, a.y / b);
}

template <class T>
inline T operator*(const point<T> &a, const point<T> &b) {
  return a.x * b.x + a.y * b.y;
}

template <class T>
inline point<T> abs(const point<T> &a) {
  return point<T>(abs(a.x), abs(a.y));
}

template <class T>
inline point<int> sign(const point<T> &a) {
  return point<int>(sign(a.x), sign(a.y));
}

template <class T>
inline T area(const point<T> &a, const point<T> &b, const point<T> &c) {
  return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

template <class T>
inline T norm2(const point<T> &a) {
  return a.x * a.x + a.y * a.y;
}

template <class T>
inline double norm(const point<T> &a) {
  return (sqrt((double)norm2(a)));
}

template <class T>
inline bool right(const point<T> &a, const point<T> &b, const point<T> &c) {
  return area(a, b, c) > 0;
}

template <class T>
inline bool intersect(const point<T> &a, const point<T> &b,
		      const point<T> &c, const point<T> &d) {
  return 
    (right(a, b, c) ^ right(a, b, d)) &&
    (right(c, d, a) ^ right(c, d, b));
}

// check for dot product < -1 and > 1 because of rounding error!!
template <class T>
inline float angle(const point<T> &a, const point<T> &b, const point<T> &c) {
  point<T> u = a-b;
  point<T> v = c-b;
  return acos((u * v)/(norm(u)*norm(v)));
}

template <class T>
std::ostream &operator<<(std::ostream &f, point<T> a) {
  f << "(" << a.x << "," << a.y << ")";
  return f;
}

}

#endif

