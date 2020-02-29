#ifndef __UU_DEFINE_H_
#define __UU_DEFINE_H_
 

#include <string>
#include <map>
#import <Foundation/Foundation.h>
#include <assert.h>
#import <UIKit/UIKit.h>
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
    ,ulLoginTime(0)
    {}
    std::string   strUserName;
    std::string strHeadUrl;
    std::string strNickName;
    double ulLatitude;
    double ulLongitude;
    long ulMtgTime;
    unsigned long ulUserId;
    unsigned long ulUserAudioID;
    unsigned long ulLoginTime;
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
@protocol VideoChatDelegate <NSObject>

@required
/**
 调用loginMtgServer()后的回调
 @param cs 链接状态
 */
- (void) IConnectStatusCallback:(CONNECT_NET_STATUS) cs;

@optional


/**
 当有用户进入之后，会为该用户生成一个视频渲染的view
 @param view 渲染视频的视图
 @param userid 对应用户的用户id
 */
- (void) UserVideoView:(UIView*) view  userid:(NSString*)userid;

/**
 当有用户改变他的音频流或者视频流的事件回调
 @param strName 调用用户的唯一标示
 @param isvideo 改变的媒体流  true是视频，false为音频
 @param isopen  媒体流是否开启该流
 */
- (void)  UserMediaStatus:(std::string) strName isvideo:(bool) isvideo isopen:(bool) isopen;

/**
 会议的状态改变的回调
 @param mtgCmd 会议的状态
 @param result 改变是否成功
 @param info   文字信息
 */
- (void) VideoChatInfoReg:(MTG_CMD) mtgCmd result:(bool) result info:(std::string &)info;


- (void) VideoChatInfo:(MTG_CMD) mtgCmd username:(std::string) username info:(std::string &)info;

- (void)VideoChatInvite:(std::string) username  mtgId:(std::string) mtgId;

- (void)VideoChatJoin:(CLIENTUSERINFOLIST &)userinfo;

- (void)VideoChatExit:(CLIENTUSERINFOLIST &)userinfo;

- (void)VideoChatUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;


#pragma mark - 暂时用不上
- (void) INetReceiveUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;


- (void)INetReceiveUserLogin:(unsigned long )uPeerUserID  strName:(std::string) strName useraudioID:(unsigned long) useraudioID;

- (void)INetReceiveUserLogOut:(unsigned long )uPeerUserID;

- (void)INetReceiveData:(unsigned long) uPeerUserID   strName:(std::string) strName  strData:(std::string )strData nLen:(unsigned long )nLen;

- (void)INetBroadcastData:(unsigned long) uPeerUserID pData:( const char*) pData nLen:(unsigned long )nLen;

- (void) INetVideoCall:(unsigned long) uPeerUserID videocall:(bool) videocall  strName:(std::string) strName ;

- (void) videoChatSnapshot:(UIImage*) image;

@end

#endif

