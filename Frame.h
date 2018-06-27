/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#pragma once



class Frame
{
public:
	Frame();
	Frame(Frame *frame);
	Frame(GUID subtype, DWORD dataSize);
	Frame(IMFMediaBuffer*, GUID format, int width, int height, int stride);
    ~Frame();

private:
	void Inversion();

public:
    // common filed
	GUID  m_subtype;
    BYTE  *m_pData = NULL;
	DWORD m_dataSize = 0;
	LONGLONG m_uTimestamp = 0;

    // video specific filed
    int m_stride;
    int m_width;
    int m_height;
};

