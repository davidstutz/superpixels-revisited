include_directories(../)

add_library(pb_maxflow instances.cxx  QPBO_Elim.cc QPBO_LazyElim.cc QPBO_MaxFlow.cc)
