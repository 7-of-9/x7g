#ifndef __MENU__
#define __MENU__

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

class CMenu ;

#define NO_MAINMENU_ITEMS 3

class CMenu
{
//
// Construction / Destruction:
//
public:
	CMenu(Cx7* pX7) ;
	~CMenu() ;

	Cx7*				  m_pX7 ;
	BOOL				  m_bActive ;
	EnterMenu() ;
	ExitMenu() ;

	SetViewMenuItems(UINT s) ;

protected:	
	CMenuScrollingBackG*  m_pBackG1 ;
	CMenuScrollingBackG*  m_pBackG2 ;
	CScreenOverlay*		  m_pTitle ;
	CMenuItem*			  m_pMainMenuItems[NO_MAINMENU_ITEMS] ;

	int					  m_ndxMainMenuSelected ;

	float				  m_fMouseYMovement ;

//
// Framework events:
//
public:
	HRESULT Render(LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM) ;
    HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, float fAspect, CScreenOverlayManager* pOM, CMenuTextures* pTex) ;
	HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, CScreenOverlayManager* pOM, CUserInput* m_pIn, CSoundManager* pSM) ;
    HRESULT InvalidateDeviceObjects() ;
} ;

#endif