//
//  PanelDetection.cpp
//  FinalPanelDetection
//
//  Created by Pandey, Deepak on 12/13/17.
//  Copyright © 2017 Pandey, Deepak. All rights reserved.
//

#include <stdio.h>
#include "PanelDetection.h"
#include "Region.h"
using namespace cv;

void PanelDetection::setMargins(int aRightMargin, int aLeftMargin, int aTopMargin, int aBottomMargin)
{
    fRightMargin = aRightMargin;
    fLeftMargin = aLeftMargin;
    fTopMargin = aTopMargin;
    fBottomMargin = aBottomMargin;
}

std::vector<Region> PanelDetection::getPanels(std::string anImagePath)
{
    Mat originalImage = imread(anImagePath, IMREAD_COLOR);
    Mat clippedImage =  ImageProcessing::clipMargins(originalImage, fRightMargin, fLeftMargin, fTopMargin, fBottomMargin);
    imshow("clippedImage", clippedImage);
    Mat preProcessedImage = ImageProcessing::preProcessing(clippedImage);
    Mat edgesImage = ImageProcessing::sobelEdgeDetection(preProcessedImage);
    Mat backGroundImage = ImageProcessing::floodFill(edgesImage);
    Mat noiselessImage = ImageProcessing::erosionAndDilation(backGroundImage);
    imshow("noiselessImage", noiselessImage);
    std::vector<std::vector<Point> > contours = ImageProcessing::detectContours(noiselessImage);
    std::vector<std::vector<Point> > polygons = ImageProcessing::convertToPolygon(contours);
    std::vector<Rect> boundingRects = ImageProcessing::getMaximumBoundRectangles(noiselessImage, polygons);
    int size = (int)boundingRects.size();
    std::vector<Region> panels;
    for(int i = 0; i < size; i++)
    {
        Region region(boundingRects[i].x, boundingRects[i].y, boundingRects[i].width, boundingRects[i].height);
        panels.push_back(region);
    }
    
    for(int i = 0; i < panels.size(); i++)
    {
        //Sobel edge detects thick edges. Here we adjust the coordinates and width and height accordingly
        panels[i].fXCoordinate += 2;
        panels[i].fYCoordinate += 2;
        panels[i].fWidth -= 4;
        panels[i].fHeight -= 4;
        
        //Reduce the X and Y Coordinates as we created a band in the begining of 3 pixels on all sides
        if(panels[i].fXCoordinate >= 3)
            panels[i].fXCoordinate -= 3;
        if(panels[i].fYCoordinate >= 3)
            panels[i].fYCoordinate -=3;
    }
    return panels;
}
