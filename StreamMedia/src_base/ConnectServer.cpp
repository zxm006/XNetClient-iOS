// TestClient.cpp : 定义控制台应用程序的入口点
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#ifdef WIN32
#include <assert.h>
#endif

//#include "XNet/XNetCore.h"
//#include "XNet/XNetSetting.h"
#include "XNet/XNetClient.h"
//#include "XNet/XNetServer.h"
#include "ConnectServer.h"
#include "AppClient.h"



CConnectServer::CConnectServer()
{
	m_pAppClient = NULL;

}
CConnectServer::~CConnectServer()
{
	
}

int CConnectServer::Start(const char* ip,unsigned port,bool isEncrypt, CmdTCPClientCallback* pCallback)
{
	assert(ip);
	assert(pCallback);

	if (m_pAppClient == NULL)
	{
		m_pAppClient = (CAppClient*)new CAppClient();
		assert(m_pAppClient);

		((CAppClient*)m_pAppClient)->SetFunPointer(pCallback);
		return ((CAppClient*)m_pAppClient)->Open(ip,port,isEncrypt);
	}
	return 0;
}

bool CConnectServer::Stop()
{
	if(m_pAppClient == NULL)
		return false;
	else{
		((CAppClient*)m_pAppClient)->Close();
		delete m_pAppClient;
		m_pAppClient = NULL;
	}
	return true;
}

  
void CConnectServer::SendAudioData(char*pData, int nLen)
{
    
}

int CConnectServer::SendData( KCmdPacketEx& pPacket)
{
    if(m_pAppClient == NULL)
        return 0;
    
	std::string data = pPacket.GetString();
	return  (int)((CAppClient*)m_pAppClient)->SendData(data.c_str(),(int)data.length()+1);
}

std::string CConnectServer::GetLocalIP()
{
	if(m_pAppClient)
		return ((CAppClient*)m_pAppClient)->GetLocalIP();
	return "";
}

std::string CConnectServer::GetNATIP()
{
	if(m_pAppClient)
		return ((CAppClient*)m_pAppClient)->GetNATIP();
	return "";
}


