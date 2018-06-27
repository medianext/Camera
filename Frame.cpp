/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "Frame.h"


Frame::Frame():
    m_pData(NULL),
	m_uTimestamp(0)
{
}


Frame::Frame(FrameType frameType, GUID  subtype, DWORD dataSize)
{
	m_pData = (BYTE*)malloc(dataSize);
	if (m_pData!=NULL)
	{
		m_dataSize = dataSize;
		m_FrameType = frameType;
		m_subtype = subtype;
	}
}


Frame::Frame(IMFMediaBuffer* pBuffer, GUID format, int width, int height, int stride) :
	m_uTimestamp(0)
{
	DWORD bufferSize = 0;
	BYTE* pData = NULL;
    pBuffer->GetCurrentLength(&bufferSize);
    m_dataSize = bufferSize;
	m_pData = (BYTE*)malloc(m_dataSize);

    LONG lStride = (LONG)stride;
    // Helper object to lock the video buffer.

    // Lock the video buffer. This method returns a pointer to the first scan line
	// in the image, and the stride in bytes.
	pBuffer->Lock(&pData, NULL, NULL);
	memcpy(m_pData, pData, m_dataSize);
	pBuffer->Unlock();
	m_subtype = format;
	m_width = width;
	m_height = height;
	if (lStride<0){
		m_stride = 0 - lStride;
		Inversion();
	}else{
		m_stride = lStride;
	}

}


Frame::~Frame()
{
	if (m_pData)
	{
		free(m_pData);
	}
}


void Frame::Inversion()
{
	BYTE *pData = (BYTE*)malloc(m_dataSize);

	for (int i = 0; i < m_height; i++)
	{
		BYTE *pDst = &pData[i*m_stride];
		BYTE *pSrc = &m_pData[(m_height - i - 1)*m_stride];
		memcpy(pDst, pSrc, m_stride);
	}
	free(m_pData);
	m_pData = pData;
}