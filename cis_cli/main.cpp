/**
 * Command line tool for the superpixel algorithm proposed in [1].
 * 
 * [1] O. Veksler, Y. Boykov, P. Mehrani.
 *     Superpixels and supervoxels in an energy optimization framework.
 *     European Conference on Computer Vision, pages 211–224, 2010.
 * 
 * **Unfortunately, the license of this superpixel algorithm prohibits us from
 * redistributing the code, thereore we only provide the command line interface.**
 * 
 * Installation intructions:
 * 
 * - Got to http://www.csd.uwo.ca/faculty/olga/ and download the code.
 * - Extract the archive into lib_cis to obtain the following folder structure:
 * 
 *  lib_cis
 *  |- vlib
 *    |- include
 *    |- utils
 *    |- CMakeLists.txt (provided by this library)
 *  |- README.txt
 *  |- maxflow.cpp
 *  |- ...
 *  |- CMakeLists.txt (provided by this library)
 * 
 * - Comment out the `main` function in `superpixels.cpp`.
 * - Either change the declaration of loadEdges in superpixels.cpp to
 * 
 *  void loadEdges(vector<Value> &weights,int num_pixels,int width,int height,
 *			   Value lambda, char *name)
 * 
 * or use -fpermissive (default).
 * - Depending on the operating system, someminor changes within the
 * downloaded code will be necessary. In particular, in energy.h change
 * occurences of 
 *  
 *  add_tweights(y, 0, C);
 *  add_edge(x, y, B+C, 0);
 * 
 * to 
 * 
 *  this->add_tweights(y, 0, C);
 *  this->add_edge(x, y, B+C, 0);
 * 
 * The code was used for evaluation purposes in [2]:
 * 
 * [2] D. Stutz, A. Hermans, B. Leibe.
 *     Superpixel Segmentation using Depth Information.
 *     Bachelor thesis, RWTH Aachen University, Aachen, Germany, 2014.
 * 
 * [2] is available online at 
 * 
 *      http://davidstutz.de/bachelor-thesis-superpixel-segmentation-using-depth-information/
 * 
 * **How to use the command line tool?**
 * 
 *  $ ./bin/cis_cli --help
 *  Allowed options:
 *   --help                  produce help message
 *   --input arg             folder containing the images to process
 *   --region-size arg (=10) maxmimum allowed region size (that is region size x 
 *                           region size patches)
 *   --type arg (=1)         0 for compact superpixels, 1 for constant intensity 
 *                           superpixels
 *   --iterations arg (=2)   number of iterations
 *   --lambda arg (=50)      lambda only influences constant intensity 
 *                           superpixels; larger lambda results in smoother 
 *                           boundaries
 *   --process               show additional information while processing
 *   --time arg              time the algorithm and save results to the given 
 *                           directory
 *   --csv                   save segmentation as CSV file
 *   --contour               save contour image of segmentation
 *   --mean                  save mean colored image of segmentation
 *   --time                  save timings in BSD evaluation format in the given 
 *                           directory
 *  --output arg (=output)  specify the output directory (default is ./output)
 * 
 * The code (only concerning this command line tool, not the downloaded code
 * from Olga Veksler!) is published under the BSD 3-Clause:
 * 
 * Copyright (c) 2014, David Stutz
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "SeedsRevised.h"
#include "Tools.h"
#include "superpixels.h"
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/timer.hpp>

#if defined(WIN32) || defined(_WIN32)
    #define DIRECTORY_SEPARATOR "\\"
#else
    #define DIRECTORY_SEPARATOR "/"
#endif

int main (int argc, char ** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("input", boost::program_options::value<std::string>(), "folder containing the images to process")
        ("region-size", boost::program_options::value<int>()->default_value(10), "maxmimum allowed region size (that is region size x region size patches)")
        ("type", boost::program_options::value<int>()->default_value(1), "0 for compact superpixels, 1 for constant intensity superpixels")
        ("iterations", boost::program_options::value<int>()->default_value(2), "number of iterations")
        ("lambda", boost::program_options::value<int>()->default_value(50), "lambda only influences constant intensity superpixels; larger lambda results in smoother boundaries")
        ("process", "show additional information while processing")
        ("time", boost::program_options::value<std::string>(), "time the algorithm and save results to the given directory")
        ("csv", "save segmentation as CSV file")
        ("contour", "save contour image of segmentation")
        ("mean", "save mean colored image of segmentation")
        ("time", "save timings in BSD evaluation format in the given directory")
        ("output", boost::program_options::value<std::string>()->default_value("output"), "specify the output directory (default is ./output)");
    
    boost::program_options::positional_options_description positionals;
    positionals.add("input", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path imageDir(parameters["input"].as<std::string>());
    if (!boost::filesystem::is_directory(imageDir)) {
        std::cout << "Image directory not found ..." << std::endl;
        return 1;
    }
    
    boost::filesystem::path outputDir(parameters["output"].as<std::string>());
    if (!boost::filesystem::is_directory(outputDir)) {
        boost::filesystem::create_directory(outputDir);
    }
    
    bool process = false;
    if (parameters.find("process") != parameters.end()) {
        process = true;
    }
    
    std::vector<boost::filesystem::path> pathVector;
    std::vector<boost::filesystem::path> images;
    
    std::copy(boost::filesystem::directory_iterator(imageDir), boost::filesystem::directory_iterator(), std::back_inserter(pathVector));

    std::sort(pathVector.begin(), pathVector.end());
    
    int count = 0;
    for (std::vector<boost::filesystem::path>::const_iterator iterator (pathVector.begin()); iterator != pathVector.end(); ++iterator) {
        if (boost::filesystem::is_regular_file(*iterator)) {
            
            std::string extension = iterator->extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == ".png" || extension == ".jpg") {
                images.push_back(*iterator);
                
                if (process == true) {
                    std::cout << "Found " << iterator->string() << " ..." << std::endl;
                }
                
                ++count;
            }
        }
    }
    
    std::cout << count << " images found ..." << std::endl;
    
    double totalTime = 0;
    
    int iterations = parameters["iterations"].as<int>();
    int lambda = parameters["lambda"].as<int>();
    int type = parameters["type"].as<int>();
    
    if (type != 0 && type != 1) {
        std::cout << "Type needs to be 0 for compact superpixels or 1 for constant intensity superpixels ...";
        return 1;
    }
    
    int regionSize = parameters["region-size"].as<int>();
    
    boost::timer timer;
    
    cv::Mat time(images.size(), 2, cv::DataType<double>::type);
    for(std::vector<boost::filesystem::path>::iterator iterator = images.begin(); iterator != images.end(); ++iterator) {
        
        cv::Mat mat = cv::imread(iterator->string());
        
        // Convert to PGM.
        boost::filesystem::path extension = iterator->filename().extension();
        int position = iterator->filename().string().find(extension.string());

        cv::Mat matGrayScale;
        cv::cvtColor(mat, matGrayScale, SEEDS_REVISED_OPENCV_BGR2GRAY);
        
        std::string storeGray = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + ".pgm";
        cv::imwrite(storeGray, matGrayScale);

        // Generate edge maps (that is, gradient maps in x and y direction)
        cv::Mat gradX;
        cv::Mat gradY;
        cv::Mat absGradX;
        cv::Mat absGradY;

        cv::GaussianBlur(matGrayScale, matGrayScale, cv::Size(3,3), 0, 0, cv::BORDER_DEFAULT);
        
        cv::Sobel(matGrayScale, gradX, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(gradX, absGradX);

        cv::Sobel(matGrayScale, gradY, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(gradY, absGradY);
        
        std::string storeX = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + "_x.pgm";
        std::string storeY = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + "_y.pgm";
        
        cv::imwrite(storeX, absGradX);
        cv::imwrite(storeY, absGradY);
        
        image<unsigned char> *I = loadPGM(storeGray.c_str());
        assert(I != 0);
        
        int width  = I->width();
        int height = I->height();
        int num_pixels = width*height; 

        timer.restart();
        int index = std::distance(images.begin(), iterator);
        
        float variance = computeImageVariance(I, width, height);

        // Initialize and place seeds
        std::vector<int> Seeds(num_pixels);
        int numSeeds = 0;
        PlaceSeeds(I, width, height, num_pixels, Seeds, &numSeeds, regionSize);
        MoveSeedsFromEdges(I, width, height, num_pixels, Seeds, numSeeds, regionSize);

        std::vector<int> horizWeights(num_pixels,lambda);
        std::vector<int> vertWeights(num_pixels,lambda);
        std::vector<int> diag1Weights(num_pixels,lambda);
        std::vector<int> diag2Weights(num_pixels,lambda);

        loadEdges(horizWeights, num_pixels, width, height, lambda, storeX.c_str());
        loadEdges(vertWeights, num_pixels, width, height, lambda, storeY.c_str());
        // loadEdges(diag1Weights, num_pixels, width, height, lambda, edges[2].c_str());
        // loadEdges(diag2Weights, num_pixels, width, height, lambda, edges[3].c_str());
        computeWeights(horizWeights, num_pixels, width,height, lambda, variance, -1, 0, I, type);
        computeWeights(vertWeights, num_pixels, width,height, lambda, variance, 0, -1, I, type);
        computeWeights(diag1Weights, num_pixels, width,height, lambda, variance, -1, -1, I, type);
        computeWeights(diag2Weights, num_pixels, width,height, lambda, variance, 1, -1, I, type);
        
        vector<int> labeling(num_pixels);

        initializeLabeling(labeling, width, height, Seeds, numSeeds, regionSize);

        int oldEnergy, newEnergy;

        std::vector<int> changeMask(num_pixels, 1);
        std::vector<int> changeMaskNew(num_pixels, 0);

        std::vector<int> order(numSeeds);
        for (int i = 0; i < numSeeds; i++) {
            order[i] = i;
        }
        
        int j = 0;
        //purturbSeeds(order,numSeeds);

        while (true) {
            newEnergy = computeEnergy(labeling, width, height, num_pixels, horizWeights, vertWeights, diag1Weights, diag2Weights, Seeds, I, type);

            if (j == 0) {
                oldEnergy = newEnergy + 1;
            }

            if (newEnergy == oldEnergy || j >= iterations) { 
                break;
            }
            
            oldEnergy = newEnergy;

            for (int i = 0; i < numSeeds; i++) {
                expandOnLabel(order[i], width, height, num_pixels, Seeds, numSeeds, labeling, horizWeights,
                             vertWeights, lambda, diag1Weights, diag2Weights, regionSize, changeMask,
                             changeMaskNew, I, type, variance);
            }
            
            for (int i = 0; i < num_pixels; i++) {
                changeMask[i] = changeMaskNew[i];
                changeMaskNew[i] = 0;
            }

            //purturbSeeds(order,numSeeds);
            j++;
        }

        time.at<double>(index, 1) = timer.elapsed();
        time.at<double>(index, 0) = index + 1;
        totalTime += time.at<double>(index, 1);
        
        delete I;

        int** labels = new int*[height];
        for (int i = 0; i < height; ++i) {
            labels[i] = new int[width];
            
            for (int j = 0; j < width; ++j) {
                labels[i][j] = labeling[j + i*width];
            }
        }
        
        if (parameters.find("csv") != parameters.end()) {

            boost::filesystem::path csvFile(outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + ".csv");
            Export::CSV(labels, height, width, csvFile);

            if (process == true) {
                std::cout << "Labels for image " << iterator->string() << " saved in " << csvFile.string() << " ..." << std::endl;
            }
        }
            
        if (parameters.find("contour") != parameters.end()) {

            std::string store = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + "_contours.png";

            int bgr[] = {0, 0, 204};
            cv::Mat contourImage = Draw::contourImage(labels, mat, bgr);
            cv::imwrite(store, contourImage);

            if (process == true) {
                std::cout << "Image " << iterator->string() << " with contours saved to " << store << " ..." << std::endl;
            }
        }

        if (parameters.find("mean") != parameters.end()) {

            std::string store = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + "_mean.png";

            cv::Mat meanImage = Draw::meanImage(labels, mat);
            cv::imwrite(store, meanImage);

            if (process == true) {
                std::cout << "Image " << iterator->string() << " with mean colors saved to " << store << " ..." << std::endl;
            }
        }
        
        for (int i = 0; i < height; ++i) {
            delete[] labels[i];
        }
        
        delete[] labels;
    }
    
    if (parameters.find("time") != parameters.end()) {
        
        boost::filesystem::path timeDir(parameters["time"].as<std::string>());
        if (!boost::filesystem::is_directory(timeDir)) {
            boost::filesystem::create_directories(timeDir);
        }
        
        boost::filesystem::path timeImgFile(timeDir.string() + DIRECTORY_SEPARATOR + "eval_time_img.txt");
        boost::filesystem::path timeFile(timeDir.string() + DIRECTORY_SEPARATOR + "eval_time.txt");
        
        Export::BSDEvaluationFile<double>(time, 4, timeImgFile);
        
        cv::Mat avgTime(1, 1, cv::DataType<double>::type);
        avgTime.at<double>(0, 0) = totalTime/((double) images.size());
        Export::BSDEvaluationFile<double>(avgTime, 6, timeFile);
    }
    
    std::cout << "On average, " << totalTime/images.size() << " seconds needed ..." << std::endl;
    
    return 0;
}
