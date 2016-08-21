# Superpixels Revisited

This library combines several state-of-the-art superpixel algorithms in a single library. For each approach, a user-friendly command line tool is provided - these command line tools were used for evaluation in [1] and [2]. An overview over all superpixel approaches is provided below.

    [1] D. Stutz, A. Hermans, B. Leibe.
        Superpixel Segmentation using Depth Information.
        Bachelor thesis, RWTH Aachen University, Aachen, Germany, 2014.
	[2] D. Stutz.
		Superpixel Segmentation: An Evaluation.
		Pattern Recognition (J. Gall, P. Gehler, B. Leibe (Eds.)), Lecture Notes in Computer Science, vol. 9358, pages 555 - 562, 2015.

An overview over all superpixel algorithms and their evaluation results can be found online at [3]:

    [3] http://davidstutz.de/projects/superpixelsseeds/

![Example: several superpixel segmentations.](screenshot.png?raw=true "Example: several superpixel segmentations")

## Index

* [Superpixel Algorithms](#superpixel-algorithms)
* [Building](#building)
* [Usage](#usage)
    * [FH](#fh)
    * [SLIC](#slic)
    * [CIS/CS](#cis)
    * [ERS](#ers)
    * [PB](#pb)
    * [CRS](#crs)
    * [SEEDS](#seeds)
* [License](#license)

## Superpixel Algorithms

Provided superpixels algorithms:

* FH - Felzenswalb & Huttenlocher [4];
* SLIC - Simple Linear Iterative Clustering [5];
* CIS/CS - Constant Intensity Superpixels/Compact Superpixels [6];
* ERS - Entropy Rate Superpixels [7];
* PB - Pseudo Boolean Superpixels [8];
* CRS - Contour Relaxed Superpixels [9];
* SEEDS - Superpixels Extracted via Energy-Driven Sampling [10].

Note that the library of CIS/CS is, due to license restrictions, not directly included. See `lib_cis/README.md` for details.

Further, note that for SLIC, both the original implementation as well as the implementation as part of the VLFeat library [11] is provided. Similarly, for SEEDS the original implementation and SEEDS Revised - an implementation written during the bachelor thesis [2] - is provided.

    [4] P. F. Felzenswalb, D. P. Huttenlocher.
        Efficient graph-based image segmentation.
        International Journal of Computer Vision, 59(2), 2004.
    [5] R. Achanta, A. Shaji, K. Smith, A. Lucchi, P. Fua, S. Süsstrunk.
        SLIC superpixels.
        Technical report, École Polytechnique Fédérale de Lausanne, 2010.
    [6] O. Veksler, Y. Boykov, P. Mehrani.
        Superpixels and supervoxels in an energy optimization framework.
        European Conference on Computer Vision, pages 211–224, 2010.
    [7] M. Y. Lui, O. Tuzel, S. Ramalingam, R. Chellappa.
        Entropy rate superpixel segmentation.
        Conference on Computer Vision and Pattern Recognition, pages 2097–2104, 2011.
    [8] Superpixels via pseudo-boolean optimization. 
        Y. Zhang, R. Hartley, J. Mashford, and S. Burn.
        In International Conference on Computer Vision, 2011.
    [9] C. Conrad, M. Mertz, R. Mester.
        Contour-relaxed superpixels.
        Energy Minimization Methods in Computer Vision and Pattern Recognition,
        volume 8081 of Lecture Notes in Computer Science, pages 280–293, 2013.
    [10] M. van den Bergh, X. Boix, G. Roig, B. de Capitani, L. van Gool.
        SEEDS: Superpixels extracted via energy-driven sampling.
        European Conference on Computer Vision, pages 13–26, 2012.
	[11] A. Vedaldi, B. Fulkerson.
		VLFeat: An Open and Portable Library of Computer Vision Algorithms.
		\url{http://www.vlfeat.org/, 2008.

## Building

**Note:** The library was tested primarily on Ubuntu 14.04, Ubuntu 16.04 and OpenCV 2.4.10 as well as OpenCV 2.4.13. Comments on building instructions are welcome!

The library can be built using CMake and Boost:

    sudo apt-get install build-essential
    sudo apt-get install cmake
    sudo apt-get install libboost-all-dev

OpenCV can be installed using:

    sudo apt-get install libopencv-dev

Or following this guide: [http://docs.opencv.org/doc/tutorials/introduction/linux_install/linux_install.html](http://docs.opencv.org/doc/tutorials/introduction/linux_install/linux_install.html). Then, the library can be built using:
    
    git clone --recursive https://github.com/davidstutz/superpixels-revisited.git
    cd superpixels-revisited
    mkdir -p build
    cd build
    cmake ..
    make

**Note:** This repository currently includes the [VLFeat](https://github.com/vlfeat/vlfeat) library [11] for simplicity. However, VLFeat can also be installed using:

    sudo apt-get install libvlfeat-dev libvlfeat0

Then, the target `vlfeat` in `vlfeat_slic_cli/CMakeLists.txt` can be commented out.

**Also see `.travis.yml` for building instructions.**

The executables will be created in `superpixels-revisited/bin` while the libraries will be written to `superpixels-revisited/lib`.

**For building CIS/CS [6] you need to download the corresponding library first, see `lib_cis/README.md`.**

Per default, all superpixel algorithms are built. By adapting `superpixels-revisited/CMakeLists.txt`, this behavior can be adapted by commenting out the corresponding subdirectories:

    # SEEDS Revised
    add_subdirectory(lib_seeds_revised)

    # Constant Intensity Superpixels/Compact Superpixels
    # Remove comments after installing the library as described in
    # lib_cli/README.md!
    # add_subdirectory(lib_cis)
    # add_subdirectory(cis_cli)

    # Entropy Rate Superpixels
    add_subdirectory(lib_ers)
    add_subdirectory(ers_cli)

    # Contour Relaxed Superpixels
    add_subdirectory(lib_crs)
    add_subdirectory(crs_cli)

    # Felzenswalb & Huttenlocher
    add_subdirectory(lib_fh)
    add_subdirectory(fh_cli)

    # Pseudo Boolean Superpixels
    add_subdirectory(lib_pb)
    add_subdirectory(pb_cli)

    # SEEDS
    add_subdirectory(lib_seeds)
    add_subdirectory(seeds_cli)

    # SLIC
    add_subdirectory(lib_slic)
    add_subdirectory(slic_cli)

    # VLFeat SLIC
    add_subdirectory(vlfeat_slic_cli)

## Usage

**Note:** Usage details can also be found in the corresponding `main.cpp` files (e.g. `crs_cli/main.cpp` or `lib_seeds_revised/cli/main.cpp`).

In general, the following executables are provided:

* `bin/cli`: SEEDS Revised;
* `bin/cis_cli`: CIS/CS;
* `bin/crs_cli`: CRS;
* `bin/ers_cli`: ERS;
* `bin/fh_cli`: FH;
* `bin/pb_cli`: PB;
* `bin/seeds_cli`: SEEDS;
* `bin/slic_cli`: SLIC;
* `bin/vlfeat_slic_cli`: VLFeat SLIC.

Each command line tool is provided on an input directory containing a variables number of images to be oversegmented. Further, each executable is able to write the resulting segmentations to `.csv` files and create boundary images using the `--csv` and `--contour` options, respectively, see the example below. Using the `--help` option, all available options are printed, e.g.:

    $ ./bin/cli --help
    Allowed options:
      --help                          produce help message
      --input arg                     the folder to process, may contain several 
                                      images
      --bins arg (=5)                 number of bins used for color histograms
      --neighborhood arg (=1)         neighborhood size used for smoothing prior
      --confidence arg (=0.100000001) minimum confidence used for block update
      --iterations arg (=2)           iterations at each level
      --spatial-weight arg (=0.25)    spatial weight
      --superpixels arg (=400)        desired number of supüerpixels
      --verbose                       show additional information while processing
      --csv                           save segmentation as CSV file
      --contour                       save contour image of segmentation
      --labels                        save label image of segmentation
      --mean                          save mean colored image of segmentation
      --output arg (=output)          specify the output directory (default is 
                                      ./output)

As example, for running SEEDS Revised on the test set of the Berkeley Segmentation Dataset [12], use:

    $ cd superpixels-revisited
    $ wget http://www.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/BSR/BSR_bsds500.tgz
    $ tar -xvzf BSR_bsds500.tgz
    $ mkdir output
    $ ./bin/cli ./BSR/BSDS500/data/images/test/ ./output --contour
    200 images total ...
    On average, 0.118183 seconds needed ...

For details on the Berkeley Segmentation Dataset [12], see:

    [12] P. Arbeláez, M. Maire, C. Fowlkes, J. Malik.
         Contour detection and hierarchical image segmentation.
         Transactions on Pattern Analysis and Machine Intelligence, volume 33, number 5, pages 898–916, 2011.

In the following, each executable is described in detail.

### FH

    $ ./bin/fh_cli --help
    Allowed options:
      --help                   produce help message
      --input arg              the folder to process
      --sigma arg (=1)         sigma used for smoothing
      --threshold arg (=20)    constant for threshold function
      --minimum-size arg (=10) minimum component size
      --time arg               time the algorithm and save results to the given 
                               directory
      --process                show additional information while processing
      --csv                    save segmentation as CSV file
      --contour                save contour image of segmentation
      --mean                   save mean colored image of segmentation
      --output arg (=output)   specify the output directory (default is ./output)

### SLIC

For SLIC, two executables are provided, the original implementation and the implementation as part of the VLFeat library:

    # OriginalSLIC:
    $ ./bin/slic_cli --help
    Allowed options:
      --help                   produce help message
      --input arg              the folder to process (can also be passed as 
                               positional argument)
      --superpixels arg (=400) number of superpixles
      --compactness arg (=40)  compactness
      --perturb-seeds          perturb seeds
      --iterations arg (=10)   iterations
      --time arg               time the algorithm and save results to the given 
                               directory
      --process                show additional information while processing
      --csv                    save segmentation as CSV file
      --contour                save contour image of segmentation
      --mean                   save mean colored image of segmentation
      --output arg (=output)   specify the output directory (default is ./output)
    # VLFeat SLIC:
    $ ./bin/vlfeat_slic_cli --help
    Allowed options:
      --help                         produce help message
      --input arg                    the folder to process (can also be passed as 
                                     positional argument)
      --region-size arg (=10)        region size used; defines the number of 
                                     superpixels
      --minimum-region-size arg (=1) minimum region size allowed
      --regularization arg (=100)    regularization trades off color for spatial 
                                     closeness
      --time arg                     time the algorithm and save results to the 
                                     given directory
      --process                      show additional information while processing
      --csv                          save segmentation as CSV file
      --contour                      save contour image of segmentation
      --mean                         save mean colored image of segmentation
      --process                      show additional information
      --output arg (=output)         specify the output directory (default is 
                                     ./output)

### CIS

    $ ./bin/cis_cli --help
    Allowed options:
      --help                  produce help message
      --input arg             folder containing the images to process
      --region-size arg (=10) maxmimum allowed region size (that is region size x 
                              region size patches)
      --type arg (=1)         0 for compact superpixels, 1 for constant intensity 
                              superpixels
      --iterations arg (=2)   number of iterations
      --lambda arg (=50)      lambda only influences constant intensity 
                              superpixels; larger lambda results in smoother 
                              boundaries
      --process               show additional information while processing
      --time arg              time the algorithm and save results to the given 
                              directory
      --csv                   save segmentation as CSV file
      --contour               save contour image of segmentation
      --mean                  save mean colored image of segmentation
      --time                  save timings in BSD evaluation format in the given 
                              directory
      --output arg (=output)  specify the output directory (default is ./output)

### ERS

    $ ./bin/ers_cli --help
    Allowed options:
      --help                   produce help message
      --input arg              the folder to process
      --lambda arg (=0.5)      lambda
      --sigma arg (=5)         sigma
      --four-connected         use 4-connected
      --superpixels arg (=400) number of superpixels
      --time arg               time the algorithm and save results to the given 
                               directory
      --process                show additional information while processing
      --csv                    save segmentation as CSV file
      --contour                save contour image of segmentation
      --mean                   save mean colored image of segmentation
      --output arg (=output)   specify the output directory (default is ./output)

### PB

    $ ./bin/pb_cli --help
    Allowed options:
      --help                 produce help message
      --input arg            the folder to process (can also be passed as 
                             positional argument)
      --height arg (=10)     height of initial vertical strips
      --width arg (=10)      width of initial vertical strips
      --sigma arg (=20)      balancing the weight between regular shape and 
                             accurate edge
      --max-flow             use max flow algorithm instead of elimination
      --time arg             time the algorithm and save results to the given 
                             directory
      --process              show additional information while processing
      --csv                  save segmentation as CSV file
      --contour              save contour image of segmentation
      --mean                 save mean colored image of segmentation
      --output arg (=output) specify the output directory (default is ./output)

### CRS

    $ ./bin/crs_cli --help
    Allowed options:
      --help                                produce help message
      --input arg                           the folder to process
      --width arg (=20)                     width of blocks in initial superpixel 
                                            segmentation
      --height arg (=20)                    height of blocks in initial superpixel 
                                            segmentation
      --compactness arg (=0.045)            compactness weight
      --clique-cost arg (=0.3)              direct clique cost
      --iterations arg (=3)                 number of iterations to perform
      --time arg                            time the algorithm and save results to 
                                            the given directory
      --process                             show additional information while 
                                            processing
      --csv                                 save segmentation as CSV file
      --contour                             save contour image of segmentation
      --mean                                save mean colored image of segmentation
      --output arg (=output)                specify the output directory (default 
                                            is ./output)

### SEEDS

For SEEDS, two implementations are provided. The first implementation, called SEEDS Revised, is published as result of the bachelor thesis [1]. The second implementation is the original implementation provided by van den Bergh et al.:

    # SEEDS Revised:
    $ ./bin/cli --help
    Allowed options:
      --help                          produce help message
      --input arg                     the folder to process, may contain several 
                                      images
      --bins arg (=5)                 number of bins used for color histograms
      --neighborhood arg (=1)         neighborhood size used for smoothing prior
      --confidence arg (=0.100000001) minimum confidence used for block update
      --iterations arg (=2)           iterations at each level
      --spatial-weight arg (=0.25)    spatial weight
      --superpixels arg (=400)        desired number of supüerpixels
      --verbose                       show additional information while processing
      --csv                           save segmentation as CSV file
      --contour                       save contour image of segmentation
      --labels                        save label image of segmentation
      --mean                          save mean colored image of segmentation
      --output arg (=output)          specify the output directory (default is 
                                      ./output)
    # Original SEEDS:
    $ ./bin/seeds_cli --help
    Allowed options:
      --help                 produce help message
      --input arg            the folder to process (can also be passed as 
                             positional argument)
      --bins arg (=5)        number of bins
      --iterations arg (=2)  iterations at each level
      --bsd arg              number of superpixels for BSDS500
      --nyucropped arg       number of superpixels for the cropped NYU Depth V2
      --nyuhalf arg          number of superpixel for NYU Depth V2 halfed
      --nyuhalfcropped arg   number of superpixels for the cropped NYU Depth V2 
                             halfed
      --time arg             time the algorithm and save results to the given 
                             directory
      --process              show additional information while processing
      --csv                  save segmentation as CSV file
      --contour              save contour image of segmentation
      --mean                 save mean colored image of segmentation
      --output arg (=output) specify the output directory (default is ./output)

## License

For the license of a specific library, see the `README.md` in the corresponding folder (e.g. `lib_crs/README.md`).

All command line tools are licensed under the BSD 3-Clause License:

Copyright (c) 2014 - 2016, David Stutz
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
