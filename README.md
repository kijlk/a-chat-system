# a-chat-system

### **Requirements**

Design and implement a chat system that allows multiple groups of users to chat. A chat coordinator resides at a well-known network address, uses UDP for communication with chat clients, sets up chat servers for each chat session, and maintains a chat session directory. There is one chat server per chat session. A chat sever uses TCP for communication with clients. A chat client allows users to start, join, and leave a chat session. Design and implement the coordinator, server, and client code.

### **Analysis**

设计并实现一个聊天系统，允许多组用户聊天。

聊天协调器位于一个众所周知的网络地址，使用UDP与聊天客户端通信，为每个聊天会话建立聊天服务器，并维护一个聊天会话目录。（聊天协调器在一个well known网址，设置为0.0.0.0/1000）

 每个聊天会话有一个聊天服务器。

聊天服务器使用TCP与客户端通信。

聊天客户端允许用户开始、加入和离开聊天会话。

对于TCP服务端，用代码构建有以下几个步骤:
 (1)启动网络库;
 (2)绑定服务端套接字;
 (3)监听服务端套接字;
 (4)服务端套接字接收客户端连接，之后即可自由通信；

对于客户端:(1)启动网络库;
 (2)向协调器发送请求
 (3)连接服务端套接字，连接成功即可自由通信

对于协调器:
 (1)启动网络库;
 (2)绑定套接字;
 (3)回应客户端请求
 (4)若是create，则创建服务端。若是join，则将其加入对话队列
