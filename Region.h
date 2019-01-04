//
//  Region.h
//  FinalPanelDetection
//
//  Created by Pandey, Deepak on 12/13/17.
//  Copyright © 2017 Pandey, Deepak. All rights reserved.
//

#ifndef Region_h
#define Region_h

#include <stdio.h>

class Region
{
public:
    Region(int aXCoordinate, int aYCoordinate, int aWidth, int aHeight);
    int fXCoordinate;
    int fYCoordinate;
    int fWidth;
    int fHeight;
};


#endif /* Region_h */
