#ifndef __UU_DEFINE_H_
#define __UU_DEFINE_H_
 

#include <string>
#include <map>
#import <Foundation/Foundation.h>
#include <assert.h>

using namespace std;

@protocol MicBlowDelegate <NSObject>

@required
- (void) micBlowAudiolevel:(float) level;
@end


#define  LOCAL_MCUPort  5566
#define  REMOTE_MCUPort 5566

typedef int (*UNITYSENDMSG)(const char *cunityObj,const char* unityFunSucces,const char* msg);
typedef void (*AG_SM_OC_CALLBACK)(const char *retMsg ,id params);


typedef enum {
    CS_CONNECTING=0,		//正在连接
    CS_FAILED,				//无法连接
    CS_CONNECTED,			//已经连接
    CS_DISCONNECTED,		//断开连接
    CS_BUSY,				//网络忙(已断开正重连)
    CS_RECONNECTED,			//重连成功
    CS_IDLE,				//空闲
    CS_RESTARTED,			//连接重置了【连接断开了，并且又重新连接上了，但是换了一个新连接】
    CS_LOGINED,             //登陆服务器成功
    CS_LOGINFAILED,         //登陆服务器失败
    CS_LOGOUT,       //退出服务器
    CS_RELOGIN         //退出服务器
} CONNECT_NET_STATUS;


typedef enum {
    MTG_CREAT=0,		//正在连接
    MTG_JOIN,
    MTG_EXIT,
    MTG_DESTROY
} MTG_CMD;

typedef enum {
    MTG_Common=0,        //正在连接
    MTG_MultIntercom,
  
} MTG_TYPE;

typedef enum
{
    AUDIO_SEND_ENABLE=0,    //可以发送
    AUDIO_SEND_DISABLE,     //不能发送
    AUDIO_SEND_SENDING         //正在发送
    
} AUDIO_SEND_STATUS;

typedef struct _CLIENTUSERINFOLIST
{
    _CLIENTUSERINFOLIST():strUserName("")
    ,strNickName("")
    ,strHeadUrl("")
    ,ulLatitude(0)
    ,ulLongitude(0)
    ,ulUserId(0)
    ,ulUserAudioID(0)
    ,uiVideoCanSee(0)
    ,ulMtgTime(0)
    {}
    std::string   strUserName;
    std::string strHeadUrl;
    std::string strNickName;
    double ulLatitude;
    double ulLongitude;
    long ulMtgTime;
    unsigned long ulUserId;
    unsigned long ulUserAudioID;
    unsigned int  uiVideoCanSee;
    std::string   strLocalIP;
    std::string   strNATIP;
}CLIENTUSERINFOLIST;

typedef std::map<unsigned long, CLIENTUSERINFOLIST> CLIENTUSERINFOLIST_MAP;

static const char *mediaServer_URL = "media.aigamecloud.com";

void startMicBlow(id<MicBlowDelegate> del);
 
void stopMicBlow();

#define IP_SIZE     16

#define LOGI printf

unsigned long XGetTimestamp(void);

#endif
