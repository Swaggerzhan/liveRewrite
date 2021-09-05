# RTSP server


#### learn:
    
std::search使用
    
```C++
// std::search
// 一种比较好的寻找CRLF的方式
const char* crlf = "\r\n";
char* buf = new char[1024]; // data
const char* find_crlf = std::search(buf, buf+1024, crlf, crlf+2);
// 如果找到crlf，则返回指针首部，否则返回buf+1024
// std::search(迭代器头，迭代器尾，迭代器头，迭代器尾);
// 配合buffer的设计，0 <= readIndex <= writeIndex < size
// readIndex即解析到的索引，writeIndex即收到的索引，那么就可以写出
const char* find_crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
```

类似的，还有std::find_end的使用

```C++
// std::find_end，找到其中最后一个数值出现的位置
const char* crlf = "\r\n";
const char* data = "123\r\n456\r\n789\r\nabc";
const char* find_crlf = std::find_end(data, data+strlen(data), crlf, crlf+2);
// find_crlf将得到最后一个\r\n的开头。
```

## RTSP包格式

RTSP和HTTP协议非常像，同样采用CRLF来进行区分

```
cmd url version CRLF
key : value CRLF
key : value CRLF
CRLF
```

其中有一个字段，CSeq使用来区分每个命令的，一个CSeq请求对应一个CSeq响应，一般从1开始，逐个增加。

## OPTION

option是最为简单的一个命令，主要是用于获取可用的命令。

一个比较简单OPTION请求和响应结构: 

```
OPTION rtsp://127.0.0.1:554 RTSP/1.0\r\n
CSeq: 1\r\n
User-Agent: Fake VLC\r\n
\r\n
```

```
RTSP/1.0 200 OK\r\n
CSeq: 1\r\n
Public: OPTION, DESCRIBE, SETUP, PLAY, TEARDOWN\r\n
Data: Sun, Sep 5 2021 20:28:19 GMT\r\n
\r\n
```

Data其实也可以不回复的。



## DESCRIBE

DESCRIBE用来请求服务器上的流媒体相关信息，对于没有加密的流一次DESCRIBE请求就能得到信息了，如果使用了加密，则需要2次请求才能得到信息。

```
默认带了\r\n
DESCRIBE rtsp://127.0.0.1:554 RTSP/1.0
Accept: application/sdp
CSeq: 2
User-Agent: Fake VLC
```

对于加密的服务器，会返回需要加密的信息

```
RTSP/1.0 401 Unauthorized
CSeq: 2
WWW-Authenticate: Digest....等加密信息
Data: ...
```

客户端收到401后会继续发起请求

```
DESCRIBE rtsp://127.0.0.1:554 RTSP/1.0
Accept: Application/sdp
CSeq: 3
User-Agent: Fake VLC
Authorization: Digest username="admin"....
```

之后服务器将返回对应的媒体信息了

```
RTSP/1.0 200 OK
CSeq: 3
Content-Type: appliction/sdp
Content-Base: rtsp://127.0.0.1:554/
\r\n后将是sdp信息

v=0
o= - <session id> <version> IN IP4 <这里为真正的流媒体服务器，可能和rtsp所在的服务器不同>
s=Media // 这个只是一个名字而已
e=NONE
b=AS:5100 // 表示带宽，单位为kb/s
t=0 0 // 长连接
a=control:<rtps://   这个为rtsp协议所在服务器> // 下面就是具体的媒体的信息了，有video的，也有audio的

m=video 0 RTP/AVP 96
c=IN IP4 0.0.0.0
b=AS:5000
a=recvonly
a=x-dimensions:1920,1080 // 分辨率
a=rtpmap:96 H264/90000 // 未知，应该是码率
a=fmtp:96..... //  未知
m=audio 0 RTP/AVP 8 // 这里开始时音频信息了
c=IN IP4 0.0.0.0
b=AS:50
a=recvonly
a=control:rtsp://.... // 这里是rtsp协议所在的服务器
..略


```


### 其中的sdp信息

sdp即 Session Description Protocol。使用的基本是<key>=<value>这样的格式，并且`=`边没有空格。

一些关于sdp字段的值: 

####  * version (重要)

格式: v=<version>，表示的是sdp的版本。

```sdp
v=0 //表示版本为0
```

#### * origin（重要）

格式: o=<username><sessionid><version><network type><address type><address>

<username>即用户名
<sessionid>则为本次session的对应id
<version>则就是本协议中对应需要传输的数据的版本
<network type>为网络类型
<address type>表示网络类型，一般指的是IPv4或者是IPv6等
<address>表示的是IP地址

```sdp
o= - 1000000 2000000 IN IP4 127.0.0.1 
// 用户名省略为-
 // 1000000则为session id
 // 2000000 则为version
 // IN表示Internet
 // IP4 表示 network type
 // 127.0.0.1则为地址
```

#### * Session Name(重要)

会话名字，一个会话中就只有一个Session Name

格式: s=<session name>

```sdp
s=Media Name
```

#### * Connection Data

连接信息，和origin中的数据类似

格式: c=<network type><address><connection address>

```sdp
c=IN IP4 127.0.0.1
```

#### * Bandwidth

带宽信息，建议的传输速率，单位为kb/s，modifier有2种类型，CT和AS，其中CT表示总带宽，AS表示单个媒体带宽最大值，bandwidth表示具体带宽值

格式: b=<modifier>:<bandwidth-value>

```sdp
b=AS:5100
// 表示单个媒体带宽最大值为5100 kb/s
```

#### * Times (重要)

用来表示开始时间和结束时间的，为NTP时间，其中start time和stop time都为0的话则表示会话是一个持久的会话，如果只有stop为0的话表示过了start time这个值后会话是持久的。单位为秒。

格式: t=<start time><stop time>

```sdp
t=0 0
// 持久的会话
```

#### * email

字如其名

格式: e=<email>

```sdp
e=None
// 没有邮件
```

#### * phone number和url

字如其名

格式: p=<phone number> u=<url>

```sdp
p=1888888 // 电话
u=www.ctfdog.cn // url
```

#### * media information (重要)

一般用来表示媒体信息的

格式: m=<media><port><transport type><fmt list>

media表示媒体类型，也就`audio`和`video`或者`application`以及`data`，还有控制信息`control`。

port就比较重要了，RTSP默认使用UDP来传输视频信息，其中的RTP传输port就是由此定义的，一般偶数port表示传输RTP，奇数port表示传输RTCP。

transport type表示传输的协议的类型，一般使用RTP，那么这个字段将填充为RTP/AVP。

fmt list表示传输的媒体格式，分为静态和动态两种，其中静态表示数据是和RTP载荷一一对应的，比如`m=audio 0 RTP/AVP 8`

```sdp
m=video 50002 RTP/AVP 96 // 音频数据
// 或者
m=audio 50002 RTP/AVP 8 // 视频数据
```


#### * a=(*) (重要)

这个字段可以随意定义属于自己协议的各种key:value的值，不一定要有，但是经常用到

格式: a=<key>:<value>

```sdp
a=control:url // 表示控制的url
a=x-dimensions:1920,1080 // 表示分辨率
//等等各种自定义字段，主要由协议定义
```









