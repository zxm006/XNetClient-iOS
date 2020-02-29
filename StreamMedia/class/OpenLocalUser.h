#ifndef OPENLOCALUSER_H
#define OPENLOCALUSER_H

#include <string>
#include "XNet/XNetMediaSender.h"
#include "VideoLibrary.h"

enum enum_videoType
{
    enum_Screen=0,
    enum_Camera,
    enum_No
};


class OpenLocalUser : public XNetMediaSenderCallback, public MediaCameraVideoCallBack, public MediaScreenVideoCallBack
{
public:
     OpenLocalUser();
    ~OpenLocalUser();
public:
    bool ConnectMediaServer(std::string strServerIp, unsigned short nPort);
    void ReleaseMediaSever();

    bool OpenAudioSend(unsigned int ulUserAudioId);
    bool CloseAudioSend();
	bool SendAudioData(char*pData, int nLen);
    
    bool OpenVideoSend(unsigned int ulUserVideoId);
    bool CloseVideoSend();
    void openLocalVideo(UIView *localvideo,int resType,bool isCapScreen);
    
    void closeLocalVideo();
    void reOrientation();
    void openBeautify();
    void closeBeautify();
    
    void setEncodebitrate(long bitrate);
    
    bool SendVideoData(unsigned char*pData, int nLen, bool bKeyFrame, unsigned long       ulTimestamp, int nWidth, int nHeight);
    
    void switchCamera();
    
    virtual void On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight);
    
    void On_MediaReceiverCallbackScreenVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight);
private:
	bool CreateSender();
	void ReleaseSender();
    void ProcessVideoFrame(unsigned char*pData, int nLen, bool bKeyFrame, unsigned long ulTimestamp, int nWidth, int nHeight);
    void ProcessAudioFrame(char*pData, int nLen, unsigned long ulTimestamp);
    unsigned long TimeGetTimestamp();
    
    bool  OpenCapScreen(int resType);
    bool  CloseCapScreen();
    
private:
	bool m_bGotKeyFrameMain;
	char* m_pVideoPacket;
	int	m_nFrameBufferLength;
	unsigned short m_usVideoSequence;
    enum_videoType  m_MeetisCapScreen;
    char* m_pAudioPacket;
    int m_nAudioFrameBufferLength;
    unsigned short m_usAudioSequence;
    unsigned long m_ulLastFrameTimestamp;
    unsigned long m_ulLastPacketTimestamp;
private:
    XNetMediaSender*		m_pMediaSender;
    VideoCapture*           m_pVideoCapture;
    VideoCapScreen*         m_VideoCapScreen;
    unsigned int           m_ulLocalAudioID ;			//AudioID
    unsigned int           m_ulLocalVideoID ;
    std::string             m_MediaServerIp;
	unsigned short          m_nPort;
    dispatch_queue_t m_sendAudioQueue;
    dispatch_queue_t m_sendVideoQueue;
};

#endif
