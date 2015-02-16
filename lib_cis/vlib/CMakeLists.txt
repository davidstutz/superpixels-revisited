include_directories(./include/)

add_library(vlib utils/canny.cpp
utils/dilate.cpp
utils/dt.cpp
utils/grad.cpp
utils/pyramid.cpp
utils/smooth.cpp)