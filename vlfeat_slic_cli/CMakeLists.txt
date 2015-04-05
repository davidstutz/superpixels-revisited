include_directories(../lib_seeds_revised/ ../lib_vlfeat/vl/)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${EIGEN_INCLUDE_DIRS})


add_library(vlfeat
    ../lib_vlfeat/vl/generic.c
    ../lib_vlfeat/vl/mathop.c
    ../lib_vlfeat/vl/aib.c
    ../lib_vlfeat/vl/array.c
    ../lib_vlfeat/vl/covdet.c
    ../lib_vlfeat/vl/dsift.c
    ../lib_vlfeat/vl/fisher.c
    ../lib_vlfeat/vl/getopt_long.c
    ../lib_vlfeat/vl/gmm.c
    ../lib_vlfeat/vl/hikmeans.c
    ../lib_vlfeat/vl/hog.c
    ../lib_vlfeat/vl/homkermap.c
    ../lib_vlfeat/vl/host.c
    ../lib_vlfeat/vl/ikmeans.c
    ../lib_vlfeat/vl/ikmeans_elkan.tc
    ../lib_vlfeat/vl/ikmeans_init.tc
    ../lib_vlfeat/vl/ikmeans_lloyd.tc
    ../lib_vlfeat/vl/imopv.c
    ../lib_vlfeat/vl/kdtree.c
    ../lib_vlfeat/vl/kmeans.c
    ../lib_vlfeat/vl/lbp.c
    ../lib_vlfeat/vl/liop.c
    ../lib_vlfeat/vl/mathop.c
    ../lib_vlfeat/vl/mser.c
    ../lib_vlfeat/vl/pgm.c
    ../lib_vlfeat/vl/quickshift.c
    ../lib_vlfeat/vl/random.c
    ../lib_vlfeat/vl/rodrigues.c
    ../lib_vlfeat/vl/scalespace.c
    ../lib_vlfeat/vl/sift.c
    ../lib_vlfeat/vl/slic.c
    ../lib_vlfeat/vl/stringop.c
    ../lib_vlfeat/vl/svm.c
    ../lib_vlfeat/vl/svmdataset.c
    ../lib_vlfeat/vl/vlad.c)

add_executable(vlfeat_slic_cli main.cpp)
target_link_libraries(vlfeat_slic_cli ${Boost_LIBRARIES} ${OpenCV_LIBS} vlfeat seeds_revised ${CMAKE_THREAD_LIBS_INIT})
