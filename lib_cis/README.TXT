##############################################################################
#                                                                            #
#    Superpixels - software for superpixel segmentation with graph cuts      #
#                        Version 1.1                                         #
#    http://www.csd.uwo.ca/faculty/olga/code.html                            #
#                                                                            #
#    Copyright 2010 Olga Veksler  <olga@csd.uwo.ca>                          #
#                                                                            #
##############################################################################

C++ requires at least Visual C++ 2005 (VC8) 
To compile, ensure that vlib/include is an additional include directory
vlib imaging library is courtesy Pedro Felzenszwalb

##############################################################################

IMPORTANT:
If you use this software,please site the following in any resulting publication:

[1] Superpixels and Supervoxels in an Energy Optimization Framework, 
    O. Veksler, Y. Boykov, P. Mehrani, in European Conference on Computer Vision (ECCV), 2010.    

[2] Efficient Approximate Energy Minimization via Graph Cuts.
    Y. Boykov, O. Veksler, R.Zabih. IEEE TPAMI, 20(12):1222-1239, Nov 2001.

[3] What Energy Functions can be Minimized via Graph Cuts?
    V. Kolmogorov, R.Zabih. IEEE TPAMI, 26(2):147-159, Feb 2004. 

[4] An Experimental Comparison of Min-Cut/Max-Flow Algorithms for 
    Energy Minimization in Vision. Y. Boykov, V. Kolmogorov. 
    IEEE TPAMI, 26(9):1124-1137, Sep 2004.

##############################################################################

Command Line Usage: 
superpixels input.pgm output.ppm patch_size [NUM_ITERATIONS] [TYPE] [lambda]  [h_edges.pgm v_edges.pgm d_b_edges.pgm d_f_edges.pgm]

Required Parameters:
input.pgm   --  grayscale input image
output.ppm  -- color output image
patch_size  -- largest allowed patch size, i.e. patch_size by patch_size

Optional Parameters:
NUM_ITERATIONS -- number of iterations to run the algorithm for. Default is 2 iterations
TYPE           -- use 0 for compact superpixels and 1 for  constant intensity superpixels   
Lambda         -- makes a difference only for constant intensity superpixels. The larger is lambda, the smoother are 
                  the boundaries and less small superpixels. Recommended value is 50
h_edges        -- horizontal edge weights: h_edges[x+y*width] holds the weight between pixel (x,y) and (x+1,y)
v_edges        -- vertical edge weights: v_edges[x+y*width] holds the weight between pixel (x,y) and (x,y+1)
d_b_edges      -- diagonal backward edge weights: d_b_edges[x+y*width] holds the weight between pixel (x,y) and (x+1,y+1)
d_f_edges      -- diagonal forward edge weights: d_f_edges[x+y*width] holds the weight between pixel (x,y) and (x-1,y+1)

In optimization, if the optional all edge weights are provided (h_edges,v_edges,d_b_edges,d_f_edges), they also
get multiplied by lambda.

##############################################################################

License & disclaimer.

    Copyright 2010 Olga Veksler  <olga@csd.uwo.ca>

    This software and its modifications can be used and distributed for 
    research purposes only. Publications resulting from use of this code
    must cite publications according to the rules given above. Only
    Olga Veksler has the right to redistribute this code, unless expressed
    permission is given otherwise. Commercial use of this code, any of 
    its parts, or its modifications is not permited. The copyright notices 
    must not be removed in case of any modifications. This Licence 
    commences on the date it is electronically or physically delivered 
    to you and continues in effect unless you fail to comply with any of 
    the terms of the License and fail to cure such breach within 30 days 
    of becoming aware of the breach, in which case the Licence automatically 
    terminates. This Licence is governed by the laws of Canada and all 
    disputes arising from or relating to this Licence must be brought 
    in Toronto, Ontario.


    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

---------------------------------------------------------------------------
