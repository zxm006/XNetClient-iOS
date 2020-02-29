#include "OpenLocalUser.h"
#include <AUDEC/AUDEC_Header.h>
#include <AUDEC/AUDEC_CodecID.h>
#include "VIDEC/VIDEC_Header.h"
#include<stdlib.h>
#import "CameraHelp.h"

#include<sys/time.h>
#ifdef WIN32
#include <MMSystem.h>
#endif

typedef enum {
    VIDEC_CODEC_H261=0,		//unsupport
    VIDEC_CODEC_H263,
    VIDEC_CODEC_H263P,
    VIDEC_CODEC_H263PP,		//same to H263P
    VIDEC_CODEC_H264,
    VIDEC_CODEC_LSCC,		//unsupport
    VIDEC_CODEC_AH400,
    VIDEC_CODEC_MPEG4,		//unsupport
    VIDEC_CODEC_DH264,
    VIDEC_CODEC_HIKH,
    VIDEC_CODEC_H264_SVC,
    VIDEC_CODEC_HIKC,
    VIDEC_CODEC_COUNT
}VIDEC_CODEC_TYPE;

OpenLocalUser::OpenLocalUser()
: m_nPort(0)
, m_ulLocalAudioID(0)
, m_pMediaSender(NULL)
, m_bGotKeyFrameMain(false)
, m_pVideoPacket(NULL)
, m_nFrameBufferLength(0)
, m_pAudioPacket(NULL)
, m_nAudioFrameBufferLength(0)
, m_usAudioSequence(0)
, m_ulLastFrameTimestamp(0)
, m_ulLastPacketTimestamp(0)
, m_MediaServerIp("")
, m_pVideoCapture(NULL)
, m_VideoCapScreen(NULL)
,m_sendAudioQueue (nil)
,m_sendVideoQueue(nil)
{
    
}

OpenLocalUser::~OpenLocalUser()
{
    
}

bool OpenLocalUser::ConnectMediaServer(std::string strServerIp, unsigned short nPort)
{
    m_MediaServerIp = strServerIp;
    m_nPort = nPort;
    if(!CreateSender())
        return false;
    return true;
}

void OpenLocalUser::ReleaseMediaSever()
{
    ReleaseSender();
}

bool OpenLocalUser::OpenAudioSend(unsigned int ulUserAudioId)
{
    if( ulUserAudioId == 0)
        return false;
    
    if (!m_sendAudioQueue) {
        m_sendAudioQueue = dispatch_queue_create("dispatch_queue--sendAudioQueue ", DISPATCH_QUEUE_SERIAL);
    }
    m_ulLocalAudioID = ulUserAudioId;
    if (0!=m_pMediaSender->StartAudio(m_ulLocalAudioID)) {
        return false;
    }
    return true;
}


