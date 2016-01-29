# Teameeting-MsgServer<br>
MsgServer and MsgServerClient<br>

---

### 项目简介<br>
MsgServer是[Teameeting] (https://github.com/Teameeting) 项目的一个消息服务，用于Teameeting客户端消息发送和消息转发<br>
MsgServer由MsgServer、MsgServerConnector、MsgServerDispatcher和MsgServerMeeting组成<br>
<br>
MsgServer<br>
	MsgServer:提供基于[Darvin Streaming Server](http://dss.macosforge.org/)的CommonUtilitiesLib而构建的基础公用架构<br>
	MsgServerConnector:用于接入客户端消息与客户端保持通信<br>
	MsgServerDispatcher:用于转发消息<br>
	MsgServerMeeting:用于处理Teameeting的消息逻辑<br>

MsgServerClient是Teameeting客户端消息sdk的基础库，并提供Mac上的Demo程序<br>

目前支持Mac和Linux两个系统<br>

---

### 项目编译<br>
参考每个目录下的README.md<br>
有任何问题欢迎联系[zhengxiaokai@dync.cc](zhengxiaokai@dync.cc)<br>

---

### 开原协议<br>
Teameeting iOS app is under the Apache license. See the LICENSE file for more details.<br>
