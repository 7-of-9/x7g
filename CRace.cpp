#pragma warning( disable : 4183 )	// 'missing return type'

#include "CRace.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CRace::CRace( Cx7 *pX7, LPDIRECT3DDEVICE8 pDev,
				  int nLevel,
				  float fMaxTime,
				  int nOceanTex,
				  D3DXCOLOR* pOceanColour,
				  bool bSolidOcean,				// <-- TODO
				  bool bNight,
				  DWORD dwAmbient,
				  float fMountainFactor,
				  int nTex1,
				  int nTex2,
				  unsigned nNoisePatches1,
				  unsigned nNoisePatches2 )
	{
		m_fSkill = (float)nLevel / (float)( MAX_LEV / 4 );
		srand( (unsigned)time( NULL ) );

		ods( "*** Level %d ***\n\tNight=%s\n\tdwAmbient=%08x\n\tfMountainFactor=%0.2f\n\tNoisePatches=%d\n\tnNoisePatches2=%d\n\n", nLevel, bNight ? "yes" : "no", dwAmbient, fMountainFactor, nNoisePatches1, nNoisePatches2 );

		m_pX7 = pX7 ;
		m_fTerrainLenSq = 1000.0f ;
		m_fFighterSize = m_fTerrainLenSq / 2000 / 2 ;

		m_fTerrainLenSqM = 10000 ;								// 10 KM
		m_fWorldSpaceToM = m_fTerrainLenSqM / m_fTerrainLenSq ;	// multiply by this to convert from world units
																// to meters.
		m_fGameTimeRemaining = fMaxTime ;
		m_fGameMaxTime = fMaxTime ;
		
		m_bDoneOneTimeSetup = FALSE ;
		m_bTimeRanOut = FALSE ;
		m_nLevel = nLevel ;

	//
	// Game State:
	//
		m_State = GCS_LEVEL_PREINTRO ;
		m_fIntroTimeS = 4 ;

	//
	// Global Lighting & Bits:
	//
		m_dwAmbientLight = dwAmbient ;
		m_bNight = bNight ;
		m_OceanColour = *pOceanColour ;

	//
	// Camera state:
	//
		m_ctCamMode = CT_SHOWOFF
					  /*CT_FREE*/
					  /*CT_BEHIND_FIXED1*/
					  /*CT_BEHIND_FIXED2*/
					  /*CT_BEHIND_FIXED3*/
					  /*CT_BEHIND_VARIABLE1*/
					  /*CT_BEHIND_VARIABLE2*/
					  /*CT_COCKPIT_ROLL*/
					  /*CT_COCKPIT_FIXED*/
					  ;

	//
	// Debug toggles:
	//
		m_bShowTerrain = FALSE ;
		m_bShowSky = FALSE ;
		m_bShowFighter = FALSE ;
		m_bShowOcean = FALSE ;

	//
	// Main Directional Light:
	//
		ZeroMemory(&m_litMain, sizeof(D3DLIGHT8)) ;
		m_litMain.Type         = D3DLIGHT_DIRECTIONAL;
		m_litMain.Diffuse.r    = m_litMain.Diffuse.g  = m_litMain.Diffuse.b  = 0.1f ;
		m_litMain.Specular.r   = m_litMain.Specular.g = m_litMain.Specular.b = 1.0f ;
		m_litMain.Ambient.r    = m_litMain.Ambient.g  = m_litMain.Ambient.b  = 0.1f ;
		m_litMain.Position     = xVec3( 0.0f, 0.0f, 0.0f );	// not relevant to directional lights - ignored
		D3DXVec3Normalize( (xVec3*)&m_litMain.Direction, &xVec3( +1.0f, -0.5f, +0.25f ) );
		m_litMain.Attenuation0 = m_litMain.Attenuation1 = m_litMain.Attenuation2 = 0.0f;
		m_litMain.Range        = sqrtf( FLT_MAX );

	//
	// Object Instances:
	//
		m_pUnderWaterQuad = NULL ;

		m_pCamera = NULL ;
		m_pCamera = new CCamera ;
		assert( m_pCamera );
		m_pCamera->SetViewMatrix(pDev, FALSE) ;

		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pTerrain") ;
		m_pTerrain = NULL ;

		//int nOfItem = m_pX7->m_pCmdLine->GetInt( "Terrain2", 32 ) / 4 ;
		float fRedColorFactor = rnd() ;
		float fGreenColorFactor = rnd() ;
		float fBlueColorFactor = rnd() ;
		m_pTerrain = new CTerrain(
				m_fTerrainLenSq,
				pX7,
				m_pX7->m_pCmdLine->GetInt( "Terrain1", 8  ),
				m_pX7->m_pCmdLine->GetInt( "Terrain2", 32 ),
				m_nLevel + 1,		/*cherries*/
				1,					/*shield: THESE DON'T DO ANYTHING YET WHEN PICKED*/
				m_nLevel + 2,		/*engine*/
				m_nLevel + 2,		/*weapons*/
				(m_nLevel / 10 + 1),/*health*/
				fMountainFactor,
				nTex1, nTex2,
				nNoisePatches1, nNoisePatches2,
				fRedColorFactor < 0.33f ? 1 : fRedColorFactor  < 0.66f ? 2 : 3,
				fGreenColorFactor < 0.33f ? 1 : fGreenColorFactor  < 0.66f ? 2 : 3,
				fBlueColorFactor < 0.33f ? 1 : fBlueColorFactor  < 0.66f ? 2 : 3
								 );
		m_bShowTerrain = TRUE ;
		assert( m_pTerrain );

		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pSky") ;
		m_pSky = NULL ;
		m_pSky = new CSky( m_fTerrainLenSq * 10.0f, m_pX7->m_pCmdLine->GetInt( "BlackSky", 0) /*|| m_bNight*/ );
		m_bShowSky = TRUE ;
		assert( m_pSky );

		/*strcpy(m_pX7->m_szLoadingGameInfo1, "m_pSkyDome") ;
		m_pSkyDome = NULL ;
		m_pSkyDome = new CSkyDome( m_fTerrainLenSq * 10,
								   (m_pX7->m_pCmdLine->GetInt( "BlackSky", 0) != 0),
								   20 );
		m_bShowSky = TRUE ;
		assert( m_pSkyDome );*/

		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pOcean") ;
		m_pOcean = NULL ;
		m_pOcean = new COcean( m_fTerrainLenSq * 20.0f, nOceanTex, pOceanColour );
		m_bShowOcean = TRUE ;
		assert( m_pOcean );

		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pFighter") ;
		m_pFighter = NULL ;
		e_FighterTypes ft ;
		int frm ;
		switch( m_nLevel % 3 )
		{
			case 2: ft = FT_FIGHTER ; frm =  FRT_GLASS ; break ;
			case 0: ft = FT_STORM ;  frm =  FRT_GLASS ; break ;
			case 1: ft = FT_BIGSHIP1 ;  frm =  FRT_CHROME ; break ;
		}
		m_pFighter = new CUserFighter( m_fSkill,
									   m_nLevel,
									   m_pX7->m_nLives,
									   ft, 
									   m_fFighterSize,
									   m_pTerrain,
									   m_pX7->m_pOverlayMan,
									   NULL );
		m_bShowFighter = TRUE ;
		assert( m_pFighter );
		m_pFighter->m_rtFighterMode = frm ;


		m_bShowHUD = TRUE ;

	//
	// BotFighters:
	//
		m_pBots = NULL ;
		m_nBots = 0 ;
		/*
	FT_BIGSHIP1 = 0,
	FT_FIGHTER,
	FT_STORM,
	FT_FIGHTER1,
	FT_FIGHTER2,
	FT_LYRAX,
		*/
		int nBots = ( m_nLevel + 3 ) / 2 + 1 ;
		if( m_pX7->m_pCmdLine->GetInt( "Bots", -1 ) != -1 )
			nBots = m_pX7->m_pCmdLine->GetInt( "Bots", 0 );

		for( int i=0 ; i < nBots ; i++ )
		{
			AddBot( FT_FIGHTER1/*i+1*/,
					i==0 ? NULL : m_pBots[ 0 ].pFighter->m_pFRO->m_pRenderData/*NULL*/ );
		}

	//
	// Game State:
	//
		float fCharSize = 0.05f ;

		char sz[ 256 ];
		sprintf( sz, "LEVEL %d", m_nLevel );
		m_pHUDLevel = NULL ;
		m_pHUDLevel = new CHUDText( sz, HUD_TEX_START + 0, 1.0f - fCharSize * 0.6f * (strlen(sz) + 1), 0, fCharSize, fCharSize, fCharSize * 0.4f, OT_HUD_STRING1 );

		m_fGameElapsedTime = 0.0f ;
		m_pHUDElapsedTime = NULL ;
		m_pHUDElapsedTime = new CHUDText( "m:ss:mm", HUD_TEX_START + 0, 1.0f - 0.6f * fCharSize * 8, fCharSize, fCharSize, fCharSize, fCharSize * 0.4f, OT_HUD_STRING2 );
	}

	CRace::~CRace()
	{
		SAFE_DELETE( m_pTerrain )
		SAFE_DELETE( m_pSky )
		/*SAFE_DELETE( m_pSkyDome )*/
		SAFE_DELETE( m_pOcean )
		SAFE_DELETE( m_pCamera )
		SAFE_DELETE( m_pFighter )
		BOT_FIGHTER* pBot = m_pBots ;
		while( pBot )
		{
			SAFE_DELETE( pBot->pFighter )
			pBot = pBot->pNext ;
		}
		SAFE_DELETE( m_pUnderWaterQuad )
		SAFE_DELETE( m_pHUDLevel )
		SAFE_DELETE( m_pHUDElapsedTime )
	}

	void CRace::AddBot( int ftType, CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData )
	{
		static DWORD dwID = 0 ;
		BOT_FIGHTER* pLastBot = m_pBots ;
		BOT_FIGHTER* pNewBot = NULL ;

		if( !m_pBots )
		{
			m_pBots = pNewBot = new BOT_FIGHTER ;
			pLastBot = NULL ;
		}
		else
		{
			while( pLastBot->pNext )
				pLastBot = pLastBot->pNext ;
			pNewBot = new BOT_FIGHTER ;
		}
		assert( pNewBot );
		
		pNewBot->dwID = dwID++ ;
		pNewBot->pPrev = pLastBot ;
		pNewBot->pNext = NULL ;
		pNewBot->pFighter = NULL ;
		pNewBot->pFighter = new CBotFighter( m_fSkill,
											 m_nLevel,
											 ftType,
											 /*m_fTerrainLenSq*/m_fFighterSize,
											 pSharedRenderData );
		assert( pNewBot->pFighter );
		pNewBot->pFighter->m_dwID = pNewBot->dwID ;		// internal dbg use only

		if( pLastBot )
			pLastBot->pNext = pNewBot ;

		m_nBots++ ;
	}

	CRace::EnableHUDs( BOOL bActive )
	{
		if( m_pHUDLevel )
			m_pHUDLevel->Enable( bActive );
		if( m_pHUDElapsedTime )
			m_pHUDElapsedTime->Enable( bActive );

		m_pFighter->EnableHUD( bActive );
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT CRace::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, float fAspect, CScreenOverlayManager* pOM, CTextures* pTex)
	{
#ifdef FULL_DBG_TRACE 
	ods( "CRace::RestoreDeviceObjects\n" );
#endif

		HRESULT hr ;

	// shared particle system VB;
		hr = pDev->CreateVertexBuffer( 3000 *
			sizeof(POINTSPRITEVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
			MYFVF_POINTSPRITEVERTEX, D3DPOOL_DEFAULT, &m_pSharedParticleSystemVB ) ;
		assert( !FAILED(hr) );

	// overlays;
		pOM->m_bVBLocked = TRUE ;

		D3DXCOLOR c = D3DXCOLOR( m_OceanColour.r, m_OceanColour.g, m_OceanColour.b, 0.3f );
		m_pUnderWaterQuad = new CScreenOverlay( NULL, 1.0f, -0.5f, -0.5f, 0.0f, 2.0f, 2.0f, &c, &c, &c, &c, OT_FULLSCREENQUAD );
		pOM->RegisterNewOverlay( m_pUnderWaterQuad );
		m_pUnderWaterQuad->m_bActive = FALSE ;
		m_pUnderWaterQuad->m_pSharedVB = pOM->m_pVB ;
		m_pUnderWaterQuad->RestoreDeviceObjects( pDev, fAspect );

		pOM->m_bVBLocked = FALSE ;

		if( m_pHUDLevel )
		{
			m_pHUDLevel->RestoreDeviceObjects( pDev, fAspect, pOM, pTex );
			m_pHUDLevel->Enable( TRUE );
		}
		if( m_pHUDElapsedTime )
		{
			m_pHUDElapsedTime->RestoreDeviceObjects( pDev, fAspect, pOM, pTex );
			m_pHUDElapsedTime->Enable( TRUE );
		}

		if (m_pTerrain)
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pTerrain") ;
			if (FAILED(m_pTerrain->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;
		}
		
		if (m_pSky)
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pSky") ;
			if (FAILED(m_pSky->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;
		}

		/*if (m_pSkyDome)
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pSkyDome") ;
			if (FAILED(m_pSkyDome->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;
		}*/

		if (m_pOcean)
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pOcean") ;
			if (FAILED(m_pOcean->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;
		}

	//
	// Fighters:
	//
		if( m_pFighter && m_pTerrain )
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pFighter") ;
			if (FAILED(m_pFighter->RestoreDeviceObjects(pD3D, pDev, m_pX7->m_pOverlayMan, m_pX7->m_pTextures, fAspect))) return E_FAIL ;

		// scale the fighter to a good size;
			m_pFighter->m_pFRO->SetLargestSide( m_fFighterSize );
			
			/*ods( "Fighter bbox center: %0.1f, %0.1f, %0.1f\n", m_pFighter->m_pFRO->m_pBBox->m_vCenter.x, m_pFighter->m_pFRO->m_pBBox->m_vCenter.y, m_pFighter->m_pFRO->m_pBBox->m_vCenter.z );
			ods( "Fighter bsrad: %0.1f\n", m_pFighter->m_pFRO->m_pRenderData->m_fBSphereRadius );
			ods( "Fighter orientation: %0.1f, %0.1f, %0.1f\n", m_pFighter->m_vOrientation.x, m_pFighter->m_vOrientation.y, m_pFighter->m_vOrientation.z );
			ods( "Fighter largest side: %0.1f\n", m_pFighter->m_pFRO->m_fLargestSide );				   
			ods( "Fighter scaled largest side: %0.1f\n", m_pFighter->m_pFRO->m_fScaledLargestSide );*/

		// move the fighter to a random spawn location;
			if( !m_bDoneOneTimeSetup )
			{
				int n = (int)((float)(m_pTerrain->m_nSpawnPos-1) * rnd()) ;

				/*m_pFighter->SetPositionFromValues( m_pTerrain->m_vSpawnPos[ n ].x,
												   m_pTerrain->m_vSpawnPos[ n ].y + m_fFighterSize * 4,
												   m_pTerrain->m_vSpawnPos[ n ].z );*/

				// HACK:
				// position at exact center, every time
				BOOL b ;
				float y ;
				m_pTerrain->LERPdY( 0, 0, &y, &b, NULL );
				m_pFighter->SetPositionFromValues( 0, y + m_fFighterSize * 4, 0 );
				
				ods( "***Fighter position: %0.1f, %0.1f, %0.1f [fighter size=%0.1f]\n",
					 m_pFighter->m_vPosition.x,
					 m_pFighter->m_vPosition.y,
					 m_pFighter->m_vPosition.z,
					 m_fFighterSize );

				//m_pFighter->SetPositionFromValues( 0, m_pTerrain->m_fLenSQ / 2, 0 );
			}
		}

		BOT_FIGHTER* pBot = m_pBots ;
		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pBots") ;
		while( pBot )
		{
			//
			// BIGBOTS
			//
			if (FAILED(pBot->pFighter->RestoreDeviceObjects(pD3D, pDev, m_pX7->m_pOverlayMan, m_pX7->m_pTextures, fAspect))) return E_FAIL ;
			pBot->pFighter->m_pFRO->SetLargestSide( m_fFighterSize * 3 );
		
		/*ods( "Bot bbox center: %0.1f, %0.1f, %0.1f\n", pBot->pFighter->m_pFRO->m_pBBox->m_vCenter.x, pBot->pFighter->m_pFRO->m_pBBox->m_vCenter.y, pBot->pFighter->m_pFRO->m_pBBox->m_vCenter.z );
		ods( "Bot bsrad: %0.1f\n", pBot->pFighter->m_pFRO->m_pRenderData->m_fBSphereRadius );
		ods( "Bot orientation: %0.1f, %0.1f, %0.1f\n", pBot->pFighter->m_vOrientation.x, pBot->pFighter->m_vOrientation.y, pBot->pFighter->m_vOrientation.z );
		ods( "Bot largest side: %0.1f\n", pBot->pFighter->m_pFRO->m_fLargestSide );				   
		ods( "Bot scaled largest side: %0.1f\n", pBot->pFighter->m_pFRO->m_fScaledLargestSide );*/

			if( !m_bDoneOneTimeSetup )
			{
				float f = 3 ;
				float fX = m_pFighter->m_vPosition.x + m_fFighterSize * 2 * f * (pBot->dwID+1) * (pBot->dwID%2==0?+1:-1) ;
				float fZ = m_pFighter->m_vPosition.z - m_fFighterSize * 4 * f * (pBot->dwID+1) ;
				float fY ;
				BOOL b ;
				m_pTerrain->LERPdY( fX, fZ, &fY, &b, NULL );
				fY += m_fFighterSize * 4 ;

				pBot->pFighter->SetPositionFromValues( fX, fY, fZ );

				ods( "***Bot position: %0.1f, %0.1f, %0.1f [fighter size=%0.1f]\n",
					 pBot->pFighter->m_vPosition.x,
					 pBot->pFighter->m_vPosition.y,
					 pBot->pFighter->m_vPosition.z,
					 m_fFighterSize );

			}
			pBot = pBot->pNext ;
		}

		m_bDoneOneTimeSetup = TRUE ;
		return S_OK ;
	}
	HRESULT	CRace::InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		SAFE_RELEASE( m_pSharedParticleSystemVB )

		if (m_pTerrain)
			m_pTerrain->InvalidateDeviceObjects( pDev );

		if (m_pSky)
			m_pSky->InvalidateDeviceObjects() ;

		/*if (m_pSkyDome)
			m_pSkyDome->InvalidateDeviceObjects() ;*/

		if (m_pOcean)
			m_pOcean->InvalidateDeviceObjects() ;

		if (m_pFighter)
			m_pFighter->InvalidateDeviceObjects( pDev );

		BOT_FIGHTER* pBot = m_pBots ;
		while( pBot )
		{
			pBot->pFighter->InvalidateDeviceObjects( pDev );
			pBot = pBot->pNext ;
		}

		if( m_pHUDLevel )
			m_pHUDLevel->InvalidateDeviceObjects() ;

		if( m_pHUDElapsedTime )
			m_pHUDElapsedTime->InvalidateDeviceObjects() ;

		SAFE_DELETE( m_pUnderWaterQuad )

		return S_OK ;
	}

	HRESULT CRace::Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, float fAspect, float fFPS, int nMaxActiveLights )
	{
		xMat matWorld, m0, m1, m2, m3 ;
		BOT_FIGHTER* pBot ;

	//
	// Set vertex shader constants for main directional light and ambient:
	//
		/*pDev->SetVertexShaderConstant(  7, &m_litMain.Direction, 1);	// c7; light direction
		pDev->SetVertexShaderConstant(  8, &m_litMain.Diffuse, 1);		// c8; light colour
		DWORD dw = m_dwAmbientLight & 0xff ;
		float f = (float)dw / (float)0xff ;
		pDev->SetVertexShaderConstant( 10, &xVec4( f,f,f,f ), 1 );// c10; ambient*/

	//
	// Both set in FrameMove, but may be reset by InGameMenu
	//
		m_pCamera->SetViewMatrix( pDev, FALSE );
		pDev->SetRenderState( D3DRS_ZENABLE, TRUE );
	
	//
	// Modify view matrix for cockpit roll:
	//
		matWorld = g_matIdent ;
		if( m_ctCamMode == CT_COCKPIT_ROLL && m_pFighter && m_pCamera )
		{
			D3DXMatrixTranslation( &m0, -m_pCamera->m_vEyePt.x, -m_pCamera->m_vEyePt.y, -m_pCamera->m_vEyePt.z ); 
			D3DXMatrixRotationAxis( &m1, &m_pFighter->m_vOrientation, m_pFighter->m_fRoll );
			D3DXMatrixTranslation( &m2, +m_pCamera->m_vEyePt.x, +m_pCamera->m_vEyePt.y, +m_pCamera->m_vEyePt.z ); 
			D3DXMatrixMultiply( &m3, &m0, &m1 );
			D3DXMatrixMultiply( &m3, &m3, &m2 );
			D3DXMatrixMultiply( &m_pCamera->m_matView, &m3, &m_pCamera->m_matView );
			pDev->SetTransform( D3DTS_VIEW, &m_pCamera->m_matView );
		}

	//
	// proj. mat. hell.
	//
		D3DXMatrixPerspectiveFovLH( &m_pX7->m_matProj, D3DX_PI/4, fAspect, m_fFighterSize / 2, m_fTerrainLenSq * 20 );
		pDev->SetTransform( D3DTS_PROJECTION, &m_pX7->m_matProj );

	//
	// Ambient lighting + Main directional light, default them to OFF
	//
		pDev->SetRenderState(D3DRS_AMBIENT, m_dwAmbientLight) ;
		pDev->SetLight(0, &m_litMain) ;
		pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
		pDev->LightEnable(0, FALSE) ;

	// def. fog state;
		pDev->SetRenderState( D3DRS_FOGENABLE, FALSE );
		
	//
	// Sky
	//
		if( m_pSky && m_bShowSky && m_pFighter )
		{
			pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			m_pSky->Render( pDev, pTex, pulPolyCount, &matWorld, &m_pCamera->m_vEyePt );
		}
		/*if( m_pSkyDome && m_bShowSky && m_pFighter )
		{
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
			m_pSkyDome->Render( pDev, pTex, pulPolyCount, &matWorld, &m_pFighter->m_vPosition, fFPS );
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW) ;
		}*/
	
	//
    // Underwater fullscreen quad & blue fog;
	//
		m_pUnderWaterQuad->m_bActive = FALSE ;
		if( m_bShowOcean )
			if (m_pCamera->m_vEyePt.y < 0 )
			{
				m_pUnderWaterQuad->m_bActive = TRUE ;
				
				float fStart = 1.0f;
				float fEnd = m_fTerrainLenSq / 6 ;
				DWORD dwColor = D3DXCOLOR( m_OceanColour.r, m_OceanColour.g, m_OceanColour.b, 1 );
				pDev->SetRenderState( D3DRS_FOGENABLE, TRUE );
				pDev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
				pDev->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&fStart) );
				pDev->SetRenderState( D3DRS_FOGEND,   *(DWORD *)(&fEnd) );
				pDev->SetRenderState( D3DRS_FOGCOLOR, dwColor );
			}

	//
	// Night black fog;
	//
		if( m_bNight )
		{
			float fStart = 1.0f;
			float fEnd = m_fTerrainLenSq ;
			DWORD dwColor = D3DXCOLOR( 0, 0, 0, 0 );
			pDev->SetRenderState( D3DRS_FOGENABLE, TRUE );
			pDev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
			pDev->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&fStart) );
			pDev->SetRenderState( D3DRS_FOGEND,   *(DWORD *)(&fEnd) );
			pDev->SetRenderState( D3DRS_FOGCOLOR, dwColor );
		}

	//
	// Distance Sort Terrain gameitems and enable some gameitem lights:
	//
	// NOTE: this call is REQUIRED in order for CTerrain::Render to perform as expected
	// NOTE: use lights carefully; they have a large performance penalty
	//
		m_pTerrain->DistanceSortItemsAndSetLights( pDev, 3, 1, m_pCamera );
		

	//
	// Terrain:
	//
		pDev->SetRenderState(D3DRS_LIGHTING, TRUE) ;
		pDev->LightEnable(0, TRUE) ;

		if( m_pTerrain && m_bShowTerrain )
		{
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW) ;
			m_pTerrain->Render( pDev, pTex, pulPolyCount, m_pCamera, &matWorld, &m_pX7->m_matProj, &m_pX7->m_vpViewport, fFPS );
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW) ;
		}

	//
	// Ocean:
	//
		if( m_pOcean && m_bShowOcean && m_pFighter )
		{
			pDev->SetRenderState(D3DRS_ZBIAS, 1) ;
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
			pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
			m_pOcean->Render( pDev, pTex, pulPolyCount, &matWorld, &m_pFighter->m_vPosition );
			pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW) ;
			pDev->SetRenderState(D3DRS_ZBIAS, 0) ;
		}

	//
	// Fighters:
	//
		pDev->SetRenderState( D3DRS_LIGHTING, TRUE );
		pDev->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pDev->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
		pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START + 0 ] );
		m_dwNumFightersPVS = 0 ;
		m_dwNumFightersDrawn = 0 ;
		if( m_pFighter && m_bShowFighter )
		{	
		// don't draw when inside cockpit
			if( m_ctCamMode != CT_COCKPIT_FIXED && m_ctCamMode != CT_COCKPIT_ROLL )
			{
				m_dwNumFightersPVS++ ;
				m_dwNumFightersDrawn += m_pFighter->Render( pDev, pTex, pulPolyCount, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, &matWorld, m_pCamera->m_matView, m_pX7->m_matProj, m_pX7->m_fTime, (const xVec3)m_litMain.Direction );
			}
		}
		pBot = m_pBots ;
		while( pBot )
		{
			if( !pBot->pFighter->m_bBotIsDead )
			{
				m_dwNumFightersPVS++ ;
				m_dwNumFightersDrawn += pBot->pFighter->Render( pDev, pTex, pulPolyCount, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, &matWorld, m_pCamera->m_matView, m_pX7->m_matProj, m_pX7->m_fTime, (const xVec3)m_litMain.Direction );
			}
			pBot = pBot->pNext ;
		}
		pDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );  
		pDev->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );

	//
	// Fighters' FX systems:
	//
		pDev->SetTexture( 0, pTex->m_pTex[ PARTICLE_TEX_START+1 ] );
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
		pDev->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// contrails;
		pDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		if( m_pFighter && m_bShowFighter )
		{
			m_pFighter->RenderFX_Contrails( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, SB, DB );
		}
		pBot = m_pBots ;
		while( pBot )
		{
			if( !pBot->pFighter->m_bBotIsDead )
				pBot->pFighter->RenderFX_Contrails( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, SB, DB );
			pBot = pBot->pNext ;
		}

	// weapons;
		if( m_pX7->m_pUserInput->m_bF6 ) ods("FM: ");
		//pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		if( m_pFighter && m_bShowFighter )
		{
			for( int w=WT_GREENLASER; w<MAX_WEAPON_TYPES; w++ )
			{
				BOOL bMustPopulateVB = TRUE ;
				int r ;

			// all bots;
				pBot = m_pBots ;
				while( pBot )
				{
					if( !pBot->pFighter->m_bBotIsDead )
					{
						r = pBot->pFighter->RenderFX_Weapons( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, &bMustPopulateVB, w );
						if( m_pX7->m_pUserInput->m_bF6 ) ods("%02d ", r);
					}
					pBot = pBot->pNext ;
				}

			// user fighter;
				r = m_pFighter->RenderFX_Weapons( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, &bMustPopulateVB, w );
				if( m_pX7->m_pUserInput->m_bF6 ) ods("[%02d] ", r);
			}
		}
		//pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		if( m_pX7->m_pUserInput->m_bF6 ) ods("\n");

	// explosions;
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		pDev->SetRenderState( D3DRS_LIGHTING,  FALSE );
		pDev->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		if( m_pFighter && m_bShowFighter )
		{	
			m_pFighter->RenderFX_Explosion( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, SB, DB );
		}
		pBot = m_pBots ;
		while( pBot )
		{
			if( !pBot->pFighter->m_bBotIsDead )
				pBot->pFighter->RenderFX_Explosion( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, SB, DB );
			pBot = pBot->pNext ;
		}
		pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	// shield;
		if( m_pFighter && m_bShowFighter )
		{	
			pDev->SetRenderState( D3DRS_LIGHTING, TRUE );
			pDev->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
			pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pDev->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
			pDev->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
			pDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			
			// SRCCOLOR, INVSRCCOLOR = nice v. translucent, but doesn't respect material alpha
			// SRCALPHA, ONE = nice, respects material alpha, but never becomes fully opaque
			pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		
			m_pFighter->RenderFX_Shield( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, SB, DB );
			pBot = m_pBots ;
			while( pBot )
			{
				if( !pBot->pFighter->m_bBotIsDead )
					pBot->pFighter->RenderFX_Shield( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, m_pTerrain, m_pSharedParticleSystemVB, fFPS, SB, DB );
				pBot = pBot->pNext ;
			}

			pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			pDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );  
			pDev->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );
			pDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE) ;
		}

	//
	// Disable GameItem lights & D3D lighting:
	//
		m_pTerrain->DisableItemLights( pDev, nMaxActiveLights - 1, 1 );
		pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
		pDev->LightEnable( 0, FALSE );
		
	//
	// Fighter's HUD; (overlay system, destroys proj. mat.)
	//
		if( m_pFighter && m_bShowHUD )
		{	
			m_pFighter->RenderHUD( pDev, pTex, pulPolyCount, m_ctCamMode, &matWorld, m_pCamera, m_pX7->m_pOverlayMan, this, fFPS );
		}

	//
	// Fullscreen Quads; (overlay system, destroys proj. mat.)
	//
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pX7->m_pOverlayMan->Render( pDev, OT_FULLSCREENQUAD, ORT_FAST );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		return S_OK ;
	}

	HRESULT CRace::FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS )
	{
#ifdef FULL_DBG_TRACE
	ods( "CRace::FrameMove START\n" );
#endif 

		BOT_FIGHTER* pBot ;

		if (fFPS == 0)
			return S_OK ;

	//
	// LEVEL SKIP: If all bots are dead, next level:
	//
		bool bSkipLevel = false ;
		/*int nDeadBots = 0 ;
		pBot = m_pBots ;
		while( pBot )
		{
			if( pBot->pFighter->m_bBotIsDead )
				nDeadBots++ ;
			pBot = pBot->pNext ;
		}
		if( nDeadBots==m_nBots || pInput->m_bKB_Pressed[ DIK_F9 ] )		// DBG
		{
			m_pX7->m_bGamePaused = FALSE ;
			m_pX7->m_bInGame = FALSE ;
			m_pX7->m_bInMainMenu = FALSE ;
			m_pX7->m_bInLevelTransition = TRUE ;
			return S_OK ;
		}*/

	//
	// LEVEL SKIP: DBG
	//
		if( pInput->m_bKB_Pressed[ DIK_F9 ] )
			bSkipLevel = true ;

	//
	// LEVEL SKIP: If all cherries are collected!
	//
		if( m_pFighter->m_nCherries == m_pTerrain->m_nCherries )
			bSkipLevel = true ;

		if( bSkipLevel )
		{
			m_pX7->m_bGamePaused = FALSE ;
			m_pX7->m_bInGame = FALSE ;
			m_pX7->m_bInMainMenu = FALSE ;
			m_pX7->m_bInLevelTransition = TRUE ;
			return S_OK ;
		}

	// DBG; blend ops.
		SB = m_pX7->SB ;
		DB = m_pX7->DB ;

	// DBG; bot tracking
		static bool bDbgTrackBot = false ;
		static int nTrackBot = 0 ;
		if( pInput->m_bKB_Pressed[ DIK_B ] )
		{
			if( !bDbgTrackBot )
			{
				nTrackBot = 0 ;
				bDbgTrackBot = true ;
			}
			else if( ++nTrackBot == m_nBots )
			{
				bDbgTrackBot = false ;
				nTrackBot = 0 ;
			}
			m_bShowHUD = !bDbgTrackBot ;
		}

	// DBG; player damage
		if( !m_pX7->m_bPlayerDamage )
		{
			m_pFighter->m_fShieldLevel = 1 ;
		}

#ifdef FULL_DBG_TRACE
	ods( "1\n" );
#endif

	//
	// Race State:
	//
		switch( m_State )
		{
			case GCS_LEVEL_PREINTRO:
				m_State = GCS_LEVEL_INTRO ;

				m_pFighter->m_bShowingHUDText = true ;
				sprintf( m_pFighter->m_szHUDText, "GET %d ITEMS", m_pTerrain->m_nCherries );

				m_pFighter->m_fHUDTextAge = 0 ;	m_pFighter->m_fHUDTextLifespan = 2 ;
				m_pFighter->m_fHUDTextX = -1 ; m_pFighter->m_fHUDTextY = -0.4f ;
				m_pFighter->m_fHUDTextXScale = 0.2f ; m_pFighter->m_fHUDTextYScale = 0.3f ;
				m_pFighter->m_fHUDTextRed = 1 ;	m_pFighter->m_fHUDTextGreen = 1 ; m_pFighter->m_fHUDTextBlue = 1 ; m_pFighter->m_fHUDTextAlpha = 0.3f ;
				break ;

			case GCS_LEVEL_INTRO:
				m_ctCamMode = CT_SHOWOFF ;
				m_fIntroTimeS -= ( 1 / fFPS );
				if( m_fIntroTimeS < 0 )
				{
					m_State = GCS_LEVEL_INGAME ;
					m_ctCamMode = CT_BEHIND_VARIABLE1 ;

					m_pFighter->m_bShowingHUDText = true ;
					sprintf( m_pFighter->m_szHUDText, "GO!", m_pTerrain->m_nCherries );
					m_pFighter->m_fHUDTextAge = 0 ;	m_pFighter->m_fHUDTextLifespan = 0.5f ;
					m_pFighter->m_fHUDTextX = -0.93f ; m_pFighter->m_fHUDTextY = -0.77f;
					m_pFighter->m_fHUDTextXScale = 0.65f ; m_pFighter->m_fHUDTextYScale = 0.65f ;
					m_pFighter->m_fHUDTextRed = 1 ; m_pFighter->m_fHUDTextGreen = 0 ; m_pFighter->m_fHUDTextBlue = 0 ; m_pFighter->m_fHUDTextAlpha = 0.2f ;
				}
				break ;

			case GCS_LEVEL_INGAME:
				break ;

			case GCS_LEVEL_COMPLETE:
				break ;
		}

	// rotate dynamic light;
		const float fDegRotPer60th = 0.1f ;
		xVec3 v = xVec3(m_litMain.Direction.x, m_litMain.Direction.y, m_litMain.Direction.z) ; 
		RotateVectorY(&v, (60.0f / fFPS) * fDegRotPer60th, &(xVec3(0, 0, 0))) ;
		m_litMain.Direction.x = v.x ;
		m_litMain.Direction.y = v.y ;
		m_litMain.Direction.z = v.z ;

#ifdef FULL_DBG_TRACE
	ods( "2\n" );
#endif

	// adjust ambient light level;
		if (pInput->m_bF3)
			if (m_dwAmbientLight > 0x00)
				m_dwAmbientLight -= 0x010101 ;
		if (pInput->m_bF4)
			if (m_dwAmbientLight < 0xffffff)
				m_dwAmbientLight += 0x010101 ;

	// determine terrain patch visibility;
		if (m_pTerrain && m_pCamera && m_bShowTerrain)
			m_pTerrain->FrameMove(pDev, pInput, fFPS, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport) ;

#ifdef FULL_DBG_TRACE
	ods( "3\n" );
#endif

	// pre-framemove fighters (some stuff needs to happen in one batch before each fighter is f'moved);
		m_pFighter->PreFrameMove( this );
		pBot = m_pBots ;
		while( pBot )
		{
			if( !pBot->pFighter->m_bBotIsDead )
				pBot->pFighter->PreFrameMove( this );
			pBot = pBot->pNext ;
		}

#ifdef FULL_DBG_TRACE
	ods( "4\n" );
#endif

	// move the user's fighter (this also manages the camera, as appropriate);
		int r ;
		if( pInput->m_bF7 ) ods("FM: ");
		if( m_pFighter && m_pCamera && m_bShowFighter )
		{
			BOOL bManageCam, bAcceptInput ;
			
			bAcceptInput = (m_ctCamMode != CT_FREE) ;
			bManageCam = (m_ctCamMode == CT_BEHIND_FIXED1 ||
						  m_ctCamMode == CT_BEHIND_FIXED2 || 
						  m_ctCamMode == CT_BEHIND_FIXED3 || 
						  m_ctCamMode == CT_BEHIND_VARIABLE1 || 
						  m_ctCamMode == CT_BEHIND_VARIABLE2 || 
						  m_ctCamMode == CT_BEHIND_VARIABLE3 || 
						  m_ctCamMode == CT_COCKPIT_FIXED || 
						  m_ctCamMode == CT_COCKPIT_ROLL) 
						  &&
						  !bDbgTrackBot ;
			
			r = m_pFighter->FrameMove(pDev, pInput, fFPS, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, bAcceptInput, bManageCam, m_pTerrain, m_ctCamMode, m_fGameElapsedTime, m_bShowHUD, this) ;
			if( pInput->m_bF7 ) ods("[%d] ", r);
		}

#ifdef FULL_DBG_TRACE
	ods( "4.5\n" );
#endif

	// update the sound manager's 3D sound listener
		m_pX7->m_pSound->FrameMove3D( &m_pFighter->m_vPosition,
									/*&m_pFighter->m_vVelocityMS*/NULL,
									&m_pFighter->m_vDirectionOfTravel,
									&m_pFighter->m_vUp );

#ifdef FULL_DBG_TRACE
	ods( "5\n" );
#endif

	// update bot fighters;
		pBot = m_pBots ;
		int i = 0 ;
		while( pBot )
		{
			if( !pBot->pFighter->m_bBotIsDead )
				r = pBot->pFighter->FrameMove(pDev, pInput, fFPS, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport, bDbgTrackBot && nTrackBot==i, m_pTerrain, m_ctCamMode, m_fGameElapsedTime, this) ;
			if( pInput->m_bF7 ) ods("%d ", r);
			pBot = pBot->pNext ;
			i++ ;
		}
		if( pInput->m_bF7 ) ods("\n");

#ifdef FULL_DBG_TRACE
	ods( "6\n" );
#endif

	// independent camera movement;
		if (m_pCamera && m_ctCamMode == CT_FREE)
		{
			m_pTerrain->LERPdY(m_pCamera->m_vEyePt.x, m_pCamera->m_vEyePt.z, &m_fY, &m_bCameraAboveGround, NULL) ;
			m_pCamera->SetManualFromInput(pDev, pInput, fFPS, m_fY) ;
			m_pCamera->SetPresetFromInput(pDev, pInput) ;
		}

#ifdef FULL_DBG_TRACE
	ods( "7\n" );
#endif

	// showoff camera mode;
		if( m_pCamera && m_pFighter && m_bShowFighter && m_ctCamMode == CT_SHOWOFF )
			m_pCamera->SetShowoff( pDev, &m_pFighter->m_pFRO->m_pBBox->m_vCenter,
								   m_pFighter->m_pFRO->m_pRenderData->m_fBSphereRadius * 2.5f,
								   fFPS, 0.01f, -0.005f, 0.0f) ;

	// sky animation;
		if (m_pSky && m_bShowSky)
			m_pSky->FrameMove(pDev, pInput, fFPS, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport) ;

#ifdef FULL_DBG_TRACE
	ods( "8\n" );
#endif

		/*if (m_pSkyDome && m_bShowSky)
			m_pSkyDome->FrameMove(pDev, pInput, fFPS, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport) ;*/

	// ocean animation;
		if (m_pOcean && m_bShowOcean)
			m_pOcean->FrameMove(pDev, pInput, fFPS, m_pCamera, &m_pX7->m_matProj, &m_pX7->m_vpViewport) ;

#ifdef FULL_DBG_TRACE
	ods( "9\n" );
#endif

	// reset time to max if the user just died as a result of time running out;
		if( m_bTimeRanOut && m_pFighter->m_bJustFinishedExploding )
		{
			m_bTimeRanOut = FALSE ;
			m_fGameTimeRemaining = m_fGameMaxTime ;
			m_fGameElapsedTime = 0 ;
		}

#ifdef FULL_DBG_TRACE
	ods( "10\n" );
#endif

	// elapsed time;
		m_fGameElapsedTime += ( 1.0f / fFPS );
		if( !m_pX7->m_bGodMode )
			m_fGameTimeRemaining -= ( 1.0f / fFPS );
		if( m_fGameTimeRemaining < 0 )
		{	// kill user when time runs out;
			m_fGameTimeRemaining = 0 ;
			m_pFighter->m_fShieldLevel = 0 ;
			m_bTimeRanOut = TRUE ;
		}
		char szMillis[ 256 ];
		sprintf(szMillis, "%02.0f", (m_fGameTimeRemaining - floorf( m_fGameTimeRemaining )) * 100.0f );
		sprintf( m_pHUDElapsedTime->m_szText, "%01d:%02d.%c%c",
				 (int)( floorf( m_fGameTimeRemaining / 60.0f ) ),
				 (int)( fmodf( m_fGameTimeRemaining, 60.0f ) ), szMillis[ 0 ], szMillis[ 1 ] );
		m_pHUDElapsedTime->SetFrameMoveRequired( TRUE );
		m_pHUDElapsedTime->FrameMove( pDev, pInput, m_pX7->m_pOverlayMan->m_pVBVertices );

	// DBG: dec. user fighter lives;
		if( m_pX7->m_pUserInput->m_bF5 )
			m_pFighter->m_fShieldLevel = 0.0f ;

#ifdef FULL_DBG_TRACE
	ods( "CRace::FrameMove() END\n" );
#endif

		return S_OK ;
	}

