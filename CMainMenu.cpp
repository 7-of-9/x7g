#pragma warning( disable : 4183 )

#include "CMainMenu.h"




CMainMenu::CMainMenu(Cx7* pX7)
{
	m_pX7 = pX7 ;
	m_pBackG1 = NULL ;
	m_pBackG2 = NULL ;
	m_pTitle = NULL ;
	
	m_nMenuItems = 3 ;
	m_ppMenuItems = new CMenuItem*[ m_nMenuItems ];

	for (int i = 0 ; i < m_nMenuItems ; i ++)
		m_ppMenuItems[i] = NULL ;
	m_ndxMenuSelected = 0 ;

	m_fMouseYMovement = 0.0f ;

	m_bActive = FALSE ;
	
	m_msState = MS_IDLE ;
	m_ndxSelected = 0 ;
}

CMainMenu::~CMainMenu()
{
	SAFE_DELETE_ARRAY( m_ppMenuItems )
}

CMainMenu::EnterMenu()
{
	SetViewMenuItems(MF_GRAYED) ;
	m_msState = MS_IDLE ;
	m_ndxSelected = 0 ;
	m_bActive = TRUE ;

	m_pBackG1->m_bActive = TRUE ;
	m_pBackG1->m_bFrameMoveRequired = TRUE ;
	m_pBackG1->m_matWorld = g_matIdent ;

	m_pBackG2->m_bActive = TRUE ;
	m_pBackG2->m_bFrameMoveRequired = TRUE ;
	m_pBackG2->m_matWorld = g_matIdent ;

	m_pTitle->m_bActive = TRUE ;
	m_pTitle->m_bFrameMoveRequired = TRUE ;
	m_pTitle->m_matWorld = g_matIdent ;

	for (int i = 0 ; i < m_nMenuItems ; i ++)
		if (m_ppMenuItems[i])
		{
			m_ppMenuItems[i]->m_bActive = TRUE ;
			m_ppMenuItems[i]->m_bFrameMoveRequired = TRUE ;
			m_ppMenuItems[i]->m_matWorld = g_matIdent ;
		}
}
CMainMenu::ExitMenu()
{
	SetViewMenuItems(MF_ENABLED) ;
	m_bActive = FALSE ;
	m_pBackG1->m_bActive = FALSE ;
	m_pBackG2->m_bActive = FALSE ;
	m_pTitle->m_bActive = FALSE ;
	for (int i = 0 ; i < m_nMenuItems ; i ++)
		if (m_ppMenuItems[i])
			m_ppMenuItems[i]->m_bActive = FALSE ;
}

HRESULT CMainMenu::Render(LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM)
{
	pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
	pDev->SetRenderState(D3DRS_ZENABLE, FALSE) ;

	SetTextureColorStage(pDev, 0, D3DTA_TEXTURE, D3DTOP_MODULATE,	D3DTA_DIFFUSE)
	SetTextureColorStage(pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	D3DTA_CURRENT)

	SetTextureAlphaStage(pDev, 0, D3DTA_TEXTURE, D3DTOP_MODULATE,	D3DTA_DIFFUSE)
	SetTextureAlphaStage(pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	D3DTA_CURRENT)

	pOM->SetProjMat( pDev );
	pOM->Render( pDev, OT_MENU, ORT_FAST );

	return S_OK ;
}

HRESULT CMainMenu::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect, CScreenOverlayManager* pOM, CMenuTextures* pTex)
{
	ods( "CMainMenu::RestoreDeviceObjects\n" );

	float fLogoDistance = +0.2f ;
	float fCoplanarSep = +0.0f ;

// background 1
	pOM->RegisterNewOverlay((m_pBackG1 = new CMenuScrollingBackG(pTex->m_pTex[MENU_TEX_START + 0], 3.0f, 0.2f, 1.0f, 0.01f, 0.3f, fLogoDistance + fCoplanarSep * 2.0f, 0.98f, 0.4f)));
	m_pBackG1->m_bActive = m_bActive ;
	m_pBackG1->m_bFrameMoveRequired  = TRUE ;

// background 2
	pOM->RegisterNewOverlay((m_pBackG2 = new CMenuScrollingBackG(pTex->m_pTex[MENU_TEX_START + 0], 1.0f, 1.0f, 0.75f, 0.01f, 0.3f, fLogoDistance + fCoplanarSep * 1.0f, 0.98f, 0.4f)));
	m_pBackG2->m_bActive = m_bActive ;
	m_pBackG2->m_bFrameMoveRequired  = TRUE ;

// overlay
	pOM->RegisterNewOverlay((m_pTitle = new CScreenOverlay(pTex->m_pTex[MENU_TEX_START + 1], 1.0f, 0.0f, 0.0f, fLogoDistance, 1.0f, 1.0f, OT_MENU)));
	m_pTitle->m_bActive = m_bActive ;
	m_pTitle->m_bFrameMoveRequired  = TRUE ;

// main menu: Play, Settings & Exit
	pOM->RegisterNewOverlay((m_ppMenuItems[0] = new CMenuItem(pTex->m_pTex[MENU_TEX_START + 2], 1.0f, 0.25f, 0.00f, 0.0f, 0.5f, 0.5f)));
	m_ppMenuItems[0]->m_bActive = m_bActive ;
	m_ppMenuItems[0]->m_bFrameMoveRequired  = TRUE ;

	pOM->RegisterNewOverlay((m_ppMenuItems[1] = new CMenuItem(pTex->m_pTex[MENU_TEX_START + 3], 1.0f, 0.25f, 0.25f, 0.0f, 0.5f, 0.5f)));
	m_ppMenuItems[1]->m_bActive = m_bActive ;
	m_ppMenuItems[1]->m_bFrameMoveRequired  = TRUE ;

	pOM->RegisterNewOverlay((m_ppMenuItems[2] = new CMenuItem(pTex->m_pTex[MENU_TEX_START + 4], 1.0f, 0.25f, 0.50f, 0.0f, 0.5f, 0.5f)));
	m_ppMenuItems[2]->m_bActive = m_bActive ;
	m_ppMenuItems[2]->m_bFrameMoveRequired  = TRUE ;

	return S_OK ;
}

