# 重要提醒
anyRTC 对该版本已经不再维护，如需多人音视频会议，请前往:https://github.com/anyRTC-UseCase/ARCall, 新版本SDK支持上麦人数50人，观看人数不限制，大大提高了视频会议的效率~

新版本一行代码，30分钟即可使应用有音视频能力，新版本开源示例如下：

**[直播连麦，PK连麦，旁路推流开源示例](https://github.com/anyRTC-UseCase/VideoLive)**

**[音频连麦，语音开黑，音频直播开源示例](https://github.com/anyRTC-UseCase/AudioLive)**

**[音频连麦，语音开黑，音频直播开源示例](https://github.com/anyRTC-UseCase/AudioLive)**

**[语聊房，语音开黑，狼人杀等开源示例](https://github.com/anyRTC-UseCase/ARChatRoom)**

**[高仿ClubHouse](https://github.com/anyRTC-UseCase/anyHouse)**

**[在线自习室开源示例](https://github.com/anyRTC-UseCase/StudyLive)**

更多示列请前往**公司网址： [www.anyrtc.io](https://www.anyrtc.io)**

# Teameeting-MsgServer<br>
MsgServer and MsgServerClient and MsgClient Sdks<br>

---

### 项目简介<br>
MsgServer是[AnyRTC](https://github.com/AnyRTC)的消息服务，(以前是[Teameeting] (https://github.com/Teameeting) 项目的一个消息服务，升级后服务于AnyRTC消息)，用于客户端消息发送和消息转发<br>
MsgServer由多个模块构成，基础模块，接入模块，业务模块，存储模块。<br>
<br>
MsgServer<br>
基础模块：
	MsgServer:提供基于[Darvin Streaming Server](http://dss.macosforge.org/)的CommonUtilitiesLib而构建的基础公用框架<br>
接入模块：
	MsgServerConnector:用于接入客户端消息与客户端保持通信<br>
业务模块：
	MsgServerDispatcher:用于转发其他业务消息到接入模块<br>
	MsgServerMeeting:用于处理Teameeting的消息逻辑<br>
	MsgServerRTLive:用于处理直播的消息逻辑<br>
	MsgServerGrouper: 用于群发消息<br>
	MsgServerPusher: 用于推送消息<br>
存储模块：
  MsgServerLogical：用于处理消息的逻辑存储<br>
	MsgServerSequence: 用于产生序列号，供消息存储使用<br>
	MsgServerStorage: 用于存储消息到redis<br>

MsgServerClient是MsgServer客户端消息sdk的基础库，并提供Linux上的Demo程序<br>

MsgServerClientSdk提供MsgServerClient的sdk，目前提供ios和android<br>

服务目前只支持Linux系统, 由于人力时间原因，Mac系统不在支持XCode的编译<br>

---

### 项目编译<br>
参考每个目录下的README.md<br>
有任何问题欢迎联系[zhengxiaokai@dync.cc](zhengxiaokai@dync.cc)<br>

---

### 开原协议<br>
MsgServer is under the GNU GENERAL PUBLIC LICENSE. See the LICENSE file for more details.<br>
