//AVCONEC.h
#ifndef __AVCONEC_H__
#define __AVCONEC_H__

#include <AVCONEC/libAVCONEC.h>
#include <AVCONEC/libXMediaFileCapture.h>
#include <AVCONProduct.h>
#include <VIDEC/VIDEC.h>

//nAECType回音消除强度 0-4 默认4
AFX_EXT_CLASS void AVCONECInit(int nAECType,int nUsedByProductType=AVCON_PRODUCT_NORMAL,unsigned int ulVIDECFlag=VIDEC_INIT_FLAG_ALL);
AFX_EXT_CLASS void AVCONECUninit(void);

AFX_EXT_CLASS int AVCONECVideoCaptureDeviceCount(void);
#ifdef _WIN32
AFX_EXT_CLASS bool AVCONECVideoCaptureDeviceName(int nDevice,TCHAR*szDeviceName);
#else
AFX_EXT_CLASS bool AVCONECVideoCaptureDeviceName(int nDevice,char*szDeviceName);
#endif


AFX_EXT_CLASS IXMediaFileCapture*CreateXMediaFileCapture(IXMediaFileCaptureNotify&rNotify,HWND hWnd,const char*cszMediaFile,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bLoop,int nNetworkType);

#endif