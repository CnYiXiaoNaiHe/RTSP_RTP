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
	//��������

	//�����л�ȡ�豸 
	//ffmpeg -list_devices true -f dshow -i dummy
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);

	AVInputFormat *ifmt = av_find_input_format("dshow");

	mux.lock();
	int re = -1;
	//Windos����ͷ�ɼ�
	/*
	�����л�ȡ�豸��Ϣ
	ffmpeg -list_options true -f dshow -i video="Full HD webcam"
	ffplay -f dshow -i video="Full HD webcam"
	����1280 x 720 �ֱ��ʲ���
	ffplay -s 1280x720 -f dshow -i video="Full HD webcam"
	*/
	printf("========Device Option Info======\n");
	if (re = avformat_open_input(&ic, "video=Full HD webcam", ifmt, NULL) != 0) {
		printf("Couldn't open input stream.���޷�����������\n");
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

	//��ȡ����Ϣ ,����Ҫ����һ�������������������
	re = avformat_find_stream_info(ic, 0);

	//��ʱ�� ����
	int totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;

	//��ӡ��Ƶ����ϸ��Ϣ
	av_dump_format(ic, 0, url, 0);


	//��ȡ��Ƶ��
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream *as = ic->streams[videoStream];
	width = as->codecpar->width;
	height = as->codecpar->height;

	cout << "=======================================================" << endl;
	cout << videoStream << "��Ƶ��Ϣ" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "width=" << as->codecpar->width << endl;
	cout << "height=" << as->codecpar->height << endl;
	cout << "bit_rate= " << as->codecpar->bit_rate << endl;
	//֡�� fps ����ת��
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
	cout << "=======================================================" << endl;
	mux.unlock();
	return true;
}
//��ն�ȡ����
void XDemux::Clear()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	//�����ȡ����
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
	//ý����ʱ�������룩
	totalMs = 0;
	mux.unlock();
}

//��ȡ��Ƶ����  ���صĿռ���Ҫ����  avcodec_parameters_free
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
//�ռ���Ҫ�������ͷ� ���ͷ�AVPacket����ռ䣬�����ݿռ� av_packet_free
AVPacket *XDemux::Read()
{
	mux.lock();
	if (!ic) //�ݴ�
	{
		mux.unlock();
		return 0;
	}
	AVPacket *pkt = av_packet_alloc();
	//��ȡһ֡��������ռ�
	int re = av_read_frame(ic, pkt);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//ptsת��Ϊ����
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
		vctx,	//��NULL���´���
		frame->width, frame->height,	//����Ŀ��
		ConvertDeprecatedFormat((AVPixelFormat)frame->format),//�����ʽת��
		frame->width, frame->height,	//����Ŀ��
		AV_PIX_FMT_YUV420P,				//�����ʽYUV420
		SWS_BILINEAR,					//�ߴ�仯���㷨
		0, 0, 0);
	//if (vctx)
	//	cout << "���ظ�ʽ�ߴ�ת�������Ĵ������߻�ȡ�ɹ���" << endl;
	//else
	//	cout << "���ظ�ʽ�ߴ�ת�������Ĵ������߻�ȡʧ�ܣ�" << endl;

	av_image_alloc(Frameyuv->data, Frameyuv->linesize, frame->width, frame->height, AV_PIX_FMT_YUV420P, 1);

	//����ռ�
	Frameyuv->format = AV_PIX_FMT_YUV420P;
	Frameyuv->width = frame->width;
	Frameyuv->height = frame->height;

	if (vctx)
	{

		//��֡����ת��
		int re = sws_scale(vctx,
			frame->data,		//��������
			frame->linesize,	//�����д�С
			0,
			frame->height,		//����߶�
			Frameyuv->data,	    //������ݺʹ�С
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
		//��ʼ����װ��
		av_register_all();

		//��ʼ������� �����Դ�rtsp rtmp http Э�����ý����Ƶ��
		avformat_network_init();

		//ע������Ƶ�ɼ���
		avdevice_register_all();
		isFirst = false;
	}
	dmux.unlock();
}


XDemux::~XDemux()
{
}
