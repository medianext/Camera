#include "stdafx.h"
#include "Camera.h"
#include "Frame.h"


Camera::Camera(void * priv)
	:render(nullptr)
	,m_pReader(nullptr)
{
	InitializeCriticalSection(&m_critsec);

	IMFActivate *pDevices = (IMFActivate*)priv;

	HRESULT hr = S_OK;

	WCHAR * str;
	hr = pDevices->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &str, NULL);
	Name = str;
	CoTaskMemFree(str);
	hr = pDevices->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &str, NULL);
	Link = str;
	CoTaskMemFree(str);

	IMFMediaSource  *pSource = nullptr;
	hr = pDevices->ActivateObject(__uuidof(IMFMediaSource),(void**)&pSource);
	if (SUCCEEDED(hr))
	{
		IMFPresentationDescriptor* presentationDescriptor = nullptr;
		hr = pSource->CreatePresentationDescriptor(&presentationDescriptor);
		if (SUCCEEDED(hr))
		{
			DWORD dwCount = 0;
			presentationDescriptor->GetStreamDescriptorCount(&dwCount);
			for (DWORD i = 0; i < dwCount; i++)
			{
				BOOL bSelect;
				IMFStreamDescriptor *pStreamDescriptor = nullptr;
				hr = presentationDescriptor->GetStreamDescriptorByIndex(i, &bSelect, &pStreamDescriptor);
				if (SUCCEEDED(hr) && bSelect == TRUE)
				{
					IMFMediaTypeHandler *pMediaTypeHandler = nullptr;
					hr = pStreamDescriptor->GetMediaTypeHandler(&pMediaTypeHandler);
					if (SUCCEEDED(hr))
					{
						IMFMediaType * pMediaType = nullptr;
						hr = pMediaTypeHandler->GetCurrentMediaType(&pMediaType);
						if (SUCCEEDED(hr))
						{
							UINT32 uWidth, uHeight, uNummerator, uDenominator;
							LONG lStride;
							GUID subType;
							pMediaType->GetGUID(MF_MT_SUBTYPE, &subType);
							MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &uWidth, &uHeight);
							MFGetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, &uNummerator, &uDenominator);
							hr = pMediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32*)&lStride);
							if (FAILED(hr))
							{
								hr = MFGetStrideForBitmapInfoHeader(subType.Data1, uWidth, &lStride);
							}
							format = subType;
							stride = lStride;
							width = uWidth;
							height = uHeight;
							fps = uNummerator / uDenominator;
						}
						SafeRelease(&pMediaType);
					}
					SafeRelease(&pMediaTypeHandler);
					break;
				}
				SafeRelease(&pStreamDescriptor);
			}
			SafeRelease(&presentationDescriptor);
		}
	}
}


Camera::~Camera()
{
	EnterCriticalSection(&m_critsec);
	SafeRelease(&m_pReader);
	if (render != nullptr)
	{
		delete render;
		render = nullptr;
	}
	LeaveCriticalSection(&m_critsec);

	DeleteCriticalSection(&m_critsec);
}


/////////////// IUnknown methods ///////////////

ULONG Camera::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}


ULONG Camera::Release()
{
	ULONG uCount = InterlockedDecrement(&m_nRefCount);
	if (uCount == 0)
	{
		delete this;
	}
	// For thread safety, return a temporary variable.
	return uCount;
}


HRESULT Camera::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(Camera, IMFSourceReaderCallback),
	{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

HRESULT Camera::OnReadSample(
	HRESULT hrStatus,
	DWORD /* dwStreamIndex */,
	DWORD /* dwStreamFlags */,
	LONGLONG llTimestamp,
	IMFSample *pSample      // Can be NULL
)
{
	HRESULT hr = S_OK;
	IMFMediaBuffer *pBuffer = NULL;

	static int count = 0;

	int ret = 0;

	EnterCriticalSection(&m_critsec);

	if (m_pReader == nullptr)
	{
		LeaveCriticalSection(&m_critsec);
		return hr;
	}

	if (FAILED(hrStatus))
	{
		hr = hrStatus;
	}

	if (SUCCEEDED(hr))
	{
		if (pSample)
		{
			// Get the video frame buffer from the sample.
			hr = pSample->GetBufferByIndex(0, &pBuffer);
			if (SUCCEEDED(hr))
			{
				if (SUCCEEDED(hr))
				{
					Frame frame(pBuffer,  format, width, height, stride);
					frame.m_uTimestamp = llTimestamp / 10;
					
					if(render != nullptr)
					{
						render->DrawFrame(&frame);
					}
				}
			}
		}
	}

	// Request the next frame.
	if (SUCCEEDED(hr))
	{
		hr = m_pReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			NULL,   // actual
			NULL,   // flags
			NULL,   // timestamp
			NULL    // sample
		);
	}

	SafeRelease(&pBuffer);

	LeaveCriticalSection(&m_critsec);
	return hr;
}


int Camera::Start(HWND hWnd)
{
	EnterCriticalSection(&m_critsec);

	//SafeRelease(&m_pReader);

	if (hWnd != NULL)
	{
		render = new Render(hWnd, format, width, height);
	}

	HRESULT hr = S_OK;

	IMFMediaSource  *pSource = NULL;
	IMFAttributes   *pAttributes = NULL;

	hr = MFCreateAttributes(&pAttributes, 2);
	pAttributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, Link.c_str());
	pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	hr = MFCreateDeviceSource(pAttributes, &pSource);
	SafeRelease(&pAttributes);

	// Create the IMFSourceReader
	hr = MFCreateAttributes(&pAttributes, 2);
	if (SUCCEEDED(hr))
	{
		hr = pAttributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
		hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);

		hr = MFCreateSourceReaderFromMediaSource(pSource, pAttributes, &m_pReader);
		if (!SUCCEEDED(hr))
		{
		}
	}
	SafeRelease(&pAttributes);
	SafeRelease(&pSource);

	m_pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);

	LeaveCriticalSection(&m_critsec);

	return 0;
}

int Camera::Stop() 
{
	EnterCriticalSection(&m_critsec);

	if (render != nullptr)
	{
		delete render;
		render = nullptr;
	}
	m_pReader->Flush(MF_SOURCE_READER_FIRST_VIDEO_STREAM);
	SafeRelease(&m_pReader);

	LeaveCriticalSection(&m_critsec);

	return 0;
}