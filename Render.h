#pragma once

#include "stdafx.h"
#include "Frame.h"
#include "Transcode.h"


class Render
{

public:
	Render(HWND hWnd, GUID format, int width, int height);
	~Render();

private:
	HRESULT TestCooperativeLevel();

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
