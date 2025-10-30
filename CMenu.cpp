#include "CMenu.h"

CMenu::CMenu(Cx7* pX7)
{
	m_pX7 = pX7 ;
	m_pBackG1 = NULL ;
	m_pBackG2 = NULL ;
	m_pTitle = NULL ;
	for (int i = 0 ; i < NO_MAINMENU_ITEMS ; i ++)
		m_pMainMenuItems[i] = NULL ;
	m_ndxMainMenuSelected = 0 ;

	m_fMouseYMovement = 0.0f ;

	m_bActive = FALSE ;
}

CMenu::~CMenu()
{
}

CMenu::EnterMenu()
{
	SetViewMenuItems(MF_GRAYED) ;
	m_bActive = TRUE ;
	m_pBackG1->m_bActive = TRUE ;
	m_pBackG2->m_bActive = TRUE ;
	m_pTitle->m_bActive = TRUE ;
	for (int i = 0 ; i < NO_MAINMENU_ITEMS ; i ++)
		if (m_pMainMenuItems[i])
			m_pMainMenuItems[i]->m_bActive = TRUE ;
}
CMenu::ExitMenu()
{
	SetViewMenuItems(MF_ENABLED) ;
	m_bActive = FALSE ;
	m_pBackG1->m_bActive = FALSE ;
	m_pBackG2->m_bActive = FALSE ;
	m_pTitle->m_bActive = FALSE ;
	for (int i = 0 ; i < NO_MAINMENU_ITEMS ; i ++)
		if (m_pMainMenuItems[i])
			m_pMainMenuItems[i]->m_bActive = FALSE ;
}

HRESULT CMenu::Render(LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM)
{
	pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
	pDev->SetRenderState(D3DRS_ZENABLE, FALSE) ;

	SetTextureColorStage(pDev, 0, D3DTA_TEXTURE, D3DTOP_MODULATE,	D3DTA_DIFFUSE)
	SetTextureColorStage(pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	D3DTA_CURRENT)

	SetTextureAlphaStage(pDev, 0, D3DTA_TEXTURE, D3DTOP_MODULATE,	D3DTA_DIFFUSE)
	SetTextureAlphaStage(pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	D3DTA_CURRENT)

	pOM->Render(pDev) ;
	return S_OK ;
}

HRESULT CMenu::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect, CScreenOverlayManager* pOM, CMenuTextures* pTex)
{
	float fLogoDistance = -0.05f ;
	float fCoplanarSep = +0.0f ;

// background 1
	pOM->RegisterNewOverlay((m_pBackG1 = new CMenuScrollingBackG(pTex->m_pTex[MENU_TEX_START + 0], 3.0f, 0.2f, 1.0f, 0.01f, 0.3f, fLogoDistance + fCoplanarSep * 2.0f, 0.98f, 0.4f)));
	m_pBackG1->m_bActive = m_bActive ;

// background 2
	pOM->RegisterNewOverlay((m_pBackG2 = new CMenuScrollingBackG(pTex->m_pTex[MENU_TEX_START + 0], 1.0f, 1.0f, 0.75f, 0.01f, 0.3f, fLogoDistance + fCoplanarSep * 1.0f, 0.98f, 0.4f)));
	m_pBackG2->m_bActive = m_bActive ;

// overlay
	pOM->RegisterNewOverlay((m_pTitle = new CScreenOverlay(pTex->m_pTex[MENU_TEX_START + 1], 1.0f, 0.0f, 0.0f, fLogoDistance, 1.0f, 1.0f)));
	m_pTitle->m_bActive = m_bActive ;

// main menu: Play, Settings & Exit
	pOM->RegisterNewOverlay((m_pMainMenuItems[0] = new CMenuItem(pTex->m_pTex[MENU_TEX_START + 2], 1.0f, 0.25f, 0.00f, 0.0f, 0.5f, 0.5f)));
	m_pMainMenuItems[0]->m_bActive = m_bActive ;

	pOM->RegisterNewOverlay((m_pMainMenuItems[1] = new CMenuItem(pTex->m_pTex[MENU_TEX_START + 3], 1.0f, 0.25f, 0.25f, 0.0f, 0.5f, 0.5f)));
	m_pMainMenuItems[1]->m_bActive = m_bActive ;

	pOM->RegisterNewOverlay((m_pMainMenuItems[2] = new CMenuItem(pTex->m_pTex[MENU_TEX_START + 4], 1.0f, 0.25f, 0.50f, 0.0f, 0.5f, 0.5f)));
	m_pMainMenuItems[2]->m_bActive = m_bActive ;

	return S_OK ;
}

