#include "NetworkMonitoring.h"
#include "Utils.h"
#include "AutoLock.h"

CNetMonitoring::CNetMonitoring(ISendCmdServer* pSendCallback, CMonitoringCallBack* pMonitoringCallBack)
:m_pSendCallback(pSendCallback)
,m_pMonitoringCallBack(pMonitoringCallBack)
,m_nMonitorStatus(MON_IDLE)
,m_bWorkThreadRun(false)
,m_nlRunTime(0)
,m_nlUpBandwidth(0)
,m_ulUpTime(0)
,m_ulDownBandwidth(0)
,m_ulDownTime(0)
,m_nDataPacketNum(0)
,m_nFristPacketTime(0)
,m_uAllPacket(0)
{

}

CNetMonitoring::~CNetMonitoring()
{
	StopNetMonitor();
}

bool CNetMonitoring::BeginNetMonitor()
{
	if(m_pSendCallback == NULL || m_pMonitoringCallBack == NULL)
		return false;
	if(m_nMonitorStatus == MON_GETON || m_bWorkThreadRun)
		return false;

	m_nMonitorStatus = MON_GETON;

	m_nlRunTime = GetTimeStamp();
	m_bWorkThreadRun = true;
	StartThread();
	
	return true;
}

void CNetMonitoring::StopNetMonitor()
{
	KAutoLock l(m_kCritSec);
	if(m_bWorkThreadRun)
	{
		m_bWorkThreadRun = false;
		WaitForStop();
		m_nMonitorStatus = MON_IDLE;
	}
}

unsigned int CNetMonitoring::GetMonitorStatus()
{
	return m_nMonitorStatus;
}

void CNetMonitoring::ThreadProcMain(void)
{
	char cTextData[102400];
	memset(cTextData, 0x45, 102400);
	std::string strData = cTextData;

	{
		KAutoLock l(m_kCritSec);

		std::string strUplinkTest = "UplinkTest";
		KCmdPacketEx rPacket(strUplinkTest.c_str(),(int)strUplinkTest.length()+1);
		std::string strCMD = "UPLINKTEST";
		rPacket.SetCMD(strCMD);
		std::string strType = "TESTTYPE";
		rPacket.SetAttrib(strType,"TESTBEGIND");
		m_pSendCallback->SendData(rPacket);

#ifdef WIN32
		Sleep(100);
#else
		usleep(100*1000);
#endif

		for(int j = 0; j < 15; j++)
		{
			std::string strUplinkTest = "UplinkTest";
			KCmdPacketEx rPacket(strUplinkTest.c_str(),(int)strUplinkTest.length()+1);
			std::string strCMD = "UPLINKTEST";
			rPacket.SetCMD(strCMD);
			std::string strType = "TESTTYPE";
			rPacket.SetAttrib(strType,"TESTDATA");
			rPacket.SetAttrib("DATA", strData);
			m_pSendCallback->SendData(rPacket);
		}
	}

	while (m_bWorkThreadRun)
	{
		if(( GetTimeStamp() - m_nlRunTime ) >  RUNTIMEOUT)
		{
			KAutoLock l(m_kCritSec);
			m_bWorkThreadRun = false;
			m_nMonitorStatus = MON_TIMEOUT;
			if(m_pMonitoringCallBack)
				m_pMonitoringCallBack->CallBackError(m_nMonitorStatus);
			break;
		}

		if(m_nMonitorStatus == MON_CARRYOUT)
		{
			KAutoLock l(m_kCritSec);
			if(m_pMonitoringCallBack)
				m_pMonitoringCallBack->CallBackResult(m_nlUpBandwidth, m_ulUpTime, m_ulDownBandwidth, m_ulDownTime);
			m_bWorkThreadRun = false;
			break;
		}
#ifdef WIN32
		Sleep(200);
#else
		usleep(200*1000);
#endif
	}
}

void CNetMonitoring::OnDispatchCmd(KCmdPacketEx& pPacket)
{
	KAutoLock l(m_kCritSec);

	std::string strCMD = pPacket.GetCMD();
	std::string strData = pPacket.GetString();
	printf("Get TCP Server Packet packet Value = '%s'\n" ,strData.c_str());

	if(strCMD=="UPLINKTESTEND")
	{
		m_nlUpBandwidth = pPacket.GetAttrib("UPBANDWIDTH").AsUnsignedLong();
		m_ulUpTime = pPacket.GetAttrib("UPTIME").AsUnsignedLong();
	}
	else if(strCMD=="DOWMLINKTEST")
	{
		std::string strInfo = pPacket.GetAttrib("TESTTYPE").AsString();

		if(strInfo == "TESTBEGIND")
		{
			m_nDataPacketNum = 0;
			m_uAllPacket = 0;
			m_nFristPacketTime = GetTimeStamp();
		}
		else if(strInfo == "TESTDATA")
		{
			++m_nDataPacketNum;
			std::string data = pPacket.GetString();
			m_uAllPacket += data.length();

			if(m_nDataPacketNum == 15)
			{
				unsigned long nAllTime = GetTimeStamp() - m_nFristPacketTime;

				m_ulDownBandwidth = (m_uAllPacket * 1000) / nAllTime;
				m_ulDownTime = nAllTime / 15;
				m_nMonitorStatus = MON_CARRYOUT;
			}
		}
		
	}
}
