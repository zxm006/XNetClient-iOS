//libAVCONEC.h
#ifndef __LIBAVCONEC_H__
#define __LIBAVCONEC_H__

#include <AVCONEC/AVCONECC.h>
#include <VIDEC/VIDEC.h>


AFX_EXT_CLASS bool IsAudioHardware(int nChannelIndex);
AFX_EXT_CLASS void SetAudioRecordDevice(unsigned int nDevice);
AFX_EXT_CLASS unsigned int GetAudioRecordDevice(void);
AFX_EXT_CLASS void SetAudioPlayDevice(unsigned int nDevice);

//判断是高清设备
AFX_EXT_CLASS bool IsHDDevice(int nDevID);


//{{xmc add 20070830
AFX_EXT_CLASS int SD_GetFormatCount(void);
AFX_EXT_CLASS bool SD_GetFormatName(int nIndex,char*szName);
AFX_EXT_CLASS bool SD_GetFormatSize(int nIndex,int&nWidth,int&nHeight);

AFX_EXT_CLASS int HD_GetFormatCount(void);
AFX_EXT_CLASS bool HD_GetFormatName(int nIndex,char*szName);
AFX_EXT_CLASS bool HD_GetFormatSize(int nIndex,int&nWidth,int&nHeight);
//}}


class IHPVideoWindow
{
public:
	IHPVideoWindow(void){};
	virtual~IHPVideoWindow(void){};
public:
	virtual unsigned short GetWidth(void)=0;
	virtual unsigned short GetHeight(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPos(int x,int y,int cx,int cy)=0;
	virtual HWND GetHWnd(void)=0;
};

class IHPAVCaptureNotify
{
public:
	IHPAVCaptureNotify(void){};
	virtual~IHPAVCaptureNotify(void){};
public:
	virtual void OnIHPAVCaptureNotifyOutputAudioData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIHPAVCaptureNotifyOutputVideoData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIHPAVCaptureNotifyOutputSubVideoData(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnIHPAVCaptureNotifyOutputQSubVideoData(unsigned char*pPacketData,int nPacketLen)=0;
	//nStreamType[0x01:main 0x02:sub 0x04:qsub]
	virtual bool OnIHPAVCaptureNotifyVideoCapture(int nStreamType)=0;//return bool,if true,encoder current frame,else skip it
	virtual void OnIHPAVCaptureNotifyMotionDetected(int*pResultList,int nResultCount)=0;//pResultList[i]==0,没有动，否则在动
};

class IHPAVCapture
	: public IHPVideoWindow
{
public:
	IHPAVCapture(void){};
	virtual~IHPAVCapture(void){};
public:
	virtual void ReleaseConnections(void)=0;

	virtual void OpenAudioCapture(HP_AUDIO_CODEC_TYPE HPAudioCodec,bool bUseMic=false)=0;
	virtual bool HasAudioCapture(void)=0;
	virtual void ResetAudioParam(HP_AUDIO_CODEC_TYPE HPAudioCodec,bool bUseMic=false)=0;
	virtual void CloseAudioCapture(void)=0;
	virtual int GetAudioBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual void PlayDTMF(int nEventNumber)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	/*
	bEnableSub:是否启动子流,缺省为false
	nSubScaleType:子流图像缩小比率，0:不缩小,1:缩小1/4,2:缩小1/8,缺省为1
	nSubQuality:主流图像质量,定义同主流图像质量定义，缺省为0:64kbps
	nNetworkType,参见VIDEC.h中的VIDEC_BITRATE_CONTROL_VBR等定义
	*/
	virtual void OpenVideoCapture(int nVideoCaptureDevice,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE codecType,int nNetworkType,bool bSub=false,bool bQSub=false)=0;
	virtual bool HasVideoCapture(void)=0;
	virtual void ResetVideoCaptureParam(int nVideoCaptureDevice,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE codecType,int nNetworkType,bool bSub=false,bool bQSub=false)=0;
	virtual void CloseVideoCapture(void)=0;
	virtual void CaptureVideoKeyFrame(int nKeyFrame)=0;
	virtual void Denoise(bool bDenoise)=0;
	virtual bool IsDenoised(void)=0;
	virtual int GetVideoBitrate(void)=0;//建议每隔2000毫秒调用一次
	virtual bool HasVideoInputWindow(void)=0;
	virtual void ShowVideoInputWindow(void)=0;
	virtual bool HasVideoAdjustWindow(void)=0;
	virtual void ShowVideoAdjustWindow(void)=0;
	virtual void InvertVideo(bool bInvert)=0;
	virtual bool IsVideoInverted(void)=0;
	virtual bool CaptureToJPGFile(const char*cszJPGFile)=0;

	virtual bool GetMotionImageSize(int&nWidth,int&nHeight)=0;
	virtual bool StartMotionDetect(void)=0;
	virtual void StopMotionDetect(void)=0;
	virtual bool SetupMotionDetect(	int iGrade/*灵敏度等级0-6*/,
									int iFastMotionDetectFps/*高速运动检测的帧间隔 0-12 缺省2 0不作检测*/,
									int iSlowMotionDetectFps/*低速运动检测的帧间隔 13-30 缺省18 0不作检测*/,
									RECT *RectList, int iRectCount,bool bSimple=false)=0;

	virtual bool TVTunerPutChannel(int lChannel/*1-n*/)=0;

	//设置遮掩并启动遮掩
	virtual bool SetupMask(RECT *RectList, int iRectCount)=0;
	//停止遮掩
	virtual bool StopMask(void)=0;

	virtual bool CanSetVideoParam(void)=0;
	virtual void SetVideoParam(int nBrightness=128/*亮度值0-255*/, int nContrast=64/*对比度0-127*/, int nSaturation=64/*饱和度0-127*/, int nHue=0/*色调0-255*/,int nGamma=100/*伽马1-400*/,int nSharpen=0/*锐化0-127*/,bool bSharpenLocalOnly=true)=0;
	
	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual void SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;

	/******************************************************************************
	* EnableOSD
	*描述：设置是否显示OSD
	*输入：	bEnable			-是否显示OSD,为true时显示，否则不显示
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void EnableOSD(bool bEnable)=0;

	/******************************************************************************
	* IsOSDEnable
	*描述：获取是否显示OSD
	*输入：无
	*输出：无
	*返回值：为true时显示，否则不显示
	*****************************************************************************/
	virtual bool IsOSDEnable(void)=0;

	/******************************************************************************
	* SetOSDText
	*描述：设置OSD的文本
	*输入：	cszText			-要显示的文本
			nAlign			-对齐方式，参考VIDEC.h中的VIDEC_OSD_ALIGN_TOP_LEFT的定义
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetOSDText(const TCHAR*cszText,int nAlign=0)=0;

	/******************************************************************************
	* SetOSDFont
	*描述：设置OSD的字体
	*输入：	logfont			-OSD的字体
			crText			-OSD的文本的颜色
			crBk			-OSD的文本的背景颜色
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetOSDFont(LOGFONT logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255))=0;

	/******************************************************************************
	* StartRecordToAVI
	*描述：开始录像
	*输入：	cszPathName		-录像文件名称
			nStandardFormat	-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordToAVI(const char*cszPathName,int nStandardFormat=1)=0;
	
	/******************************************************************************
	* StopRecordToAVI
	*描述：停止录像
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordToAVI(void)=0;

	/******************************************************************************
	* SetScreenWnd
	*描述：设置所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;
};


AFX_EXT_CLASS IHPAVCapture*CreateIHPAVCapture(IHPAVCaptureNotify&rIHPAVCaptureNotify
#ifdef _WIN32
											  ,HWND hWndVideo
#endif
											  );


class IHPAVPlayer
	: public IHPVideoWindow
{
public:
	IHPAVPlayer(void){};
	virtual~IHPAVPlayer(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void InputAudioPacket(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void InputVideoPacket(unsigned char*pPacketData,int nPacketLen)=0;

	virtual bool HasAudio(void)=0;
	virtual void Mute(bool bMute)=0;
	virtual bool IsMute(void)=0;
	virtual void SetVolume(int nNewVolume)=0;
	virtual int GetVolume(void)=0;
	virtual unsigned int GetAudioLostCount(void)=0;
	virtual int GetAudioBitrate(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	virtual bool HasVideo(void)=0;
	virtual unsigned int GetVideoLostCount(void)=0;
	virtual int GetVideoBitrate(void)=0;

	virtual bool CaptureToJPGFile(const char*cszJPGFile)=0;

	virtual void EnableSyn(bool bEnableSyn)=0;
	virtual void SetVideoParam(int nBrightness=128/*亮度值0-255*/, int nContrast=64/*对比度0-127*/, int nSaturation=64/*饱和度0-127*/, int nHue=0/*色调0-255*/,int nGamma=100/*伽马1-400*/,int nSharpen=0/*锐化0-127*/)=0;
	
	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual void SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)=0;

	//获取是否硬件解码错误，提示用户【“超出解码分辨率的限制，无法解码并显示该视频”】
	virtual bool GetHardwareError(void)=0;

	/******************************************************************************
	* StartRecordToAVI
	*描述：开始录像
	*输入：	cszPathName		-录像文件名称
			nStandardFormat	-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordToAVI(const char*cszPathName,int nStandardFormat=1)=0;
	
	/******************************************************************************
	* StopRecordToAVI
	*描述：停止录像
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordToAVI(void)=0;


	/******************************************************************************
	* SetScreenWnd
	*描述：设置所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;
};

AFX_EXT_CLASS IHPAVPlayer*CreateIHPAVPlayer(HWND hWndVideo);

#endif