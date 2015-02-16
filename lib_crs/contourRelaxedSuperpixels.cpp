// Copyright 2013 Visual Sensorics and Information Processing Lab, Goethe University, Frankfurt
//
// This file is part of Contour-relaxed Superpixels.
//
// Contour-relaxed Superpixels is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Contour-relaxed Superpixels is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Contour-relaxed Superpixels.  If not, see <http://www.gnu.org/licenses/>.


/**
 * @file contourRelaxedSuperpixels.cpp
 * @brief Main file for creating superpixels with Contour Relaxation.
 */

#include <iostream>
#include <cstdlib>
#include <boost/version.hpp>
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "SeedsRevised.h"

#include "contourRelaxation/FeatureType.h"
#include "contourRelaxation/ContourRelaxation.h"
#include "contourRelaxation/InitializationFunctions.h"
#include "VisualizationHelper.h"


/**
 * @brief Main function. Reads input image, creates superpixels and saves results and some visualizations.
 * @param argc number of command line arguments
 * @param argv values of command line arguments
 * @return 0 on success, some positive number on error
 *
 * The main function takes the following arguments, in this order:
 *
 * inputImage: absolute or relative path to the input image file (1 (gray) or 3 (color) channels, must be readable by OpenCV), mandatory
 *
 * blockWidth: width of the rectangular blocks that are used to initialize the label image, optional, default value = 20
 *
 * blockHeight: height of the rectangular blocks that are used to initialize the label image, optional, default value = 20
 *
 * directCliqueCost: Markov clique cost for horizontal and vertical cliques, optional, default value = 0.3
 *
 * compactnessWeight: weight for the compactness feature, optional, default value = 0.045 for color images, 0.015 for grayscale images
 *
 * Results will be saved in the current working directory. The output filenames will be constructed from the input filename
 * and the type of visualization the file contains.
 */
