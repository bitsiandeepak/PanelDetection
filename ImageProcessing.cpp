//
//  ImageProcessing.cpp
//  FinalPanelDetection
//
//  Created by Pandey, Deepak on 12/12/17.
//  Copyright © 2017 Pandey, Deepak. All rights reserved.
//

#include <stdio.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "ImageProcessing.h"
#include<map>
#include<list>

using namespace cv;


Mat ImageProcessing::clipMargins(Mat image, int aRightMargin, int aLeftMargin, int aTopMargin, int aBottomMargin )
{
    cv::Rect rect;
    rect.x = aRightMargin;
    rect.y = aLeftMargin;
    rect.width = image.cols - (aLeftMargin + aRightMargin);
    rect.height = image.rows - (aTopMargin + aRightMargin);
    if(rect.width <= 0 || rect.height <=0)
        return image;
    Mat croppedImage = image(rect);
    imshow("Cropped Image", croppedImage);
    return croppedImage;
}

Mat ImageProcessing::convertToGrayscale(Mat anImage)
{
    Mat grayImg;
    cvtColor(anImage, anImage, CV_BGR2GRAY);
    imshow("GrayScale", anImage);
    return anImage;
}

int ImageProcessing::getBackgroundColor(Mat anImage)
{
    //get candidate colors
    std::vector<CvScalar> borderPixels;
    CvScalar startColor, comparisonColor;
    //Threshold for comparison between start and comparison color. 10 has been set after observing the test data
    const int comparisonThreshold = 10;
    int flag;
    for (int i = 0; i < anImage.rows; i++)
    {
        //Color at the start of the row
        startColor = anImage.at<uchar>(i, 0);
        flag = 1;
        for (int j = 0; j < anImage.rows; j++)
        {
            //Color at the present coordinate
            comparisonColor = anImage.at<uchar>(i, j);
            //Compare both the colors with the comparison threshold
            if (abs(startColor.val[0] - comparisonColor.val[0]) > comparisonThreshold)
            {
                flag = 0;
                break;
            }
        }
        if (flag == 1)
            borderPixels.push_back(startColor);
    }
    
    //get max from candidate colors
    std::map<int,int> colorCount;
    
    for (CvScalar pixel:borderPixels)
    {
        if (colorCount.find(pixel.val[0]) == colorCount.end())
            colorCount[pixel.val[0]] = 1;
        else
            colorCount[pixel.val[0]]++;
    }
    
    std::map<int,int>::iterator it;
    
    //Get the maximum used color
    int maxCount = INT_MIN;
    int maxUsedColor = 0;
    for (it = colorCount.begin(); it != colorCount.end(); it++)
    {
        if (maxCount < it->second)
        {
            maxCount = it->second;
            maxUsedColor = it->first;
        }
    }
    return maxUsedColor;
}

Mat ImageProcessing::extendBorders(Mat anImage)
{
    //Find the background color by using getBackgroundColor
    Mat grayImage = convertToGrayscale(anImage);
    CvScalar backgroundColor;
    int Color = getBackgroundColor(grayImage);
    backgroundColor.val[0] = Color;
    
    //Create a band of 3 pixel thickness on both sides using the color which we got from getBackgroundColor
    const int thickness = 3;
    Mat bandedImage;
    copyMakeBorder(grayImage, bandedImage, thickness, thickness, thickness, thickness, BORDER_CONSTANT, backgroundColor);
    imshow("Extend", bandedImage);
    return bandedImage;
}

Mat ImageProcessing::preProcessing(Mat anImage)
{
    Mat grayImage = convertToGrayscale(anImage);
    Mat extendedImage = extendBorders(grayImage);
    return extendedImage;
}

Mat ImageProcessing::sobelEdgeDetection(Mat anImage)
{
    const int scale = 1;
    const int delta = 0;
    
    //depth of the output image. We take it as 16 bit signed to avoid overflow
    const int ddepth = CV_16S;
    const int xOrderDerivative = 1;
    const int yOrderDerivative = 1;
    const int kernelSize =3;
    //Apply Gaussian Blur to reduce noise
    GaussianBlur( anImage, anImage, Size(3, 3), 0, 0, BORDER_DEFAULT );
    Mat gradientX, gradientY;
    Mat absGradientX, absGradientY;
    
    //Gradient in X
    Sobel( anImage, gradientX, ddepth, xOrderDerivative, 0, kernelSize, scale, delta, BORDER_DEFAULT );
    //Gradient in Y
    Sobel( anImage, gradientY, ddepth, 0, yOrderDerivative, kernelSize, scale, delta, BORDER_DEFAULT );
    //Convert results back to depth CV_8U which is the default type which we use
    convertScaleAbs( gradientX, absGradientX );
    convertScaleAbs( gradientY, absGradientY );
    Mat edgesImage;
    
    //Value to be added in X Gradient. Should be close to 0.01. For viewing the edges increase this Value
    //Choosing a bigger amount will result in the panels getting merged after edge detection
    const double Alpha = 0.01;
    //Value to be added in Y Gradient. Should be close to 0.01.
    const double Beta = 0.01;
    
    //We don't use gamma. hence the value 0
    const double gamma = 0;
    //Add both the directional Gradients X and Y
    addWeighted( absGradientX, Alpha, absGradientY, Beta, gamma, edgesImage );
    return edgesImage;
}

