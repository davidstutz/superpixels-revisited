/**
 * Command line tool for Contour Relaxed Superpixels [1]:
 * 
 * [1] C. Conrad, M. Mertz, R. Mester.
 *     Contour-relaxed superpixels.
 *     Energy Minimization Methods in Computer Vision and Pattern Recognition,
 *     volume 8081 of Lecture Notes in Computer Science, pages 280–293, 2013.
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
 *  $ ./bin/crs_cli --help
 *  Allowed options:
 *   --help                                produce help message
 *   --input arg                           the folder to process
 *   --width arg (=20)                     width of blocks in initial superpixel 
 *                                         segmentation
 *   --height arg (=20)                    height of blocks in initial superpixel 
 *                                         segmentation
 *   --compactness arg (=0.044999999999999998)
 *                                         compactness weight
 *   --clique-cost arg (=0.29999999999999999)
 *                                         direct clique cost
 *   --iterations arg (=3)                 number of iterations to perform
 *   --time arg                            time the algorithm and save results to 
 *                                         the given directory
 *   --process                             show additional information while 
 *                                         processing
 *   --csv                                 save segmentation as CSV file
 *   --contour                             save contour image of segmentation
 *   --mean                                save mean colored image of segmentation
 *   --output arg (=output)                specify the output directory (default 
 *                                         is ./output)
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
#include "Tools.h"
#include "SeedsRevised.h"
#include "FeatureType.h"
#include "ContourRelaxation.h"
#include "InitializationFunctions.h"
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#if defined(WIN32) || defined(_WIN32)
    #define DIRECTORY_SEPARATOR "\\"
#else
    #define DIRECTORY_SEPARATOR "/"
#endif

int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("input", boost::program_options::value<std::string>(), "the folder to process")
        ("width", boost::program_options::value<int>()->default_value(20), "width of blocks in initial superpixel segmentation")
        ("height", boost::program_options::value<int>()->default_value(20), "height of blocks in initial superpixel segmentation")
        ("compactness", boost::program_options::value<double>()->default_value(0.045), "compactness weight")
        ("clique-cost", boost::program_options::value<double>()->default_value(0.3),  "direct clique cost")
        ("iterations", boost::program_options::value<int>()->default_value(3), "number of iterations to perform")
        ("time", boost::program_options::value<std::string>(), "time the algorithm and save results to the given directory")
        ("process", "show additional information while processing")
        ("csv", "save segmentation as CSV file")
        ("contour", "save contour image of segmentation")
        ("mean", "save mean colored image of segmentation")
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
    
    boost::filesystem::path outputDir(parameters["output"].as<std::string>());
    if (!boost::filesystem::is_directory(outputDir)) {
        boost::filesystem::create_directory(outputDir);
    }
    
    boost::filesystem::path inputDir(parameters["input"].as<std::string>());
    if (!boost::filesystem::is_directory(inputDir)) {
        std::cout << "Input directory not found ..." << std::endl;
        return 1;
    }
    
    bool process = false;
    if (parameters.find("process") != parameters.end()) {
        process = true;
    }
    
    std::vector<boost::filesystem::path> pathVector;
    std::vector<boost::filesystem::path> images;
    
    std::copy(boost::filesystem::directory_iterator(inputDir), boost::filesystem::directory_iterator(), std::back_inserter(pathVector));

    std::sort(pathVector.begin(), pathVector.end());
    
    std::string extension;
    int count = 0;
    
    for (std::vector<boost::filesystem::path>::const_iterator iterator (pathVector.begin()); iterator != pathVector.end(); ++iterator) {
        if (boost::filesystem::is_regular_file(*iterator)) {
            
            // Check supported file extensions.
            extension = iterator->extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
                images.push_back(*iterator);
                
                if (process == true) {
                    std::cout << "Found " << iterator->string() << " ..." << std::endl;
                }
                
                ++count;
            }
        }
    }
    
    std::cout << count << " images total ..." << std::endl;
    
    boost::timer timer;
    double totalTime = 0;
        
    int width = parameters["width"].as<int>();
    int height = parameters["height"].as<int>();
    double directCost = parameters["clique-cost"].as<double>();
    double diagonalCost = directCost/sqrt(2);
    double compactness = parameters["compactness"].as<double>();
    int iterations = parameters["iterations"].as<int>();
    
    cv::Mat time(images.size(), 2, cv::DataType<double>::type);
    for(std::vector<boost::filesystem::path>::iterator iterator = images.begin(); iterator != images.end(); ++iterator) {
        cv::Mat image = cv::imread(iterator->string());

        bool isColorImage = false;
        if (image.channels() == 3) {
            isColorImage = true;
        }
        
        std::vector<FeatureType> enabledFeatures;
        if (isColorImage) {
            enabledFeatures.push_back(Color);
        }
        else {
            enabledFeatures.push_back(Grayvalue);
        }

        enabledFeatures.push_back(Compactness);
        
        ContourRelaxation<boost::uint16_t> contourRelaxationInst(enabledFeatures);
        contourRelaxationInst.setCompactnessData(compactness);
        
        cv::Mat labelImage = createBlockInitialization<boost::uint16_t>(image.size(), width, height);
        
        if (isColorImage) {
            cv::Mat imageYCrCb;
            cv::cvtColor(image, imageYCrCb, SEEDS_REVISED_OPENCV_BGR2YCrCb);
            std::vector<cv::Mat> imageYCrCbChannels;
            cv::split(imageYCrCb, imageYCrCbChannels);

            contourRelaxationInst.setColorData(imageYCrCbChannels[0], imageYCrCbChannels[1], imageYCrCbChannels[2]);
        }
        else {
            cv::Mat imageGray = image.clone();
            cv::cvtColor(imageGray, image, SEEDS_REVISED_OPENCV_GRAY2BGR);

            contourRelaxationInst.setGrayvalueData(imageGray);
        }

        cv::Mat relaxedLabelImage;
        cv::Mat regionMeanImage;
        
        timer.restart();
        int index = std::distance(images.begin(), iterator);
        
        contourRelaxationInst.relax(labelImage, directCost, diagonalCost, iterations, relaxedLabelImage, regionMeanImage);
        
        time.at<double>(index, 1) = timer.elapsed();
        time.at<double>(index, 0) = index + 1;
        totalTime += time.at<double>(index, 1);
        
        int** labels = new int*[image.rows];
        for (int i = 0; i < image.rows; i++) {
            labels[i] = new int[image.cols];
            
            for (int j = 0; j < image.cols; j++) {
                labels[i][j] = relaxedLabelImage.at<boost::uint16_t>(i, j);
            }
        }
        
        Integrity::relabel(labels, image.rows, image.cols);

        if (parameters.find("contour") != parameters.end()) {

            boost::filesystem::path extension = iterator->filename().extension();
            int position = iterator->filename().string().find(extension.string());
            std::string store = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + "_contours.png";

            int bgr[] = {0, 0, 204};
            cv::Mat contourImage = Draw::contourImage(labels, image, bgr);
            cv::imwrite(store, contourImage);

            if (process == true) {
                std::cout << "Image " << iterator->string() << " with contours saved to " << store << " ..." << std::endl;
            }
        }

        if (parameters.find("mean") != parameters.end()) {

            boost::filesystem::path extension = iterator->extension();
            int position = iterator->filename().string().find(extension.string());
            std::string store = outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + "_mean.png";

            cv::Mat meanImage = Draw::meanImage(labels, image);
            cv::imwrite(store, meanImage);

            if (process == true) {
                std::cout << "Image " << iterator->string() << " with mean colors saved to " << store << " ..." << std::endl;
            }
        }

        if (parameters.find("csv") != parameters.end()) {

            boost::filesystem::path extension = iterator->extension();
            int position = iterator->filename().string().find(extension.string());
            
            boost::filesystem::path csvFile(outputDir.string() + DIRECTORY_SEPARATOR + iterator->filename().string().substr(0, position) + ".csv");
            Export::CSV(labels, image.rows, image.cols, csvFile);

            if (process == true) {
                std::cout << "Labels for image " << iterator->string() << " saved in " << csvFile.string() << " ..." << std::endl;
            }
        }
        
        for (int i = 0; i < image.rows; i++) {
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