/*
 *
 * CD; One Projectile --> All Fighters
 *
 * Doesn't allow the fighter that launched the projectile to be hit.
 * Doesn't allow bot's projectiles to hit other bots.
 * TODO: change to obbox->sphere test.
 *
 */
	BOOL CRace::DetectAndHandleProjectileAgainstAllFighters( void* pvProjectile, void* pvOwningFighter, void** ppvHitFighter )
	{
		//if( pvOwningFighter == m_pFighter )
		if( ((CFighter*)pvOwningFighter)->m_bUserFighter )				// test user's projectiles against bots only!
		{
			BOT_FIGHTER* pBot = m_pBots ;
			while( pBot )
			{
				if( !pBot->pFighter->m_bBotIsDead )
				{
					if( DetectAndHandleFighterProjectileCollision(
							pBot->pFighter,
							pvProjectile,
							true
							) )
					{
						*ppvHitFighter = pBot->pFighter ;
						return TRUE ;
					}
				}
				pBot = pBot->pNext ;
			}
		}
		else												// note; we only test bots' projectiles against the user fighter
		//if( pvOwningFighter != m_pFighter )
			if( DetectAndHandleFighterProjectileCollision(
					m_pFighter,
					pvProjectile,
					false ) )
			{
				*ppvHitFighter = m_pFighter ;
				return TRUE ;
			}

		return FALSE ;
	}

