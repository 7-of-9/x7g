#ifndef __MENUSCROLLINGBACKG__
#define __MENUSCROLLINGBACKG__

#include "CScreenOverlayManager.h"
#include "X7_Utils.h"

class CMenuScrollingBackG : public CScreenOverlay
{
public:
	CMenuScrollingBackG() ;
	CMenuScrollingBackG(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fMoveSpeed, float fAlpha, float fX, float fY, float fZ, float fWidth, float fHeight) ;
	~CMenuScrollingBackG() ;

	D3DXCOLOR m_c1, m_c2, m_c3, m_c4 ;	   // target colors, for LERPing.
	D3DXCOLOR m_co1, m_co2, m_co3, m_co4 ; // original colors, for LERPing.
	float	  m_fLERPDone ;			       // how much LERP is currently done, 0-1.

	float     m_fMoveSpeed ;
	float	  m_fAlpha ;

// virtual fn. implementations:
    HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices) ;
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
} ;

#endif