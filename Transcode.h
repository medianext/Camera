#pragma once

#include "stdafx.h"

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


RECT LetterBoxRect(const RECT& rcSrc, const RECT& rcDst);
RECT CorrectAspectRatio(const RECT& src, const MFRatio& srcPAR);

void TransformImage_RGB32(BYTE* pDst, LONG dstStride, const BYTE* pSrc, LONG srcStride, DWORD dwWidthInPixels, DWORD dwHeightInPixels);
void TransformImage_RGB24(BYTE* pDst, LONG dstStride, const BYTE* pSrc, LONG srcStride, DWORD dwWidthInPixels, DWORD dwHeightInPixels);
void TransformImage_YUY2(BYTE* pDst, LONG dstStride, const BYTE* pSrc, LONG srcStride, DWORD dwWidthInPixels, DWORD dwHeightInPixels);
void TransformImage_I420(BYTE* pDst, LONG dstStride, const BYTE* pSrc, LONG srcStride, DWORD dwWidthInPixels, DWORD dwHeightInPixels);
void TransformImage_NV12(BYTE* pDst, LONG dstStride, const BYTE* pSrc, LONG srcStride, DWORD dwWidthInPixels, DWORD dwHeightInPixels);


IMAGE_TRANSFORM_FN ChooseTranscodeFn(REFGUID subtype);