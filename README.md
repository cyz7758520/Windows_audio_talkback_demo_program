# Windows下音视频对讲演示程序
# 必读说明

# 简介
&emsp;&emsp;本软件根据《道德经》为核心思想而设计，实现了两个设备之间通过TCP或UDP协议进行实时半双工和实时全双工音视频对讲，并做了以下增强处理：  
&emsp;&emsp;* 支持8000Hz、16000Hz、32000Hz、48000Hz的音频。  
&emsp;&emsp;* 支持声学回音消除，通过本人自己设计的音频输入输出帧同步方法、自适应设置回音延迟方法、三重声学回音消除器，声学回音可以消除到99%以上，还可以消除同一房间回音，且收敛时间很短，无论网络如何抖动都可以消除。  
&emsp;&emsp;* 支持噪音抑制，对常见的底噪音、嘈杂的街道音、风吹音、等都有抑制效果。  
&emsp;&emsp;* 支持语音活动检测，只有在人说话时才发送网络数据，无人说话时不产生网络数据，从而降低噪音、降低网络流量。  
&emsp;&emsp;* 支持自动增益控制，当人说话声音较小时会自动增大音量，当人说话声音较大时会自动减小音量。  
&emsp;&emsp;* 支持音频编解码，对音频数据的压缩率在1~20%之间，且支持动态比特率，从而大幅度降低网络流量，还支持数据包丢失隐藏，当网络丢包率高达30%时，仍然可以进行对讲。  
&emsp;&emsp;* 支持保存音频到文件和绘制音频波形到窗口，可以直观且方便的调试音频。  
&emsp;&emsp;* 支持视频软编解码，支持指定比特率，最低到10KB/s仍然可以进行视频对讲。  
&emsp;&emsp;* 支持音视频自适应抖动缓冲，当网络存在丢包、乱序、延时等抖动情况时，通过自适应调节缓冲深度来应对这些抖动。  
&emsp;&emsp;* 支持自定义调节各种功能的参数来适应不同的设备，绝大部分情况下都不需要修改。  
&emsp;&emsp;* 支持保存音视频输入输出到文件。  
&emsp;&emsp;* 支持与Android下音视频对讲演示程序进行音视频对讲。  

&emsp;&emsp;声学回音消除器效果对比：
|名称|收敛时间|回音延迟不稳定|残余回音|远近端同时说话|同一房间对讲|运算量|
|:---|:---|:---|:---|:---|:---|:---|
|Speex声学回音消除器|有语音活动1~3秒|0~3秒自适应调节|回音延迟稳定时没有<br>回音延迟不稳定时有很大|近端语音被消除20%|会产生一定回音|一般|
|WebRtc定点版声学回音消除器|0秒|延迟400ms以内0秒自适应调节<br>延后超过400ms将无法消除|回音延迟稳定时没有<br>回音延迟不稳定时偶尔有一丝丝，偶尔有很大|近端语音被完全消除|会产生较大回音|一般|
|WebRtc浮点版声学回音消除器|有语音活动1秒|0秒自适应调节|回音延迟稳定时没有<br>回音延迟不稳定时偶尔有一丝丝|近端语音被消除50%|会产生较小回音|较大|
|Speex声学回音消除器<br>+WebRtc定点版声学回音消除器|0秒|0~3秒自适应调节|回音延迟稳定时没有<br>回音延迟不稳定时有很大|近端语音被消除20%|会产生一定回音|一般|
|WebRtc定点版声学回音消除器<br>+WebRtc浮点版声学回音消除器|0秒|0秒自适应调节|回音延迟稳定时没有<br>回音延迟不稳定时偶尔有一丝丝|近端语音被消除50%|会产生较小回音|较大|
|Speex声学回音消除器<br>+WebRtc定点版声学回音消除器<br>+WebRtc浮点版声学回音消除器|0秒|0秒自适应调节|回音延迟稳定时没有<br>回音延迟不稳定时极低概率会有一丝丝|近端语音被消除50%|会产生很小回音|很大|

&emsp;&emsp;特别注意：以上是在不使用系统自带的声学回音消除器的效果，且不同设备或不同环境或不同时间效果都会不同，所以需要自己亲自测试。  

# 准备
&emsp;&emsp;准备两台安装了Windows 7及以上系统的设备（已适配到Windows 11），其中一台设备作为客户端可以连接到另一台作为服务端的设备（可以用Ping工具测试，建议两台设备在同一局域网内），且两台设备都安装相同版本的本软件。

