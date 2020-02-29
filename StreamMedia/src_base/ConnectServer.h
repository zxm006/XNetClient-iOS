#ifndef _UU_CMD_TCPCLIENT_H_
#define _UU_CMD_TCPCLIENT_H_

#include "CmdPacket.h"
#include "XNet/XNetCore.h"
#include "XNet/XNetSetting.h"
#include "XNet/XNetClient.h"
//#include "XNet/XNetServer.h"
//#include "XNet/XNetMediaSender.h"
//#include "XNet/XNetMediaReceiver.h"
#include "uuDefine.h"


class CAppClient;

class CmdTCPClientCallback
{
public:
	//返回连接服务端状态
	virtual void On_SessionConnectStatus(CONNECT_NET_STATUS cs) = 0;
	//返回服务端数据
	virtual void OnDispatchCmd(KCmdPacketEx& pPacket) = 0;
};

class ISendCmdServer
{
public:
	virtual int SendData(KCmdPacketEx& pPacket) = 0;
	virtual std::string GetLocalIP() = 0;
	virtual std::string GetNATIP() = 0;
    virtual void SendAudioData(char*pData, int nLen)=0;
};

class CConnectServer
	:public ISendCmdServer
{
public:
	CConnectServer();
	~CConnectServer();
public:
	int Start(const char* ip,unsigned port,bool isEncrypt, CmdTCPClientCallback* pCallback);
	bool Stop();
	virtual int SendData(KCmdPacketEx& pPacket);
	virtual std::string GetLocalIP();
	virtual std::string GetNATIP();
    virtual void SendAudioData(char*pData, int nLen);
private:
	CAppClient* m_pAppClient;
};


#endif
