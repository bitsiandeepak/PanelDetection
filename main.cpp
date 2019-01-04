//
//  main.cpp
//  FinalPanelDetection
//
//  Created by Pandey, Deepak on 12/12/17.
//  Copyright © 2017 Pandey, Deepak. All rights reserved.
//

#include <iostream>
#include "PanelDetection.h"

int main(int argc, const char * argv[])
{
    std::string imagePath = "/Users/panddee/test_data/invincible.jpg";
    PanelDetection currentImage;
    currentImage.setMargins(0, 0, 0, 0);
    currentImage.getPanels(imagePath);
    return 0;
}
