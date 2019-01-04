//
//  ImageProcessing.h
//  FinalPanelDetection
//
//  Created by Pandey, Deepak on 12/12/17.
//  Copyright © 2017 Pandey, Deepak. All rights reserved.
//

#ifndef ImageProcessing_h
#define ImageProcessing_h

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

class ImageProcessing
{
public:
    static Mat clipMargins(Mat image, int aRightMargin, int aLeftMargin, int aTopMargin, int aBottomMargin );
    static Mat convertToGrayscale(Mat anImage);
    static int getBackgroundColor(Mat anImage);
    static Mat extendBorders(Mat image);
    static Mat preProcessing(Mat image);
    static Mat sobelEdgeDetection(Mat image);
    static Mat floodFill(Mat image);
    static Mat erosionAndDilation(Mat image);
    static std::vector<std::vector<Point> > detectContours(Mat anImage);
    static std::vector<std::vector<Point> > convertToPolygon(std::vector<std::vector<Point> > aContours);
    static std::vector<Rect> getMaximumBoundRectangles(Mat anImage, std::vector<std::vector<Point> > aPolygons);
};

#endif /* ImageProcessing_h */
