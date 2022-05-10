#pragma once

struct AVFormatContext;
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct SwsContext;
struct AVCodecContext;

///����Ƶ����ӿ���
class XMediaEncode
{
public:

	XMediaEncode();
	~XMediaEncode();

	//��Ƶ��������ʼ��
	bool InitVideoCodec(AVCodecParameters *para);

	//��Ƶ���� ����ֵ�������������
	AVPacket EncodeVideo(AVFrame *frame);

	void Close();

	void SaveH264(AVPacket * packet);

protected:
	AVCodecContext *vc = nullptr;	//��Ƶ������������
	AVCodecContext *ac = nullptr; //��Ƶ������������
	AVFrame *yuv = nullptr;		//�����YUV
	AVFrame *pcm = nullptr;		//�ز������PCM
	AVPacket *apack = nullptr;		//��Ƶ֡
	AVPacket *vpack = nullptr;
	int vpts = 0;
	int apts = 0;


};

