#ifndef _NETWORKMONITORING_H_
#define _NETWORKMONITORING_H_

#ifdef WIN32
#include <WinSock2.h>
#endif
#include "Thread.h"
#include "ConnectServer.h"
#include "CritSec.h"

#define RUNTIMEOUT 5 * 60 * 1000 //5

enum MonitorStatus
{
	MON_IDLE,
	MON_GETON,
	MON_CARRYOUT,
	MON_TIMEOUT,
	MON_ERROR
};

class CMonitoringCallBack
{
public:
	CMonitoringCallBack(){};
	virtual ~CMonitoringCallBack(){};
public:
	virtual void CallBackResult(unsigned long nlUpBandwidth/*B/s*/, unsigned long ulUpTime /*millisecond*/, 
		unsigned long ulDownBandwidth/*B/s*/, unsigned long ulDownTime /*millisecond*/) = 0;
	virtual void CallBackError(unsigned int nStatus) = 0;
};

class CNetMonitoring : public KThread
{
public:
	CNetMonitoring(ISendCmdServer* pSendCallback, CMonitoringCallBack* pMonitoringCallBack);
	~CNetMonitoring();
public:
	bool BeginNetMonitor();
	void StopNetMonitor();
	unsigned int GetMonitorStatus();
public:
	virtual void ThreadProcMain(void);
	void OnDispatchCmd(KCmdPacketEx& pPacket);
private:
	unsigned int    m_nMonitorStatus;
	ISendCmdServer* m_pSendCallback;
	CMonitoringCallBack* m_pMonitoringCallBack;
	bool m_bWorkThreadRun;
	unsigned long m_nlRunTime;
	KCritSec      m_kCritSec;
	unsigned int  m_nDataPacketNum;
	unsigned long m_nFristPacketTime;
	unsigned long m_uAllPacket;
private:
	unsigned long m_nlUpBandwidth;/*B/s*/
	unsigned long m_ulUpTime; /*millisecond*/
	unsigned long m_ulDownBandwidth;/*B/s*/
	unsigned long m_ulDownTime;/*millisecond*/
};

#endif