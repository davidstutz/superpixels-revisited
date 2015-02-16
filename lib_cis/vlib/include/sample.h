/* random sampling */

#ifndef SAMPLE_H
#define SAMPLE_H

#include <cmath>
#include <sys/time.h>

namespace vlib {

/* seed the random number generator with the current time */
void seed_time() {
 struct timeval tp;
  if (gettimeofday(&tp, NULL) != 0) {
    printf("error getting time of day\n");
    exit(1);
  }

  /* initialize random number generator */
  srand48((long)tp.tv_usec);
}

/* sample from a gaussian with sigma = 1 */
double gaussian() {
  double x;
  double x1;

  static double x2;
  static int x2_valid = 0;

  if (x2_valid) {
    x2_valid = 0;
    return x2;
  }

  /*
   * Algorithm P (Polar method for normal deviates),
   * Knuth, D., "The Art of Computer Programming", Vol. 2, 3rd Edition, p. 122
   */
  do {
    x1 = 2.0 * drand48() - 1.0;
    x2 = 2.0 * drand48() - 1.0;
    x = x1 * x1 + x2 * x2;
  } while (x >= 1.0);
  x1 *= sqrt((-2.0) * log(x) / x);
  x2 *= sqrt((-2.0) * log(x) / x);

  x2_valid = 1;
  return x1;
}

/* sample from a uniform distribution in [a,b) */
double uniform(double a, double b) {
  double r = drand48();
  return a + r*(b-a);
}

/* sample from a discrete distribution */
template <class T>
int sample(T *dist, int length) {
  if (length == 1)
    return 0;

  int middle = length/2;
  double a = 0;
  double b = 0;
  for (int i = 0; i < middle; i++)
    a += dist[i];
  for (int i = middle; i < length; i++)
    b += dist[i];

  double r = drand48();
  if (r*(a+b) < a)
    return sample(dist, middle);
  else
    return middle+sample(dist+middle, length-middle);
}

}

#endif