/*
 *
 * One fighter -> One projectile test & handle
 *
 * Note that bounding spheres of bots are artificially increased to make them easier to hit.
 * 
 */
	BOOL CRace::DetectAndHandleFighterProjectileCollision( void* pvFighter, void* pvProjectile, bool bOwningFighterIsUserFighter )
	{
		CFighter* pFighter = (CFighter*)pvFighter ;
		CFighter::FIGHTER_PROJECTILE* pProjectile = (CFighter::FIGHTER_PROJECTILE*)pvProjectile ;

		/*xVec3 vTrailingPosBounding[ 4 ];
		vTrailingPosBounding[ 0 ] = pProjectile->pBBox->m_vVerts[ 0 ];
		vTrailingPosBounding[ 1 ] = pProjectile->pBBox->m_vVerts[ 1 ];
		vTrailingPosBounding[ 2 ] = pProjectile->pBBox->m_vVerts[ 2 ];
		vTrailingPosBounding[ 3 ] = pProjectile->pBBox->m_vVerts[ 3 ];*/

		//for( int i=0 ; i<4 ; i++ )
		{
			int nIntersects ;
			xVec3 vIntersect1, vIntersect2, vAvgIntersect ;

		// make bots easier to hit!
			float fBSRad = pFighter->m_pFRO->m_pRenderData->m_fBSphereRadius ;

			//if( pvFighter != m_pFighter )
			if( !((CFighter*)pvFighter)->m_bUserFighter )
			{
				fBSRad *= ( 2 + ( m_pFighter->m_fWeaponsLevel * 3 ) );
			}

		// line-sphere test;
			BOOL bCollision = SphereLineTest( pProjectile->vTrailingPos,
											  ( pProjectile->vTrailingPos + pProjectile->vVel ),
											  //vTrailingPosBounding[ i ],
											  //( vTrailingPosBounding[ i ] + pProjectile->vVel ),
							  				  pFighter->m_vPosition,
											  fBSRad/*pFighter->m_pFRO->m_pRenderData->m_fBSphereRadius*/,
											  &nIntersects, &vIntersect1, &vIntersect2 );
			if( bCollision )
			{
			//
			// DO THE DAMAGE!
			//
				float bJustKilledAnEnemy = false ;
				if( pFighter->m_fShieldLevel - pProjectile->fDamage <= 0 && 
					pFighter->m_fShieldLevel > 0 )
					bJustKilledAnEnemy = true ;

			// scale damage to player's skill setting; ( 0.5 through 1.5 times base damage )
				float fModifier = 1 ;
				if( !bOwningFighterIsUserFighter )
					fModifier = 0.5f + ( m_pFighter->m_fSkill * 1.5f );
				pFighter->m_fShieldLevel -= pProjectile->fDamage * fModifier ;

				if( pFighter->m_fShieldLevel < 0 )
					pFighter->m_fShieldLevel = 0 ;
				if( pFighter->m_fShieldLevel > 0 )
					pFighter->StartShieldEffect( pProjectile->fDamage, this );
				pFighter->m_bShieldLevelChanged = TRUE ;

			//
			// EXCELLENT and IMPRESSIVE
			// HACK: check to see if a bot is dead, and that we (the user ship) ship fired the round
			//
				if( bJustKilledAnEnemy && bOwningFighterIsUserFighter )
				{
					float s = rnd () ;
					if( s < 0.5f )
						m_pX7->m_pSound->PlaySFX3D( SFX_EXCELLENT, &m_pFighter->m_vPosition, NULL, 1 );
					else
						m_pX7->m_pSound->PlaySFX3D( SFX_IMPRESSIVE, &m_pFighter->m_vPosition, NULL, 1 );

				// bots need this flag set to stop them respawning after
				// they run out of BotLife (CBotFighter::m_nBotLives)
					((CBotFighter*)pvFighter)->m_bBotWasKilledByProjectile = true ;

				// bigup points bonus for this!
					m_pFighter->m_fScoreAddition = ( m_pX7->m_fScore * 0.05f );
				}

			// 'rewind' the projectile so that the projectile is centered between the sphere intersects;
				xVec3 vD = pProjectile->vLeadingPos - vIntersect1 ;
				xVec3 vD2= (pProjectile->vLeadingPos - pProjectile->vTrailingPos) / 1 ;
				xMat matD ;
				D3DXMatrixTranslation( &matD, -vD.x + vD2.x, -vD.y + vD2.y, -vD.z + vD2.z );
				D3DXMatrixMultiply( &pProjectile->matWorld, &pProjectile->matWorld, &matD );

				return TRUE ;
			}
		}

		return FALSE ;
	}
