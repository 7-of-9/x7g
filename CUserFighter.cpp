#pragma warning( disable : 4183 )	// 'missing return type'

#include "CFighters.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CUserFighter::CUserFighter( float fSkill,
								int nLevel,
								int nLives,
								int/*e_FighterTypes*/ ftType,
								float fMyBSRadius,
								CTerrain* pTerrain,
								CScreenOverlayManager* pOM,
								CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData ):
	CFighter( fSkill, ftType, fMyBSRadius, pSharedRenderData, TRUE )
	{
	//
	// Skill Modifier:
	//
		m_fEngineLevel = 0.35f ; 
		m_fMaxThrustMag *= 1 + fSkill ;

	//
	// Terrain Force:
	//
		m_fMaxTerrainForce = m_fMaxThrustMag / 100 ;
		m_fTerrainForceBalanceAltitude = fMyBSRadius * 10 ;

	//
	// Misc.
	//
		m_bUserFighter = TRUE ;

		m_pOM = pOM ;
		m_fScoreAddition = 0 ;
		m_fTimeInScoreZone = 0 ;
		m_nCherries = 0 ;
		m_bGameOver = FALSE ;

	//
	// HUD screen overlays:
	//
		m_pHUDShieldLevel = NULL ;
		m_pHUDShieldBackG = NULL ;
		m_pHUDWeaponsLevel = NULL ;
		m_pHUDWeaponsBackG = NULL ;
		m_pHUDEngineLevel = NULL ;
		m_pHUDEngineBackG = NULL ;

		float fCharSize ;
		
		m_pHUDSpeed = NULL ;
		m_pHUDScore = NULL ;
		m_pHUDItems = NULL ;
		m_pHUDLives = NULL ;

		char sz[256] ;
		fCharSize = 0.035f ;
		sprintf(sz, "%2d/%2d", m_nCherries, pTerrain->m_nCherries );
		m_pHUDScore = new CHUDText( "0           ",	   HUD_TEX_START + 0, 0.0f, 0.0f, fCharSize*4, fCharSize*4, fCharSize*4 * 0.4f, OT_HUD_STRING4 );
		m_pHUDSpeed = new CHUDText( "0000 m/s [000%]", HUD_TEX_START + 0, 0.4f, 0.825f + 0.0525f * 1, fCharSize, fCharSize, fCharSize * 0.4f, OT_HUD_STRING3 );
		m_pHUDItems = new CHUDText( sz,				   HUD_TEX_START + 0, 0.4f, 0.825f + 0.0525f * 2, fCharSize, fCharSize, fCharSize * 0.4f, OT_HUD_STRING3 );

		fCharSize = 0.05f ;
		sprintf( sz, "LIVES %d", nLives );
		m_pHUDLives = new CHUDText( sz, HUD_TEX_START + 0, 0.5f - fCharSize * 0.6f * 4, 0, fCharSize, fCharSize, fCharSize * 0.4f, OT_HUD_STRING1 );

	//
	// HUD SpaceScanner:
	//
		m_pScannerBackG = NULL ;
		m_pVB_Scanner = NULL ;
		m_nScannerPoints = 0 ;
		m_fScannerRot = 0.0f ;
		m_fScannerSize = 0.25f ;
		m_vScannerPos = xVec2( 0.35f, -0.35f );

	//
	// HUD target icon:
	//
		m_pTargetIcon = NULL ;
		m_fTargetIconMinSize = 0.5f ;
		m_fTargetIconMaxSize = 1.0f ;

	//
	// HUD crosshair:
	//
		m_pCrosshair = NULL ;

	//
	// HUD pain:
	//
		m_pRedQuad = NULL ;

	//
	// HUD Misc. text:
	//
		m_bShowingHUDText = false ;
		m_fHUDTextAge = 0 ;
		m_fHUDTextLifespan = 0 ;
	}
	CUserFighter::~CUserFighter()
	{
		SAFE_DELETE( m_pHUDScore )
		SAFE_DELETE( m_pHUDItems )
		SAFE_DELETE( m_pHUDSpeed )
		SAFE_DELETE( m_pHUDLives )
		SAFE_DELETE( m_pScannerBackG )
		SAFE_DELETE( m_pTargetIcon )
		SAFE_DELETE( m_pCrosshair )
		SAFE_DELETE( m_pRedQuad )
	}

	HRESULT CUserFighter::RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM, CTextures* pTex, float fAspect )
	{
		ods( "CUserFighter::RestoreDeviceObjects\n" );

		HRESULT hr ;
		D3DXCOLOR c, c2 ;

		ods("CUserFighter::RestoreDeviceObjects\n") ;

		CFighter::RestoreDeviceObjects( pD3D, pDev, pOM, pTex, fAspect );
		if( m_pHUDScore )
		{
			m_pHUDScore->RestoreDeviceObjects( pDev, fAspect, pOM, pTex );
			m_pHUDScore->Enable( TRUE );
		}
		if( m_pHUDItems )
		{
			m_pHUDItems->RestoreDeviceObjects( pDev, fAspect, pOM, pTex );
			m_pHUDItems->Enable( TRUE );
		}
		if( m_pHUDSpeed )
		{
			m_pHUDSpeed->RestoreDeviceObjects( pDev, fAspect, pOM, pTex );
			m_pHUDSpeed->Enable( TRUE );
		}
		if( m_pHUDLives )
		{
			m_pHUDLives->RestoreDeviceObjects( pDev, fAspect, pOM, pTex );
			m_pHUDLives->Enable( TRUE );
		}

		pOM->m_bVBLocked = TRUE ;

	//
	// HUD SpaceScanner:
	//
		c = D3DXCOLOR( 0.2f, 0.2f, 1.0f, 1.00f );
		m_pScannerBackG = new CScreenOverlay( pTex->m_pTex[ HUD_TEX_START+1 ], 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, &c, &c, &c, &c, OT_HUD_SCANNERBACKG );
		assert( m_pScannerBackG != NULL );
		pOM->RegisterNewOverlay( m_pScannerBackG );
		m_pScannerBackG->m_bActive = TRUE ;
		m_pScannerBackG->m_pSharedVB = pOM->m_pVB ;
		m_pScannerBackG->RestoreDeviceObjects( pDev, fAspect );

		hr = pDev->CreateVertexBuffer( MAX_SCANNER_POINTS * sizeof( POINTSPRITEVERTEX ),
		                               D3DUSAGE_DYNAMIC,
									   MYFVF_POINTSPRITEVERTEX,
				                       D3DPOOL_DEFAULT,
									   &m_pVB_Scanner );
		assert( !FAILED(hr) );

	//
	// HUD target icon:
	//
		c  = D3DXCOLOR( 1,1,1,0.5f );
		c2 = D3DXCOLOR( 1,1,1,0.5f );
		m_pTargetIcon = new CScreenOverlay( pTex->m_pTex[ HUD_TEX_START+2 ], 1.0f, 0.5f - (0.25f/2), 0.0f, 0.0f, 0.25f, 0.5f, &c, &c, &c2, &c2, OT_HUD_TARGETICON );
		assert( m_pTargetIcon != NULL );
		pOM->RegisterNewOverlay( m_pTargetIcon );
		m_pTargetIcon->m_bActive = TRUE ;
		m_pTargetIcon->m_pSharedVB = pOM->m_pVB ;
		m_pTargetIcon->RestoreDeviceObjects( pDev, fAspect );

	//
	// HUD crosshair:
	//
		c = D3DXCOLOR( 1,1,1,0.5f );
		float fW = 1 * (1/fAspect) ;
		m_pCrosshair = new CScreenOverlay( pTex->m_pTex[ HUD_TEX_START+3 ], 1.0f, (1-fW)/2, 0, 0, fW, 1, &c, &c, &c, &c, OT_HUD_CROSSHAIR );
		assert( m_pCrosshair != NULL );
		pOM->RegisterNewOverlay( m_pCrosshair );
		m_pCrosshair->m_bActive = TRUE ;
		m_pCrosshair->m_pSharedVB = pOM->m_pVB ;
		m_pCrosshair->RestoreDeviceObjects( pDev, fAspect );

	//
	// HUD pain:
	//
		c = D3DXCOLOR( 0,0,0,0 );	// colours will come from material specified in RenderHUD()
		m_pRedQuad = new CScreenOverlay( pTex->m_pTex[ MISC_TEX_START+4 ], 1.0f, 0, 0, 0, 1, 1, &c, &c, &c, &c, OT_HUD_PAIN );
		assert( m_pRedQuad != NULL );
		pOM->RegisterNewOverlay( m_pRedQuad );
		m_pRedQuad->m_bActive = TRUE ;
		m_pRedQuad->m_pSharedVB = pOM->m_pVB ;
		m_pRedQuad->RestoreDeviceObjects( pDev, fAspect );

	//
	// HUD Overlays:
	//
		float fWidth = 0.35f ;
		float fHeight = 0.035f ;
		float fSep = fHeight / 2.0f ;
		float fTop = 0.825f ;
		float fLeft = 0.035f ;
		float fBackGAlpha = 0.1f ;
		float fForeGAlpha = 0.1f ;

		/*D3DXCOLOR c = D3DXCOLOR(0.5f, 0.5f, 0.9f, 0.75f) ;
		m_pSystems = new CScreenOverlay( pTex->m_pTex[ HUD_TEX_START+1 ], 1.0f, -0.1f, 0.5f, 0.0f, 0.5f, 0.5f, &c, &c, &c, &c, OT_HUD_LEGEND);
		pOM->RegisterNewOverlay( m_pSystems );
		m_pSystems->m_bActive = TRUE ;
		m_pSystems->m_pSharedVB = pOM->m_pVB ;
		m_pSystems->RestoreDeviceObjects( pDev, fAspect );*/

	// shield scalebar
		m_pHUDShieldBackG = new CHUDScaleBar( NULL, 10.0f, 1.0f, 0.5f, 0.5f, 0.5f, fBackGAlpha, FALSE, fLeft, fTop, 0.0f, fWidth, fHeight ); 
		pOM->RegisterNewOverlay( m_pHUDShieldBackG );
		m_pHUDShieldBackG->m_bActive = TRUE ;
		m_pHUDShieldBackG->m_pSharedVB = pOM->m_pVB ;
		m_pHUDShieldBackG->RestoreDeviceObjects( pDev, fAspect );
		m_pHUDShieldBackG->m_fLevel = 1.0f ;
		m_pHUDShieldBackG->m_bFrameMoveRequired  = TRUE ;

		m_pHUDShieldLevel = new CHUDScaleBar( NULL, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, fForeGAlpha, TRUE, fLeft, fTop, 0.0f, fWidth, fHeight ); 
		pOM->RegisterNewOverlay( m_pHUDShieldLevel );
		m_pHUDShieldLevel->m_bActive = TRUE ;
		m_pHUDShieldLevel->m_pSharedVB = pOM->m_pVB ;
		m_pHUDShieldLevel->RestoreDeviceObjects( pDev, fAspect );
		m_pHUDShieldLevel->m_fLevel = m_fShieldLevel ;
		m_pHUDShieldLevel->m_bFrameMoveRequired  = TRUE ;

		fTop += fHeight ;
		fTop += fSep ;

	// weapons scalebar
		m_pHUDWeaponsBackG = new CHUDScaleBar( NULL, 10.0f, 1.0f, 0.5f, 0.5f, 0.5f, fBackGAlpha, FALSE, fLeft, fTop, 0.0f, fWidth, fHeight ); 
		pOM->RegisterNewOverlay( m_pHUDWeaponsBackG );
		m_pHUDWeaponsBackG->m_bActive = TRUE ;
		m_pHUDWeaponsBackG->m_pSharedVB = pOM->m_pVB ;
		m_pHUDWeaponsBackG->RestoreDeviceObjects( pDev, fAspect );
		m_pHUDWeaponsBackG->m_fLevel = 1.0f ;
		m_pHUDWeaponsBackG->m_bFrameMoveRequired  = TRUE ;

		m_pHUDWeaponsLevel = new CHUDScaleBar( NULL, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, fForeGAlpha, TRUE, fLeft, fTop, 0.0f, fWidth, fHeight ); 
		pOM->RegisterNewOverlay( m_pHUDWeaponsLevel );
		m_pHUDWeaponsLevel->m_bActive = TRUE ;
		m_pHUDWeaponsLevel->m_pSharedVB = pOM->m_pVB ;
		m_pHUDWeaponsLevel->RestoreDeviceObjects( pDev, fAspect);
		m_pHUDWeaponsLevel->m_fLevel = m_fWeaponsLevel ;
		m_pHUDWeaponsLevel->m_bFrameMoveRequired  = TRUE ;

		fTop += fHeight ;
		fTop += fSep ;

	// engine scalebar
		m_pHUDEngineBackG = new CHUDScaleBar( NULL, 10.0f, 1.0f, 0.5f, 0.5f, 0.5f, fBackGAlpha, FALSE, fLeft, fTop, 0.0f, fWidth, fHeight ); 
		pOM->RegisterNewOverlay( m_pHUDEngineBackG );
		m_pHUDEngineBackG->m_bActive = TRUE ;
		m_pHUDEngineBackG->m_pSharedVB = pOM->m_pVB ;
		m_pHUDEngineBackG->RestoreDeviceObjects( pDev, fAspect );
		m_pHUDEngineBackG->m_fLevel = 1.0f ;
		m_pHUDEngineBackG->m_bFrameMoveRequired  = TRUE ;

		m_pHUDEngineLevel = new CHUDScaleBar( NULL, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, fForeGAlpha, TRUE, fLeft, fTop, 0.0f, fWidth, fHeight ); 
		pOM->RegisterNewOverlay( m_pHUDEngineLevel );
		m_pHUDEngineLevel->m_bActive = TRUE ;
		m_pHUDEngineLevel->m_pSharedVB = pOM->m_pVB ;
		m_pHUDEngineLevel->RestoreDeviceObjects( pDev, fAspect );
		m_pHUDEngineLevel->m_fLevel = m_fEngineLevel ;
		m_pHUDEngineLevel->m_bFrameMoveRequired  = TRUE ;

		pOM->m_bVBLocked = FALSE ;
		return S_OK ;
	}

	HRESULT	CUserFighter::InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		ods("CUserFighter::InvalidateDeviceObjects\n") ;

		CFighter::InvalidateDeviceObjects( pDev );
		if( m_pHUDItems )
			m_pHUDItems->InvalidateDeviceObjects() ;
		if( m_pHUDScore )
			m_pHUDScore->InvalidateDeviceObjects() ;
		if( m_pHUDLives )
			m_pHUDScore->InvalidateDeviceObjects() ;
		if( m_pHUDSpeed )
			m_pHUDSpeed->InvalidateDeviceObjects() ;

		SAFE_DELETE( m_pHUDShieldLevel )
		SAFE_DELETE( m_pHUDShieldBackG )
		SAFE_DELETE( m_pHUDWeaponsLevel )
		SAFE_DELETE( m_pHUDWeaponsBackG )
		SAFE_DELETE( m_pHUDEngineLevel )
		SAFE_DELETE( m_pHUDEngineBackG )


		SAFE_RELEASE( m_pVB_Scanner )
		SAFE_DELETE( m_pScannerBackG )
		
		SAFE_DELETE( m_pTargetIcon )
		SAFE_DELETE( m_pCrosshair )

		SAFE_DELETE( m_pRedQuad )

		return S_OK ;
	}


	CUserFighter::EnableHUD( BOOL bActive )
	{
		if( m_pHUDShieldLevel )
			m_pHUDShieldLevel->m_bActive = bActive ;
		if( m_pHUDShieldBackG )
			m_pHUDShieldBackG->m_bActive = bActive ;
		
		if( m_pHUDWeaponsLevel )
			m_pHUDWeaponsLevel->m_bActive = bActive ;
		if( m_pHUDWeaponsBackG )
			m_pHUDWeaponsBackG->m_bActive = bActive ;
		
		if( m_pHUDEngineLevel )
			m_pHUDEngineLevel->m_bActive = bActive ;
		if( m_pHUDEngineBackG )
			m_pHUDEngineBackG->m_bActive = bActive ;

		if( m_pHUDSpeed )
			m_pHUDSpeed->Enable( bActive );
		if( m_pHUDItems )
			m_pHUDItems->Enable( bActive );
		if( m_pHUDScore )
			m_pHUDScore->Enable( bActive );
		if( m_pHUDLives )
			m_pHUDLives->Enable( bActive );

		if( m_pScannerBackG )
			m_pScannerBackG->m_bActive = bActive ;

		if( m_pTargetIcon )
			m_pTargetIcon->m_bActive = bActive ;
		if( m_pCrosshair )
			m_pCrosshair->m_bActive = bActive ;

		if( m_pRedQuad )
			m_pRedQuad->m_bActive = bActive ;
	}


