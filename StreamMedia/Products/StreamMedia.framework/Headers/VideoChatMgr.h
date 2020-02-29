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

@end

#import <Foundation/Foundation.h>

#ifdef __cplusplus
extern "C" {
#endif
    //登录状态回调
    void  setVideoChatDelegate( id<VideoChatDelegate> delegate);
    
    //登录服务器
    void loginVideoChatServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,const char* nickName,double latitude,double longitude,const char* gameid,bool isencrypt);
    void  logoutVideoChatServer();
    
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
