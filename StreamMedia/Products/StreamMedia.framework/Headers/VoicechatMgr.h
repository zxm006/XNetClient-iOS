//
//  AiYouVoicechatMgr.h
//  MultiLinkMicTalk
//
//  Created by zhangxinming on 17/7/11.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#ifndef AiYouVoicechatMgr_h
#define AiYouVoicechatMgr_h
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "uuDefine.h"



@protocol VoiceChatDelegate <NSObject>

@required
 - (void) IConnectStatusCallback:(CONNECT_NET_STATUS) cs;

@optional
- (void) UserVideoView:(UIView*) view  userid:(NSString*)userid;

- (void)  UserMediaStatus:(std::string) strName isvideo:(bool) isvideo isopen:(bool) isopen;

- (void) VoicechatInfoReg:(MTG_CMD) VoicechatCmd result:(bool) result info:(std::string &)info;

- (void) VoicechatInfo:(MTG_CMD) VoicechatCmd username:(std::string) username info:(std::string &)info;

- (void)VoicechatInvite:(std::string) username  VoicechatId:(std::string) VoicechatId;

- (void)VoicechatJoin:(CLIENTUSERINFOLIST &)userinfo;

- (void)VoicechatExit:(CLIENTUSERINFOLIST &)userinfo;

- (void) INetReceiveUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;

- (void)VoiceChatUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;

- (void)INetReceiveUserLogin:(unsigned long )uPeerUserID  strName:(std::string) strName useraudioID:(unsigned long) useraudioID;

- (void)INetReceiveUserLogOut:(unsigned long )uPeerUserID;

- (void)INetReceiveData:(unsigned long) uPeerUserID   strName:(std::string) strName  strData:(std::string )strData nLen:(unsigned long )nLen;

- (void)INetBroadcastData:(unsigned long) uPeerUserID pData:( const char*) pData nLen:(unsigned long )nLen;
 
- (void) INetVideoCall:(unsigned long) uPeerUserID videocall:(bool) videocall  strName:(std::string) strName ;

@end

#import <Foundation/Foundation.h>

#ifdef __cplusplus
extern "C" {
#endif
    //登录状态回调
    void  setVoicechatDelegate( id<VoiceChatDelegate> delegate);
    
    //登录服务器
    void loginVoicechatServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,const char* nickName,double latitude,double longitude,const char* gameid,bool isencrypt);
    void  logoutVoicechatServer();
    
    void creatVoicechat(std::string Voicechatid);
    void joinVoicechat(std::string Voicechatid);
    void exitVoicechat();
    void destroyVoicechat();
    
    void setVoicechatSession();
    void VoicechatInviteUser(const char* VoicechatId, const char* iuserId);
 
    
    
    void startVoicechatAudio();
    void stopVoicechatAudio();
    
    void openVoicechatRemoteUserAudio(const char* username);
    void closeVoicechatRemoteUserAudio(const char* username);
 
    
 
    
#ifdef __cplusplus
}
#endif

#endif /* AiYouVoicechatMgr_h */
