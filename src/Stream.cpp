#include "Stream.h"

CStream::CStream(const char *filename)
{
	m_filename = filename;
	file_info();
	m_inputfile = fopen(m_filename, "rb");
	if (m_inputfile == nullptr)
	{
		file_error(0);
	}
}

CStream::~CStream()
{
	if (m_inputfile)
	{
		fclose(m_inputfile);
		m_inputfile = nullptr;
	}

}

int CStream::Parse_h264_bitstream()
{
	int ret = 0;
	do
	{

		ret = find_nal_prefix();
		//解析NAL UNIT
		if (!m_nalVec.empty())
		{
			uint8 nalType = m_nalVec[0] & 0x1f;
			switch (nalType)
			{
			case 1:
				printf("NAL TYPE :Slice\n");
				ebsp_to_soob();
				DataAnalysis(&m_nalVec[1], m_nalVec.size() - 1);
				break;
			case 5:
				printf("NAL TYPE :IDR\n");
				ebsp_to_soob();
				break;
			case 7:
				printf("NAL TYPE :SPS\n");
				ebsp_to_soob();
				break;
			case 8:
				printf("NAL TYPE :PPS\n");
				ebsp_to_soob();
				break;
			default:
				break;
			}
		}
		//
	} while (ret);
	return 0;
}

void CStream::file_info()
{
	printf("%s\n", m_filename);
}

void CStream::file_error(int idx)
{
	switch (idx)
	{
	case 0:
		printf("Open FILE fail\n");
		break;
	default:
		break;
	}
}

int CStream::find_nal_prefix()
{
	uint8 prefix[3] = { 0 };
	uint8 fileByte;


	m_nalVec.clear();

	int pos = 0, getPrefix = 0;
	//从内存中读取三个字节数据
	for (int idx = 0; idx < 3; idx++)
	{
		
		prefix[idx] = getc(m_inputfile);
		m_nalVec.push_back(prefix[idx]);

	}
	while (!feof(m_inputfile))
	{
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1))
		{
			//0x 00 00 01
			getPrefix = 1;
			m_nalVec.pop_back();
			m_nalVec.pop_back();
			m_nalVec.pop_back();
			break;
		}
		else if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0))
		{
			//0x 00 00 00 01
			if (getc(m_inputfile) == 1)
			{
				getPrefix = 2;
				m_nalVec.pop_back();
				m_nalVec.pop_back();
				m_nalVec.pop_back();
				break;
			}
		}
		else
		{
			fileByte = getc(m_inputfile);
			prefix[(pos++) % 3] = fileByte;
			m_nalVec.push_back(fileByte);
		}

	}
	return getPrefix;
}

void CStream::ebsp_to_soob()
{
	if (m_nalVec.empty())return;
	for (auto itor = m_nalVec.begin() + 2; itor != m_nalVec.end();)
	{
		if ((3 == *itor) && (0 == *(itor - 1)) && (0 == *(itor - 2)))
		{
			//创建临迭代器，防止在循环中迭代器失效
			auto temp = m_nalVec.erase(itor);
			itor = temp;
		}
		else
		{
			itor++;
		}
	}
}

void CStream::DataAnalysis(uint8 * SOOB, uint32 lengthSOOB)
{

}