Mat ImageProcessing::floodFill(Mat aSobelImage)
{
    //Create a black image for flood fill operation
    Mat backgroundImage(aSobelImage.rows, aSobelImage.cols, CV_8UC1, Scalar(0));
    imshow("Background", backgroundImage);
    //cvWaitKey(0);
    std::list<CvPoint> aSeeds;
    aSeeds.push_back(cvPoint(aSobelImage.cols - 1, 0));
    CvScalar originalImageColor, backgroundImageColor;
    CvScalar color;
    color.val[0] = 255;
    CvPoint seed;
    int i, x, y;
    
    // 4 neighbours corrresponding to the top, down, left and right pixels
    const int numberOfNeighbours = 4;
    const int neighbourX[numberOfNeighbours] = {0, 1, -1, 0};
    const int neighbourY[numberOfNeighbours] = {1, 0, 0, -1};
    
    while (!aSeeds.empty())
    {
        seed = aSeeds.front();
        aSeeds.pop_front();
        
        for (i = 0; i < numberOfNeighbours; i++)
        {
            x = seed.x + neighbourX[i];    y = seed.y + neighbourY[i];
            if (x >= 0 && x < aSobelImage.cols && y >= 0 && y < aSobelImage.rows)
            {
                originalImageColor = aSobelImage.at<uchar>(y, x);
                backgroundImageColor = backgroundImage.at<uchar>(y, x);
                
                //if both the Colors are black we change the color of the background image to white and push
                // the corresponding point in the list to process it's neighbours
                if (backgroundImageColor.val[0] == 0 && originalImageColor.val[0] == 0)    // same region
                {
                    backgroundImage.at<uchar>(y, x) = 255;
                    aSeeds.push_back(cvPoint(x, y));
                }
            }
        }
    }
    imshow("Window", backgroundImage);
    return backgroundImage;
}

Mat ImageProcessing::erosionAndDilation(Mat anImage)
{
    const int iterations=20;
    const int kernelSize=3;
    //    IplImage *tmp = cvCreateImage(cvGetSize(anImage), 8, 1);
    IplConvKernel *structEl = cvCreateStructuringElementEx(kernelSize, kernelSize, 1, 1, CV_SHAPE_RECT);
    Mat element = getStructuringElement( MORPH_RECT, Size(kernelSize, kernelSize), Point(0, 0));
    Mat dilation;
    dilate( anImage, dilation, element, Point(-1, -1), iterations);
    Mat erosion;
    erode( dilation, erosion, element, Point(-1, -1), iterations);
    imshow("Eroded", erosion);
    imshow("Dilated", dilation);
    cvReleaseStructuringElement(&structEl);
    return erosion;
}

std::vector<std::vector<Point> > ImageProcessing::detectContours(Mat anImage)
{
    RNG rng(12345);
    std::vector<std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;
    
    imshow("ContourImage", anImage);
    
    findContours( anImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    /// Draw contours
    Mat drawing = Mat::zeros( anImage.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
    imshow("Contours", drawing);
    return contours;
}

std::vector<std::vector<Point> > ImageProcessing::convertToPolygon(std::vector<std::vector<Point> > aContours)
{
    std::vector<std::vector<Point> > contourPolygons(aContours.size());
    for(int i = 0; i < aContours.size(); i++)
    {
        approxPolyDP(Mat(aContours[i]), contourPolygons[i], 0.02, true);
    }
    return contourPolygons;
}

std::vector<Rect> ImageProcessing::getMaximumBoundRectangles(Mat anImage,std::vector<std::vector<Point> > aPolygons)
{
    std::vector<Rect> boundRect;
    double minArea =  (1.0 / 64.0) * ((anImage.rows ) * (anImage.cols));
    double maxArea = (anImage.rows - 1) * (anImage.cols - 1);
    for(int i = 0; i < aPolygons.size(); i++)
    {
        int area = contourArea(aPolygons[i]);
        if((area > minArea) && (area <maxArea))
            boundRect.push_back(boundingRect(Mat(aPolygons[i])));
    }
    return boundRect;
}



