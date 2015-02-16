/* a simple matrix class */

#ifndef MATRIX_H
#define MATRIX_H

namespace vlib {

template <class T>
class matrix {
 public:
  /* create a matrix */
  matrix(const int rows, const int cols, const bool init = true);

  /* delete a matrix */
  ~matrix();

  /* init a matrix */
  void init(const T &val);

  /* copy a matrix */
  matrix<T> *copy() const;
  
  /* get number of rows in a matrix. */
  int rows() const { return r; }
  
  /* get number of columns n a matrix. */
  int cols() const { return c; }
  
  /* matrix data. */
  T *data;
  
  /* row pointers. */
  T **access;
  
 private:
  int r, c;
};

/* use matRef to access matrix data. */
#define matRef(im, i, j) (im->access[i][j])
  
/* use matPtr to get pointer to matrix data. */
#define matPtr(im, i, j) &(im->access[i][j])

template <class T>
matrix<T>::matrix(const int rows, const int cols, const bool init) {
  r = rows;
  c = cols;
  data = new T[r * c];  // allocate space for matrix data
  access = new T*[r];   // allocate space for row pointers
  
  // initialize row pointers
  for (int i = 0; i < r; i++)
    access[i] = data + (i * c);  
  
  if (init)
    memset(data, 0, r * c * sizeof(T));
}

template <class T>
matrix<T>::~matrix() {
  delete [] data; 
  delete [] access;
}

template <class T>
void matrix<T>::init(const T &val) {
  T *ptr = matPtr(this, 0, 0);
  T *end = matPtr(this, r-1, c-1);
  while (ptr <= end)
    *ptr++ = val;
}


template <class T>
matrix<T> *matrix<T>::copy() const {
  matrix<T> *mat = new matrix<T>(r, c, false);
  memcpy(mat->data, data, r * c * sizeof(T));
  return mat;
}

}

#endif
  
