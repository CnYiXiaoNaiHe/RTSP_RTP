#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct SwsContext;

class XDemux
{
public:

	//��ý���ļ���������ý�� rtmp http rstp
	virtual bool Open(const char *url);

	//�ռ���Ҫ�������ͷ� ���ͷ�AVPacket����ռ䣬�����ݿռ� av_packet_free
	virtual AVPacket *Read();

	virtual bool IsAudio(AVPacket *pkt);

	//��ȡ��Ƶ����  ���صĿռ���Ҫ����  avcodec_parameters_free
	virtual AVCodecParameters *CopyVPara();

	//��ն�ȡ����
	virtual void Clear();
	virtual void Close();

	//vedioת��
	AVFrame* ToYUV420P(AVFrame *frame);

	XDemux();
	virtual ~XDemux();

	//ý����ʱ�������룩
	int totalMs = 0;
	int width = 0;
	int height = 0;



protected:
	std::mutex mux;
	//���װ������
	AVFormatContext *ic = NULL;
	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int audioStream = 1;
	//���ظ�ʽ�ͳߴ�ת��������
	SwsContext *vctx = NULL;
	unsigned char *rgb = NULL;
};