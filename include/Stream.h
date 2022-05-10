#ifndef STREAM_H
#define STREAM_H

#include<stdio.h>
#include<vector>
#include"Global.h"

class CStream
{
public:
	CStream(const char *filename);
	~CStream();

	int Parse_h264_bitstream();

private:
	FILE *m_inputfile;
	const char *m_filename;
	std::vector<uint8>m_nalVec;

	void file_info();
	void file_error(int idx);
	int  find_nal_prefix();//��ȡNAL Unit��Դ����
	void ebsp_to_soob();
	void DataAnalysis(uint8 *SOOB, uint32 lengthSOOB);//Դ���ݷ���
};

#endif // !GLOBAL