# 开始
&emsp;&emsp;在一台设备上直接点击创建服务端，再在另一台设备上将IP地址改为服务端设备的IP地址，并点击连接服务端，即可开始对讲，在任意一端点击中断，即可中断对讲。  

&emsp;&emsp;设置按钮提供了各项功能的参数设置，绝大部分情况下都不需要修改，当然你也可以根据需要自行修改。  

&emsp;&emsp;**特别注意：如果把两台设备放在同一房间里测试，有可能会出现啸叫、声音不完整、等问题，这是因为现在的麦克风都很灵敏了，一点小小的声音都会被录进去，两台设备会相互录音，导致软件无法正确识别回音，所以建议放在不同的房间里测试。如果实在要测试这种情况，就在设置里，Speex预处理器的其他功能设置里，关闭“使用自动增益控制”后再测试。**  

# 移植
&emsp;&emsp;如果需要在自己的软件中使用本软件的音视频功能，需要以下几个步骤：  
&emsp;&emsp;**C语言：**  
&emsp;&emsp;1、将MediaPocsThrd.h、MediaPocsThrd.cpp和各个库文件夹复制到自己的软件中。  
&emsp;&emsp;2、实现UserInit、UserPocs、UserDstoy、UserMsg、UserReadAdoVdoInptFrm、UserWriteAdoOtptFrm、UserGetPcmAdoOtptFrm、UserWriteVdoOtptFrm、UserGetYU12VdoOtptFrm这九个回调函数。  
&emsp;&emsp;3、调用MediaPocsThrdInit()函数创建并初始化媒体处理线程，然后调用媒体处理线程的相关设置函数，最后调用MediaPocsThrdStart()函数启动媒体处理线程即可。  
&emsp;&emsp;4、当需要媒体处理线程退出时，调用MediaPocsThrdRqirExit()函数即可。  
&emsp;&emsp;**C++语言：**  
&emsp;&emsp;1、将MediaPocsThrd.h、MediaPocsThrd.cpp和各个库文件夹复制到自己的软件中。  
&emsp;&emsp;2、继承MediaPocsThrd媒体处理线程类，实现UserInit、UserPocs、UserDstoy、UserMsg、UserReadAdoVdoInptFrm、UserWriteAdoOtptFrm、UserGetPcmAdoOtptFrm、UserWriteVdoOtptFrm、UserGetYU12VdoOtptFrm这九个回调成员函数。  
&emsp;&emsp;3、new这个继承的类，然后调用类的相关设置成员函数，最后调用start()成员函数启动媒体处理线程即可。  
&emsp;&emsp;4、当需要媒体处理线程退出时，调用MediaPocsThrdRqirExit()函数即可。  

&emsp;&emsp;如果有不需要的部分功能，则只需要删除该功能对应头文件和库文件，然后修改MediaPocsThrd.h、MediaPocsThrd.cpp文件即可。  

&emsp;&emsp;**普通免费功能包括：WebRtc定点版声学回音消除器、Speex预处理器的噪音抑制、WebRtc定点版噪音抑制器、WebRtc浮点版噪音抑制器、Speex预处理器的其他功能、Speex编解码器、Wave文件读取器、Wave文件写入器、本端TCP协议服务端套接字、本端TCP协议客户端套接字、本端UDP协议套接字。**  

&emsp;&emsp;**高级收费功能包括：Speex声学回音消除器、WebRtc浮点版声学回音消除器、SpeexWebRtc三重声学回音消除器、RNNoise噪音抑制器、OpenH264编解码器、自己设计的自适应抖动缓冲器、本端高级UDP协议套接字。**  

# 注意
&emsp;&emsp;从老版本更新到新版本时，类文件和库文件全部都要更新，不能只更新类文件或库文件，否则会导致意想不到的问题。  
&emsp;&emsp;如果要使用8000Hz采样频率时，最好不要使用RNNoise噪音抑制器，因为它可能对8000Hz的声音抑制非常强烈。  
&emsp;&emsp;本软件不支持音乐，尤其是RNNoise噪音抑制器可能对音乐的抑制非常强烈。  
&emsp;&emsp;某些Windows设备的软硬件环境可能存在问题，从而可能会导致声学回音消除失败，这种情况必须要先解决这些问题。  
&emsp;&emsp;保存音视频输入输出的AdoVdoInptOtpt.avi文件不能直接播放，需要使用FFmpeg命令转码后才能播放，建议用VLC播放器，转码命令为：ffmpeg -i AdoVdoInptOtpt.avi -filter_complex "[0:a:1][0:a:2]amix=inputs=2:duration=max[aout]" -map [aout] -map 0:v -acodec pcm_s16le -vcodec copy AdoVdoInptOtpt_Mix.avi -y。

