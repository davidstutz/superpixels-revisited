# Constant Intensity Superpixels

Unfortunately, the code from Constant Intensity Superpixels/Compact Superpixels [1] cannot be shipped with this library due to the restrictive license of the corresponding code. Therefore, only a command line tool in `cis_cli/` is provided.

    [3] O. Veksler, Y. Boykov, P. Mehrani.
       Superpixels and supervoxels in an energy optimization framework.
       European Conference on Computer Vision, pages 211–224, 2010.

Installation instructions:

Go to [http://www.csd.uwo.ca/faculty/olga/](http://www.csd.uwo.ca/faculty/olga/) and download the code.

Extract the archive into `lib_cis` in order to obtaint he following directory structure (where the `CMakeLists.txt` files are provided by this library):

    lib_cis
    |- vlib
        |- include
        |- utils
        |- CMakeLists.txt
    |- README.txt (original README)
    |- README.md
    |- maxflow.cpp
    |- ...
    |- superpixels.h (provided by this library, not included in the download described above)
    |- CMakeLists.txt

Comment out the `main` function in `superpixels.cpp`.

Either change the declaration of `loadEdges` inf `superpixels.cpp` to

    void loadEdges(vector<Value> &weights,int num_pixels,int width,int height,
			   Value lambda, char *name)

or use `-fpermissive` (default).

Depending on the operating system, some changes in energy.h` are required: Change occurrences of

    add_tweights(y, 0, C);
    add_edge(x, y, B+C, 0);

to

    this->add_tweights(y, 0, C);
    this->add_edge(x, y, B+C, 0);

**Documentation on the command line tool can be found in `cis_cli/main.cpp`!**
