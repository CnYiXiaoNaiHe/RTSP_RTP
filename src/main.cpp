#include "EncodeCamero.h"
#include <QtWidgets/QApplication>
#include <iostream>
using namespace std;

#include "XDemux.h"
#include "XDecode.h"
#include "rtp_h264.h"
#include <QThread>
#include "XMediaEncode.h"
extern "C"
{
#include<libavcodec/avcodec.h>
}
class TestThread :public QThread
{
public:
	void Init()
	{
		const char *url = "TWO.mp4";
		cout << "demux.Open = " << demux.Open(url);
		cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
		cout << "InitVideoCodec() = " << mediaencode.InitVideoCodec(demux.CopyVPara()) << endl;;
		rtp.InitNetwork();
	}

	void run()
	{
		for (;;)
		{
			AVPacket *pkt = demux.Read();
			AVPacket outpkt;
	
			vdecode.Send(pkt);
			AVFrame *frame = vdecode.Recv();
			AVFrame *outframe = demux.ToYUV420P(frame);
			outpkt = mediaencode.EncodeVideo(outframe);
			cout << "out packet size is :" << outpkt.size << endl;
			rtp.doClient((char*)outpkt.data, outpkt.size);
			msleep(1);
			cout << "Video:" << frame << endl;

			if (!pkt)break;

		}
	}
	///²âÊÔXDemux
	XDemux demux;
	///½âÂë²âÊÔ
	XDecode vdecode;

	XMediaEncode mediaencode;

	rtp_h264  rtp;


};

int main(int argc, char *argv[])
{
	TestThread tt;
	tt.Init();

    QApplication a(argc, argv);
    EncodeCamero w;
    w.show();

	tt.start();

    return a.exec();
}