/*
 *
 * Item Collision Detection and Response:
 *
 * Uses (fast?) sphere-sphere test if distance travelled is less than item's radius,
 * otherwise falls back on (slower?) sphere-ray test.
 *
 * FIXME: sphere-ray test produces incorrect results when moving *backwards*
 * This doesn't seem crucial, as we never seem to travel fast enough to have to have to use it.
 *
 */

#define TER_RC2ABS(r,c)\
			(r)*pTerrain->m_nPatchesSQ+(c)

	BOOL CUserFighter::DetectAndHandleItemCollisions( xVec3* pvCurPos, xVec3* pvTestPos, CTerrain* pTerrain, CRace* pRace )
	{
		BOOL bCollision ;
		xVec3 vProbe = (*pvTestPos) - (*pvCurPos) ;
		xVec3 vPos ;
		float fDistance = D3DXVec3Length( &vProbe );

		if( fDistance == 0.0f )
			return FALSE ;

		for( int x = 0 ; x < pTerrain->m_nPatchesSQ ; x++ )
			for( int y = 0 ; y < pTerrain->m_nPatchesSQ ; y++ )
				if( pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_bVisible )
				{
					int nItemsOnThisPatch = pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_nItems ;
					for( int i = 0 ; i < nItemsOnThisPatch ; i++ )
					{
						bCollision = FALSE ;

float fItemAbsRadius = pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].fRadius * ( pTerrain->m_fLenSQ / 160.0f );
xVec3 vItemAbsPos = pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].vPos + xVec3( 0, fItemAbsRadius, 0 );

						if( fDistance < fItemAbsRadius )
						{
						// perform sphere-sphere intersection test; fast?
							float fMinD = fItemAbsRadius + m_pFRO->m_pRenderData->m_fBSphereRadius ;
							float fD = D3DXVec3Length( &(vItemAbsPos -  m_vPosition) );
							if( fD < fMinD )
								bCollision = TRUE ;
						}
						else
						{
						// perform sphere-ray intersection test; slower?
							bCollision = SphereLineTest( *pvCurPos, *pvTestPos, vItemAbsPos, fItemAbsRadius, NULL, NULL, NULL );
							//ods(" INFO: CUserFighter::DetectAndHandleItemCollisions: Falling back on SphereLineTest\n" );
						}

					// respond to collision
						if( bCollision )
						{
							pRace->m_pX7->m_pSound->PlaySFX3D( SFX_MENU_SELECT, &m_vPosition, NULL, 1 );

							if( IT_CHERRY == pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].nType )
							{
								m_nCherries++ ;
								sprintf( m_pHUDItems->m_szText, "%2d/%2d", m_nCherries, pTerrain->m_nCherries );
								m_pHUDItems->SetFrameMoveRequired( TRUE );
								pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_nCherries-- ;
								
								m_bShowingHUDText = true ;
								if( pRace->m_pTerrain->m_nCherries < 10 )
									sprintf( m_szHUDText, " %2d/%d",
										m_nCherries, pRace->m_pTerrain->m_nCherries );
								else
									sprintf( m_szHUDText, "%2d/%d",
										m_nCherries, pRace->m_pTerrain->m_nCherries );

								m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 2 ;
								m_fHUDTextX = -1 ; m_fHUDTextY = -0.7f ;
								m_fHUDTextXScale = 0.5f ; m_fHUDTextYScale = 0.6f ;
								m_fHUDTextRed = 1 ; m_fHUDTextGreen = 1 ; m_fHUDTextBlue = 1 ; m_fHUDTextAlpha = 0.1f ;
							}
							if( IT_HEALTH == pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].nType )
							{
								pRace->m_pX7->m_nLives++ ;
								sprintf( m_pHUDLives->m_szText, "LIVES %d", pRace->m_pX7->m_nLives );
								m_pHUDLives->SetFrameMoveRequired( TRUE );
								pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_nHealthPwrUps-- ;
								
								m_bShowingHUDText = true ;
								strcpy( m_szHUDText, "LIFE" );
								m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 2 ;
								m_fHUDTextX = -1 ; m_fHUDTextY = -1 ;
								m_fHUDTextXScale = 0.65f ; m_fHUDTextYScale = 0.65f ;
								m_fHUDTextRed = 1 ; m_fHUDTextGreen = 0 ; m_fHUDTextBlue = 0 ; m_fHUDTextAlpha = 0.2f ;
							}
							if( IT_ENGINE == pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].nType )
							{
								m_fEngineLevel += 0.55f ;
								//if( m_fEngineLevel > 1 ) m_fEngineLevel = 1 ;
								m_pHUDEngineLevel->m_fLevel = m_fEngineLevel ;
								m_pHUDEngineLevel->m_bFrameMoveRequired  = TRUE ;

								m_bShowingHUDText = true ;
								strcpy( m_szHUDText, "ENGINE BOOST" );
								m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 2 ;
								m_fHUDTextX = -1 ; m_fHUDTextY = -0.25f ;
								m_fHUDTextXScale = 0.16f ; m_fHUDTextYScale = 0.25f ;
								m_fHUDTextRed = 0 ; m_fHUDTextGreen = 1 ; m_fHUDTextBlue = 0 ; m_fHUDTextAlpha = 0.15f ;
							}
							if( IT_WEAPON == pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].nType )
							{
								m_fWeaponsLevel += 0.5f ;
								if( m_fWeaponsLevel > 1 ) m_fWeaponsLevel = 1 ;
								m_pHUDWeaponsLevel->m_fLevel = m_fWeaponsLevel ;
								m_pHUDWeaponsLevel->m_bFrameMoveRequired  = TRUE ;

								m_bShowingHUDText = true ;
								strcpy( m_szHUDText, "WEAPON BOOST" );
								m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 2 ;
								m_fHUDTextX = -1 ; m_fHUDTextY = -0.25f ;
								m_fHUDTextXScale = 0.16f ; m_fHUDTextYScale = 0.25f ;
								m_fHUDTextRed = 1 ; m_fHUDTextGreen = 0 ; m_fHUDTextBlue = 0 ; m_fHUDTextAlpha = 0.15f ;
							}

							// DBG:
								/*m_bShowingHUDText = true ;
								strcpy( m_szHUDText, "LIFE" );
								m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 2 ;
								m_fHUDTextX = -1 ; m_fHUDTextY = -1 ;
								m_fHUDTextXScale = 0.65f ; m_fHUDTextYScale = 0.65f ;
								m_fHUDTextRed = 1 ; m_fHUDTextGreen = 0 ; m_fHUDTextBlue = 0 ; m_fHUDTextAlpha = 0.1f ;*/
							
						// remove the item from the terrain patch's list of items;
							if( i != nItemsOnThisPatch-1 && nItemsOnThisPatch != 1 )
								memcpy( &pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ],
										&pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i+1 ],
										(nItemsOnThisPatch - i - 1) * sizeof(GAMEITEM) );
							
							pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_nItems-- ;
							nItemsOnThisPatch-- ;
							i-- ;

							ods("got an item, i was==%d, @(%0.1f, %0.1f, %0.1f), %d left on this patch, I've got %d cherries\n", i+1, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z, pTerrain->m_pPatches[TER_RC2ABS(x,y)]->m_nItems, m_nCherries );
						}
					}
				}

		return TRUE ;
	}
	
