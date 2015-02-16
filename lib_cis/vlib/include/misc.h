/* random stuff */

#ifndef MISC_H
#define MISC_H

#include <cmath>


namespace vlib {

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#ifndef INF
#define INF 1E20
#endif
  
  typedef unsigned char uchar;
  typedef unsigned long ulong;
  
  typedef struct { uchar r, g, b; } rgb;
  typedef struct { float r, g, b; } rgb_float;

  static inline bool operator==(rgb &v, rgb &u) {
    return (v.r == u.r && v.g == u.g && v.b == u.b);
  }

  static inline bool operator==(rgb_float &v, rgb_float &u) {
    return (v.r == u.r && v.g == u.g && v.b == u.b);
  }
  
  static inline rgb_float operator*(float s, const rgb_float &v) {
    rgb_float result = { s * v.r, s * v.g, s * v.b };
    return result;
  }

  static inline rgb_float operator*(const rgb_float &v, float s) {
    rgb_float result = { s * v.r, s * v.g, s * v.b };
    return result;
  }

  static inline rgb_float operator*(const rgb_float &u, const rgb_float &v) {
    rgb_float result = { u.r * v.r, u.g * v.g, u.b * v.b };
    return result;
  }

  static inline rgb_float operator+(const rgb_float &u, const rgb_float &v) {
    rgb_float result = { u.r + v.r, u.g + v.g, u.b + v.b };
    return result;
  }

  static inline rgb_float operator-(const rgb_float &u, const rgb_float &v) {
    rgb_float result = { u.r - v.r, u.g - v.g, u.b - v.b };
    return result;
  }
  
  template <class T>
    inline T abs(const T &x) { return (x > 0 ? x : -x); };
  
  template <class T>
    inline int sign(const T &x) { return (x >= 0 ? 1 : -1); };
  
  template <class T>
    inline T square(const T &x) { return x*x; };
  
  template <class T>
    inline T bound(const T &x, const T &min, const T &max) {
    return (x < min ? min : (x > max ? max : x));
  }
  
  template <class T>
    inline bool check_bound(const T &x, const T&min, const T &max) {
    return ((x < min) || (x > max));
  }
 

  //inline int vlib_round(float x) { return (int)rintf(x); }


  //inline int vlib_round(double x) { return (int)rint(x); }
  
  inline double gaussian(double val, double sigma) {
    return exp(-square(val/sigma)/2)/(sqrt(2*M_PI)*sigma);
  }

}

#endif
