//
//  AiYouStreamMediaMgr.h
//  MultiLinkMicTalk
//
//  Created by zhangxinming on 17/7/11.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#ifndef AiYouStreamMediaMgr_h
#define AiYouStreamMediaMgr_h
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "uuDefine.h"



@protocol VideoChatDelegate <NSObject>

@required
 - (void) IConnectStatusCallback:(CONNECT_NET_STATUS) cs;

@optional
- (void) UserVideoView:(UIView*) view  userid:(NSString*)userid;

- (void)  UserMediaStatus:(std::string) strName isvideo:(bool) isvideo isopen:(bool) isopen;

- (void) MtgInfoReg:(MTG_CMD) mtgCmd result:(bool) result info:(std::string &)info;

- (void) MtgInfo:(MTG_CMD) mtgCmd username:(std::string) username info:(std::string &)info;

- (void)MtgInvite:(std::string) username  mtgId:(std::string) mtgId;

- (void)MtgJoin:(CLIENTUSERINFOLIST &)userinfo;

- (void)MtgExit:(CLIENTUSERINFOLIST &)userinfo;

- (void)MtgUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;

- (void) INetReceiveUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;


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
    void  setVideoChatDelegate( id<VideoChatDelegate> delegate);
    
    //登录服务器
    void loginVideoChatServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,const char* nickName,long latitude,long longitude,const char* gameid,bool isencrypt);
    void  logoutVideoChatServer();
    
    void creatVideoChat(std::string mtgid, bool openCamera);
    void joinVideoChat(std::string mtgid, bool openCamera);
    void exitVideoChat();
    void destroyVideoChat();
    
    void setVideoChatSession();
    void VideoChatInviteUser(const char* mtgId, const char* iuserId);
    
    void openVideoChatVideoWtihName(const char* username ,int x,int y,int width,int height,bool round);
    void closeVideoChatVideoWtihName(const char *name);
   
    void setVideoChatRemoteUserVideoFrame(const char* username ,int x,int y,int width,int height,bool round);
    
    void openVideoChatLocalVideo();
    void closeVideoChatLocalVideo();
    
    void openVideoChatRemoteUserAudio(const char* username);
    void closeVideoChatRemoteUserAudio(const char* username);
    
    void setVideoChatLocalCamera(bool isFront);
    void  resetVideoChatPreview();
    
    void openVideoChatLocalAudio();
    void closeVideoChatLocalAudio();
    
    //设置美颜，默认开启
    void  setVideoChatBeautifyFace(bool beautifyFace);
    
#ifdef __cplusplus
}
#endif

#endif /* AiYouStreamMediaMgr_h */
