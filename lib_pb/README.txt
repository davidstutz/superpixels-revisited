###################################################################
#                                                                 #
#         Superpixels via Pseudo-Boolean Optimization             #
#                                                                 #
#                            v2.0                                 #
#                                                                 #
#           http://yuhang.rsise.anu.edu.au/yuhang/                #
#                                                                 #
#            Yuhang Zhang(yuhang.zhang@anu.edu.au)                #
#            Research School of Computer Science                  #
#            The Australian National University                   #
#                                                                 #
###################################################################

1. This code can only be used for none commercial purpose. 

2. If you are using it for research, please cite paper [1] below. Depends on the optimization algorithm you choose, you might cite [2] or [3] as well.

3. CMakeLists.txt files have been provided to simplify the compiling. Users unfamiliar with CMake are recommended to read the step-by-step.txt file. 

4. To compile the code properly, a third party library named VXL is needed. Please download it from http://sourceforge.net/projects/vxl/ and unzip the downloaded file under the given directory SuperPB. In this case the unzipped files should be delivered into the empty folder "vxl-1.14.0". Then please build VXL under "vxl-1.14.0/bin". This will keep the consistency with the CMakeLists.txt. If you prefer to install VXL at another place or are using a version different from 1.14.0, please modify the corresponding lines in CMakeLists.txt
	
5. The executable file can be launched with the following command:

>SuperPB.exe imagename hsize vsize sigma algorithm

imagename: 	the input image, e.g. test.jpg
hsize:		the width of the initial vertical strips, controlling the width of superpixels, e.g. 20
vsize:		the height of the initial horizontal strips, controlling the height of superpixels, e.g. 20
sigma:		balancing the weight between regular shape and accurate edge, e.g. 20
algorithm:	0: maxflow; 1: Elimination, e.g. 1

Elimination runs faster than maxflow. The superpixels produced by the two optimization algorithms are both accurate but usually different.

6. If you have any questions about the code or the algorithm, please write to yuhang.zhang@anu.edu.au.

[1]Superpixels via pseudo-boolean optimization. 
	Y. Zhang, R. Hartley, J. Mashford, and S. Burn.
	In International Conference on Computer Vision, 2011.

[2]Minimizing energy functions on 4-connected lattices using elimination.
	Peter Carr, Richard Hartley
	In International Conference on Computer Vision, 2009.

[3]An Experimental Comparison of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision.
	Yuri Boykov and Vladimir Kolmogorov.
	In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), September 2004
