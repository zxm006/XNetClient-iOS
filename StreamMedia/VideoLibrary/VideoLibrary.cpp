//
//  VideoLibrary.m
//  VideoLibrary
//
//  Created by wang guijun on 13-1-19.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#include "VideoLibrary.h"
#include "VideoCaptureEncod.h"
#include "CapScreenEnc.h" 

#import <UIKit/UIKit.h>
VideoCapture* g_VideoCapture = NULL;

VideoCapture* CteateVideoCapture(MediaCameraVideoCallBack* pVideoCaptureDataCallBack)
{
    if(g_VideoCapture == NULL)
    {
        g_VideoCapture = new VideoCaptureEncod(pVideoCaptureDataCallBack);
        return g_VideoCapture;
    }
    else
        return g_VideoCapture;
}
 
void ReleaseVideoCapture()
{
    if(g_VideoCapture != NULL)
    {
        delete g_VideoCapture;
        g_VideoCapture = NULL;
    }
}

VideoCapScreen* g_VideoCapScreenEnc= NULL;

VideoCapScreen* CreateCapScreen(MediaScreenVideoCallBack* pMediaScreenVideoCallBack)
{
    if(g_VideoCapScreenEnc == NULL)
    {
        g_VideoCapScreenEnc = new VideoCapScreenEnc(pMediaScreenVideoCallBack);
        return g_VideoCapScreenEnc;
    }
    else
        return g_VideoCapScreenEnc;
}



void ReleaseCapScreen()
{
    if(g_VideoCapScreenEnc != NULL)
    {
        g_VideoCapScreenEnc->StopCapture();
        
        delete g_VideoCapScreenEnc;
        g_VideoCapScreenEnc = NULL;
    }
}




