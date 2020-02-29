
#include "VideoCaptureEncod.h"

VideoCaptureEncod::VideoCaptureEncod(MediaCameraVideoCallBack* pMediaCameraVideoCallBack)
:m_MediaCameraVideoCallBack(pMediaCameraVideoCallBack)
,m_pVideoWindow(nil),
openBeautifyace(true),
pBeautifyFace(nullptr)
{
//    pBeautifyFace =   [[BeautifyFace alloc]init];
}

VideoCaptureEncod::~VideoCaptureEncod()
{
    if(pBeautifyFace) {
        [pBeautifyFace release];
        pBeautifyFace =nil;
    } else {
        [[CameraHelp shareCameraHelp] setVideoDataOutputBuffer:nullptr];
        [[CameraHelp shareCameraHelp]stopVideoCapture];
    }
}

bool VideoCaptureEncod::StartCapture(UIView *pUIImageView,int resType)
{
    if(pBeautifyFace) {
        //        dispatch_async(dispatch_get_main_queue(), ^{
        [pBeautifyFace SetPreview:pUIImageView];
        [pBeautifyFace setVideoDataOutputBuffer:this];
        [pBeautifyFace SetFrontCamera];
        [pBeautifyFace StartVideoCapture];
        //        });
    } else {
        [[CameraHelp shareCameraHelp] setVideoDataOutputBuffer:this];
        [[CameraHelp shareCameraHelp] prepareVideoCapture:resType andFps:18 andFrontCamera:YES andPreview:pUIImageView];
        //开始捕获
        [[CameraHelp shareCameraHelp] startVideoCapture];
    }
    return true;
}

void VideoCaptureEncod::setEncodebitrate(long bitrate)
{
    [[CameraHelp shareCameraHelp] setEncodebitrate:bitrate];
}

void VideoCaptureEncod::reOrientation()
{
    if(pBeautifyFace) {
        [pBeautifyFace reOrientation];
    } else {
        [[CameraHelp shareCameraHelp]reSetCamera];
    }
}

void VideoCaptureEncod::StopCapture()
{
    if(pBeautifyFace) {
        [pBeautifyFace StopVideoCapture];
    } else {
        [[CameraHelp shareCameraHelp] setVideoDataOutputBuffer:nil];
        dispatch_sync(dispatch_get_main_queue(), ^{
            [[CameraHelp shareCameraHelp] stopVideoCapture];
            [CameraHelp closeCamera];
        });
    }
}

bool VideoCaptureEncod::setPreview(void *preview)
{
    if(pBeautifyFace) {
        [pBeautifyFace SetPreview:(UIView*)preview];
    } else {
        [[CameraHelp shareCameraHelp] setPreview:preview];
    }
    return  YES;
}

void VideoCaptureEncod::openBeautify()
{
    if(pBeautifyFace) {
        [pBeautifyFace openBeautify];
    }
}

void VideoCaptureEncod::closeBeautify()
{
    [pBeautifyFace closeBeautify];
}
void VideoCaptureEncod::switchCamera()
{
    if(pBeautifyFace) {
        [pBeautifyFace switchCamera];
    }
}

void VideoCaptureEncod::On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)
{
    if (m_MediaCameraVideoCallBack) {
        m_MediaCameraVideoCallBack->On_MediaReceiverCallbackCameraVideo(pData,nLen,bKeyFrame,nWidth,nHeight);
    }
}







