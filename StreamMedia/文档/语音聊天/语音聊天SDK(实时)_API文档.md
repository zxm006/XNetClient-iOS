#iOS 语音聊天(实时)SDK API文档
--

###索引
<br>

>####[集成SDK](#api-config)
>####[API 接口](#api)
>>[Unity 初始化](#fun-initUnity)<br>
>>[设置Unity回调](#fun-unityCallback)<br>
>>[设置回调](#fun-callback)<br>
>>[登录语音聊天服务器](#fun-login)<br>
>>[登出语音聊天服务器](#fun-logout)<br>
>>[加入语音聊天室](#fun-join)<br>
>>[离开语音聊天室](#fun-exit)<br>
>>[离开当前聊天室并加入新聊天室](#fun-exitAndJoin)<br>
>>[邀请用户进入聊天](#fun-invite)<br>
>>[开启语音聊天](#fun-startSpeak)<br>
>>[关闭语音聊天](#fun-stopSpeak)<br>
>>[获取房间内成员间的距离（防作弊）](#fun-distance)<br>
>>[配置项方法](#fun-config)<br>
>####[特殊回调](#spe-callback)
>####[SDK 配置项](#SDK-config)
>####[回调错误命令字](#error-command)

<br>
<br>
<br>
***
<br>
<br>
<br>

##<a name="api-config"></a>集成SDK
### <a name="config-buildSetting"></a>1. 添加权限支持

info.plist 文件里添加定位权限. 如果不需要防作弊功能，可以跳过该步骤.

```
	<key>NSMicrophoneUsageDescription</key>
	<string>语音聊天功能需要Mic权限</string>
	<key>NSLocationAlwaysAndWhenInUseUsageDescription</key>
	<string>防作弊系统需要访问设备的定位</string>
	<key>NSLocationWhenInUseUsageDescription</key>
	<string>防作弊系统需要访问设备的定位</string>
```

--

### <a name="config-buildSetting"></a>2. 引入SDK

在需要调用SDK方法的地方加入下面的引用

```
	#import <StreamMedia/StreamMedia.h>
```

--


<br>
<br>
<br>
***
<br>
<br>
<br>


##<a name="api"></a>API 接口
<br>

###<a name="fun-initUnity"></a> AG_initVoicechatForUnity

`void AG_initVoicechatForUnity(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail);`

Unity 语音聊天SDK的初始化方法. 启动时调用

####调用示例：
```
	AG_initVoicechatForUnity("objName", "func", "func");
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| unityObject		| String | 是 | 回调对象|
| UnityFunSucces | String | 是 | 成功回调|
| UnityFunFail   | String | 是 | 失败回调|

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |    

####回调示例：

成功示例

```
{
	“callBackFun”:”InitVoicechatForUnity”
}
```

--
<br>


###<a name="fun-unityCallback"></a> AG_setVoiceUnityCallback

`void AG_setVoiceUnityCallback(UNITYSENDMSG unitymsg);`

设置 Unity 的回调

####调用示例：
```
	AG_setVoiceUnityCallback(callback);
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| unitymsg		| String | 是 | Unity 回调方法 |

####回调说明：
无

--
<br>

###<a name="fun-callback"></a> AG_setVoiceCallback

`void AG_setVoiceCallback(AG_SM_OC_CALLBACK callback, id params);`

设置回调

####调用示例：
```
	AG_setVoiceCallback(callback, NULL);
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| callback		| String | 是 | 回调方法 |
| params			| OC对象 | 是 | 可选参数，回调时会把该参数带过去 |

####回调说明：
无

--
<br>

###<a name="fun-login"></a> AG_loginVoicechat

`void AG_loginVoicechat(const char* gameId, const char* userId, const char* nickname, const char* headImageURL);`

登录语音聊天服务器

####调用示例：

```
	AG_loginVoicechat("0000", "123123", "昵称", "http://123.jpg");
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| gameId			| String | 是 | 游戏id 	|
| userId			| String | 是 | 用户id 	|
| nickname		| String | 是 | 昵称 	|
| headImageURL	| String | 否 | 头像图片URL |


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |   

####回调示例：

成功示例

```
{
	“callBackFun”:”LoginVoicechat”
}
```

失败示例

```
{
	“callBackFun”:”LoginVoicechat”,
	"errorCode": - 9005,
}
```

--
<br>

### <a name="fun-logout"></a>AG_logoutVoicechat

`void AG_logoutVoicechat();`

登出语音聊天服务器

####调用示例：
```
	AG_logoutVoicechat();
```

####参数说明：

无

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |   

####回调示例：

成功示例

```
{
	“callBackFun”:”LogoutVoicechat”
}
```
--
<br>

###<a name="fun-join"></a> AG_joinVoicechat

`void AG_joinVoicechat(const char* chatId);`

加入语音聊天室

####调用示例：

```
	AG_joinVoicechat("room1");
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| chatId			| String | 是 | 聊天室id |


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  
| existUsers    | list            | 房间内当前的成员列表 |  

####回调示例：

成功示例

```
{
    callBackFun : JoinVoicechat,
}
```

--
<br>

###<a name="fun-exit"></a> AG_exitVoicechat

`void AG_exitVoicechat();`

加入语音聊天室

####调用示例：

```
	AG_exitVoicechat("room1");
```

####参数说明：

无


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |   

####回调示例：

成功示例

```
{
	callBackFun : ”ExitVoicechat”
}
```

--
<br>


###<a name="fun-exitAndJoin"></a> AG_exitAndJoinVoicechat

`void AG_exitAndJoinVoicechat(const char* chatId);`

退出当前语音聊天，并加入新的聊天室

####调用示例：

```
	AG_exitAndJoinVoicechat("room1");
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| chatId			| String | 是 | 聊天室id |


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 | 
| existUsers    | list            | 房间内当前的成员列表 |   

####回调示例：

成功示例

```
{
    callBackFun : exitAndJoinVoicechat
}
```

--
<br>

###<a name="fun-invite"></a> AG_inviteVoicechat

`void AG_inviteVoicechat(const char* chatId, const char* userId);`

加入语音聊天室

####调用示例：

```
	AG_inviteVoicechat("room1", "112233");
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| chatId			| String | 是 | 聊天室id |
| userId			| String | 是 | 用户id |


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |   

####回调示例：

成功示例

```
{
	callBackFun : ”InviteVoicechat”
}
```

--
<br>


### <a name="fun-startSpeak"></a> AG_startVoicechatAudio

`void AG_startVoicechatAudio();`

开启语音聊天

####调用示例：
```
	AG_startVoicechatAudio();
```

####参数说明：

无


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  

####回调示例：

```
	callBackFun : ”StartVoicechatAudio”
```

--
<br>

### <a name="fun-stopSpeak"></a> AG_stopVoicechatAudio

`void AG_stopVoicechatAudio();`

结束语音聊天

####调用示例：
```
	AG_stopVoicechatAudio();
```
####参数说明：

无

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  

####回调示例：

```
	callBackFun : ”StopVoicechatAudio”
```
--
<br>

### <a name="fun-distance"></a> AG_getDistanceBetweenVoicechatUsers

`void AG_getDistanceBetweenVoicechatUsers();`

获取房间内成员间的距离 (防作弊). 这个方法需要定位支持

####调用示例：
```
	AG_getDistanceBetweenVoicechatUsers();
```
####参数说明：

无

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 配置成功的参数集 |  
| distances	   | list 			 | 房间内所有成员间的距离，数组元素格式为 `"用户1, 用户2, 距离"`, 距离单位为`米`

####回调示例：
```
{
    callBackFun : "GetDistanceBetweenVoicechatUsers",
    retMsg :     {
        "distances" : [
            "user1, user2, 1.000000",
            "user1, user3, 5.000000",
            "user2, user2, 1000.000000"]
    }
}
```
--
<br>

### <a name="fun-config"></a> AG_setVoicechatParams

`void AG_setVoicechatParams(const char* params);`

SDK配置项方法. 以Json格式传入配置参数，支持一次传入多个配置项.

####调用示例：
```
	AG_setVoicechatParams("{ \"enableMicLevel\": true, \"key\": \"value\"}");
```
####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| params		| string | 是 | 以Json格式设置各个功能选项，具体见 [SDK 配置项](#SDK-config) |

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 配置成功的参数集 |  

####回调示例：

```
{
	callBackFun : ”SetVoicechatParams”
	retMsg : {
		enableMicLevel : true,
		key : value,
	}
}
```
--
<br>

###<a name="spe-callback">特殊回调：</a>

###<a>聊天室成员变动</a>

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  
| flag		   	   | int				 | 0：成员离开 <br>1：成员加入|
| members		   | list			 	 | 变动的成员列表 |


####回调示例：
```
{
	callBackFun :”VoicechatMemberUpdate”,
    retMsg:{
        "flag":1,
        "members":[
                        {
                            "userId":"t9gg",
                            "nickname":"昵称",
                            "headImageUrl":"http://123.jpg"
                        },
                        {
                            "userId":"user2",
                            "nickname":"昵称",
                            "headImageUrl":"http://123.jpg"
                        }
                    ]
            }
}
```
--
<br>

###<a>聊天室内的语音状态</a>

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  
| isOpen		   | int				 | 0：结束说话 <br>1：开始说话|
| userId		   | String			 | 用户id |


####回调示例：
```
{
    callBackFun : VoicechatStatusUpdate;
    retMsg :   {
        			isOpen = 0;
        			userId = 1010101;
    		   }
}
```
--
<br>

###<a>语音音量大小回调</a>
####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| level   	| float          | 音量大小 |


####回调示例：
```
	callBackFun : "VoicechatMicLevel",
    retMsg :     {
        level : 0.0057517415843904018
    }
```

--
<br>

###<a>账号在其他设备登录</a>

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  


####回调示例：
```
	“callBackFun”:”VoicechatDoubleLogin”,
	"retMsg":{
				"errorCode":-9007,
			 }
```

<br>
<br>
<br>
***
<br>
<br>
<br>

## <a name="SDK-config"></a>SDK 配置项
<br>
<br>

以下的配置项统一使用 [AG_setVoicechatParams()](#fun-config) 来配置SDK

| key 		| 	类型   		|   说明  		|
|:-------:| :--------: | :---------- |
|  enableMicLevel  | bool  | 语音的音量大小回调 |


<br>
<br>
<br>
***
<br>
<br>
<br>



## <a name="error-command"></a>回调错误命令字

<br>
<br>

| Command | 			错误说明		  |
|:-------:| :--------------------- |
|  -9001  | 未初始化SDK  |
|	-9002  | 未登录语音聊天服务器		  |
|  -9003  | 非法参数	|
|  -9004  | 登录失败 		|
|	-9005	| 连接失败（网络）　|
|  -9006  | 服务器错误		|
|	-9007  | 账号在其他设备登录 |
|	-9008  | 没有权限(MIC)  |
|  -9009  | 不能重复登录 |
|	-9010  | 同时只能进入一个聊天室  |
|  -9011  | 未加入聊天室时，不能调用相关方法 |
|  -9100  | 其他	|	

<br>