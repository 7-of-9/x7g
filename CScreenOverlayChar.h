#ifndef __SCREENOVERLAY__
#define __SCREENOVERLAY__

#include "CScreenOverlay.h"
#include "X7_Utils.h"

class CScreenOverlayChar : public CScreenOverlay
{
public:
	CScreenOverlayChar(LPDIRECT3DTEXTURE8 pTex, char c, float fAlpha, float fX, float fY, float fZ, float fWidth, float fHeight) ;
	~CScreenOverlayChar() ;

	char	  m_cChar ;
	float	  m_fAlpha ;

// virtual fn. implementations:
    HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult) ;
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
} ;

#endif