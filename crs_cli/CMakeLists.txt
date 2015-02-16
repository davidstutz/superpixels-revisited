include_directories(../lib_seeds_revised/ ../lib_crs/)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${EIGEN_INCLUDE_DIRS})

add_executable(crs_cli main.cpp)
target_link_libraries(crs_cli ${Boost_LIBRARIES} ${OpenCV_LIBS} seeds_revised)
