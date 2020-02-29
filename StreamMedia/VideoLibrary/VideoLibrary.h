//
//  VideoLibrary.h
//  VideoLibrary
//
//  Created by wang guijun on 13-1-19.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//
#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H
#import <UIKit/UIKit.h>
class VideoCapture
{
public:
  VideoCapture(){};
    virtual ~VideoCapture(){};
public:
    virtual bool StartCapture(UIView *pUIImageView,int resType) = 0;
    virtual void StopCapture() = 0;
    virtual bool setPreview(void *preview) = 0;
    virtual void switchCamera() = 0;
    virtual void reOrientation() = 0;
    virtual void setEncodebitrate(long bitrate) = 0;
    virtual void openBeautify() = 0;
    virtual void closeBeautify() = 0;
};

class VideoCapScreen
{
public:
    VideoCapScreen(){};
    virtual ~VideoCapScreen(){};
public:
    virtual bool StartCapture(int resType) = 0;
    virtual void StopCapture() = 0;
    
    virtual void SetResolution(unsigned int uiWidth, unsigned int uiHeight) = 0;
 
};



class MediaCameraVideoCallBack
{
public:
    MediaCameraVideoCallBack(){};
    ~MediaCameraVideoCallBack(){};
public:
    virtual void On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)=0;
};

class  MediaScreenVideoCallBack
{
public:
    MediaScreenVideoCallBack(){};
    ~MediaScreenVideoCallBack(){};
public:
    virtual void On_MediaReceiverCallbackScreenVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight) = 0;
};




VideoCapture* CteateVideoCapture(MediaCameraVideoCallBack* pVideoCaptureDataCallBack);
void ReleaseVideoCapture();

VideoCapScreen* CreateCapScreen(MediaScreenVideoCallBack* pVideoCaptureDataCallBack);
void ReleaseCapScreen();

class VideoPalyback
{
public:
    VideoPalyback(){};
    virtual ~VideoPalyback(){};
public:
    virtual bool Start() = 0;
    virtual void Stop() = 0;
public:
    virtual bool StartDisplayVideo() = 0;
    virtual void StopDisplayVideo() = 0;
public:
    virtual uint32_t GetTimeStamp() = 0;
    virtual bool SeekCleanBuffer() = 0;
    virtual bool SetPlayAndStop(bool bPaly) = 0;
public:
    virtual void SetVideoWindow(void* pVideoWindow, int nVideoType) = 0;
    virtual bool AddVideoData(unsigned char*pData,int nLen, uint32_t nTimeStamp) = 0;
};

VideoPalyback* CteateVideoplayback();
void ReleaseVideoplayback(VideoPalyback* pVideoPalyback);

#endif
