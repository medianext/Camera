#pragma once

#include "stdafx.h"
#include "Frame.h"


typedef void(*IMAGE_TRANSFORM_FN)(
	BYTE*       pDest,
	LONG        lDestStride,
	const BYTE* pSrc,
	LONG        lSrcStride,
	DWORD       dwWidthInPixels,
	DWORD       dwHeightInPixels
	);

struct VideoConversionFunction
{
	GUID               subtype;
	IMAGE_TRANSFORM_FN xform;
};



class Render
{

public:
	Render(HWND hWnd, GUID format, int width, int height);
	~Render();

private:
	HRESULT TestCooperativeLevel();
	HRESULT ChooseConversionFunction(REFGUID subtype);

public:

	int DrawFrame(Frame * frame);

private:
	HWND m_hWnd;

	IDirect3D9              *m_pD3D;
	IDirect3DDevice9        *m_pDevice;
	IDirect3DSwapChain9     *m_pSwapChain;

	D3DPRESENT_PARAMETERS   m_d3dpp;

	RECT                    m_rcCanvas;
	RECT                    m_rcDest;

	GUID                    m_subtype;
	D3DFORMAT               m_format;

	IMAGE_TRANSFORM_FN      m_convertFn;
};