HRESULT CMenu::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, CScreenOverlayManager* pOM, CUserInput* pIn, CSoundManager* pSM)
{
	static float fRot1 = 0.0f, fRot2 = 0.0f, fRot3 = 0.0f ;
	D3DXMATRIX matR1, matR2, matR3, matR ;

// FrameMove of overlays handled by OverlayManager, called by owner.
	;
// check for exit status is done by owner.
	;

// animate the title & backg
	if ((fRot1 += (0.02f) * fTimeMult) >= g_PI * 2.0f)
		fRot1 = 0.0f ;
	if ((fRot2 += (0.01f) * fTimeMult) >= g_PI * 2.0f)
		fRot2 = 0.0f ;
	if ((fRot3 += (0.00f) * fTimeMult) >= g_PI * 2.0f)
		fRot3 = 0.0f ;
	D3DXMatrixIdentity(&matR1) ;
	D3DXMatrixIdentity(&matR2) ;
	D3DXMatrixIdentity(&matR3) ;
	D3DXMatrixRotationY(&matR1, fRot1) ;
	D3DXMatrixRotationX(&matR2, fRot2) ;
	D3DXMatrixRotationZ(&matR3, fRot3) ;
	D3DXMatrixMultiply(&matR, &matR1, &matR2) ;
	D3DXMatrixMultiply(&matR, &matR, &matR3) ;
	m_pBackG1->m_matWorld = matR ;
	m_pBackG2->m_matWorld = matR ;
	m_pTitle->m_matWorld = matR ;

// move current item
	m_fMouseYMovement -= pIn->m_fMouseY ;

	if (pIn->m_bKB_Pressed[DIK_UP] || pIn->m_bKB_Pressed[DIK_NUMPAD8] || m_fMouseYMovement < -150.0f)
	{
		m_fMouseYMovement = 0.0f ;
		if (--m_ndxMainMenuSelected < 0)
			m_ndxMainMenuSelected = NO_MAINMENU_ITEMS - 1 ;
		pSM->PlaySFX( SFX_MENU_MOVE );
	}

	if (pIn->m_bKB_Pressed[DIK_DOWN] || pIn->m_bKB_Pressed[DIK_NUMPAD2] || m_fMouseYMovement > +150.0f)
	{
		m_fMouseYMovement = 0.0f ;
		if (++m_ndxMainMenuSelected > NO_MAINMENU_ITEMS - 1)
			m_ndxMainMenuSelected = 0 ;
		pSM->PlaySFX( SFX_MENU_MOVE );
	}

	for (int i = 0 ; i < NO_MAINMENU_ITEMS ; i ++)
		if (m_pMainMenuItems[i])
			m_pMainMenuItems[i]->SetSelected(m_ndxMainMenuSelected == i) ;

// test for selection
	if (pIn->m_bKB_Pressed[DIK_RETURN] ||
		pIn->m_bKB_Pressed[DIK_NUMPADENTER] ||
		pIn->m_bMouseLB)
	{
		pSM->PlaySFX( SFX_MENU_SELECT );

		if (m_ndxMainMenuSelected == 0) // play
		{
			ExitMenu() ;
			m_pX7->m_bInMenu = FALSE ;
			m_pX7->StartGame() ;
		}

		if (m_ndxMainMenuSelected == 2) // exit
		{
			ExitMenu() ;
			m_pX7->m_bAppMustExit = TRUE ;
		}
	}

	return S_OK ;
}

HRESULT CMenu::InvalidateDeviceObjects()
{
	SAFE_DELETE( m_pBackG1 )
	SAFE_DELETE( m_pBackG2 )
	SAFE_DELETE( m_pTitle ) 

	for (int i = 0 ; i < NO_MAINMENU_ITEMS ; i ++)
		SAFE_DELETE( m_pMainMenuItems[i] ) 
	
	return S_OK ;
}

CMenu::SetViewMenuItems(UINT s)
{
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_OBJECTS_SKYDOME, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_OBJECTS_TERRAIN2, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_OBJECTS_FLATGROUND, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_OBJECTS_TRACK, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_OBJECTS_FIGHTER, s) ;

	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_WIREFRAME, s) ;

	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_FREE, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_CAMERA_SHOWOFF, s) ;

	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_FIGHTER_NORMAL, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_FIGHTER_CHROME, s) ;
	EnableMenuItem(m_pX7->m_hMenu, ID_VIEW_FIGHTER_GLASS, s) ;
}