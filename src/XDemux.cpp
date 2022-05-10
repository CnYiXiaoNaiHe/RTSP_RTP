#include "XDemux.h"
#include <iostream>
using namespace std;
extern "C" {
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include "libswscale/swscale.h"
#include"libavutil/imgutils.h"
}
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"avcodec.lib")

static AVPixelFormat ConvertDeprecatedFormat(enum AVPixelFormat format)
{
	switch (format)
	{
	case AV_PIX_FMT_YUVJ420P:
		return AV_PIX_FMT_YUV420P;
		break;
	case AV_PIX_FMT_YUVJ422P:
		return AV_PIX_FMT_YUV422P;
		break;
	case AV_PIX_FMT_YUVJ444P:
		return AV_PIX_FMT_YUV444P;
		break;
	case AV_PIX_FMT_YUVJ440P:
		return AV_PIX_FMT_YUV440P;
		break;
	default:
		return format;
		break;
	}
}


static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

bool XDemux::Open(const char *url)
{
	Close();
	//参数设置

	//命令行获取设备 
	//ffmpeg -list_devices true -f dshow -i dummy
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);

	AVInputFormat *ifmt = av_find_input_format("dshow");

	mux.lock();
	int re = -1;
	//Windos摄像头采集
	/*
	命令行获取设备信息
	ffmpeg -list_options true -f dshow -i video="Full HD webcam"
	ffplay -f dshow -i video="Full HD webcam"
	设置1280 x 720 分辨率播放
	ffplay -s 1280x720 -f dshow -i video="Full HD webcam"
	*/
	printf("========Device Option Info======\n");
	if (re = avformat_open_input(&ic, "video=Full HD webcam", ifmt, NULL) != 0) {
		printf("Couldn't open input stream.（无法打开输入流）\n");
		return false;
	}
	if (re != 0)
	{
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;
		return false;
	}
	cout << "open " << url << " success! " << endl;

	//获取流信息 ,必须要有这一项否则解网络流会出问题
	re = avformat_find_stream_info(ic, 0);

	//总时长 毫秒
	int totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;

	//打印视频流详细信息
	av_dump_format(ic, 0, url, 0);


	//获取视频流
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream *as = ic->streams[videoStream];
	width = as->codecpar->width;
	height = as->codecpar->height;

	cout << "=======================================================" << endl;
	cout << videoStream << "视频信息" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "width=" << as->codecpar->width << endl;
	cout << "height=" << as->codecpar->height << endl;
	cout << "bit_rate= " << as->codecpar->bit_rate << endl;
	//帧率 fps 分数转换
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
	cout << "=======================================================" << endl;
	mux.unlock();
	return true;
}
//清空读取缓存
void XDemux::Clear()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	//清理读取缓冲
	avformat_flush(ic);
	mux.unlock();
}
void XDemux::Close()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	avformat_close_input(&ic);
	//媒体总时长（毫秒）
	totalMs = 0;
	mux.unlock();
}

//获取视频参数  返回的空间需要清理  avcodec_parameters_free
AVCodecParameters *XDemux::CopyVPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return NULL;
	}
	AVCodecParameters *pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
	mux.unlock();
	return pa;
}


bool XDemux::IsAudio(AVPacket *pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == videoStream)
		return false;
	return true;

}
//空间需要调用者释放 ，释放AVPacket对象空间，和数据空间 av_packet_free
AVPacket *XDemux::Read()
{
	mux.lock();
	if (!ic) //容错
	{
		mux.unlock();
		return 0;
	}
	AVPacket *pkt = av_packet_alloc();
	//读取一帧，并分配空间
	int re = av_read_frame(ic, pkt);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//pts转换为毫秒
	pkt->pts = pkt->pts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	pkt->dts = pkt->dts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	mux.unlock();
	//cout << pkt->pts << " "<<flush;
	return pkt;

}

AVFrame * XDemux::ToYUV420P(AVFrame *frame)
{
	AVFrame *Frameyuv = av_frame_alloc();
	vctx = sws_getCachedContext(
		vctx,	//传NULL会新创建
		frame->width, frame->height,	//输入的宽高
		ConvertDeprecatedFormat((AVPixelFormat)frame->format),//输入格式转换
		frame->width, frame->height,	//输出的宽高
		AV_PIX_FMT_YUV420P,				//输出格式YUV420
		SWS_BILINEAR,					//尺寸变化的算法
		0, 0, 0);
	//if (vctx)
	//	cout << "像素格式尺寸转换上下文创建或者获取成功！" << endl;
	//else
	//	cout << "像素格式尺寸转换上下文创建或者获取失败！" << endl;

	av_image_alloc(Frameyuv->data, Frameyuv->linesize, frame->width, frame->height, AV_PIX_FMT_YUV420P, 1);

	//输出空间
	Frameyuv->format = AV_PIX_FMT_YUV420P;
	Frameyuv->width = frame->width;
	Frameyuv->height = frame->height;

	if (vctx)
	{

		//逐帧进行转换
		int re = sws_scale(vctx,
			frame->data,		//输入数据
			frame->linesize,	//输入行大小
			0,
			frame->height,		//输入高度
			Frameyuv->data,	    //输出数据和大小
			Frameyuv->linesize
		);

	}

	return Frameyuv;

}

XDemux::XDemux()
{
	static bool isFirst = true;
	static std::mutex dmux;
	dmux.lock();
	if (isFirst)
	{
		//初始化封装库
		av_register_all();

		//初始化网络库 （可以打开rtsp rtmp http 协议的流媒体视频）
		avformat_network_init();

		//注册音视频采集类
		avdevice_register_all();
		isFirst = false;
	}
	dmux.unlock();
}


XDemux::~XDemux()
{
}