# 其他
&emsp;&emsp;本软件采用了Speex的1.2.1版本、SpeexDsp的1.2.1版本、WebRtc的2019年7月份版本、OpenH264的2.3.1版本为基础，并进行了大量优化。  
&emsp;&emsp;讨论QQ群：511046632    欢迎大家参与测试和讨论！  
&emsp;&emsp;本人QQ号：280604597    赤勇玄心行天道  
&emsp;&emsp;本人博客：http://www.cnblogs.com/gaoyaguo  
&emsp;&emsp;Windows版源代码：https://github.com/cyz7758520/Windows_audio_talkback_demo_program  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;https://gitee.com/chen_yi_ze/Windows_audio_talkback_demo_program  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;https://gitcode.net/cyz7758520/Windows_audio_talkback_demo_program  
&emsp;&emsp;Android版源代码：https://github.com/cyz7758520/Android_audio_talkback_demo_program  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;https://gitee.com/chen_yi_ze/Android_audio_talkback_demo_program  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;https://gitcode.net/cyz7758520/Android_audio_talkback_demo_program  

# 版权
&emsp;&emsp;Speex：https://gitlab.xiph.org/xiph/speex/-/blob/master/COPYING  
&emsp;&emsp;WebRtc：https://gitlab.com/webrtc-mirror/webrtc/-/blob/master/LICENSE  
&emsp;&emsp;RNNoise：https://gitlab.xiph.org/xiph/rnnoise/-/blob/master/COPYING  
&emsp;&emsp;OpenH264：https://github.com/cisco/openh264/blob/master/LICENSE  
&emsp;&emsp;LibYUV：https://github.com/lemenkov/libyuv/blob/master/LICENSE  

# 感谢
&emsp;&emsp;感谢 WELEN、善书、陈国福 对 Speex、WebRTC 的指点！  

