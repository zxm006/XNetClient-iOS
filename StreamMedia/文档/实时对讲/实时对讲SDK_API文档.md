#iOS 实时对讲SDK API文档
--

###索引
<br>
>####[集成SDK](#api-config)

>####[API 接口](#api)
>>[Unity 初始化](#fun-initUnity)<br>
>>[设置Unity回调](#fun-unityCallback)<br>
>>[设置回调](#fun-callback)<br>
>>[登录对讲服务器](#fun-login)<br>
>>[登出对讲服务器](#fun-logout)<br>
>>[开启对讲](#fun-startSpeak)<br>
>>[关闭对讲](#fun-stopSpeak)<br>
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
	<string>实时对讲功能需要Mic权限</string>
	<key>NSLocationAlwaysAndWhenInUseUsageDescription</key>
	<string>防作弊系统需要访问设备的定位</string>
	<key>NSLocationWhenInUseUsageDescription</key>
	<string>防作弊系统需要访问设备的定位</string>
```

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

###<a name="fun-initUnity"></a> AG_initIntercomForUnity

`void AG_initIntercomForUnity(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail);`

Unity 实时对讲SDK的初始化方法. 启动时调用

####调用示例：
```
	AG_initIntercomForUnity("objName", "func", "func");
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
	“callBackFun”:”InitIntercomForUnity”
}
```

--
<br>


###<a name="fun-unityCallback"></a> AG_setIntercomUnityCallback

`void AG_setIntercomUnityCallback(UNITYSENDMSG unitymsg);`

设置 Unity 的回调

####调用示例：
```
	AG_setIntercomUnityCallback(callback);
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| unitymsg		| String | 是 | Unity 回调方法 |

####回调说明：
无

--
<br>

###<a name="fun-callback"></a> AG_setIntercomCallback

`void AG_setIntercomCallback(AGI_OC_CALLBACK callback, id params);`

设置回调

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| callback		| String | 是 | 回调方法 |
| params			| OC对象 | 是 | 可选参数，回调时会把该参数带过去 |

####回调说明：
无

--
<br>

###<a name="fun-login"></a> AG_loginIntercomServer

`    void AG_loginIntercomServer(const char* gameId, const char* roomId, const char* groupId, const char* userId, const char* nickname, const char* headUrl);`

登录实时对讲的服务器

####调用示例：

```
AG_loginIntercomServer("000000000", "room_0", "group_1", "userid", "nickname", "uri");
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| gameId			| String | 是 | 游戏id 	|
| roomId			| String | 是 | 房间id 	|
| groupId			| String | 是 | 组id 	|
| userId		| String | 是 | 用户id 	|
| nickname		| String | 否 | 用户昵称 |
| headUrl		| String | 否 | 头像图片地址 |

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |   

####回调示例：

成功示例

```
{
	callBackFun : "LoginIntercomServer",
}
```

失败示例

```
{
	“callBackFun”:”LoginIntercomServer”,
	"retMsg":{
				"errorCode":-9004,
			 }
}
```

--
<br>

### <a name="fun-logout"></a>AG_logoutIntercomServer

`void AG_logoutIntercomServer();`

登出实时对讲服务器

####调用示例：
```
	AG_logoutIntercomServer();
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
    callBackFun : "LogoutIntercomServer",
}
```

--
<br>

### <a name="fun-startSpeak"></a> AG_startIntercom

`void AG_startIntercom(bool isSpeakInRoom);`

开启实时对讲

####调用示例：
```
	AG_startIntercom(true);
```

####参数说明：

| 参数  | 类型  | 必填 | 说明|
|:------------- |:---------------:| :-------------:| :-------------|
| isSpeakInRoom		| bool | 是 | true:  对房间内所有人说话<br>false: 对组内成员说话 |


####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  

####回调示例：

```
{
	callBackFun:”StartIntercom”
}
```

--
<br>

### <a name="fun-stopSpeak"></a> AG_stopIntercom

`void AG_stopIntercom();`

结束实时对讲

####调用示例：
```
	AG_stopIntercom();
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
{
	callBackFun : ”StopIntercom”
}
```
--
<br>


### <a name="fun-distance"></a> AG_getDistanceBetweenIntercomUsers

`void AG_getDistanceBetweenIntercomUsers();`

获取房间内成员间的距离 (防作弊). 这个方法需要定位支持

####调用示例：
```
	AG_getDistanceBetweenIntercomUsers();
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
    callBackFun : "GetDistanceBetweenIntercomUsers",
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

### <a name="fun-config"></a> AG_setIntercomParams

`void AG_setIntercomParams(const char* params);`

SDK配置项方法. 以Json格式传入配置参数，支持一次传入多个配置项.

####调用示例：
```
	AG_setIntercomParams("{ \"enableMicLevel\": true, \"key\": \"value\"}");
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
	callBackFun : ”SetIntercomParams”
	retMsg : {
		enableMicLevel : true,
		key : value,
	}
}
```
--
<br>

###<a name="spe-callback">特殊回调：</a>
###<a>对讲状态变化</a>

####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  
| audioStatus	   | int				 | 0：空闲状态，可以开启对讲<br>1：有人正在说话，不能开启对讲<br>2：对讲已开启|
| inRoom		   | int				 | 0：在组内说话<br>1：在房间内说话|
| user			   | String			 | 当前说话的用户 |


####回调示例：
```
{
	“callBackFun”:”IntercomStatusChange”,
	"retMsg":{
				"audioStatus":0,
				"inRoom":0,
				"user":"user1"
			 }
}
```
--
<br>

###<a>对讲室里成员变动</a>
####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| callBackFun   | String          | 回调方法名 |
| retMsg        | String          | 回调信息集 |  
| flag          | int          	 | 0：成员离开 <br>1：成员加入 |  
| user          | String          | 发生变动的用户 |  


####回调示例：
```
{
	"callBackFun":"IntercomMemberChange",
	"retMsg":{
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

###<a>语音音量大小回调</a>
####回调参数说明：
| 参数  | 类型  | 说明|
|:------------- |:---------------:| :--------------|
| level   	| float          | 音量大小 |


####回调示例：
```
{
	callBackFun : "IntercomMicLevel",
    retMsg :     {
        level : 0.0057517415843904018
    }
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
{
	“callBackFun”:”IntercomDoubleLogin”,
	"retMsg":{
				"errorCode":-9007,
			 }
}
```

--
<br>


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

以下的配置项统一使用 [AG_setIntercomParams()](#fun-config) 来配置SDK

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
|	-9002  | 未登录对讲服务器		  |
|  -9003  | 非法参数	|
|  -9004  | 登录失败 		|
|	-9005	| 连接失败（网络）　|
|  -9006  | 服务器错误		|
|	-9007  | 账号在其他设备登录  |
|	-9008  | 没有权限(MIC)  |
|	-9009  | 重复开启对讲 |
|  -9100  | 其他	|	

<br>