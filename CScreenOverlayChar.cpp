#include "CScreenOverlayChar.h"

CScreenOverlayChar::CScreenOverlayChar(LPDIRECT3DTEXTURE8 pTex, char c, float fAlpha, float fX, float fY, float fZ, float fWidth, float fHeight)
: CScreenOverlay(pTex, 1.0f, fX, fY, fZ, fWidth, fHeight)
{
	m_cChar = c ;
	m_fAlpha = fAlpha ;
}

CScreenOverlayChar::~CScreenOverlayChar()
{
}

HRESULT CScreenOverlayChar::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect)
{
	CScreenOverlay::RestoreDeviceObjects(pDev, fAspect) ;

//
// Alter the texture coords. to isolate a single char from within
// the texture. Assumes 16x16 char layout on the texture.
//
    OVERLAYVERTEX* pVertices ; 
    if (FAILED(m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL)))
        return E_FAIL ;

    pVertices += m_dwIndexOffset ;
	
	float col = (float)((m_cChar) % 16) ;
	float row = (float)floor((double)(m_cChar) / 16) ;

	pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha) ;
	pVertices->tu = 0.0625f * col ;
	pVertices->tv = 0.0625f * row ;
    pVertices++ ;
	pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha) ;
	pVertices->tu = 0.0625f * (col + 1.0f) ;
	pVertices->tv = 0.0625f * row ;
    pVertices++ ;
	pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha) ;
	pVertices->tu = 0.0625f * col ;
	pVertices->tv = 0.0625f * (row + 1.0f) ;
    pVertices++ ;
	pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha) ;
	pVertices->tu = 0.0625f * (col + 1.0f) ;
	pVertices->tv = 0.0625f * (row + 1.0f) ;
    pVertices++ ;

    m_pSharedVB->Unlock();

	return S_OK ;
}

HRESULT CScreenOverlayChar::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult)
{
	return S_OK ;
}
