# Superpixels Revisited

This library combines several state-of-the-art superpixel algorithms in a single library. For each approach, a user-friendly command line tool is provided - these command line tools were used for evaluation in [1]. An overview over all superpixel approaches is provided below.

    [1] D. Stutz, A. Hermans, B. Leibe.
        Superpixel Segmentation using Depth Information.
        Bachelor thesis, RWTH Aachen University, Aachen, Germany, 2014.

An overview over all superpixel algorithms and their evaluation results can be found online at [2]:

    [2] http://davidstutz.de/projects/superpixelsseeds/

Provided superpixels algorithms:

* FH - Felzenswalb & Huttenlocher [3];
* SLIC - Simple Linear Iterative Clustering [4];
* CIS/CS - Constant Intensity Superpixels/Compact Superpixels [5];
* ERS - Entropy Rate Superpixels [6];
* PB - Pseudo Boolean Superpixels [7];
* CRS - Contour Relaxed Superpixels [8];
* SEEDS - Superpixels Extracted via Energy-Driven Sampling [9].

Note that the library of CIS/CS is, due to license restrictions, not directly included. See `lib_cis/README.md` for details.

Further, note that for SLIC, both the original implementation as well as the implementation as part of the VLFeat library [10] is provided. Similarly, for SEEDS the original implementation and SEEDS Revised - an implementation written during the bachelor thesis [2] - is provided.

    [3] P. F. Felzenswalb, D. P. Huttenlocher.
        Efficient graph-based image segmentation.
        International Journal of Computer Vision, 59(2), 2004.
    [4] R. Achanta, A. Shaji, K. Smith, A. Lucchi, P. Fua, S. Süsstrunk.
        SLIC superpixels.
        Technical report, École Polytechnique Fédérale de Lausanne, 2010.
    [5] O. Veksler, Y. Boykov, P. Mehrani.
        Superpixels and supervoxels in an energy optimization framework.
        European Conference on Computer Vision, pages 211–224, 2010.
    [6] M. Y. Lui, O. Tuzel, S. Ramalingam, R. Chellappa.
        Entropy rate superpixel segmentation.
        Conference on Computer Vision and Pattern Recognition, pages 2097–2104, 2011.
    [7] Superpixels via pseudo-boolean optimization. 
        Y. Zhang, R. Hartley, J. Mashford, and S. Burn.
        In International Conference on Computer Vision, 2011.
    [8] C. Conrad, M. Mertz, R. Mester.
        Contour-relaxed superpixels.
        Energy Minimization Methods in Computer Vision and Pattern Recognition,
        volume 8081 of Lecture Notes in Computer Science, pages 280–293, 2013.
    [9] M. van den Bergh, X. Boix, G. Roig, B. de Capitani, L. van Gool.
        SEEDS: Superpixels extracted via energy-driven sampling.
        European Conference on Computer Vision, pages 13–26, 2012.

## Building

**Work in progress.**

The library can be built using CMake (`sudo apt-get install cmake`):

    cd superpixels-revisited
    cd build
    cmake ..
    make

For building CIS/CS [5] you need to download the corresponding library first, see `lib_cis/README.md`.

## Usage

**Note:** Usage details can also be found in the corresponding `main.cpp` files (e.g. `crs_cli/main.cpp` or `lib_seeds_revised/cli/main.cpp`).

**Work in progress.**

## License

For the license of a specific library, see the `README.md` in the corresponding folder (e.g. `lib_crs/README.md`).

All command line tools are licensed under the BSD 3-Clause License:

Copyright (c) 2014 - 2015, David Stutz
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.