HRESULT CMainMenu::FrameMove( LPDIRECT3DDEVICE8 pDev, float fFPS, CScreenOverlayManager* pOM, CUserInput* pIn, CSoundManager* pSM, float fElapsedTime, BOOL bAutoStart )
{
	int i ;
	static float fRot1 = 0.0f, fRot2 = 0.0f, fRot3 = 0.0f ;
	xMat matR1, matR2, matR3, matR ;

// animate the title & backg
	if ((fRot1 += (0.025f) * (60.0f/fFPS)) >= g_PI * 2.0f) fRot1 = 0.0f ;
	if ((fRot2 += (0.00f) * (60.0f/fFPS)) >= g_PI * 2.0f) fRot2 = 0.0f ;
	if ((fRot3 += (0.00f) * (60.0f/fFPS)) >= g_PI * 2.0f) fRot3 = 0.0f ;
	matR1 = matR2 = matR3 = g_matIdent ;
	D3DXMatrixRotationY(&matR1, fRot1) ;
	D3DXMatrixRotationX(&matR2, fRot2) ;
	D3DXMatrixRotationZ(&matR3, fRot3) ;
	D3DXMatrixMultiply(&matR, &matR1, &matR2) ;
	D3DXMatrixMultiply(&matR, &matR, &matR3) ;
	m_pBackG1->m_matWorld = matR ;
	m_pBackG2->m_matWorld = matR ;
	m_pTitle->m_matWorld = matR ;

	switch( m_msState )
	{
		case MS_IDLE:
		// move current item
			m_fMouseYMovement -= pIn->m_fMouseY ;
			if( pIn->m_bKB_Pressed[DIK_UP] ||
				pIn->m_bKB_Pressed[DIK_NUMPAD8] ||
				m_fMouseYMovement < -150.0f || 
				pIn->m_bMouseWheelUp )
			{
				m_fMouseYMovement = 0.0f ;
				if (--m_ndxMenuSelected < 0)
					m_ndxMenuSelected = m_nMenuItems - 1 ;
				pSM->PlaySFX( SFX_MENU_MOVE );
			}
			if( pIn->m_bKB_Pressed[DIK_DOWN] ||
				pIn->m_bKB_Pressed[DIK_NUMPAD2] ||
				m_fMouseYMovement > +150.0f ||
				pIn->m_bMouseWheelDown )
			{
				m_fMouseYMovement = 0.0f ;
				if (++m_ndxMenuSelected > m_nMenuItems - 1)
					m_ndxMenuSelected = 0 ;
				pSM->PlaySFX( SFX_MENU_MOVE );
			}

		// update highlight
			for ( i = 0 ; i < m_nMenuItems ; i ++)
				if (m_ppMenuItems[i])
					m_ppMenuItems[i]->SetSelected(m_ndxMenuSelected == i, pOM->m_pVBVertices) ;

		// test for selection
			if (pIn->m_bKB_Pressed[DIK_RETURN] ||
				pIn->m_bKB_Pressed[DIK_NUMPADENTER] ||
				pIn->m_bKB_Pressed[DIK_SPACE] ||
				pIn->m_bMouseLB || 
				bAutoStart )
			{
				if( m_ndxMenuSelected == 0 || m_ndxMenuSelected == 2 )
				{
					pSM->PlaySFX( SFX_MENU_SELECT );
					m_msState = MS_OUTRO ;
					m_fOutroPlaytime = 0.0f ;
					m_ndxSelected = m_ndxMenuSelected ;
				}
			}
			
			break ;

		case MS_OUTRO:
		// spin non-picked menuitems
			m_fOutroPlaytime += (1.0f/fFPS) ;
			for( i = 0 ; i < m_nMenuItems ; i++ )
				if( i != m_ndxSelected)
				{
					D3DXMatrixTranslation( &matR1, 0.0f, 0.0f, m_fOutroPlaytime * 20.0f );
					m_ppMenuItems[i]->m_matWorld = matR1 ;
				}
				else
				{
					D3DXMatrixTranslation( &matR1, 0.0f, 0.0f, -m_fOutroPlaytime * 2.0f );
					m_ppMenuItems[i]->m_matWorld = matR1 ;
				}

			if( m_fOutroPlaytime > 1.0f )
			{
				if( m_ndxSelected == 0 ) // play
				{
					ExitMenu() ;
					m_pX7->m_bInMainMenu = FALSE ;
					m_pX7->StartGame() ;
				}
				if( m_ndxSelected == 2 ) // exit
				{
					ExitMenu() ;
					m_pX7->m_bAppMustExit = TRUE ;
				}
			}
			break ;
	}

	return S_OK ;
}

HRESULT CMainMenu::InvalidateDeviceObjects()
{
	SAFE_DELETE( m_pBackG1 )
	SAFE_DELETE( m_pBackG2 )
	SAFE_DELETE( m_pTitle ) 

	for (int i = 0 ; i < m_nMenuItems ; i ++)
		SAFE_DELETE( m_ppMenuItems[i] ) 

	return S_OK ;
}

CMainMenu::SetViewMenuItems(UINT s)
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