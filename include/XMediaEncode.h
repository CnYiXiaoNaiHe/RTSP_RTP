#pragma once

struct AVFormatContext;
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct SwsContext;
struct AVCodecContext;

///音视频编码接口类
class XMediaEncode
{
public:

	XMediaEncode();
	~XMediaEncode();

	//视频编码器初始化
	bool InitVideoCodec(AVCodecParameters *para);

	//视频编码 返回值无需调用者清理
	AVPacket EncodeVideo(AVFrame *frame);

	void Close();

	void SaveH264(AVPacket * packet);

protected:
	AVCodecContext *vc = nullptr;	//音频编码器上下文
	AVCodecContext *ac = nullptr; //音频编码器上下文
	AVFrame *yuv = nullptr;		//输出的YUV
	AVFrame *pcm = nullptr;		//重采样输出PCM
	AVPacket *apack = nullptr;		//音频帧
	AVPacket *vpack = nullptr;
	int vpts = 0;
	int apts = 0;


};

