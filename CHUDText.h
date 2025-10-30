#ifndef __CHUDTEXT__
#define __CHUDTEXT__

class CHUDText ;

	#include <stdio.h>
	#include <D3DX8.h>

	#include "CUserInput.h"
	#include "CScreenOverlayManager.h"
	#include "CScreenOverlay.h"
	#include "CMenuItem.h"
	#include "CSoundManager.h"
	#include "x7.h"
	#include "resource.h"

class CHUDText
{
//
// Construction / Destruction:
//
public:
	CHUDText( char* szText, int nTex, float fX, float fY, float fWidth, float fHeight, float fOverlap, int OverlayType );
	~CHUDText() ;

	int					  m_nTex ;
	BOOL				  m_bActive ;
	float				  m_fX, m_fY, m_fWidth, m_fHeight ;
	float				  m_fOverlap ;
	Enable( BOOL bActive );
	char				  m_szText[ 256 ];
	int					  m_OverlayType ;

	BOOL				  m_bFrameMoveRequired ;
	SetFrameMoveRequired( BOOL bRequired );

protected:	
	CScreenOverlay**	  m_ppChars ;
	int					  m_nChars ;

//
// Framework events:
//
public:
    HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, float fAspect, CScreenOverlayManager* pOM, CTextures* pTex) ;
	HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, /*float fTimeMult,*/ CUserInput* m_pIn, OVERLAYVERTEX* pVBVertices) ;
    HRESULT InvalidateDeviceObjects() ;
} ;

#endif