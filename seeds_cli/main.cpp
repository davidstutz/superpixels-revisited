/**
 * Command line tool for SEEDS [1]:
 * 
 * [1] M. van den Bergh, X. Boix, G. Roig, B. de Capitani, L. van Gool.
 *     SEEDS: Superpixels extracted via energy-driven sampling.
 *     European Conference on Computer Vision, pages 13–26, 2012.
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
 * $ ./bin/seeds_cli --help
 * Allowed options:
 *   --help                 produce help message
 *   --input arg            the folder to process (can also be passed as 
 *                          positional argument)
 *   --bins arg (=5)        number of bins
 *   --iterations arg (=2)  iterations at each level
 *   --bsd arg              number of superpixels for BSDS500
 *   --nyucropped arg       number of superpixels for the cropped NYU Depth V2
 *   --nyuhalf arg          number of superpixel for NYU Depth V2 halfed
 *   --nyuhalfcropped arg   number of superpixels for the cropped NYU Depth V2 
 *                          halfed
 *   --time arg             time the algorithm and save results to the given 
 *                          directory
 *   --process              show additional information while processing
 *   --csv                  save segmentation as CSV file
 *   --contour              save contour image of segmentation
 *   --mean                 save mean colored image of segmentation
 *   --output arg (=output) specify the output directory (default is ./output)
 * 
 * The code (this command line tool) is published under the BSD 3-Clause:
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
#include "seeds2.h"
#include "Tools.h"
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
        ("input", boost::program_options::value<std::string>(), "the folder to process (can also be passed as positional argument)")
        ("bins", boost::program_options::value<int>()->default_value(5), "number of bins")
        ("iterations", boost::program_options::value<int>()->default_value(2), "iterations at each level")
        ("bsd", boost::program_options::value<int>(), "number of superpixels for BSDS500")
        ("nyucropped", boost::program_options::value<int>(), "number of superpixels for the cropped NYU Depth V2")
        ("nyuhalf", boost::program_options::value<int>(), "number of superpixel for NYU Depth V2 halfed")
        ("nyuhalfcropped", boost::program_options::value<int>(), "number of superpixels for the cropped NYU Depth V2 halfed")
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
    
    int iterations = parameters["iterations"].as<int>();
    int bins = parameters["bins"].as<int>();
    
    boost::timer timer;
    double totalTime = 0;
    
    cv::Mat time(images.size(), 2, cv::DataType<double>::type);
    for(std::vector<boost::filesystem::path>::iterator iterator = images.begin(); iterator != images.end(); ++iterator) {
        cv::Mat image = cv::imread(iterator->string());

        int levels = 4;
        int width = 3;
        int height = 4;

        if (parameters.find("bsd") != parameters.end()) {
            int bsd = parameters["bsd"].as<int>();

            if (image.cols >= image.rows) {
                switch (bsd) {
                    case 1040:
                        levels = 3;
                        width = 3;
                        height = 3;
                        break;
                    case 800:
                        levels = 3;
                        width = 3;
                        height = 4;
                        break;
                    case 600:
                        levels = 4;
                        width = 2;
                        height = 2;
                        break;
                    case 400:
                        levels = 4;
                        width = 3;
                        height = 2;
                        break;
                    case 260:
                        levels = 4;
                        width = 3;
                        height = 3;
                        break;
                    case 200:
                        levels = 4;
                        width = 3;
                        height = 4;
                        break;
                    case 150:
                        levels = 5;
                        width = 2;
                        height = 2;
                        break;
                    case 100:
                        levels = 5;
                        width = 3;
                        height = 2;
                        break;
                    case 50:
                        levels = 5;
                        width = 3;
                        height = 4;
                        break;
                    case 25:
                        levels = 6;
                        width = 3;
                        height = 2;
                        break;
                    case 17:
                        levels = 6;
                        width = 3;
                        height = 3;
                        break;
                    case 12:
                        levels = 6;
                        width = 3;
                        height = 4;
                        break;
                    case 9:
                        levels = 7;
                        width = 2;
                        height = 2;
                        break;
                    case 6:
                        levels = 7;
                        width = 3;
                        height = 2;
                        break;
                }
            }
            else {
                switch (bsd) {
                    case 1040:
                        levels = 3;
                        width = 3;
                        height = 3;
                        break;
                    case 800:
                        levels = 3;
                        width = 4;
                        height = 3;
                        break;
                    case 600:
                        levels = 4;
                        width = 2;
                        height = 2;
                        break;
                    case 400:
                        levels = 4;
                        width = 2;
                        height = 3;
                        break;
                    case 260:
                        levels = 4;
                        width = 3;
                        height = 3;
                        break;
                    case 200:
                        levels = 4;
                        width = 4;
                        height = 3;
                        break;
                    case 150:
                        levels = 5;
                        width = 2;
                        height = 2;
                        break;
                    case 100:
                        levels = 5;
                        width = 2;
                        height = 3;
                        break;
                    case 50:
                        levels = 5;
                        width = 4;
                        height = 3;
                        break;
                    case 25:
                        levels = 6;
                        width = 2;
                        height = 3;
                        break;
                    case 17:
                        levels = 6;
                        width = 3;
                        height = 3;
                        break;
                    case 12:
                        levels = 6;
                        width = 4;
                        height = 3;
                        break;
                    case 9:
                        levels = 7;
                        width = 2;
                        height = 2;
                        break;
                    case 6:
                        levels = 7;
                        width = 2;
                        height = 3;
                        break;
                }
            }
        }

        if (parameters.find("nyucropped") != parameters.end()) {
            int nyu = parameters["nyucropped"].as<int>();

            switch (nyu) {
                case 1850:
                    levels = 3;
                    width = 3;
                    height = 3;
                    break;
                case 1400:
                    levels = 3;
                    width = 3;
                    height = 4;
                    break;
                case 1100:
                    levels = 3;
                    width = 3;
                    height = 5;
                    break;
                case 840:
                    levels = 3;
                    width = 5;
                    height = 4;
                    break;
                case 700:
                    levels = 4;
                    width = 3;
                    height = 2;
                    break;
                case 660:
                    levels = 3;
                    width = 5;
                    height = 5;
                    break;
                case 450:
                    levels = 4;
                    width = 3;
                    height = 3;
                    break;
                case 350:
                    levels = 4;
                    width = 4;
                    height = 4;
                    break;
                case 275:
                    levels = 4;
                    width = 3;
                    height = 5;
                    break;
                case 266:
                    levels = 5;
                    width = 2;
                    height = 2;
                    break;
                case 210:
                    levels = 4;
                    width = 5;
                    height = 4;
                    break;
                case 165:
                    levels = 4;
                    width = 5;
                    height = 5;
                    break;
                case 108:
                    levels = 5;
                    width = 3;
                    height = 3;
                    break;
                case 84:
                    levels = 5;
                    width = 3;
                    height = 4;
                    break;
                case 60:
                    levels = 5;
                    width = 3;
                    height = 5;
                    break;
                case 42:
                    levels = 6;
                    width = 3;
                    height = 2;
                    break;
                case 24:
                    levels = 6;
                    width = 3;
                    height = 3;
                    break;
                case 18:
                    levels = 6;
                    width = 3;
                    height = 4;
                    break;
                case 16:
                    levels = 6;
                    width = 4;
                    height = 3;
                    break;
                case 12:
                    levels = 6;
                    width = 4;
                    height = 4;
                    break;
                case 8:
                    levels = 6;
                    width = 4;
                    height = 5;
                    break;
                case 6:
                    levels = 6;
                    width = 5;
                    height = 5;
                    break;
            }
        }
        
        // If we are running on NYU half dataset choose levels and minimum block
        // size automatically.
        if (parameters.find("nyuhalf") != parameters.end()) {
            int nyu = parameters["nyuhalf"].as<int>();

            switch (nyu) {
                case 780:
                    levels = 3;
                    width = 3;
                    height = 2;
                    break;
                case 520:
                    levels = 3;
                    width = 3;
                    height = 3;
                    break;
                case 400:
                    levels = 3;
                    width = 4;
                    height = 3;
                    break;
                case 300:
                    levels = 3;
                    width = 4;
                    height = 4;
                    break;
                case 200:
                    levels = 4;
                    width = 2;
                    height = 3;
                    break;
                case 195:
                    levels = 4;
                    width = 3;
                    height = 2;
                    break;
                case 130:
                    levels = 4;
                    width = 3;
                    height = 3;
                    break;
                case 100:
                    levels = 4;
                    width = 4;
                    height = 3;
                    break;
                case 70:
                    levels = 4;
                    width = 4;
                    height = 4;
                    break;
                case 50:
                    levels = 5;
                    width = 2;
                    height = 3;
                    break;
                case 45:
                    levels = 5;
                    width = 3;
                    height = 2;
                    break;
                case 30:
                    levels = 5;
                    width = 3;
                    height = 3;
                    break;
                case 25:
                    levels = 5;
                    width = 4;
                    height = 3;
                    break;
                case 15:
                    levels = 5;
                    width = 4;
                    height = 4;
                    break;
                case 10:
                    levels = 6;
                    width = 2;
                    height = 3;
                    break;
                case 6:
                    levels = 6;
                    width = 3;
                    height = 3;
                    break;
                case 4:
                    levels = 6;
                    width = 4;
                    height = 3;
                    break;
            }
        }

        if (parameters.find("nyuhalfcropped") != parameters.end()) {
            int nyu = parameters["nyuhalfcropped"].as<int>();

            switch (nyu) {
                case 700:
                    levels = 3;
                    width = 3;
                    height = 2;
                    break;
                case 450:
                    levels = 3;
                    width = 3;
                    height = 3;
                    break;
                case 350:
                    levels = 3;
                    width = 3;
                    height = 4;
                    break;
                case 266:
                    levels = 3;
                    width = 4;
                    height = 4;
                    break;
                case 168:
                    levels = 4;
                    width = 3;
                    height = 2;
                    break;
                case 108:
                    levels = 4;
                    width = 3;
                    height = 3;
                    break;
                case 84:
                    levels = 4;
                    width = 3;
                    height = 4;
                    break;
                case 63:
                    levels = 4;
                    width = 4;
                    height = 4;
                    break;
                case 42:
                    levels = 5;
                    width = 3;
                    height = 2;
                    break;
                case 24:
                    levels = 5;
                    width = 3;
                    height = 3;
                    break;
                case 18:
                    levels = 5;
                    width = 3;
                    height = 4;
                    break;
                case 16:
                    levels = 5;
                    width = 4;
                    height = 3;
                    break;
                case 12:
                    levels = 5;
                    width = 4;
                    height = 4;
                    break;
                case 8:
                    levels = 6;
                    width = 2;
                    height = 3;
                    break;
                case 6:
                    levels = 6;
                    width = 3;
                    height = 3;
                    break;
            }
        }

        SEEDS seeds(image.cols, image.rows, image.channels(), bins, 0);

        timer.restart();
        int index = std::distance(images.begin(), iterator);
        
        seeds.initialize(image, width, height, levels);
        seeds.iterate(iterations);
        
        time.at<double>(index, 1) = timer.elapsed();
        time.at<double>(index, 0) = index + 1;
        totalTime += time.at<double>(index, 1);

        // Convert labels.
        int** labels = new int*[image.rows];
        for (int i = 0; i < image.rows; ++i) {
            labels[i] = new int[image.cols];

            for (int j = 0; j < image.cols; ++j) {
                labels[i][j] = seeds.labels[levels - 1][j + image.cols*i];
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
        
        for (int i = 0; i < image.rows; ++i) {
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