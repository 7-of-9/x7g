#ifndef __InGameMenu__
#define __InGameMenu__

class CInGameMenu ;

	#include <stdio.h>
	#include <D3DX8.h>

	#include "CMenuTextures.h"
	#include "CUserInput.h"

	#include "CScreenOverlayManager.h"
	#include "CMenuScrollingBackG.h"
	#include "CMenuItem.h"
	#include "CSoundManager.h"
	#include "x7.h"
	#include "resource.h"

class CInGameMenu
{
//
// Construction / Destruction:
//
public:
	CInGameMenu(Cx7* pX7) ;
	~CInGameMenu() ;

	Cx7*				  m_pX7 ;
	BOOL				  m_bActive ;
	EnterMenu() ;
	ExitMenu() ;

	SetViewMenuItems(UINT s) ;

protected:	
	CMenuItem**			  m_ppMenuItems ;
	int					  m_nMenuItems ;

	CMenuScrollingBackG*  m_pTitle ;
	
	int					  m_ndxMenuSelected ;
	float				  m_fMouseYMovement ;
	e_MenuStates		  m_msState ;
	int					  m_ndxSelected ;
	float				  m_fOutroPlaytime ;


//
// Framework events:
//
public:
	HRESULT Render(LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM) ;
    HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, float fAspect, CScreenOverlayManager* pOM, CMenuTextures* pTex) ;
	HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fFPS, CScreenOverlayManager* pOM, CUserInput* m_pIn, CSoundManager* pSM) ;
    HRESULT InvalidateDeviceObjects() ;
} ;

#endif