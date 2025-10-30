#ifndef __MENUITEM__
#define __MENUITEM__

#include "CScreenOverlayManager.h"
#include "X7_Utils.h"


enum e_MenuStates
{
	MS_INTO,
	MS_IDLE,
	MS_OUTRO,
	MAX_MENU_STATES
} ;

class CMenuItem : public CScreenOverlay
{
public:
	CMenuItem() ;
	CMenuItem(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fX, float fY, float fZ, float fWidth, float fHeight) ;
	~CMenuItem() ;

	SetSelected(BOOL bSelected, OVERLAYVERTEX* pVertices) ;
	BOOL	  m_bSelected ;

	float	  m_fUnselectedAlpha ;
	float	  m_fSelectedAlpha ;

	float     m_fTimeSelected ;			// use for sin cycling alpha

// virtual fn. implementations:
    HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVBVertices) ;
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
} ;

#endif