/**
 * Command line tool for Pseudo-Boolean Superpixels [1]:
 * 
 * [1] Superpixels via pseudo-boolean optimization. 
 *     Y. Zhang, R. Hartley, J. Mashford, and S. Burn.
 *     In International Conference on Computer Vision, 2011.
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
 * $ ./bin/pb_cli --help
 * Allowed options:
 *   --help                 produce help message
 *   --input arg            the folder to process (can also be passed as 
 *                          positional argument)
 *   --height arg (=10)     height of initial vertical strips
 *   --width arg (=10)      width of initial vertical strips
 *   --sigma arg (=20)      balancing the weight between regular shape and 
 *                          accurate edge
 *   --max-flow             use max flow algorithm instead of elimination
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
#include "MaxFlow/graph.h"
#include "MaxFlow/QPBO_MaxFlow.h"
#include "Elimination/Elimination.h"
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

struct scost {
    float h00;
    float h01;
    float h10;
    float h11;
    float v00;
    float v01;
    float v10;
    float v11;
};

int myfloor(float t) {
    return (int) t > t ? (int) t - 1 : (int) t;
}

void smoothcost(cv::Mat &im, int i1, int i2, int j1, int j2, int hsize, int vsize, float sigma, scost *sc) {
    float temp;
    float colordiff = 0;

    temp = im.at<cv::Vec3b>(j1,i1)[0];
    temp -= im.at<cv::Vec3b>(j2,i2)[0];
    colordiff += fabs(temp);

    temp = im.at<cv::Vec3b>(j1,i1)[1];
    temp -= im.at<cv::Vec3b>(j2,i2)[1];
    colordiff += fabs(temp);

    temp = im.at<cv::Vec3b>(j1,i1)[2];
    temp -= im.at<cv::Vec3b>(j2,i2)[2];
    colordiff += fabs(temp);

    colordiff = exp(-colordiff/sigma);

    sc->h01 = colordiff;
    sc->h10 = colordiff;
    sc->v01 = colordiff;
    sc->v10 = colordiff;

//    float tau = 0.001;
    
    if(myfloor((float) i1/hsize)!=myfloor((float) i2/hsize)) {
        sc->h00 = colordiff;
    }
    else {
        sc->h00 = 0;
        
//        if (colordiff < tau) {
//            sc->h00 = 1 - colordiff;
//        }
    }

    if(myfloor((float) (i1 - hsize/2)/hsize) != myfloor((float) (i2 - hsize/2)/hsize)) {
        sc->h11 = colordiff;
    }
    else {
        sc->h11 = 0;
        
//        if (colordiff < tau) {
//            sc->h11 = 1 - colordiff; 
//        }
    }


    if(myfloor((float) j1/vsize)!=myfloor((float) j2/vsize)) {
        sc->v00 = colordiff;
    }
    else {
        sc->v00 = 0;
        
//        if (colordiff < tau) {
//            sc->v00 = 1 - colordiff;
//        }
    }

    if(myfloor((float) (j1 - vsize/2)/vsize) != myfloor((float) (j2 - vsize/2)/vsize)) {
        sc->v11 = colordiff;
    }
    else {
        sc->v11 = 0;
        
//        if (colordiff < tau) {
//            sc->v11 = 1 - colordiff;
//        }
    }
}

int main(int argc, const char** argv) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("input", boost::program_options::value<std::string>(), "the folder to process (can also be passed as positional argument)")
        ("height", boost::program_options::value<int>()->default_value(10), "height of initial vertical strips")
        ("width", boost::program_options::value<int>()->default_value(10), "width of initial vertical strips")
        ("sigma", boost::program_options::value<float>()->default_value(20), "balancing the weight between regular shape and accurate edge")
        ("max-flow", "use max flow algorithm instead of elimination")
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

    int stripHeight = parameters["height"].as<int>()*2;
    int stripWidth = parameters["width"].as<int>()*2;
    float sigma = parameters["sigma"].as<float>();
    
    cv::Mat time(images.size(), 2, cv::DataType<double>::type);
    for(std::vector<boost::filesystem::path>::iterator iterator = images.begin(); iterator != images.end(); ++iterator) {
        cv::Mat image = cv::imread(iterator->string());

        int width = image.cols;
        int height = image.rows;

        Matrix<float> U1(height, width), U2(height, width); //to hold data cost
        Matrix<float> Bh1(height, width), Bh2(height, width); //horizontal smooth cost
        Matrix<float> Bv1(height, width), Bv2(height, width); //vertical smooth cost
        Matrix<unsigned char> solution1(height, width), solution2(height, width);

        struct scost *sc = (struct scost*)malloc(sizeof(struct scost));

        U1.fill(0);
        Bh1.fill(0);
        Bv1.fill(0);
        U2.fill(0);
        Bh2.fill(0);
        Bv2.fill(0);

        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(i<width-1) {
                    smoothcost(image, i, i + 1, j, j, stripWidth, stripHeight, sigma, sc);
                    Bh1[j][i]   = sc->h00-sc->h01-sc->h10+sc->h11;
                    U1[j][i]   += sc->h10-sc->h00;
                    U1[j][i+1] += sc->h01-sc->h00;
                    Bh2[j][i]   = sc->v00-sc->v01-sc->v10+sc->v11;
                    U2[j][i]   += sc->v10-sc->v00;
                    U2[j][i+1] += sc->v01-sc->v00;
                }

                if(j<height-1) {
                    smoothcost(image, i, i, j, j + 1, stripWidth, stripHeight, sigma, sc);

                    Bv1[j][i]   = sc->h00-sc->h01-sc->h10+sc->h11;
                    U1[j][i]   += sc->h10-sc->h00;
                    U1[j+1][i] += sc->h01-sc->h00;
                    Bv2[j][i]   = sc->v00-sc->v01-sc->v10+sc->v11;
                    U2[j][i]   += sc->v10-sc->v00;
                    U2[j+1][i] += sc->v01-sc->v00;
                }
            }
        }

        timer.restart();
        int index = std::distance(images.begin(), iterator);
        
        if(parameters.find("max-flow") != parameters.end()) {
            MaxFlowQPBO solve1(U1,Bh1,Bv1,solution1);
            MaxFlowQPBO solve2(U2,Bh2,Bv2,solution2);
        }
        else {
            Elimination< float >::solve(U1, Bh1, Bv1, solution1);
            Elimination< float >::solve(U2, Bh2, Bv2, solution2);
        }
        
        time.at<double>(index, 1) = timer.elapsed();
        time.at<double>(index, 0) = index + 1;
        totalTime += time.at<double>(index, 1);
        
        cv::Mat imh(height, width, CV_16UC1);
        cv::Mat imv(height, width, CV_16UC1);

        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(solution1[j][i] == 0) {
                    imh.at<unsigned short>(j,i) = myfloor((float) i/stripWidth)*2;
                }
                else {
                    imh.at<unsigned short>(j,i) = myfloor((float) (i + stripWidth/2)/stripWidth)*2 + 1;
                }

                if(solution2[j][i] == 0) {
                    imv.at<unsigned short>(j,i) = myfloor((float) j/stripHeight)*2;
                }
                else {
                    imv.at<unsigned short>(j,i) = myfloor((float) (j + stripHeight/2)/stripHeight)*2 + 1;
                }
            }
        }

        int** labels = new int*[height];
        for (int i = 0; i < height; ++i) {
            labels[i] = new int[width];

            for (int j = 0; j < width; ++j) {
                labels[i][j] = imh.at<unsigned short>(i, j)*width + imv.at<unsigned short>(i, j);
            }
        }

        Integrity::relabel(labels, height, width);

        if (process == true) {
            std::cout << Integrity::countSuperpixels(labels, image.rows, image.cols) << " superpixels for " << iterator->string() << " ..." << std::endl;
        }

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