# 函数
### 九个回调函数
___
函数名称：UserInit  
功能说明：用户定义的初始化函数，在本线程刚启动时回调一次。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
返回说明：0：成功。  
&emsp;&emsp;&emsp;&emsp;&emsp;非0：失败。  
___
函数名称：UserPocs  
功能说明：用户定义的处理函数，在本线程运行时每隔1毫秒就回调一次。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
返回说明：0：成功。  
&emsp;&emsp;&emsp;&emsp;&emsp;非0：失败。  
___
函数名称：UserDstoy  
功能说明：用户定义的销毁函数，在本线程退出时回调一次。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
返回说明：无。  
___
函数名称：UserMsg  
功能说明：用户定义的消息函数，在接收到用户消息时回调一次。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;MsgArgPt：\[输入\]，存放消息参数的指针。如果没有消息参数，则本参数为NULL。  
返回说明：无。  
___
函数名称：UserReadAdoVdoInptFrm  
功能说明：用户定义的读取音视频输入帧函数，在读取到一个音频输入帧或视频输入帧并处理完后回调一次。如果没有使用音频输入和视频输入，则本函数不会被回调。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;PcmAdoInptFrmPt：\[输入\]，存放PCM格式音频输入帧的指针。如果没有使用音频输入，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;PcmAdoRsltFrmPt：\[输入\]，存放PCM格式音频结果帧的指针。如果没有使用音频输入，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;VoiceActSts：\[输入\]，存放语音活动状态，为非0表示有语音活动，为0表示无语音活动。如果没有使用音频输入，则本参数为非0。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdAdoInptFrmPt：\[输入\]，存放已编码格式音频输入帧的指针。如果没有使用音频输入，或音频输入编码器使用PCM原始数据，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdAdoInptFrmLenPt：\[输入\]，存放已编码格式音频输入帧长度的指针，单位为字节。如果没有使用音频输入，或音频输入编码器使用PCM原始数据，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdAdoInptFrmIsNeedTrans：\[输入\]，存放已编码格式音频输入帧是否需要传输，为非0表示需要传输，为0表示不需要传输。如果没有使用音频输入，或音频输入编码器使用PCM原始数据，则本参数为非0。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoInptFrmPt：\[输入\]，存放YU12格式视频输入帧的指针。如果没有使用视频输入，或本次没有读取到视频输入帧，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoInptFrmWidth：\[输入\]，存放YU12格式视频输入帧的宽度。如果没有使用视频输入，或本次没有读取到视频输入帧，则本参数为0。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoInptFrmHeight：\[输入\]，存放YU12格式视频输入帧的高度。如果没有使用视频输入，或本次没有读取到视频输入帧，则本参数为0。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdVdoInptFrmPt：\[输入\]，存放已编码格式视频输入帧的指针。如果没有使用视频输入，或本次没有读取到视频输入帧，或视频输入编码器使用YU12原始数据，则本参数为null。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdVdoInptFrmLen：\[输入\]，存放已编码格式视频输入帧的长度，单位为字节。如果没有使用视频输入，或本次没有读取到视频输入帧，或视频输入编码器使用YU12原始数据，则本参数为0。  
返回说明：0：成功。  
&emsp;&emsp;&emsp;&emsp;&emsp;非0：失败。  
___
函数名称：UserWriteAdoOtptFrm  
功能说明：用户定义的写入音频输出帧函数，在需要写入一个音频输出帧时回调一次。如果没有使用音频输出，则本函数不会被回调。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;PcmAdoOtptFrmPt：\[输出\]，存放PCM格式音频输出帧的指针。如果音频输出解码器没有使用PCM原始数据，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdAdoOtptFrmPt：\[输出\]，存放已编码格式音频输出帧的指针。如果音频输出解码器使用PCM原始数据，则本参数为NULL。  
&emsp;&emsp;&emsp;&emsp;&emsp;AdoOtptFrmLenPt：\[输入&输出\]，输入时，存放音频输出帧最大长度的指针，输出时，存放音频输出帧长度的指针。如果音频输出解码器使用PCM原始数据，则单位为采样。如果音频输出解码器没有使用PCM原始数据，则单位为字节。  
返回说明：无。  
___
函数名称：UserGetPcmAdoOtptFrm  
功能说明：用户定义的获取PCM格式音频输出帧函数，在解码完一个已编码音频输出帧时回调一次。如果没有使用音频输出，则本函数不会被回调。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;PcmAdoOtptFrmPt：\[输入\]，存放PCM格式音频输出帧的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;PcmAdoOtptFrmLen：\[输入\]，存放PCM格式音频输出帧的长度，单位为采样。  
返回说明：无。  
___
函数名称：UserWriteVdoOtptFrm  
功能说明：用户定义的写入视频输出帧函数，在可以显示一个视频输出帧时回调一次。如果没有使用视频输出，则本函数不会被回调。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoOtptFrmPt：\[输出\]，存放YU12格式视频输出帧的指针。如果视频输出解码器没有使用YU12原始数据，则本参数为null。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoInptFrmWidthPt：\[输出\]，存放YU12格式视频输出帧宽度的指针。如果视频输出解码器没有使用YU12原始数据，则本参数为null。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoInptFrmHeightPt：\[输出\]，存放YU12格式视频输出帧高度的指针。如果视频输出解码器没有使用YU12原始数据，则本参数为null。  
&emsp;&emsp;&emsp;&emsp;&emsp;EncdVdoOtptFrmPt：\[输出\]，存放已编码格式视频输出帧的指针。如果视频输出解码器使用YU12原始数据，则本参数为null。  
&emsp;&emsp;&emsp;&emsp;&emsp;VdoOtptFrmLenPt：\[输入&输出\]，输入时，存放视频输出帧最大长度的指针，输出时，存放视频输出帧长度的指针。单位为字节。   
返回说明：无。  
___
函数名称：UserGetYU12VdoOtptFrm  
功能说明：用户定义的获取YU12格式视频输出帧函数，在解码完一个已编码视频输出帧时回调一次。如果没有使用视频输出，则本函数不会被回调。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。  
参数说明：MediaPocsThrdPt：\[输入\]，存放媒体处理线程的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoOtptFrmPt：\[输入\]，存放YU12格式视频输出帧的指针。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoOtptFrmWidth：\[输入\]，存放YU12格式视频输出帧的宽度。  
&emsp;&emsp;&emsp;&emsp;&emsp;YU12VdoOtptFrmHeight：\[输入\]，存放YU12格式视频输出帧的高度。  
返回说明：无。  
___