int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 6)
    {
        std::cout << "Usage: " << argv[0] << " inputImage [blockWidth = 20] [blockHeight = 20] [directCliqueCost = 0.3] [compactnessWeight = 0.045 (color) / 0.015 (grayscale)]" << std::endl;

        return 1;
    }

    boost::filesystem::path inputImage(argv[1]);

    if (!boost::filesystem::exists(inputImage))
    {
        std::cout << "Input image does not exist. Aborting." << std::endl;

        return 1;
    }

    // Load image and detect type.
    cv::Mat image = cv::imread(inputImage.string(), -1);
    bool isColorImage = false;

    if (image.channels() == 3)
    {
        isColorImage = true;
    }
    else if (image.channels() != 1)
    {
        std::cout << "Input image must have 1 (gray) or 3 (color) channels, format not recognized. Aborting." << std::endl;

        return 1;
    }

    // Set desired features.
    // The set of features enabled here must also be provided with data below!
    std::vector<FeatureType> enabledFeatures;

    if (isColorImage)
    {
        enabledFeatures.push_back(Color);
    }
    else
    {
        enabledFeatures.push_back(Grayvalue);
    }

    enabledFeatures.push_back(Compactness);

    // Set some parameters for Contour Relaxation.
    int const blockWidth = (argc > 2) ? atoi(argv[2]) : 20;
    int const blockHeight = (argc > 3) ? atoi(argv[3]) : 20;
    if (blockWidth < 1 || blockHeight < 1)
    {
        std::cout << "blockWidth and blockHeight must be positive. Aborting." << std::endl;

        return 1;
    }

    double const directCliqueCost = (argc > 4) ? atof(argv[4]) : 0.3;
    if (directCliqueCost < 0)
    {
        std::cout << "directCliqueCost must be non-negative. Aborting." << std::endl;

        return 1;
    }

    // Diagonal clique cost is set relative to the direct clique cost, weighted with the distance between pixel centers.
    double const diagonalCliqueCost = directCliqueCost / sqrt(2);

    // Default value for compactness feature weight depends on the type of image.
    // Since color images generate a total of 3 cost terms for the separate channels, we choose
    // the compactness weight at 3 times the value for grayvalue images to preserve the relative weighting.
    double const defaultCompactnessWeight = isColorImage ? 0.045 : 0.015;
    double const compactnessWeight = (argc > 5) ? atof(argv[5]) : defaultCompactnessWeight;
    if (compactnessWeight < 0)
    {
        std::cout << "compactnessWeight must be non-negative. Aborting." << std::endl;

        return 1;
    }

    // Number of iterations to perform. 3 is usually more than enough but still reasonably fast.
    unsigned int const numIterations = 3;

    // Set the desired type of the label image here.
    // Must be unsigned integer type since it will be used to index vectors.
    // A minimum size should be guaranteed so it can accomodate enough different labels.
    typedef boost::uint16_t TLabelImage;

    // Create an instance of the Contour Relaxation class with the selected features.
    ContourRelaxation<TLabelImage> contourRelaxationInst(enabledFeatures);

    // Initialize the label image with a "blind segmentation" of rectangular blocks.
    // You can also use createDiamondInitialization from contourRelaxation/InitializationFunctions.h
    // or create a custom initialization as cv::Mat of type cv::DataType<TLabelImage>::type and the
    // same size as the input image.
    cv::Mat labelImage = createBlockInitialization<TLabelImage>(image.size(), blockWidth, blockHeight);

    // Set data for all features.
    contourRelaxationInst.setCompactnessData(compactnessWeight);

    if (isColorImage)
    {
        // Convert image to YUV-like YCrCb for uncorrelated color channels.
        cv::Mat imageYCrCb;
        cv::cvtColor(image, imageYCrCb, SEEDS_REVISED_OPENCV_BGR2YCrCb);
        std::vector<cv::Mat> imageYCrCbChannels;
        cv::split(imageYCrCb, imageYCrCbChannels);

        contourRelaxationInst.setColorData(imageYCrCbChannels[0], imageYCrCbChannels[1], imageYCrCbChannels[2]);
    }
    else
    {
        // Generate a 3-channel version of the grayscale image, which we will need later on
        // to generate the boundary overlay. Save it in the "image" variable so we won't
        // have to care about the original type of the image anymore.
        cv::Mat imageGray = image.clone();
        cv::cvtColor(imageGray, image, CV_GRAY2BGR);

        contourRelaxationInst.setGrayvalueData(imageGray);
    }

    // Perform contour relaxation.
    cv::Mat relaxedLabelImage;
    cv::Mat regionMeanImage;
    contourRelaxationInst.relax(labelImage, directCliqueCost, diagonalCliqueCost,
                                numIterations, relaxedLabelImage, regionMeanImage);

    if (isColorImage)
    {
        // Convert region-mean image back to BGR.
        // If we are dealing with a grayscale image, the region-mean image is grayscale, too.
        cv::cvtColor(regionMeanImage, regionMeanImage, CV_YCrCb2BGR);
    }

    // Get the name of the input image without the extension for creating the output filenames.
    // Since the boost library has changed starting with version 1.46 and stem() now returns a
    // path object instead of a string, we need to handle both cases.
    #if BOOST_VERSION < 104600
    std::string shortInputName = inputImage.stem();
    #else
    std::string shortInputName = inputImage.stem().string();
    #endif

    // Save label image.
    // For TLabelImage = boost::uint16_t, this saves a 16bit PNG file which conserves all the labels.
    cv::imwrite(shortInputName + "_labelImage.png", relaxedLabelImage);

    // Save region mean image.
    cv::imwrite(shortInputName + "_regionMean.png", regionMeanImage);

    // Create and save boundary image and boundary overlay visualization.
    cv::Mat boundaryImage;
    computeBoundaryImage<TLabelImage>(relaxedLabelImage, boundaryImage);
    // Changing the boundaries from 1 to 255 so the resulting image can be interpreted directly in image viewers.
    boundaryImage *= 255;
    cv::imwrite(shortInputName + "_boundaryImage.png", boundaryImage);

    cv::Mat boundaryOverlay;
    computeBoundaryOverlay<TLabelImage>(image, relaxedLabelImage, boundaryOverlay);
    cv::imwrite(shortInputName + "_boundaryOverlay.png", boundaryOverlay);

    return 0;
}
