//
//  VideoCaptureEncoding.h
 

#ifndef VideoLibrary_VideoCaptureEncoding_h
#define VideoLibrary_VideoCaptureEncoding_h
//#define BEAUTIFYFACE 1

#import "VideoLibrary.h"
#import "CameraHelp.h"
#import <UIKit/UIKit.h>
#import "BeautifyFace.h"

class VideoCaptureEncod : public VideoCapture
{
public:
    VideoCaptureEncod(MediaCameraVideoCallBack* pMediaCameraVideoCallBack);
    ~VideoCaptureEncod();
public:
    virtual bool StartCapture(UIView *pUIImageView,int resType);
    virtual void StopCapture();
    virtual bool setPreview(void *preview);
public:
 
    virtual void  switchCamera();
    virtual void  setEncodebitrate(long bitrate);
    virtual void  reOrientation();
    virtual void  openBeautify();
    virtual void  closeBeautify();
    
    void On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight);
  private:
 
    bool         m_isFront;
    bool  openBeautifyace;
    void*        m_pVideoWindow;
 
    BeautifyFace* pBeautifyFace;
 
    MediaCameraVideoCallBack* m_MediaCameraVideoCallBack;
};

#endif