/*
 *
 * Physics Modeling from User Input.
 *
 */

	int CUserFighter::FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, BOOL bAcceptInput, BOOL bManageCamera, CTerrain *pTerrain, int ctCamMode, float fElapsedTime, BOOL bShowHUD, CRace* pRace )
	{
		BOOL bAcceptUserInput ;

	//
	// DBG:
	//
		/*m_bShowingHUDText = true ;
		strcpy( m_szHUDText, "GO!" );
		m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 2 ;
		m_fHUDTextX = -0.93f ; m_fHUDTextY = -0.77f;
		m_fHUDTextXScale = 0.65f ; m_fHUDTextYScale = 0.65f ;
		m_fHUDTextRed = 1 ; m_fHUDTextGreen = 0 ; m_fHUDTextBlue = 0 ; m_fHUDTextAlpha = 0.2f ;*/


	//
	// Process user-supplied flight controls:
	//
		bAcceptUserInput = TRUE ;
		if( !bAcceptInput )
			bAcceptUserInput = FALSE ;
		else
			if( m_fsState == FS_EXPLODING )
				bAcceptUserInput = FALSE ;

		xVec3 vStartPosition = m_vPosition ;
		float fYawDelta ;

#ifdef FULL_DBG_TRACE
	ods( "\tCUserFighter::FrameMove() 1\n" );
#endif

		ProcessPhysicsFromFlightControls( &pInput->m_FlightControls, fFPS, pTerrain, bAcceptUserInput, pRace, pDev, fElapsedTime, &fYawDelta );
		m_fScannerRot += fYawDelta ;

#ifdef FULL_DBG_TRACE
	ods( "\tCUserFighter::FrameMove() 2\n" );
#endif

	//
	// Test the updated position for gameitem collisions:
	//
		DetectAndHandleItemCollisions( &m_vPosition, &vStartPosition, pRace->m_pTerrain, pRace );

#ifdef FULL_DBG_TRACE
	ods( "\tCUserFighter::FrameMove() 3\n" );
#endif

	//
	// Check for Death!
	//
		if( m_fShieldLevel == 0 && m_fsState == FS_NORMAL )
		{
			pRace->m_ctCamMode = CT_SHOWOFF ;
			if( pRace->m_pX7->m_nLives > 0 )
			{
				if( !pRace->m_pX7->m_bGodMode )
				{
					pRace->m_pX7->m_nLives-- ;
					sprintf( m_pHUDLives->m_szText, "LIVES %d", pRace->m_pX7->m_nLives );
					m_pHUDLives->SetFrameMoveRequired( TRUE );
				}
			}
			else
			{
				m_bShowingHUDText = true ;
				strcpy( m_szHUDText, "GAME OVER!" );
				m_fHUDTextAge = 0 ;	m_fHUDTextLifespan = 1 ;
				m_fHUDTextX = -1 ; m_fHUDTextY = -0.25f ;
				m_fHUDTextXScale = 0.2f ;	m_fHUDTextYScale = 0.25f ;
				m_fHUDTextRed = 1 ;	m_fHUDTextGreen = 1 ; m_fHUDTextBlue = 1 ; m_fHUDTextAlpha = 1 ;

				m_bGameOver = TRUE ;
			}
		}

#ifdef FULL_DBG_TRACE
	ods( "\tCUserFighter::FrameMove() 4\n" );
#endif

	//
	// Baseclass FrameMove;
	//
		int ret ;
		ret = CFighter::FrameMove( pDev, pInput, fFPS, pCamera, pmatProj, pVP, bManageCamera, pTerrain, ctCamMode, fElapsedTime, pRace );

#ifdef FULL_DBG_TRACE
	ods( "\tCUserFighter::FrameMove() 5\n" );
#endif

	//
	// Respawn, or Game Over:
	//
		if( m_bJustFinishedExploding )
		{
			pRace->m_ctCamMode = CT_BEHIND_VARIABLE2 ;
			m_bJustFinishedExploding = FALSE ;
			if( m_bGameOver )
			{
				pRace->m_pX7->m_bGamePaused = FALSE ;
				pRace->m_pX7->m_bInGame = FALSE ;
				pRace->m_pX7->m_bInMainMenu = FALSE ;
				pRace->m_pX7->m_bPlayerJustLost = TRUE ;
				return 0 ;
			}
			else
			{
				Respawn( pTerrain );

			}
		}

	//
	// Update score (movement bonus);
	//
		float fScoreZone = pRace->m_fTerrainLenSq / 30 ;
		
		if( m_fDistanceToGround > 0 ) // !
		{
			if( m_fDistanceToGround < fScoreZone)
			{
				m_fTimeInScoreZone += ( 1 / fFPS );

				float fScoreDelta = 3 ;
				//fScoreDelta *= ( fabsf(m_fThrustMag) / m_fMaxThrustMag );
				fScoreDelta *= ( fabsf(m_fSpeed) / m_fMaxThrustMag );
				fScoreDelta *= ( 1.0f - (m_fDistanceToGround / fScoreZone) );
				fScoreDelta *= (m_fTimeInScoreZone) /* * (m_fTimeInScoreZone)*/ ;
				fScoreDelta *= ( 60 / fFPS );
				m_fScoreAddition += fScoreDelta ;
			}
			else
				m_fTimeInScoreZone = 0 ;
		}

	//
	// Apply score additions incrementally;
	//
		if( m_fScoreAddition > 0 )
		{
			/* messy, not good interface with scorezone additions; too difficult to tell
			   where scorezone starts/stops! */
			float f = ( m_fScoreAddition * 0.99f ); //  *FPS ;	// add this fraction of the of available points, every frame
			pRace->m_pX7->m_fScore +=	f ;
			m_fScoreAddition -= f ;
			if( m_fScoreAddition < 1 ) m_fScoreAddition = 0 ;

			/*m_fScore += m_fScoreAddition ;
			m_fScoreAddition = 0 ;*/

			m_bUpdatedScore = true ;
			if( m_pHUDScore )
			{
				sprintf( m_pHUDScore->m_szText, "%-12d", (int)pRace->m_pX7->m_fScore );
				m_pHUDScore->SetFrameMoveRequired( TRUE );
			}
		}
		else
		{
			m_bUpdatedScore = false ;
		}

	//
	// Make powerups decay over time;
	//
		if( m_fEngineLevel > 0.1f )
		{
			m_fEngineLevel *= 1.0f - ( 0.0005f * (60 / fFPS) );
			m_pHUDEngineLevel->m_fLevel = m_fEngineLevel ;
			m_pHUDEngineLevel->m_bFrameMoveRequired  = TRUE ;
		}
		if( m_fWeaponsLevel > 0.1f )
		{
			m_fWeaponsLevel *= 1.0f - ( 0.0005f * (60 / fFPS) );
			m_pHUDWeaponsLevel->m_fLevel = m_fWeaponsLevel ;
			m_pHUDWeaponsLevel->m_bFrameMoveRequired  = TRUE ;
		}

	//
	// Manage HUD;
	//
		if( bShowHUD )
		{
			FrameMoveSpaceScanner( pTerrain, pRace );

			if( m_bShieldLevelChanged )
			{
				m_pHUDShieldLevel->m_fLevel = m_fShieldLevel ;
				m_pHUDShieldLevel->m_bFrameMoveRequired = TRUE ;
				m_bShieldLevelChanged = FALSE ;
			}

			if( m_pHUDSpeed )
			{	// always framemove speed and thrust indicator;
				//sprintf( m_pHUDSpeed->m_szText, "%03d%%", (int)ceil((fabs(m_fThrustMag) / m_fMaxThrustMag) * 100.0f) );
				sprintf( m_pHUDSpeed->m_szText, "%04d m/s [%03d%%]", (int)( /*m_fSpeed * fFPS*/m_fSpeedMS ), (int)ceil((fabs(m_fThrustMag) / m_fMaxThrustMag) * 100) );

				m_pHUDSpeed->SetFrameMoveRequired( TRUE );
				m_pHUDSpeed->FrameMove( pDev, pInput, m_pOM->m_pVBVertices );
			}

			if( m_pHUDItems->m_bFrameMoveRequired )
				m_pHUDItems->FrameMove( pDev, pInput, pRace->m_pX7->m_pOverlayMan->m_pVBVertices );

			if( m_pHUDScore->m_bFrameMoveRequired )
				m_pHUDScore->FrameMove( pDev, pInput, pRace->m_pX7->m_pOverlayMan->m_pVBVertices );

			if( m_pHUDLives->m_bFrameMoveRequired )
				m_pHUDLives->FrameMove( pDev, pInput, m_pOM->m_pVBVertices );
		}

		return ret ;
	}