bool OpenLocalUser::OpenVideoSend(unsigned int ulUserVideoId)
{
    if( ulUserVideoId == 0)
        return false;
    
    if (!m_sendVideoQueue) {
        m_sendVideoQueue = dispatch_queue_create("dispatch_queue--sendVideoQueue ", DISPATCH_QUEUE_SERIAL);
    }
    m_ulLocalVideoID = ulUserVideoId;
    
    if (0!=m_pMediaSender->StartVideo(m_ulLocalVideoID)) {
        return false;
    }
    return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool OpenLocalUser::CloseVideoSend()
{
    m_ulLocalVideoID = 0;
    if (m_sendVideoQueue) {
        dispatch_release(m_sendVideoQueue);
        m_sendVideoQueue = nil;
    }
    
    if (m_pMediaSender!=NULL) {
        m_pMediaSender->StopVideo();
        return true;
    }
    return true;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
bool OpenLocalUser::CloseAudioSend()
{
    m_ulLocalAudioID = 0;
    if (m_sendAudioQueue) {
        dispatch_release(m_sendAudioQueue);
        m_sendAudioQueue = nil;
    }
    
    if (m_pAudioPacket) {
        free(m_pAudioPacket);
        m_pAudioPacket = NULL;
    }
    
    if (m_pMediaSender!=NULL) {
        m_pMediaSender->StopAudio();
        return true;
    }
    return true;
}

void OpenLocalUser::reOrientation()
{
    if(m_pVideoCapture)
        m_pVideoCapture->reOrientation();
}

void  OpenLocalUser::openBeautify()
{
    if(m_pVideoCapture)
        m_pVideoCapture->openBeautify();
}

void  OpenLocalUser::closeBeautify()
{
    if(m_pVideoCapture)
        m_pVideoCapture->closeBeautify();
}


void OpenLocalUser::openLocalVideo(UIView *localvideo,int resType,bool isCapScreen)
{
    if (isCapScreen) {
        OpenCapScreen(1);
        return;
    }
    else {
        if(m_pVideoCapture)
            return;
        m_MeetisCapScreen = enum_Camera;
        m_pVideoCapture = CteateVideoCapture(this);
        m_pVideoCapture->StartCapture(localvideo,resType);
    }
}

void OpenLocalUser::setEncodebitrate(long bitrate)
{
    if (m_pVideoCapture) {
        m_pVideoCapture->setEncodebitrate(bitrate);
    }
}

void OpenLocalUser::switchCamera()
{
    if(!m_pVideoCapture)
        return;
    m_pVideoCapture->switchCamera();
}

void OpenLocalUser::closeLocalVideo()
{
    if(m_MeetisCapScreen ==enum_Screen) {
        m_MeetisCapScreen = enum_No;
        CloseCapScreen();
        return;
    }
    
    if(m_MeetisCapScreen ==enum_Camera) {
        if(!m_pVideoCapture)
            return;
        m_pVideoCapture->StopCapture();
        ReleaseVideoCapture();
        m_pVideoCapture = NULL;
        m_MeetisCapScreen = enum_No;
        return;
    }
    return;
}

bool OpenLocalUser::OpenCapScreen(int resType)
{
    if(m_VideoCapScreen)
        return false;
    m_MeetisCapScreen = enum_Screen;
    m_VideoCapScreen = CreateCapScreen(this);
    m_VideoCapScreen->StartCapture(resType);
    return true;
}

bool OpenLocalUser::CloseCapScreen()
{
    if(!m_VideoCapScreen) {
        printf("m_VideoCapScreen = NULL");
        return false;
    }
    ReleaseCapScreen();
    m_VideoCapScreen =NULL;
    return true;
}

bool OpenLocalUser::SendAudioData(char*pData, int nLen)
{
    if (m_sendAudioQueue) {
        dispatch_async(m_sendAudioQueue, ^{
            ProcessAudioFrame(pData, nLen, TimeGetTimestamp());
        });
    }
    return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool OpenLocalUser::CreateSender()
{
    m_pMediaSender = XNetMediaSender::Create(*this);
    if (m_pMediaSender!=NULL) {
        m_pMediaSender->SetLocalMCUID("MCU001");
        m_pMediaSender->SetLocalMCIIP(m_MediaServerIp.c_str());
        m_pMediaSender->SetLocalMCUPort(m_nPort);
        int m_ConnectStatus = 2;//CS_CONNECTED;
        m_pMediaSender->SetConnectStatus(m_ConnectStatus);
        
        if ( 0 != m_pMediaSender->Open()) {
            m_pMediaSender->Close();
            delete m_pMediaSender;
            m_pMediaSender=NULL;
            return false;
        }
        return true;
    }
    return false;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void OpenLocalUser::ReleaseSender()
{
    if (m_pMediaSender!=NULL) {
        m_pMediaSender->Close();
        delete m_pMediaSender;
        m_pMediaSender = NULL;
    }
}

bool OpenLocalUser::SendVideoData(unsigned char*pData, int nLen, bool bKeyFrame, unsigned long ulTimestamp, int nWidth, int nHeight)
{
    ProcessVideoFrame(pData, nLen,bKeyFrame, TimeGetTimestamp(),nWidth,nHeight);
    return true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/

void OpenLocalUser::ProcessVideoFrame(unsigned char*pData, int nLen, bool bKeyFrame, unsigned long ulTimestamp, int nWidth, int nHeight)
{
    if (m_nFrameBufferLength<nLen+1024) {
        m_nFrameBufferLength=nLen+2048;
        if (m_pVideoPacket) {
            free(m_pVideoPacket);
            m_pVideoPacket=NULL;
        }
        m_pVideoPacket=(char*)malloc(m_nFrameBufferLength);
        if (m_pVideoPacket==NULL) {
            m_nFrameBufferLength=0;
            return;
        }
    }
    
    if (m_bGotKeyFrameMain==false && bKeyFrame && m_pVideoPacket) {
        m_bGotKeyFrameMain=true;
    }
    
    if (m_bGotKeyFrameMain && m_pVideoPacket) {
        VIDEC_HEADER_EXT_RESET(m_pVideoPacket);
        VIDEC_HEADER_EXT_SET_CODEC_ID(m_pVideoPacket,VIDEC_CODEC_H264);
        VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(m_pVideoPacket,0);
        VIDEC_HEADER_EXT_SET_KEYFRAME(m_pVideoPacket,(bKeyFrame?1:0));
        VIDEC_HEADER_EXT_SET_SEQUENCE(m_pVideoPacket,m_usVideoSequence++);
        VIDEC_HEADER_EXT_SET_TIMESTAMP(m_pVideoPacket,(unsigned int)ulTimestamp);
        
        if (bKeyFrame) {
            VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(m_pVideoPacket,nWidth);
            VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(m_pVideoPacket,nHeight);
            VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(m_pVideoPacket,nWidth);
            VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(m_pVideoPacket,nHeight);
        }
        
        int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(m_pVideoPacket);
        
        memcpy(m_pVideoPacket+nHeaderLen,pData,nLen);
        
        VIDEC_HEADER_EXT_SET_MAIN_FRAME(m_pVideoPacket,1);
        if (m_pMediaSender != NULL) {
            m_pMediaSender->SendVideo((unsigned char *)m_pVideoPacket, nLen+nHeaderLen);
            //            int nRet =
        }
    }
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void OpenLocalUser::ProcessAudioFrame(char*pData, int nLen, unsigned long ulTimestamp)
{
    if (m_nAudioFrameBufferLength<nLen+1024) {
        m_nAudioFrameBufferLength=nLen+2048;
        if (m_pAudioPacket) {
            free(m_pAudioPacket);
            m_pAudioPacket=NULL;
        }
        m_pAudioPacket=(char*)malloc(m_nAudioFrameBufferLength);
        if (m_pAudioPacket==NULL) {
            m_nAudioFrameBufferLength=0;
            return;
        }
    }
    
    if (m_pAudioPacket) {
        unsigned long ulCurTimestamp=TimeGetTimestamp();
        if (ulCurTimestamp-m_ulLastFrameTimestamp<120) {
            unsigned long ulDelta1=ulCurTimestamp-m_ulLastPacketTimestamp;
            unsigned long ulDelta2=m_ulLastPacketTimestamp-ulCurTimestamp;
            unsigned long ulDelta=(ulDelta1>ulDelta2?ulDelta2:ulDelta1);
            if (ulDelta==ulDelta2) {
                m_ulLastPacketTimestamp+=1;
            } else {
                m_ulLastPacketTimestamp=ulCurTimestamp;
            }
        } else {
            m_ulLastPacketTimestamp=ulCurTimestamp;
        }
        m_ulLastFrameTimestamp=ulCurTimestamp;
        
        AUDEC_HEADER_RESET(m_pAudioPacket);
        AUDEC_HEADER_SET_SEQUENCE(m_pAudioPacket, m_usAudioSequence++);
        AUDEC_HEADER_SET_TIMESTAMP(m_pAudioPacket, (unsigned int)m_ulLastPacketTimestamp);
        AUDEC_HEADER_SET_CODEC_ID(m_pAudioPacket, X_AUDIO_CODEC_AMR_NB_475);
        int nHeaderSize=AUDEC_HEADER_GET_LEN(m_pAudioPacket);
        memcpy(m_pAudioPacket+nHeaderSize,pData,nLen);
        if(m_pMediaSender) {
            m_pMediaSender->SendAudio((unsigned char*)m_pAudioPacket, nHeaderSize+nLen);
        }
        //      int nRet =  printf("nRet ==>%d",nRet);
    }
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
unsigned long OpenLocalUser::TimeGetTimestamp()
{
#ifdef WIN32
    return ::timeGetTime();
#else
    struct timeval now;
    gettimeofday(&now,NULL);
    return now.tv_sec*1000+now.tv_usec/1000;
#endif
}

void OpenLocalUser::On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)
{
    if (m_pMediaSender&&m_MeetisCapScreen==enum_Camera) {
        if (m_sendVideoQueue) {
            dispatch_sync(m_sendVideoQueue, ^{
                SendVideoData(pData,nLen,bKeyFrame,TimeGetTimestamp(),nWidth,nHeight);
            });
        }
    }
}

void OpenLocalUser::On_MediaReceiverCallbackScreenVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)
{
    if (m_pMediaSender&&m_MeetisCapScreen == enum_Screen) {
        if (m_sendVideoQueue) {
            dispatch_async(m_sendVideoQueue, ^{
                SendVideoData(pData,nLen,bKeyFrame,TimeGetTimestamp(),nWidth,nHeight);
            });
        }
    }
}
