#include "XMediaEncode.h"
extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h> 
}

#include <iostream>
using namespace std;

bool  OpenCodec(AVCodecContext **c);
AVCodecContext*  CreateCodec(AVCodecID cid);

#if defined WIN32 || defined _WIN32
#include <windows.h>
#endif
//获取CPU数量
static int XGetCpuNum()
{
#if defined WIN32 || defined _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	return (int)sysinfo.dwNumberOfProcessors;
#elif defined __linux__
	return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined __APPLE__
	int numCPU = 0;
	int mib[4];
	size_t len = sizeof(numCPU);

	// set the mib for hw.ncpu
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

						   // get the number of CPUs from the system
	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if (numCPU < 1)
	{
		mib[1] = HW_NCPU;
		sysctl(mib, 2, &numCPU, &len, NULL, 0);

		if (numCPU < 1)
			numCPU = 1;
	}
	return (int)numCPU;
#else
	return 1;
#endif
}


void XMediaEncode::Close()
{
		if (yuv)
		{
			av_frame_free(&yuv);
		}
		if (vc)
		{
			avcodec_free_context(&vc);
		}

		if (pcm)
		{
			av_frame_free(&pcm);
		}


		vpts = 0;
		av_packet_unref(apack);
		apts = 0;
		av_packet_unref(vpack);
}



bool XMediaEncode::InitVideoCodec(AVCodecParameters *para)
{
	    if (!para) return false;
		///4 初始化编码上下文
		//a 找到编码器
		if (!(vc = CreateCodec(AV_CODEC_ID_H264)))
		{
			return false;
		}

		//设置编码器上下文参数
		vc->width = para->width;
		vc->height = para->height;
		vc->bit_rate = 750 * 1000 ;
		AVRational r = { 1, 25 };//每秒1/N帧
		vc->time_base = r;
		vc->gop_size = 50;//GOP组
		vc->max_b_frames = 0;
		vc->codec_type = AVMEDIA_TYPE_VIDEO;
		vc->pix_fmt = AV_PIX_FMT_YUV420P;//视频格式

		vc->me_range = 16;
		vc->max_qdiff = 4;
		vc->qcompress = 0.6;

		vc->qmin = 10;
		vc->qmax = 51;

		av_opt_set(vc->priv_data, "preset", "ultrafast", 0);
		av_opt_set(vc->priv_data, "x264opts", "crf=26:vbv-maxrate=728:vbv-bufsize=364:keyint=30", 0);
		av_opt_set(vc->priv_data, "profile", "high", 0);
		av_opt_set(vc->priv_data, "tune", "zerolatency", 0);
		return OpenCodec(&vc);
}


AVPacket XMediaEncode::EncodeVideo(AVFrame *frame)
{  
	    vpack = av_packet_alloc();		//视频帧
		av_packet_unref(vpack);
		if (frame == nullptr)
		{
			cout << " AVFrame is null" << endl;
			return *vpack;
		}
			
	
		///h264编码
		frame->pts = vpts;
		vpts++;
		int ret = avcodec_send_frame(vc, frame);
		if (ret != 0)
			return *vpack;

		ret = avcodec_receive_packet(vc, vpack);
		if (ret != 0 || vpack->size <= 0)
			return *vpack;
		/*SaveH264(vpack);*/
		cout << "out packet size is " << vpack->size << endl;
		return *vpack;
}

void XMediaEncode::SaveH264(AVPacket * packet)
{
	FILE *fpSave;
	if ((fpSave = fopen("BB.h264", "ab")) == NULL) //h264保存的文件名  
	{
		cout << "fopen  SaveH264 NO" << endl;
		return;
	}
	fwrite(packet->data, 1, packet->size, fpSave);//写数据到文件中  

	fclose(fpSave);
}





bool  OpenCodec(AVCodecContext **c)
{
		//打开音频编码器
		int ret = avcodec_open2(*c, 0, 0);
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			avcodec_free_context(c);
			return false;
		}
		cout << "avcodec_open2 success!" << endl;
		return true;
}

AVCodecContext*  CreateCodec(AVCodecID cid)
{
		///4 初始化编码器 AV_CODEC_ID_AAC
		AVCodec *codec = avcodec_find_encoder(cid);
		if (!codec)
		{
			cout << "avcodec_find_encoder  failed!" << endl;
			return NULL;
		}
		//音频编码器上下文
		AVCodecContext* c = avcodec_alloc_context3(codec);
		if (!c)
		{
			cout << "avcodec_alloc_context3  failed!" << endl;
			return NULL;
		}
		cout << "avcodec_alloc_context3 success!" << endl;

		/*c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;*/
		c->thread_count = XGetCpuNum();
		c->time_base = { 1,90000 };
		return c;
}



XMediaEncode::XMediaEncode()
{
}


XMediaEncode::~XMediaEncode()
{
}
