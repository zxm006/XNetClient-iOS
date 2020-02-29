//
//  IntercomManage.h
//  IntercomTalk
//
//  Created by zhangxinming on 16/7/21.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#ifndef IntercomManage_h
#define IntercomManage_h

#include "uuDefine.h"
#import <Foundation/Foundation.h>

#ifdef __cplusplus

extern "C" {
#endif
    
    @protocol IntercomDelegate <NSObject>
    
    @required
    
    - (void) IConnectStatusCallback:(CONNECT_NET_STATUS) cs;
    
    @optional
    
    - (void) INetReceiveUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList;
    
    - (void)INetReceiveUserLogin:(CLIENTUSERINFOLIST &)userinfo;
    
    - (void)INetReceiveUserLogOut:(CLIENTUSERINFOLIST &)userinfo;
    
    - (void)INetReceiveData:(unsigned long) uPeerUserID   strName:(std::string) strName  strData:(std::string )strData nLen:(unsigned long )nLen;
    
    - (void)INetBroadcastData:(unsigned long) uPeerUserID pData:( const char*) pData nLen:(unsigned long )nLen;
    - (void) INetAudioStatus:(unsigned long )uPeerUserID isRoom:(bool) isRoom AudioStatus:(AUDIO_SEND_STATUS )AudioStatus strName:(std::string) strName;
 - (void)LoginTime:(unsigned long) loginTime;
  @end
       
     //登录对讲服务器
    void  loginIntercomServer(const char* szServerHostURL,const char* szUserid, const char* headUrl, const char* nickName,double latitude,double longitude,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId,BOOL listenInRoom,const char* szexpand,bool isencrypt);
 
    void  logoutIntercomServer();
    
    void  startIntercomAudio(BOOL isspeakinroom);
    void  stopIntercomAudio();
 
    void setIntercomAudioSession();
    
    int SendIntercomDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen);
    //底层测试接口，和上层应用无关
    
    void  setIntercomDelegate(id<IntercomDelegate> delegate);
    
#ifdef __cplusplus
}
#endif

#endif /* IntercomManage_h */
