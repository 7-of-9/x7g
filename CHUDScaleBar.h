#ifndef __HUDSHIELD__
#define __HUDSHIELD__

#include "CScreenOverlayManager.h"
#include "X7_Utils.h"

class CHUDScaleBar : public CScreenOverlay
{
public:
	CHUDScaleBar() ;
	CHUDScaleBar(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fMoveSpeed, float fRed, float fGreen, float fBlue, float fAlpha, BOOL bFade, float fX, float fY, float fZ, float fWidth, float fHeight) ;
	~CHUDScaleBar() ;

	float m_fMoveSpeed ;
	float m_fRed, m_fGreen, m_fBlue ;
	float m_fAlpha ;

	float m_fLevel ;	// 0 - 1

	BOOL  m_bFade ;

// virtual fn. implementations:
    HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices) ;
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
} ;

#endif