/*
 *
 * Builds dynamic VB of pointsprites; assumes same proj. mat. as screen overlays.
 *
 */
	void CUserFighter::FrameMoveSpaceScanner( CTerrain* pTerrain, CRace* pRace )
	{
		POINTSPRITEVERTEX* pv ;
		HRESULT		hr ;
		xVec2 v2DPosition ;
		xVec2 vPoints[ MAX_SCANNER_POINTS ];
		int			nPointTypes[ MAX_SCANNER_POINTS ];
		xMat		matRot ;
		int			nPoints = 0, i , j ;
		float		fScannerRadius = pTerrain->m_fLenSQ * 0.75f ;
		enum _ePointTypes
		{
			PT_CHERRY = 0, PT_SHIELD, PT_ENGINE, PT_WEAPON, PT_HEALTH, PT_THISFIGHTER,
			PT_ENEMY1
		} ;

		v2DPosition = xVec2( m_vPosition.x, m_vPosition.z );

		hr = m_pVB_Scanner->Lock( 0, 0, (BYTE**)&pv, D3DLOCK_DISCARD );
		assert( !FAILED(hr) );

	// add this fighter, always at the center!
		vPoints[ nPoints ] = xVec2( 0.0f, 0.0f );
		nPointTypes[ nPoints ] = PT_THISFIGHTER ;
		nPoints++ ;

	// add gameitems (CHERRIES only) ;
		for( i = 0; i < pTerrain->m_nPatchesSQ * pTerrain->m_nPatchesSQ ; i++ )
			for( j = 0 ; j < pTerrain->m_pPatches[ i ]->m_nItems ; j++ )
			{
				xVec2 v2D = xVec2( pTerrain->m_pPatches[ i ]->m_pItems[ j ].vPos.x, 
								   pTerrain->m_pPatches[ i ]->m_pItems[ j ].vPos.z );
				float fD = fabsf( D3DXVec2Length( &(v2D -v2DPosition)) );

				if( fD <  fScannerRadius )
				{
					switch( pTerrain->m_pPatches[ i ]->m_pItems[ j ].nType )
					{
						case IT_CHERRY: nPointTypes[ nPoints ] = PT_CHERRY ;
				 					    vPoints[ nPoints++ ] = v2DPosition - v2D ;
										break ;
						
						/*case IT_SHIELD: nPointTypes[ nPoints ] = PT_SHIELD ;
				 					    vPoints[ nPoints++ ] = v2DPosition - v2D ;
										break ;

						case IT_ENGINE: nPointTypes[ nPoints ] = PT_ENGINE ;
				 					    vPoints[ nPoints++ ] = v2DPosition - v2D ;
										break ;

						case IT_WEAPON: nPointTypes[ nPoints ] = PT_WEAPON ;
				 					    vPoints[ nPoints++ ] = v2DPosition - v2D ;
										break ;

						case IT_HEALTH: nPointTypes[ nPoints ] = PT_HEALTH ;
				 					    vPoints[ nPoints++ ] = v2DPosition - v2D ;
										break ;*/
						
					}
				}
			}

		BOT_FIGHTER* pBot = pRace->m_pBots ;
		while( pBot )
		{
			if( !pBot->pFighter->m_bBotIsDead )
			{
				xVec2 v2D = xVec2( pBot->pFighter->m_vPosition.x, 
								   pBot->pFighter->m_vPosition.z );
				float fD = fabsf( D3DXVec2Length( &(v2D -v2DPosition)) );
				if( fD <  fScannerRadius )
				{
					vPoints[ nPoints ] = v2DPosition - v2D ;
					nPointTypes[ nPoints ] = PT_ENEMY1 ;
					nPoints++ ;
				}
			}
			pBot = pBot->pNext ;
		}

	// move all vectors into scanner VB; map from fighter-relative x,z coords to origin centered unit x,y coords
	// (overlay proj. mat. is centered at 0,0,0 and extends 0.5 units up, left, down and right)
		for( i = 0 ; i < nPoints ; i++ )
		{
			pv->v.x = +vPoints[ i ].x / fScannerRadius ;
			pv->v.y = +vPoints[ i ].y / fScannerRadius ;

			pv->v.x /= 2.0f ;
			pv->v.y /= 2.0f ;
			pv->v.z = 0.0f ;

			switch( nPointTypes[ i ] )
			{
				case PT_CHERRY:
					pv->color = D3DXCOLOR( 1, 0, 1, 0.1f );
					break ;
				case PT_SHIELD:
					pv->color = D3DXCOLOR( 0, 0, 0, 0 );
					break ;
				case PT_ENGINE:
					pv->color = D3DXCOLOR( 0, 0, 0, 0 );
					break ;
				case PT_WEAPON:
					pv->color = D3DXCOLOR( 0, 0, 0, 0 );
					break ;
				case PT_THISFIGHTER:
					pv->color = D3DXCOLOR( 1, 1, 1, 1 );
					break ;
				case PT_ENEMY1:
					pv->color = D3DXCOLOR( 0, 0, 0, 0 );
					break ;
			}
			pv++ ;
		}
	
		m_pVB_Scanner->Unlock() ;
		m_nScannerPoints = nPoints ;
	}

		/* fun for all the family!
    D3DBLEND_ZERO               = 1,
    D3DBLEND_ONE                = 2,
    D3DBLEND_SRCCOLOR           = 3,
    D3DBLEND_INVSRCCOLOR        = 4,
    D3DBLEND_SRCALPHA           = 5,
    D3DBLEND_INVSRCALPHA        = 6,
    D3DBLEND_DESTALPHA          = 7,
    D3DBLEND_INVDESTALPHA       = 8,
    D3DBLEND_DESTCOLOR          = 9,
    D3DBLEND_INVDESTCOLOR       = 10,
    D3DBLEND_SRCALPHASAT        = 11,
    D3DBLEND_BOTHSRCALPHA       = 12,
    D3DBLEND_BOTHINVSRCALPHA    = 13,
		*/

	HRESULT CUserFighter::RenderHUD( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int ctCamMode, xMatUA* pmatCurWorld, CCamera *pCamera, CScreenOverlayManager* pOM, CRace* pRace, float fFPS )
	{
		HRESULT hr ;
		xMat matW, matT, matS, matR, matTS, matST ;
		xVec3 vScreenspaceTarget ;
		xVec3 vScreenspaceCrosshair ;
		xVec2 vTrans ;
		float fI, fS ;
		
	// set proj. mat, disable z-writes and always pass zbuffer test;
		pOM->SetProjMat( pDev );
		pDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	// enable alpha-blending;
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
		pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	// go, go, gadget blender;
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );

	//
	// Scalebars:
	//
		pOM->Render(pDev, OT_HUD_SCALEBAR, ORT_FAST) ;

	//
	// Lives:
	//
		pDev->SetRenderState( D3DRS_SRCBLEND,  /*pRace->SB*/D3DBLEND_INVSRCCOLOR );
		pDev->SetRenderState( D3DRS_DESTBLEND, /*pRace->DB*/D3DBLEND_SRCALPHA );
		pOM->Render(pDev, OT_HUD_STRING1, ORT_FAST) ;

	//
	// Time:
	//
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
		pOM->Render(pDev, OT_HUD_STRING2, ORT_FAST) ;
	
	//
	// Score:
	//
		pOM->Render(pDev, OT_HUD_STRING4, ORT_FAST) ;						// first pass with gadget blender
		if( m_bUpdatedScore )
		{
			pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );	// second pass for highlight
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			pOM->Render(pDev, OT_HUD_STRING4, ORT_FAST) ;
		}

	//
	// Speed & Item Counter
	//
		pDev->SetRenderState( D3DRS_SRCBLEND,  /*pRace->SB*/D3DBLEND_ONE );
		pDev->SetRenderState( D3DRS_DESTBLEND, /*pRace->DB*/D3DBLEND_ONE );
		pOM->Render(pDev, OT_HUD_STRING3, ORT_FAST) ;

	//
	// SpaceScanner:
	//
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );

		D3DXMatrixScaling( &matS, m_fScannerSize, m_fScannerSize, m_fScannerSize );
		D3DXMatrixRotationZ( &matR, +m_fScannerRot );
		D3DXMatrixTranslation( &matT,
							   m_vScannerPos.x * pRace->m_pX7->m_fAspect,
							   m_vScannerPos.y,
							   0 );
		D3DXMatrixMultiply( &matTS, &matS, &matT );
		D3DXMatrixMultiply( &matW, &matS, &matR );
		D3DXMatrixMultiply( &matW, &matW, &matT );

		m_pScannerBackG->m_matWorld = matTS ;					// apply scaling & trans. only to overlay's world matrix
		pOM->Render( pDev, OT_HUD_SCANNERBACKG, ORT_FAST );

		pDev->SetTransform( D3DTS_WORLD, &matW );				// apply scaling, trans & rot. matrices to pointspirtes
		if( m_nScannerPoints > 0 )
		{
			pDev->SetRenderState( D3DRS_LIGHTING,  FALSE );
			pDev->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
			pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pDev->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
			pDev->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
			pDev->SetRenderState( D3DRS_POINTSIZE, FtoDW(0.1f * m_fScannerSize) );
			pDev->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
			pDev->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
			pDev->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
			pDev->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );
			pDev->SetStreamSource( 0, m_pVB_Scanner, sizeof( POINTSPRITEVERTEX ) );
			pDev->SetVertexShader( MYFVF_POINTSPRITEVERTEX );
			pDev->SetTexture( 0, pTex->m_pTex[ PARTICLE_TEX_START+0 ] );

			hr = pDev->DrawPrimitive( D3DPT_POINTLIST, 0, m_nScannerPoints );
			assert( !FAILED(hr) );

			pDev->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
			pDev->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
		}

	//
	// Target icon:
	//
		//pDev->SetRenderState( D3DRS_SRCBLEND,  pRace->SB );
		//pDev->SetRenderState( D3DRS_DESTBLEND, pRace->DB );
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
		BOT_FIGHTER* pBot = pRace->m_pBots ;
		while( pBot )
		{
			if( pBot->pFighter->m_bVisible && !pBot->pFighter->m_bBotIsDead && 
				pBot->pFighter->m_fsState == FS_NORMAL )
			{
			// project target vector to screen space;
				D3DXVec3Project( &vScreenspaceTarget,
								 &pBot->pFighter->m_vPosition,
								 &pRace->m_pX7->m_vpViewport,
								 &pRace->m_pX7->m_matProj,
								 &pRace->m_pCamera->m_matView,
								 &g_matIdent );
				vScreenspaceTarget.x /= (float)pRace->m_pX7->m_vpViewport.Width ;
				vScreenspaceTarget.y /= (float)pRace->m_pX7->m_vpViewport.Height ;
			// build screen overlay scale+trans matrix;
				vTrans.x = vScreenspaceTarget.x - 0.5f ;
				vTrans.y = (1 - vScreenspaceTarget.y) - 0.5f  ;
				D3DXMatrixTranslation( &matT,
									   vTrans.x * pRace->m_pX7->m_fAspect,
									   vTrans.y,
									   0 );
				
				float fD = fabsf(D3DXVec3Length( &(pRace->m_pCamera->m_vEyePt - pBot->pFighter->m_vPosition) ));
				float fMaxD = pRace->m_fTerrainLenSq * 0.01f ;
				if( fD > fMaxD )
					fI = 0 ;
				else if( fD < 0 )
					fI = 1 ;
				else
					fI = 1 - (fD / fMaxD );
				fS = m_fTargetIconMinSize + (m_fTargetIconMaxSize - m_fTargetIconMinSize) * fI ;
				D3DXMatrixScaling( &matS, fS, fS, fS );

				D3DXMatrixMultiply( &matST, &matS, &matT );

			// tex. coord. animation;
				/*pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
				xMat matTex ;
				static float X = 0 ;
				D3DXMatrixRotationZ( &matTex, X );
				X+=(0.0015f) * (60.0f/fFPS) ;
				pDev->SetTransform( D3DTS_TEXTURE0, &matTex );*/

			// render target icon four times, rotating in 90 deg. increments;
				m_pTargetIcon->m_matWorld = g_matIdent ;
				D3DXMatrixMultiply( &m_pTargetIcon->m_matWorld, &m_pTargetIcon->m_matWorld, &matST );
				pOM->Render( pDev, OT_HUD_TARGETICON, ORT_FAST );

				D3DXMatrixRotationZ( &m_pTargetIcon->m_matWorld, g_PI_DIV_2 );
				D3DXMatrixMultiply( &m_pTargetIcon->m_matWorld, &m_pTargetIcon->m_matWorld, &matST );
				pOM->Render( pDev, OT_HUD_TARGETICON, ORT_FAST );

				D3DXMatrixRotationZ( &m_pTargetIcon->m_matWorld, g_PI );
				D3DXMatrixMultiply( &m_pTargetIcon->m_matWorld, &m_pTargetIcon->m_matWorld, &matST );
				pOM->Render( pDev, OT_HUD_TARGETICON, ORT_FAST );

				D3DXMatrixRotationZ( &m_pTargetIcon->m_matWorld, g_PI + g_PI_DIV_2 );
				D3DXMatrixMultiply( &m_pTargetIcon->m_matWorld, &m_pTargetIcon->m_matWorld, &matST );
				pOM->Render( pDev, OT_HUD_TARGETICON, ORT_FAST );
			}
			pBot = pBot->pNext ;
		}
		pDev->SetTransform( D3DTS_TEXTURE0, &g_matIdent );
		pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

	//
	// Crosshair:
	//
	// project target vector to screen space;
		D3DXVec3Project( &vScreenspaceCrosshair,
						 &( m_vPosition + m_vOrientation * m_pFRO->m_pRenderData->m_fBSphereRadius * 200 ),
						 &pRace->m_pX7->m_vpViewport,
						 &pRace->m_pX7->m_matProj,
						 &pRace->m_pCamera->m_matView,
						 &g_matIdent );
		vScreenspaceCrosshair.x /= (float)pRace->m_pX7->m_vpViewport.Width ;
		vScreenspaceCrosshair.y /= (float)pRace->m_pX7->m_vpViewport.Height ;
	// build screen overlay scale+trans matrix;
		vTrans.x = vScreenspaceCrosshair.x - 0.5f ;
		vTrans.y = (1 - vScreenspaceCrosshair.y) - 0.5f  ;
		D3DXMatrixTranslation( &matT,
							   vTrans.x * pRace->m_pX7->m_fAspect,
							   vTrans.y, 0 );
		D3DXMatrixScaling( &matS, 0.25f, 0.25f, 0.25f );
		D3DXMatrixMultiply( &matST, &matS, &matT );
		m_pCrosshair->m_matWorld = matST ;
		pOM->Render( pDev, OT_HUD_CROSSHAIR, ORT_FAST );

	//
	// Show pain overlay when in cockpit mode and when shield effect is active:
	//
		if( m_bShowingShieldEffect )
		{
			D3DMATERIAL8 mtr ;
			D3DUtil_InitMaterial( mtr, 1, 0, 0, 0.75f - (m_fShieldEffectAge / m_fShieldEffectLifetime)*0.75f );
			pDev->SetMaterial( &mtr );

			pDev->SetRenderState( D3DRS_LIGHTING, TRUE );
			pDev->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
			pDev->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
			pDev->SetRenderState( D3DRS_AMBIENT, 0x00ffffff );

			pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			pOM->Render( pDev, OT_HUD_PAIN, ORT_FAST );
			
			pDev->SetRenderState( D3DRS_AMBIENT, pRace->m_dwAmbientLight );
			pDev->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
			pDev->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
			pDev->SetRenderState( D3DRS_LIGHTING, FALSE );
		}

	//
	// Misc Text:
	//

		// DBG:
		/*m_bShowingHUDText = true ;
		strcpy( m_szHUDText, "ENGINE BOOST" );
		m_fHUDTextAge = 0 ;
		m_fHUDTextLifespan = 0.5f ;
		m_fHUDTextX = -1 ;
		m_fHUDTextY = -0.25f ;
		m_fHUDTextXScale = 0.16f ;
		m_fHUDTextYScale = 0.25f ;
		m_fHUDTextRed = 0 ;
		m_fHUDTextGreen = 1 ;
		m_fHUDTextBlue = 0 ;*/


		if( m_bShowingHUDText )
		{
			float i = m_fHUDTextAge / m_fHUDTextLifespan ;
			float a = ( 1 - i ) * m_fHUDTextAlpha ;

			pRace->m_pX7->m_pTitleFont->SetRenderStateDM() ;

			pRace->m_pX7->m_pTitleFont->DrawTextScaledDM( m_fHUDTextX,		m_fHUDTextY,		0,
														  m_fHUDTextXScale, m_fHUDTextYScale,
														  D3DXCOLOR( m_fHUDTextRed,
																	 m_fHUDTextGreen,
																	 m_fHUDTextBlue,
																	 a ),
														  m_szHUDText,
														  D3DFONT_FILTERED,
														  &pRace->m_pX7->m_vpViewport );

			/*pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR  );

			pRace->m_pX7->m_pTitleFont->DrawTextScaledDM( m_fHUDTextX,		m_fHUDTextY,		0,
														  m_fHUDTextXScale, m_fHUDTextYScale,
														  D3DXCOLOR(m_fHUDTextRed,
																	m_fHUDTextGreen,
																	m_fHUDTextBlue, a),
														  m_szHUDText,
														  D3DFONT_FILTERED,
														  &pRace->m_pX7->m_vpViewport );*/

			pRace->m_pX7->m_pTitleFont->ResetRenderStateDM() ;

			m_fHUDTextAge += ( 1 / fFPS );
			if( m_fHUDTextAge >= m_fHUDTextLifespan )
			{
				m_bShowingHUDText = false ;
				m_fHUDTextAge = m_fHUDTextLifespan = 0 ;
			}
		}

	// all done.
		pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		pDev->SetTransform( D3DTS_WORLD, &g_matIdent );

		return S_OK ;
	}
