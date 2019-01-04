//
//  PanelDetection.h
//  FinalPanelDetection
//
//  Created by Pandey, Deepak on 12/13/17.
//  Copyright © 2017 Pandey, Deepak. All rights reserved.
//

#ifndef PanelDetection_h
#define PanelDetection_h
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "ImageProcessing.h"
#include "Region.h"

using namespace cv;

class PanelDetection
{
public:
    void setMargins(int aRightMargin, int aLeftMargin, int aTopMargin, int aBottomMargin);
    std::vector<Rect> getRectangles(Mat image);
    std::vector<Region> getPanels(std::string anImagePath);
    
private:
    int fRightMargin;
    int fLeftMargin;
    int fTopMargin;
    int fBottomMargin;
};

#endif /* PanelDetection_h */
