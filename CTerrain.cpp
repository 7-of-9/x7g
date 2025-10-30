#pragma warning( disable : 4183 )	// 'missing return type'

#include "CTerrain.h"

//
// Row,Col to Absolute - used for indexing 2D coords. into 1D arrays
//
#define TER_RC2ABS(r,c)\
	(r)*m_nPatchesSQ+(c)

/*
 *
 * Construction / Destruction:
 *
 */
	CTerrain::CTerrain( float fSizeSQ,
						Cx7 *pX7,
						int nPatchesSQ,
						int nPatchMaxLODSQ,
						int nCherriesReq,
						int nShieldPwrUpsReq,
						int nEnginePwrUpsReq,
						int nWeaponPwrUpsReq,
						int nHealthPwrUpsReq,
						float fMountainFactor,
						int nTex1,
						int nTex2,
						unsigned nNoisePatches1,
						unsigned nNoisePatches2,
						int ColorFactor1, int ColorFactor2, int ColorFactor3 )
	{
		float		  fReqCherriesPerPatch ;
		float		  fReqShieldPwrUpsPerPatch ;
		float		  fReqEnginePwrUpsPerPatch ;
		float		  fReqWeaponPwrUpsPerPatch ;
		float		  fReqHealthPwrUpsPerPatch ;
		unsigned long i = 0 ;
		float		  fRnd[ 256 ];

		m_nCurLOD = 0 ;

	//
	// Basic Parameters:
	//
		m_nPatchesSQ = nPatchesSQ ;
		m_nPatchesReqQuadTDepth = Pow2( m_nPatchesSQ );
		
		m_nPatchMaxLODSQ = nPatchMaxLODSQ ;
		m_nPatchMaxLODLevels = Pow2( m_nPatchMaxLODSQ ) - 1 ;

	//
	// Member initialization:
	//
		m_pQT = NULL ;
		m_pX7 = pX7 ;
		m_dwMem = sizeof(CTerrain) ;
		m_fLenSQ = fSizeSQ ;
		
		for( i = 0 ; i < MAX_SPAWN_POS ; i++ )
			m_vSpawnPos[ i ] = xVec3( 0.0f, 0.0f, 0.0f );
		m_nSpawnPos = 0 ;

//return ;

	//
	// Items stuff:
	//
		float fSpotlightAttenuation0 = 0.0f ;
		float fSpotlightAttenuation1 = 0.01f ;	// linear falloff; inversely prop. to vertex distance from light
		float fSpotlightAttenuation2 = 0.0f ;
		float fSpotlightRange = m_fLenSQ / 20.0f ; //sqrtf( FLT_MAX );

		m_pItemSphereMesh = NULL ;
		SetupSpecularMaterial( &m_mtrlItemSphere, 1.0f, 1.0f, 1.0f, 0.25f );

		m_nCherriesReq = nCherriesReq ;
		m_nCherries = 0 ;

		m_nShieldPwrUpsReq = nShieldPwrUpsReq ;
		m_nShieldPwrUps = 0 ;

		m_nEnginePwrUpsReq = nEnginePwrUpsReq ;
		m_nEnginePwrUps = 0 ;

		m_nWeaponPwrUpsReq = nWeaponPwrUpsReq ;
		m_nWeaponPwrUps = 0 ;

		m_nHealthPwrUpsReq = nHealthPwrUpsReq ;
		m_nHealthPwrUps = 0 ;

		float fItemLightMaxConeAngle = g_PI * 1.0f ;


		D3DLIGHTTYPE itemLightType = D3DLIGHT_SPOT ;

	//
	// Cherry Item stuff:
	//
		D3DXCOLOR m_colCherry = D3DXCOLOR( 1.0f, 0.0f, 1.0f, 1.0f );
		SetupSpecularMaterial( &m_mtrlCherry, m_colCherry.r, m_colCherry.g, m_colCherry.b, 1.0f );
		ZeroMemory(&m_litCherry, sizeof(D3DLIGHT8)) ;
		m_litCherry.Type         = itemLightType ;
		m_litCherry.Diffuse.r	 = m_litCherry.Ambient.r = m_litCherry.Specular.r = m_colCherry.r ;
		m_litCherry.Diffuse.g	 = m_litCherry.Ambient.g = m_litCherry.Specular.g = m_colCherry.g ;
		m_litCherry.Diffuse.b    = m_litCherry.Ambient.b = m_litCherry.Specular.b = m_colCherry.b ;
		//m_litCherry.Specular.r   = m_litCherry.Specular.g =	m_litCherry.Specular.b = 1 ;
		m_litCherry.Position     = D3DXVECTOR3( 0,0,0 );
		D3DXVec3Normalize( (xVec3*)&m_litCherry.Direction, &xVec3( 0.0f, -1.0f, 0.0f ) );
		m_litCherry.Attenuation0 = fSpotlightAttenuation0 ;
		m_litCherry.Attenuation1 = fSpotlightAttenuation1 ;
		m_litCherry.Attenuation2 = fSpotlightAttenuation2 ;
		m_litCherry.Falloff		 = 1.0f ;
		m_litCherry.Theta		 = g_PI / 50.0f ; ;
		m_litCherry.Phi			 = fItemLightMaxConeAngle ;
		m_litCherry.Range        = fSpotlightRange ;

		m_pCherry = NULL ;
#ifdef LOAD_RES_FROM_FILE
		m_pCherry = new CFileRenderObject( "..\\resource\\mod0\\cherry.x", 0, NULL );
#else
		m_pCherry = new CFileRenderObject( NULL, IDR_X_CHERRY );
#endif
		assert( m_pCherry );


	//
	// Shield Item stuff:
	//
		D3DXCOLOR m_colShield = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );
		SetupSpecularMaterial( &m_mtrlShield, m_colShield.r, m_colShield.g, m_colShield.b, 0.5f );
		ZeroMemory( &m_litShield, sizeof(D3DLIGHT8) );
		m_litShield.Type         = itemLightType ;
		m_litShield.Diffuse.r	 = m_litShield.Ambient.r = m_litShield.Specular.r = m_colShield.r ;
		m_litShield.Diffuse.g	 = m_litShield.Ambient.g = m_litShield.Specular.g = m_colShield.g ;
		m_litShield.Diffuse.b    = m_litShield.Ambient.b = m_litShield.Specular.b = m_colShield.b ;
		//m_litShield.Specular.r   = m_litShield.Specular.g =	m_litShield.Specular.b = 1 ;
		m_litShield.Position     = D3DXVECTOR3( 0, 0, 0 );
		D3DXVec3Normalize( (xVec3*)&m_litShield.Direction, &xVec3( 0.0f, -1.0f, 0.0f ) );
		m_litShield.Attenuation0 = fSpotlightAttenuation0 ;
		m_litShield.Attenuation1 = fSpotlightAttenuation1 ;
		m_litShield.Attenuation2 = fSpotlightAttenuation2 ;
		m_litShield.Falloff		 = 1.0f ;
		m_litShield.Theta		 = g_PI / 50.0f ;
		m_litShield.Phi			 = fItemLightMaxConeAngle ;
		m_litShield.Range        = fSpotlightRange ;
		m_pShield = NULL ;
#ifdef LOAD_RES_FROM_FILE
		//m_pShield = new CFileRenderObject( "..\\resource\\mod0\\cherry.x", 0, NULL );
#else
		//TODO...
#endif
		//assert( m_pShield );

	//
	// Engine Item stuff:
	//
		D3DXCOLOR m_colEngine = D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f );
		SetupSpecularMaterial( &m_mtrlEngine, m_colEngine.r, m_colEngine.g, m_colEngine.b, 0.5f );
		ZeroMemory( &m_litEngine, sizeof(D3DLIGHT8) );
		m_litEngine.Type         = itemLightType ;
		m_litEngine.Diffuse.r	 = m_litEngine.Ambient.r = m_litEngine.Specular.r = m_colEngine.r ;
		m_litEngine.Diffuse.g	 = m_litEngine.Ambient.g = m_litEngine.Specular.g = m_colEngine.g ;
		m_litEngine.Diffuse.b    = m_litEngine.Ambient.b = m_litEngine.Specular.b = m_colEngine.b ;
		//m_litEngine.Specular.r   = m_litEngine.Specular.g =	m_litEngine.Specular.b = 1 ;
		m_litEngine.Position     = D3DXVECTOR3( 0, 0, 0 );
		D3DXVec3Normalize( (xVec3*)&m_litEngine.Direction, &xVec3( 0.0f, -1.0f, 0.0f ) );
		m_litEngine.Attenuation0 = fSpotlightAttenuation0 ;
		m_litEngine.Attenuation1 = fSpotlightAttenuation1 ;
		m_litEngine.Attenuation2 = fSpotlightAttenuation2 ;
		m_litEngine.Falloff		 = 1.0f ;
		m_litEngine.Theta		 = g_PI / 50.0f ;
		m_litEngine.Phi			 = fItemLightMaxConeAngle ;
		m_litEngine.Range        = fSpotlightRange ;

	//
	// Weapon Item stuff:
	//
		D3DXCOLOR m_colWeapon = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );
		SetupSpecularMaterial( &m_mtrlWeapon, m_colWeapon.r, m_colWeapon.g, m_colWeapon.b, 0.5f );
		ZeroMemory( &m_litWeapon, sizeof(D3DLIGHT8) );
		m_litWeapon.Type         = itemLightType ;
		m_litWeapon.Diffuse.r	 = m_litWeapon.Ambient.r = m_litWeapon.Specular.r = m_colWeapon.r ;
		m_litWeapon.Diffuse.g	 = m_litWeapon.Ambient.g = m_litWeapon.Specular.g = m_colWeapon.g ;
		m_litWeapon.Diffuse.b    = m_litWeapon.Ambient.b = m_litWeapon.Specular.b = m_colWeapon.b ;
		//m_litWeapon.Specular.r   = m_litWeapon.Specular.g =	m_litWeapon.Specular.b = 1 ;
		m_litWeapon.Position     = D3DXVECTOR3( 0, 0, 0 );
		D3DXVec3Normalize( (xVec3*)&m_litWeapon.Direction, &xVec3( 0.0f, -1.0f, 0.0f ) );
		m_litWeapon.Attenuation0 = fSpotlightAttenuation0 ;
		m_litWeapon.Attenuation1 = fSpotlightAttenuation1 ;
		m_litWeapon.Attenuation2 = fSpotlightAttenuation2 ;
		m_litWeapon.Falloff		 = 1.0f ;
		m_litWeapon.Theta		 = g_PI / 50.0f ;
		m_litWeapon.Phi			 = fItemLightMaxConeAngle ;
		m_litWeapon.Range        = fSpotlightRange ;
		m_pWeapon = NULL ;
#ifdef LOAD_RES_FROM_FILE
		//m_pWeapon = new CFileRenderObject( "..\\resource\\mod0\\sword.x", 0, NULL );
#else
		//TODO...
#endif
		//assert( m_pWeapon );

	//
	// Health Item stuff:
	//
		D3DXCOLOR m_colHealth = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );
		SetupSpecularMaterial( &m_mtrlHealth, m_colHealth.r, m_colHealth.g, m_colHealth.b, 0.5f );
		ZeroMemory( &m_litHealth, sizeof(D3DLIGHT8) );
		m_litHealth.Type         = itemLightType ;
		m_litHealth.Diffuse.r	 = m_litHealth.Ambient.r = m_litHealth.Specular.r = m_colHealth.r ;
		m_litHealth.Diffuse.g	 = m_litHealth.Ambient.g = m_litHealth.Specular.g = m_colHealth.g ;
		m_litHealth.Diffuse.b    = m_litHealth.Ambient.b = m_litHealth.Specular.b = m_colHealth.b ;
		//m_litHealth.Specular.r   = m_litHealth.Specular.g =	m_litHealth.Specular.b = 1 ;
		m_litHealth.Position     = D3DXVECTOR3( 0, 0, 0 );
		D3DXVec3Normalize( (xVec3*)&m_litHealth.Direction, &xVec3( 0.0f, -1.0f, 0.0f ) );
		m_litHealth.Attenuation0 = fSpotlightAttenuation0 ;
		m_litHealth.Attenuation1 = fSpotlightAttenuation1 ;
		m_litHealth.Attenuation2 = fSpotlightAttenuation2 ;
		m_litHealth.Falloff		 = 1.0f ;
		m_litHealth.Theta		 = g_PI / 50.0f ;
		m_litHealth.Phi			 = fItemLightMaxConeAngle ;
		m_litHealth.Range        = fSpotlightRange ;
		m_pHealth = NULL ;
#ifdef LOAD_RES_FROM_FILE
		m_pHealth = new CFileRenderObject( "..\\resource\\mod0\\heart.x", 0, NULL );
#else
		//TODO...
#endif
		assert( m_pHealth );


	//
	// Allocate dynamic memory arrays:
	//
		m_bPatchesPicked = new BOOL[ m_nPatchesSQ * m_nPatchesSQ ];				assert( m_bPatchesPicked );
		m_pPatches = new CTerrainPatch*[ m_nPatchesSQ * m_nPatchesSQ ];			assert( m_pPatches );
		m_nPatchesRequiredLOD = new int[ m_nPatchesSQ * m_nPatchesSQ ];			assert( m_nPatchesRequiredLOD );
		m_dsPatchesRequiredTopEdge = new int[ m_nPatchesSQ * m_nPatchesSQ ];	assert( m_dsPatchesRequiredTopEdge );
		m_dsPatchesRequiredLeftEdge = new int[ m_nPatchesSQ * m_nPatchesSQ ];	assert( m_dsPatchesRequiredLeftEdge );
		m_dsPatchesRequiredBottomEdge = new int[ m_nPatchesSQ * m_nPatchesSQ ]; assert( m_dsPatchesRequiredBottomEdge );
		m_dsPatchesRequiredRightEdge = new int[ m_nPatchesSQ * m_nPatchesSQ ];	assert( m_dsPatchesRequiredRightEdge );
		m_psPatches = new PATCH_SORT[ m_nPatchesSQ * m_nPatchesSQ ];			assert( m_psPatches );

	//
	// Allocate the heightmap:
	//
		unsigned long ulNextRowHeightmapDelta ;
		m_nHeightmapSQ = m_nPatchMaxLODSQ + 1 ;
		if (m_nPatchesSQ > 1)
			m_nHeightmapSQ += (m_nPatchesSQ - 1) * m_nPatchMaxLODSQ ;
		m_pfHeightmap = NULL ;
		m_pfHeightmap = new float[m_nHeightmapSQ * m_nHeightmapSQ] ;
		assert( m_pfHeightmap );
		m_dwMem += sizeof(float) * m_nHeightmapSQ * m_nHeightmapSQ ;

	//
	// Allocate the colormap:
	//
		m_pColormap = NULL ;
		m_pColormap = new D3DXCOLOR[m_nHeightmapSQ * m_nHeightmapSQ] ;
		assert( m_pColormap );
		m_dwMem += sizeof(D3DXCOLOR) * m_nHeightmapSQ * m_nHeightmapSQ ;

	// define unit grid size:
		m_fUnitSize = m_fLenSQ / (float)(m_nHeightmapSQ - 1) ;

	//
	// Populate the heightmap and colormap
	//
	// Heights built using superposing waves with different frequencies
	// Colors built using simple gradients on height values + random noise patches
	//
		BOOL bBuildFlat = m_pX7->m_pCmdLine->GetInt( "TerrainFlat", 0 );
		
		if( !bBuildFlat )
		{
			m_fMinY = -fSizeSQ / 50.0f ;
			m_fMaxY = +fSizeSQ / 10.0f ;
		}
		else
		{
			m_fMinY = m_fMaxY = 0.0f ;
		}
		
		float fMidY = (m_fMaxY - m_fMinY) / 2.0f + m_fMinY ;

		for (i = 0 ; i < 256 ; i ++)
			fRnd[i] = rnd() ;

		i = 0 ;
		float fDebug = 0.0f ;
		float fIndent = 0.1f ;
		for (unsigned int row = 0 ; row < m_nHeightmapSQ ; row ++)
		{
			for (unsigned int col = 0 ; col < m_nHeightmapSQ ; col ++)
			{
				sprintf(m_pX7->m_szLoadingGameInfo2, "m_pfHeightmap %03dx%03d [of %dx%d]", row, col, m_nHeightmapSQ-1, m_nHeightmapSQ-1) ;

			//
			// Compute height:
			//
				float fX, fY, fSX, fSY, fE1, fE2, fEA ;
				fEA = 0 ;
				fX  = (float)row / ((float)m_nHeightmapSQ) ;
				fY  = (float)col / ((float)m_nHeightmapSQ) ;

				if (row != 0 && row != m_nHeightmapSQ - 1 && col != 0 && col != m_nHeightmapSQ - 1)
				{
					fSX = g_PI * fX ;
					fSY = g_PI * fY ;

				// main low amp wave
					{
						fE1 = sinf(fSX) * ( fMountainFactor + 0.8f );
						fE2 = sinf(fSY) * ( fMountainFactor + 0.8f );
						fEA = ((fE1 + fE2)) / 2 ;
						if (fE1 < fE2) fEA = fE1 ; else fEA = fE2 ;
					}

				// mid amp. waves
					if( fX > fIndent && fX < 1.0f - fIndent &&
						fY > fIndent && fY < 1.0f - fIndent )
					{
						fSX = g_PI * ( fX - fIndent ) / ( 1.0f - fIndent * 2.0f );
						fSY = g_PI * ( fY - fIndent ) / ( 1.0f - fIndent * 2.0f );

						if( fMountainFactor > 0 )
						{
							fE1 = sinf(fSX * 3.0f) * 0.7f * (fRnd[0] / 0.5f) ;
							fE2 = sinf(fSY * 3.0f) * 0.7f * (fRnd[0] / 0.5f) ;
							fEA += (fE1 + fE2) / 2.0f ;
						}

						// this style OVERWRITES fEA from the main bump,
						// pulling it DOWN; this has the effect of making no HM points
						// == to fMaxY, this in turn causes no spawn positions to
						// be generated, which in turn causes the fighter to spawn under
						// the terrain. ouch. it *does* blend the smaller bumps
						// smoothly though.
						//if (fE1 < fE2) fEA = fE1 ; else fEA = fE2 ;
					
						if( fMountainFactor > 0.2f )
						{
							fE1 = sinf(fSX * 1.0f) * 0.5f * (fRnd[1] / 0.5f) ;
							fE2 = sinf(fSY * 1.0f) * 0.5f * (fRnd[1] / 0.5f) ;
							fEA += (fE1 + fE2) / 2.0f ;
						}

						if( fMountainFactor > 0.6f )
						{
							fE1 = sinf(fSX * 3.0f) * 0.4f * (fRnd[2] / 0.5f) ;
							fE2 = sinf(fSY * 3.0f) * 0.4f * (fRnd[2] / 0.5f) ;
							fEA += (fE1 + fE2) / 2.0f ;
						}

						/*if( fMountainFactor > 0.8f )
						{
							fE1 = sinf(fSX * 32.0f) * 0.3f * (fRnd[3] / 0.5f) ;
							fE2 = sinf(fSY * 32.0f) * 0.3f * (fRnd[3] / 0.5f) ;
							fEA += (fE1 + fE2) / 2.0f ;
						}*/
					}

				// low amp. waves
					if( fMountainFactor > 0.6f )
					{
						fE1 = sinf(fSX * 64.0f) * 0.03f ;
						fE2 = sinf(fSY * 64.0f) * 0.03f ;
						fEA += (fE1 + fE2) / 2.0f ;
					}

					if( fMountainFactor > 0.7f )
					{
						fE1 = sinf(fSX * 128.0f) * 0.02f ;
						fE2 = sinf(fSY * 128.0f) * 0.02f ;
						fEA += (fE1 + fE2) / 2.0f ;
					}

					if( fMountainFactor > 0.8f )
					{
						fE1 = sinf(fSX * 256.0f) * 0.01f ;
						fE2 = sinf(fSY * 256.0f) * 0.01f ;
						fEA += (fE1 + fE2) / 2.0f ;
					}
				}

			// clamp
				if (fEA > 1.0f)	fEA = 1.0f ;
				if (fEA < 0.0f)	fEA = 0.0f ;

			// randomize a little
				fEA += rnd() * 0.01f * (rnd() > 0.5f ? +1.0f : -1.0f) ;

			// clamp
				if (fEA > 1.0f)	fEA = 1.0f ;
				if (fEA < 0.0f)	fEA = 0.0f ;

			// assign to heightmap
				m_pfHeightmap[i] = m_fMinY + fEA * (m_fMaxY - m_fMinY) ;
				m_pfHeightmap[i] = fDebug++ ;
				
				if( bBuildFlat )
					m_pfHeightmap[i] = 0 ;
				else
					m_pfHeightmap[i] = m_fMinY + fEA * (m_fMaxY - m_fMinY) ;

				//flat sloping plane.
				/*m_pfHeightmap[i] = (((float)row / (float)(m_nHeightmapSQ - 1)) * -fSizeSQ/2) ;
				m_fMinY = -fSizeSQ/2 ;
				m_fMaxY = 0.0f ;*/

			// Next...!
				i++ ;
			}
		}
	
	//
	// Blend in spline-based trench. Note that this further lowers m_fMinY.
	//
		sprintf(m_pX7->m_szLoadingGameInfo2, "BlendSplineTrench()", i, m_nHeightmapSQ * m_nHeightmapSQ) ;
		BlendSplineTrench() ;

	//
	// Compute primary color gradient:
	//
		float fFullRange = m_fMaxY - m_fMinY ;
		float fHalfRange = fFullRange / 2.0f ;
		for (i = 0 ; i < m_nHeightmapSQ * m_nHeightmapSQ ; i ++)
		{
		// three different types of gradient; user-specified for each colour:
		// note that ColorFactor2 and ColorFactor3 are IGNORED!

			if( ColorFactor1 == 1 )
			{
				m_pColormap[i].r = (fabsf(fMidY - m_pfHeightmap[i]) / fHalfRange) ;		// 1 - 0 - 1
				m_pColormap[i].g = 1 ;													
				m_pColormap[i].b = (m_pfHeightmap[i] + fabsf(m_fMinY)) / fFullRange ;
			}
			else if( ColorFactor1 == 2 )
			{
				m_pColormap[i].r = 1 ;
				m_pColormap[i].g = (fabsf(fMidY - m_pfHeightmap[i]) / fHalfRange) ;		// 1 - 0 - 1													
				m_pColormap[i].b = (m_pfHeightmap[i] + fabsf(m_fMinY)) / fFullRange ;
			}
			else if( ColorFactor1 == 3 )
			{
				m_pColormap[i].r = (m_pfHeightmap[i] + fabsf(m_fMinY)) / fFullRange ;
				m_pColormap[i].g = (fabsf(fMidY - m_pfHeightmap[i]) / fHalfRange) ;		// 1 - 0 - 1													
				m_pColormap[i].b = 1 ;
			}

		// note; vertex-alpha is re-assigned from this basic height-based blend,
		// in CTerrainPatch::AddVertex() to a face normal-based blend.
			m_pColormap[i].a = (m_pfHeightmap[i] + fabsf(m_fMinY)) / fFullRange ;	// 0 - 1
			
			m_pColormap[i].r *= 255.0f ;
			m_pColormap[i].g *= 255.0f ;
			m_pColormap[i].b *= 255.0f ;
			m_pColormap[i].a *= 255.0f ;

			sprintf(m_pX7->m_szLoadingGameInfo2, "m_pColormap %lu [of %lu]", i, m_nHeightmapSQ * m_nHeightmapSQ) ;
		}

	//
	// Add random circular noise patches to the base colormap:
	//
		for (i = 0 ; i < nNoisePatches1 ; i ++)
		{
			BlendColorSpot(
						   (int)(rnd() * (float)m_nHeightmapSQ),
						   (int)(rnd() * (float)m_nHeightmapSQ),
						   (int)(rnd() * (float)m_nHeightmapSQ / 8.0f),
						   &D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f)
						  ) ;
			sprintf( m_pX7->m_szLoadingGameInfo2, "BlendColorSpot %lu [of 800]", i );
		}

		for (i = 0 ; i < nNoisePatches2 ; i ++)
		{
			BlendColorSpot(
						   (int)(rnd() * (float)m_nHeightmapSQ),
						   (int)(rnd() * (float)m_nHeightmapSQ),
						   (int)(rnd() * (float)m_nHeightmapSQ / 8.0f),
						   &D3DXCOLOR(rnd() * 255.0f, rnd() * 255.0f, rnd() * 255.0f, 0.0f)
						  ) ;
			sprintf( m_pX7->m_szLoadingGameInfo2, "BlendColorSpot %lu [of 3200]", i );
		}

	//
	// Build patches:
	//
		ulNextRowHeightmapDelta = (m_nPatchesSQ - 1) * m_nPatchMaxLODSQ ;
		float fPatchSizeSQ = fSizeSQ / (float)m_nPatchesSQ ;

		int nPatchesDone = 0 ;
		for (int y = 0 ; y < m_nPatchesSQ ; y ++)
			for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			{
				sprintf(m_pX7->m_szLoadingGameInfo2, "Patch %dx%d [of %dx%d]", y+1, x+1, m_nPatchesSQ, m_nPatchesSQ) ;

			// determine how many items to ask for, so as we don't go over the specified limits:
				float fCherries2Ask4, fShieldPwrUps2Ask4, fEnginePwrUps2Ask4, fWeaponPwrUps2Ask4, fHealthPwrUps2Ask4 ;

				fReqCherriesPerPatch = (float)m_nCherriesReq / ((float)(nPatchesSQ * nPatchesSQ)-nPatchesDone) ;
				fReqShieldPwrUpsPerPatch = (float)m_nShieldPwrUpsReq / ((float)(nPatchesSQ * nPatchesSQ)-nPatchesDone) ;
				fReqEnginePwrUpsPerPatch = (float)m_nEnginePwrUpsReq / ((float)(nPatchesSQ * nPatchesSQ)-nPatchesDone) ;
				fReqWeaponPwrUpsPerPatch = (float)m_nWeaponPwrUpsReq / ((float)(nPatchesSQ * nPatchesSQ)-nPatchesDone) ;
				fReqHealthPwrUpsPerPatch = (float)m_nHealthPwrUpsReq / ((float)(nPatchesSQ * nPatchesSQ)-nPatchesDone) ;

				if( m_nCherries >= m_nCherriesReq ) fCherries2Ask4 = 0.0f ; 
				else if( m_nCherries < m_nCherriesReq-fReqCherriesPerPatch ) fCherries2Ask4 = fReqCherriesPerPatch ;
				else fCherries2Ask4 = (float)m_nCherriesReq-m_nCherries ;
				
				if( m_nShieldPwrUps >= m_nShieldPwrUpsReq ) fShieldPwrUps2Ask4 = 0.0f ; 
				else if( m_nShieldPwrUps < m_nShieldPwrUpsReq-fReqShieldPwrUpsPerPatch ) fShieldPwrUps2Ask4 = fReqShieldPwrUpsPerPatch ;
				else fShieldPwrUps2Ask4 = (float)m_nShieldPwrUpsReq-m_nShieldPwrUps ;
				
				if( m_nEnginePwrUps >= m_nEnginePwrUpsReq ) fEnginePwrUps2Ask4 = 0.0f ; 
				else if( m_nEnginePwrUps < m_nEnginePwrUpsReq-fReqEnginePwrUpsPerPatch ) fEnginePwrUps2Ask4 = fReqEnginePwrUpsPerPatch ;
				else fEnginePwrUps2Ask4 = (float)m_nEnginePwrUpsReq-m_nEnginePwrUps ;

				if( m_nWeaponPwrUps >= m_nWeaponPwrUpsReq ) fWeaponPwrUps2Ask4 = 0.0f ; 
				else if( m_nWeaponPwrUps < m_nWeaponPwrUpsReq-fReqWeaponPwrUpsPerPatch ) fWeaponPwrUps2Ask4 = fReqWeaponPwrUpsPerPatch ;
				else fWeaponPwrUps2Ask4 = (float)m_nWeaponPwrUpsReq-m_nWeaponPwrUps ;

				if( m_nHealthPwrUps >= m_nHealthPwrUpsReq ) fHealthPwrUps2Ask4 = 0.0f ; 
				else if( m_nHealthPwrUps < m_nHealthPwrUpsReq-fReqHealthPwrUpsPerPatch ) fHealthPwrUps2Ask4 = fReqHealthPwrUpsPerPatch ;
				else fHealthPwrUps2Ask4 = (float)m_nHealthPwrUpsReq-m_nHealthPwrUps ;

			// create the patch:
				xVec3 vTL ;
				vTL = xVec3(-(fSizeSQ / 2.0f) + x * fPatchSizeSQ, 0, -(fSizeSQ / 2.0f) + y * fPatchSizeSQ) ;
				m_pPatches[TER_RC2ABS(x,y)] = NULL ;

				m_pPatches[TER_RC2ABS(x,y)] = new CTerrainPatch
						(
						 m_nPatchMaxLODSQ,
						 &vTL,
						 fPatchSizeSQ,
						 fPatchSizeSQ,
						 &m_pfHeightmap[y * m_nHeightmapSQ * m_nPatchMaxLODSQ + x * m_nPatchMaxLODSQ],
						 &m_pColormap[y * m_nHeightmapSQ * m_nPatchMaxLODSQ + x * m_nPatchMaxLODSQ],
						 ulNextRowHeightmapDelta,
						 m_nHeightmapSQ,
						 m_fMinY,
						 m_fMaxY,
						 (m_nSpawnPos < MAX_SPAWN_POS - 1) ? &m_vSpawnPos[ m_nSpawnPos ] : NULL,
						 m_fLenSQ,
						 fCherries2Ask4,
						 fShieldPwrUps2Ask4,
						 fEnginePwrUps2Ask4,
						 fWeaponPwrUps2Ask4,
						 fHealthPwrUps2Ask4,
						 nTex1, nTex2
						);

				m_nCherries += m_pPatches[TER_RC2ABS(x,y)]->m_nCherries ;
				m_nShieldPwrUps += m_pPatches[TER_RC2ABS(x,y)]->m_nShieldPwrUps ;
				m_nEnginePwrUps += m_pPatches[TER_RC2ABS(x,y)]->m_nEnginePwrUps ;
				m_nWeaponPwrUps += m_pPatches[TER_RC2ABS(x,y)]->m_nWeaponPwrUps ;
				
				//ods( "Patch %dx%d; required %0.4f, got %d cherries.\n", x, y, fReqCherriesPerPatch, m_pPatches[TER_RC2ABS(x,y)]->m_nCherries );
				
				if( m_vSpawnPos[ m_nSpawnPos ] != xVec3( 0.0f, 0.0f, 0.0f ) )
					m_nSpawnPos++ ;
				
				assert( m_pPatches[TER_RC2ABS(x,y)] );
				
			// maintain memory monitor:
				m_dwMem += m_pPatches[TER_RC2ABS(x,y)]->m_dwMem ;
				
				nPatchesDone++ ;
			}

		ods( "total CHERRIES; required %d, got %d.\n", m_nCherriesReq, m_nCherries );
		ods( "total SHIELD; required %d, got %d.\n", m_nShieldPwrUpsReq, m_nShieldPwrUps );
		ods( "total ENGINE; required %d, got %d.\n", m_nEnginePwrUpsReq, m_nEnginePwrUps );
		ods( "total WEAPON; required %d, got %d.\n", m_nWeaponPwrUpsReq, m_nWeaponPwrUps );
		ods( "MAX_SPAWN_POS=%d, we got %d.\n", MAX_SPAWN_POS, m_nSpawnPos );
		assert( m_nSpawnPos > 0 );

	//
	// Define bounding volume data:
	//
		m_vMin = xVec3( -m_fLenSQ / 2.0f, m_fMinY, -m_fLenSQ / 2.0f );
		m_vMax = xVec3( +m_fLenSQ / 2.0f, m_fMaxY, +m_fLenSQ / 2.0f );

	//
	// Build a Quadtree of the terrain patches, and map patches to terminal nodes:
	//
		m_pQT = new CQuadtree(&m_vMin, &m_vMax, m_nPatchesReqQuadTDepth) ;
		int nFoundPatches = 0 ;
		for (i = 0 ; i < (unsigned)m_pQT->m_nQTNodes ; i ++)
		{
			if (m_pQT->m_pQTNodes[i]->pKids[0] == NULL)
			{
			// terminal node found, so find corresponding patch;
				for (int y = 0 ; y < m_nPatchesSQ ; y ++)
					for (int x = 0 ; x < m_nPatchesSQ ; x ++)
					{
						if (m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_vMin.x == m_pQT->m_pQTNodes[i]->vMin.x && 
							m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_vMax.x == m_pQT->m_pQTNodes[i]->vMax.x && 
							m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_vMin.z == m_pQT->m_pQTNodes[i]->vMin.z && 
							m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_vMax.z == m_pQT->m_pQTNodes[i]->vMax.z)
						{
							nFoundPatches++ ;
							m_pQT->m_pQTNodes[i]->nX = x ;
							m_pQT->m_pQTNodes[i]->nY = y ;
							
						// neat; i like - refines terminal qtree node volumes v. cheaply
							m_pQT->m_pQTNodes[i]->vMax.y = m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_vMax.y ;
							m_pQT->m_pQTNodes[i]->vMin.y = m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_vMin.y ;
						}
					}
			}
		}
		assert( nFoundPatches == m_nPatchesSQ * m_nPatchesSQ );

	//
	// Build LERP helper:
	//
		m_pfLERPHelper = NULL ;
		m_pfLERPHelper = new float[m_nHeightmapSQ - 1] ;
		assert( m_pfLERPHelper );
		for (i = 0 ; i < m_nHeightmapSQ - 1 ; i ++)
			m_pfLERPHelper[i] = m_vMin.x + (float)i * m_fUnitSize ;
		m_dwMem += sizeof(float) * (m_nHeightmapSQ - 1) ;

	//
	// Distance-sorted array of gameitem pointers;
	//
		m_pisAllItemsDistanceSorted = NULL ;
	}
	CTerrain::~CTerrain()
	{
		for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				SAFE_DELETE( m_pPatches[TER_RC2ABS(x,y)] )
		
		SAFE_DELETE( m_pfHeightmap )
		SAFE_DELETE( m_pColormap )
		SAFE_DELETE_ARRAY( m_pfLERPHelper )
		SAFE_DELETE( m_pQT )

		SAFE_DELETE( m_bPatchesPicked )
		SAFE_DELETE( m_pPatches )
		SAFE_DELETE( m_nPatchesRequiredLOD )
		SAFE_DELETE( m_dsPatchesRequiredTopEdge )
		SAFE_DELETE( m_dsPatchesRequiredLeftEdge )
		SAFE_DELETE( m_dsPatchesRequiredBottomEdge )
		SAFE_DELETE( m_dsPatchesRequiredRightEdge )
		SAFE_DELETE( m_psPatches )

		SAFE_DELETE( m_pCherry )
		SAFE_DELETE( m_pShield )
		SAFE_DELETE( m_pWeapon )

	// just to be extra safe; maybe we exited in between FrameMove() and Render().
		if( m_pisAllItemsDistanceSorted )
			m_pisAllItemsDistanceSorted = NULL ;
	}
	DWORD CTerrain::QueryMemUsage()
	{
		DWORD dwMem = m_dwMem ;

		for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[TER_RC2ABS(x,y)])
					dwMem += m_pPatches[TER_RC2ABS(x,y)]->QueryMemUsage() ;

		return dwMem ;
	}

/*
 *
 * Blends the specified color onto the specified area of the colormap.
 *
 * Uses a random algo. to produce the most blending at the center of the area.
 *
 */
#define NDX(r,c)\
	(r)*m_nHeightmapSQ+(c)

	CTerrain::BlendColorSpot(unsigned Row, unsigned Col, int Radius, D3DXCOLOR *pColor)
	{
		float ix, iy, i ;
		for (int x = (int)Row - Radius / 2 ; x < (int)Row + Radius / 2 ; x ++)
		{
			ix = 1.0f - (float)abs(Row - x) / (Radius / 2.0f) ;
			for (int y = (int)Col - Radius / 2 ; y < (int)Col + Radius / 2 ; y ++)
			{
				if (y > (int)m_nHeightmapSQ - 1 || x > (int)m_nHeightmapSQ - 1 ||
					x < 0 || y < 0)
					continue ;

				iy = 1.0f - (float)abs(Col - y) / (Radius / 2.0f) ;
				i = ix * iy ;

				if (rnd() < ix && rnd() < iy)
				{
					m_pColormap[NDX(x, y)].r =
						(m_pColormap[NDX(x,y)].r * (1 - i + 1) + pColor->r * i) / 2.0f ;

					m_pColormap[NDX(x, y)].g =
						(m_pColormap[NDX(x,y)].g * (1 - i + 1) + pColor->g * i) / 2.0f ;

					m_pColormap[NDX(x, y)].b =
						(m_pColormap[NDX(x,y)].b * (1 - i + 1) + pColor->b * i) / 2.0f ;
				}
			}
		}
	}

/*
 *
 * Hermite-spline interpolation.
 *
 * Casts heightmap integer (row,col) values to xVec3 floats,
 * for fast operation; avoids lookups into the heightmap.
 *
 */
#define SPLINE_POINTS 16
#define SAFE_SINK(x,y,d)\
	if (x < (signed)m_nHeightmapSQ-1 && y < (signed)m_nHeightmapSQ-1 && x > 0 && y > 0)\
	{\
		m_pfHeightmap[ NDX( x,y ) ] -= ((float)m_fLenSQ / 1500.0f) * (d);	\
		if ( m_pfHeightmap[NDX(x,y)] < m_fMinY )							\
			m_fMinY = m_pfHeightmap[ NDX( x,y ) ];							\
	}

	CTerrain::BlendSplineTrench()
	{
		int i, j, x, y ;
		float s ;
		xVec2 vA[SPLINE_POINTS],
					vB[SPLINE_POINTS],
					tA[SPLINE_POINTS],
					tB[SPLINE_POINTS], vRes ;

	//
	// Define spline guide points:
	//
	// applies a rotation around the heightmap center (row,col) to produce
	// spline guide points lying on a circle.
	//
		float T = (float)m_nHeightmapSQ / 2.0f ;
		xMat matRot, m0, m1, m2 ;
		xVec2 vStart = xVec2((float)(m_nHeightmapSQ/2), (float)(m_nHeightmapSQ/8)) ;
		xVec2 vCenter = xVec2((float)(m_nHeightmapSQ/2), (float)(m_nHeightmapSQ/2)) ;

		D3DXMatrixTranslation(&m0, -(float)(m_nHeightmapSQ/2), -(float)(m_nHeightmapSQ/2), 0.0f) ;
		D3DXMatrixRotationZ(&m1, g_PI * 2.0f / (float)SPLINE_POINTS) ;
		D3DXMatrixTranslation(&m2, +(float)(m_nHeightmapSQ/2), +(float)(m_nHeightmapSQ/2), 0.0f) ;
		D3DXMatrixMultiply(&matRot, &m0, &m1) ;
		D3DXMatrixMultiply(&matRot, &matRot, &m2) ;

		for (i = 0 ; i < SPLINE_POINTS ; i ++)
		{
			if (i == 0)
			{
				D3DXVec2TransformCoord(&vStart, &vStart, &matRot) ;
				vA[0] = vStart ;
				D3DXVec2TransformCoord(&vStart, &vStart, &matRot) ;
				vB[0] = vStart ;

				tA[0] = xVec2(rnd() * T, rnd() * T) ;
				tB[0] = xVec2(rnd() * T, rnd() * T) ;
			}
			else
			{
				if (i == SPLINE_POINTS-1)
				{
					vA[i] = vB[i-1] ;
					vB[i] = vA[0] ;
					tA[i] = tB[i-1] ;
					tB[i] = tA[0] ;
				}
				else
				{
					vA[i] = vB[i-1] ;
					D3DXVec2TransformCoord(&vStart, &vStart, &matRot) ;
					vB[i] = vStart ;

					tA[i] = tB[i-1] ;
					tB[i] = xVec2(rnd() * T, rnd() * T) ;
				}
			}
		}

	//
	// Interpolate between spline guide points:
	//
		for (j = 0 ; j < SPLINE_POINTS ; j ++)
		{
			int nStepsRequired ;
			nStepsRequired = (int)fabsf(D3DXVec2Length(&(vA[j] - vB[j]))) * 10 ;

			for (i = 0 ; i < nStepsRequired ; i ++)
			{
				s = (float)i / (float)(nStepsRequired - 1) ;
				D3DXVec2Hermite(&vRes, &vA[j], &tA[j], &vB[j], &tB[j], s) ;

			// set the interpolated to a lower depth point
				x = (int)vRes.x ;
				y = (int)vRes.y ;

				int nScatter = (int)(m_fLenSQ / 10.0f / m_fUnitSize) ;
				for (int _x = x - (nScatter/2) ; _x < x + (nScatter/2) ; _x ++)
				{
					float ix = 1.0f - fabsf((float)x-_x)/(float)(nScatter/2) ;
					for (int _y = y - (nScatter/2) ; _y < y + (nScatter/2) ; _y ++)
					{
						float iy = 1.0f - fabsf((float)y-_y)/(float)(nScatter/2) ;
						SAFE_SINK( _x,_y, (ix*iy) )
					}
				}
			}
		}
	}

/*
 *
 * Linear interpolation to get minimal Y value in the heightmap for supplied coords.
 *
 */
	void CTerrain::LERPdY(float x, float z, float *pfY, BOOL *pbOverTerrain, xVec3 *pvOut)
	{
		unsigned i, ndxFloorRow, ndxFloorCol, ndxA, ndxB, ndxC, ndxD ;
		xVec3 vA, vB, vC, vD, vP, v, v0, v1, v2 ;

		ndxFloorRow = -1 ;
		ndxFloorCol = -1 ;

	// check the coords. are over the terrain at all
		*pfY = 0.0f ;
		*pbOverTerrain = FALSE ;
		if (x < m_vMin.x) return ;
		if (z < m_vMin.z) return ;
		if (x > m_vMax.x) return ;
		if (z > m_vMax.z) return ;
		*pbOverTerrain = TRUE ;

	// find the minimal bounding row and col
		/*for (i = 0 ; i < m_nHeightmapSQ - 1 ; i ++)
		{
			if (m_pfLERPHelper[i] + m_fUnitSize > x)
			{
				ndxFloorCol = i ;
				break ;
			}
		}
		for (i = 0 ; i < m_nHeightmapSQ - 1 ; i ++)
		{
			if (m_pfLERPHelper[i] + m_fUnitSize > z)
			{
				ndxFloorRow = i ;
				break ;
			}
		}*/
		for (i = 0 ; i < m_nHeightmapSQ - 1 ; i ++)
		{
			if (m_pfLERPHelper[i] + m_fUnitSize > x && ndxFloorCol == -1)
				ndxFloorCol = i ;
			if (m_pfLERPHelper[i] + m_fUnitSize > z && ndxFloorRow == -1)
				ndxFloorRow = i ;
			if (ndxFloorRow != -1 && ndxFloorCol != -1)
				break ;
		}
		
	// derive four bounding points
	// A B
	// C D
		ndxA = ndxFloorRow * m_nHeightmapSQ + ndxFloorCol ;
		ndxB = ndxFloorRow * m_nHeightmapSQ + (ndxFloorCol + 1) ;
		ndxC = (ndxFloorRow + 1) * m_nHeightmapSQ + ndxFloorCol ;
		ndxD = (ndxFloorRow + 1) * m_nHeightmapSQ + (ndxFloorCol + 1) ;
		
		vA = xVec3(m_vMin.x + ndxFloorCol * m_fUnitSize, m_pfHeightmap[ndxA], m_vMin.z + ndxFloorRow * m_fUnitSize) ;
		vB = xVec3(m_vMin.x + ndxFloorCol * m_fUnitSize, m_pfHeightmap[ndxB], m_vMin.z + (ndxFloorRow + 1) * m_fUnitSize) ;
		vC = xVec3(m_vMin.x + (ndxFloorCol + 1) * m_fUnitSize, m_pfHeightmap[ndxC], m_vMin.z + ndxFloorRow * m_fUnitSize) ;
		vD = xVec3(m_vMin.x + (ndxFloorCol + 1) * m_fUnitSize, m_pfHeightmap[ndxD], m_vMin.z + (ndxFloorRow + 1) * m_fUnitSize) ;

		vP = xVec3(x, 0.0f, z) ;
		
	// Klaus Hartmann strikes again 
		float tx = (vP.x - vA.x) / (vD.x - vA.x) ;
		float tz = (vP.z - vA.z) / (vD.z - vA.z) ;
		vP.y = (1 - tx) * (1 - tz) * vA.y
				+ tx * (1 - tz) * vB.y
				+ (1 - tx) * tz * vC.y
				+ tx * tz * vD.y ;
		*pfY = vP.y ;	// interpolated y between four points
	
	// return four bounding vectors
		if (pvOut)
		{
			pvOut[0] = vA ;
			pvOut[1] = vB ;
			pvOut[2] = vC ;
			pvOut[3] = vD ;
		}
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT CTerrain::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
#ifdef FULL_DBG_TRACE
		ods( "CTerrain::RestoreDeviceObjects\n" );
#endif

		HRESULT hr = S_OK ;
		
	// create cherry mesh
		if( m_pCherry )
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pCherry") ;
			if (FAILED(m_pCherry->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;

			ods( "Cherry center: %0.1f, %0.1f, %0.1f\n", m_pCherry->m_pBBox->m_vCenter.x, m_pCherry->m_pBBox->m_vCenter.y, m_pCherry->m_pBBox->m_vCenter.z );
			m_pCherry->SetLargestSide( 1.0f );
		}

	// create shield mesh
		if( m_pShield )
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pShield") ;
			if (FAILED(m_pShield->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;

			ods( "Shield center: %0.1f, %0.1f, %0.1f\n", m_pShield->m_pBBox->m_vCenter.x, m_pShield->m_pBBox->m_vCenter.y, m_pShield->m_pBBox->m_vCenter.z );
			m_pShield->SetLargestSide( 1.0f );
		}
		
	// create weapon mesh
		if( m_pWeapon )
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pWeapon") ;
			if (FAILED(m_pWeapon->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;

			ods( "Weapon center: %0.1f, %0.1f, %0.1f\n", m_pWeapon->m_pBBox->m_vCenter.x, m_pWeapon->m_pBBox->m_vCenter.y, m_pWeapon->m_pBBox->m_vCenter.z );
			m_pWeapon->SetLargestSide( 1.0f );
		}

	// create health mesh
		if( m_pHealth )
		{
			strcpy(m_pX7->m_szLoadingGameInfo1, "m_pHealth") ;
			if (FAILED(m_pHealth->RestoreDeviceObjects(pD3D, pDev))) return E_FAIL ;

			ods( "Health center: %0.1f, %0.1f, %0.1f\n", m_pHealth->m_pBBox->m_vCenter.x, m_pHealth->m_pBBox->m_vCenter.y, m_pHealth->m_pBBox->m_vCenter.z );
			m_pHealth->SetLargestSide( 1.0f );
		}

	// create terrain patch VBs
		for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[TER_RC2ABS(x,y)])
				{
					sprintf(m_pX7->m_szLoadingGameInfo2, "Patch %d [of %d]", (m_nPatchesSQ * x + y) + 1, m_nPatchesSQ * m_nPatchesSQ) ;
					if (FAILED(m_pPatches[TER_RC2ABS(x,y)]->RestoreDeviceObjects(pD3D, pDev)))
						return E_FAIL ;
				}

	// create quadtree VB and IB for debug rendering
		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pQT") ;
		m_pQT->RestoreDeviceObjects(pD3D, pDev) ;

	// create unit spherical mesh
		strcpy(m_pX7->m_szLoadingGameInfo1, "m_pItemSphereMesh") ;
		hr =  D3DXCreateSphere( pDev,
							    1.0f,
								20,
								20,
								&m_pItemSphereMesh,
								NULL );
		assert( D3D_OK==hr );

		m_pX7->m_szLoadingGameInfo2[0] = '\x0' ;
		return hr ;
	}
	HRESULT	CTerrain::InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		if( m_pCherry )
			m_pCherry->InvalidateDeviceObjects( pDev );
		if( m_pShield )
			m_pShield->InvalidateDeviceObjects( pDev );
		if( m_pWeapon )
			m_pWeapon->InvalidateDeviceObjects( pDev );
		if( m_pHealth )
			m_pHealth->InvalidateDeviceObjects( pDev );

		for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[TER_RC2ABS(x,y)])
					m_pPatches[TER_RC2ABS(x,y)]->InvalidateDeviceObjects() ;

		m_pQT->InvalidateDeviceObjects() ;

		SAFE_RELEASE( m_pItemSphereMesh )

		return S_OK ;
	}

	CTerrain::DisableItemLights( LPDIRECT3DDEVICE8 pDev, int nLightsToSpendOnItems, int nItemLightsStartIndex )
	{
	// disable item lights
		for( int i=0 ; i < nLightsToSpendOnItems ; i++ )
			pDev->LightEnable( nItemLightsStartIndex+i, FALSE );
	}

	HRESULT CTerrain::Render(LPDIRECT3DDEVICE8 pDev, CTextures* pTex, unsigned long *pulPolyCount, CCamera *pCamera, xMatUA *pmatCurWorld, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, float fFPS )
	{
		xMat matTex ;
		int x, y ;

	// get a LOD for each patch
		DetermineRequiredLOD( pCamera );

	// camera ray picking of intersected patches:
		/*for (x = 0 ; x < m_nPatchesSQ ; x ++)
			for (y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[TER_RC2ABS(x,y)])
					m_bPatchesPicked[TER_RC2ABS(x,y)]
					= m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[m_nPatchesRequiredLOD[TER_RC2ABS(x,y)]]->m_pBBox->LineIntersects(&(pCamera->m_vEyePt), &(pCamera->m_vLookAtPt), 100000.0f) ;*/

	// render patches;
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		//pDev->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		for( x = 0 ; x < m_nPatchesSQ ; x++ )
			for( y = 0 ; y < m_nPatchesSQ ; y++ )
				if( m_pPatches[ TER_RC2ABS(x,y) ] )
					if( m_pPatches[ TER_RC2ABS(x,y) ]->m_pPatchLOD[0]->m_bVisible )
					{
					// render patch;
						m_pPatches[ TER_RC2ABS(x,y) ]->Render( pDev,
															   pTex,
															   pulPolyCount,
															   m_nPatchesRequiredLOD[ TER_RC2ABS(x,y) ]/*m_nCurLOD*/,
															   m_dsPatchesRequiredTopEdge[ TER_RC2ABS(x,y) ],
															   m_dsPatchesRequiredLeftEdge[ TER_RC2ABS(x,y) ],
															   m_dsPatchesRequiredBottomEdge[ TER_RC2ABS(x,y) ],
															   m_dsPatchesRequiredRightEdge[ TER_RC2ABS(x,y) ],
															   pmatCurWorld );
					}
		//pDev->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );

		//m_pQT->Render(pDev, pulPolyCount, pmatCurWorld, m_nPatchesReqQuadTDepth) ;



	// render distance-sorted gameitems, furthest to nearest for correct alpha;
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

		assert( m_pisAllItemsDistanceSorted );
		int nItemsPVS = 0, nItemsDrawn = 0 ;
		for ( x=m_nItemsDistanceSorted-1 ; x>=0 ; x-- )
			if( m_pisAllItemsDistanceSorted[ x ].bVisible )
			{
				nItemsPVS++ ;
				nItemsDrawn += RenderItem( pDev, pTex, pulPolyCount, pmatCurWorld, m_pisAllItemsDistanceSorted[ x ].pItem, pmatProj, pVP, pCamera, fFPS, m_pisAllItemsDistanceSorted[ x ].d );
			}

		matTex = g_matIdent ;
		pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		//ods( "%d gameitems in PVS, %d rendered.\n", nItemsPVS, nItemsDrawn );

	// render bboxs of camera picked patches;
		/*for (x = 0 ; x < m_nPatchesSQ ; x ++)
			for (y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[TER_RC2ABS(x,y)])
					if (m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_bVisible)
						if( m_bPatchesPicked[TER_RC2ABS(x,y)] )
							m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[m_nPatchesRequiredLOD[TER_RC2ABS(x,y)]]->m_pBBox->Render(pDev, pulPolyCount,  &m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[m_nPatchesRequiredLOD[TER_RC2ABS(x,y)]]->m_matWorld) ;*/

	// deallocate list of distance-sorted gameitems;
		free( m_pisAllItemsDistanceSorted );
		m_pisAllItemsDistanceSorted = NULL ;
		return S_OK ;
	}

	int SortGameitemDistancesCallback(const VOID* arg1, const VOID* arg2)
	{
		PATCH_SORT* p1 = (PATCH_SORT*)arg1 ;
		PATCH_SORT* p2 = (PATCH_SORT*)arg2 ;

		if (p1->d > p2->d) return +1 ;
		if (p1->d < p2->d) return -1 ;

		return 0 ;
	}

/*
 *
 * Determine visible patches, and of those, closest n items.
 *
 */
	HRESULT CTerrain::FrameMove(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP)
	{
	//
	// Terrain patches, bbox/frustrum cull
	//
		DetermineVisibility( pDev, pCamera, pmatProj, pVP );

	// no need for this; FM in TerrainPatch does nothing.
		/*for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[TER_RC2ABS(x,y)])
					m_pPatches[TER_RC2ABS(x,y)]->FrameMove(pDev, pInput, fFPS) ;*/

	//
	// Toggle LOD and rotate patches:
	//
		/*static float fLODCounter = 0 ;
		fLODCounter+=(1.0f/fFPS) ;
		if (fLODCounter>1)
		{
			fLODCounter=0;
			if (++m_nCurLOD > PATCH_MAX_LOD_LEVELS-1)
				m_nCurLOD = 0 ;
		}*/

		/*for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				if (m_pPatches[x][y])
					for (int i = 0 ; i < PATCH_MAX_LOD_LEVELS ; i ++)
						if (m_pPatches[x][y]->m_pPatchLOD[i])
							m_pPatches[x][y]->m_pPatchLOD[i]->WorldRotateY(g_PI / 800 * (60 / fFPS), &(xVec3(0, 0, 0))) ;*/
		
		return S_OK ;
	}

/*
 *
 * Per-frame gameitem distance-sorting and light determination:
 *
 * NOTE: m_pisAllItemsDistanceSorted is allocated here, and deallocated in Render(). 
 *
 */
	CTerrain::DistanceSortItemsAndSetLights( LPDIRECT3DDEVICE8 pDev, int nLights, int nStartLightIndex, CCamera* pCam )
	{
		int x, y, i, nLightsSet = 0 ;
		float fAbsTime = DXUtil_Timer( TIMER_GETABSOLUTETIME );
		
		assert( nLights > 0 );
		m_nItemsDistanceSorted = 0 ;

	// allocate a sort-buffer:
	// TODO: this is counting all items, all the time, we could do this faster;
		for( x=0 ; x < m_nPatchesSQ ; x++ )
			for( y=0 ; y < m_nPatchesSQ ; y++ )
				if( m_pPatches[ TER_RC2ABS(x,y) ] )
					m_nItemsDistanceSorted += (
						  m_pPatches[ TER_RC2ABS(x,y) ]->m_nCherries 
						+ m_pPatches[ TER_RC2ABS(x,y) ]->m_nShieldPwrUps
						+ m_pPatches[ TER_RC2ABS(x,y) ]->m_nEnginePwrUps
						+ m_pPatches[ TER_RC2ABS(x,y) ]->m_nWeaponPwrUps
						+ m_pPatches[ TER_RC2ABS(x,y) ]->m_nHealthPwrUps
										) ;
		m_pisAllItemsDistanceSorted = (ITEM_SORT*)malloc( sizeof(ITEM_SORT) * m_nItemsDistanceSorted );
		assert( m_pisAllItemsDistanceSorted );

	// fill the sort-buffer:
		m_nItemsDistanceSorted = 0 ;
		for( x=0 ; x < m_nPatchesSQ ; x++ )
			for( y=0 ; y < m_nPatchesSQ ; y++ )
				if( m_pPatches[ TER_RC2ABS(x,y) ] )
					for( i=0 ; i < m_pPatches[ TER_RC2ABS(x,y) ]->m_nItems ; i++ )
					{
						xVec3 vD = pCam->m_vEyePt - m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].vPos ;
						float d = D3DXVec3Length(&vD) ;
						m_pisAllItemsDistanceSorted[ m_nItemsDistanceSorted ].pItem = &m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ] ;
						m_pisAllItemsDistanceSorted[ m_nItemsDistanceSorted ].d = d ;
						m_pisAllItemsDistanceSorted[ m_nItemsDistanceSorted ].nType = m_pPatches[TER_RC2ABS(x,y)]->m_pItems[ i ].nType ;
						m_pisAllItemsDistanceSorted[ m_nItemsDistanceSorted ].bVisible = m_pPatches[ TER_RC2ABS(x,y) ]->m_pPatchLOD[0]->m_bVisible ;
						m_nItemsDistanceSorted++ ;
					}

	// QuickSort items based on distance from camera
		qsort( m_pisAllItemsDistanceSorted, m_nItemsDistanceSorted, sizeof( ITEM_SORT ), SortItemDistancesCallback );
		
	// retrieve closest items and set lights
		for( i=0 ; i < m_nItemsDistanceSorted && nLightsSet < nLights ; i++ )
		{
			GAMEITEM* pItem = m_pisAllItemsDistanceSorted[ i ].pItem ;
			D3DLIGHT8 litItem ;

			float fItemAbsRadius = pItem->fRadius * ( m_fLenSQ / 160.0f );
			xVec3 vItemAbsPos = pItem->vPos + xVec3( 0, fItemAbsRadius, 0 );

		// setup & position the light
			switch( pItem->nType )
			{
				case IT_CHERRY:
					litItem = m_litCherry ;
					litItem.Position = vItemAbsPos - xVec3( 0, fItemAbsRadius * 0.2f, 0.0f );
					break ;
				case IT_SHIELD:
					litItem = m_litShield ;
					litItem.Position = vItemAbsPos + xVec3( 0, fItemAbsRadius * 1, 0 );
					break ;
				case IT_ENGINE:
					litItem = m_litEngine ;
					litItem.Position = vItemAbsPos + xVec3( 0, fItemAbsRadius * 1, 0 );
					break ;
				case IT_WEAPON:
					litItem = m_litWeapon ;
					litItem.Position = vItemAbsPos + xVec3( 0, fItemAbsRadius * 1, 0 );
				case IT_HEALTH:
					litItem = m_litHealth ;
					litItem.Position = vItemAbsPos + xVec3( 0, fItemAbsRadius * 1, 0 );
					break ;
			}

		// modulate the light's brightness
			if( pItem->fShineModSpeed > 0 )
			{
				pItem->fShine = 0.05f + 0.95f * (float)((sin((double)fAbsTime * 500 * pItem->fShineModSpeed * g_DEGTORAD) + 1.0f) / 2.0f) ;
			}
			//if( pItem->fShine != 1 )
			//	ods("%0.1f\n", pItem->fShine );

			litItem.Diffuse.r *= pItem->fShine ;
			litItem.Diffuse.g *= pItem->fShine ;
			litItem.Diffuse.b *= pItem->fShine ;
			litItem.Ambient.r *= pItem->fShine ;
			litItem.Ambient.g *= pItem->fShine ;
			litItem.Ambient.b *= pItem->fShine ;

		// let there be light
			pDev->SetLight( nStartLightIndex+nLightsSet, &litItem );
			pDev->LightEnable( nStartLightIndex+nLightsSet, TRUE );
			nLightsSet++ ;
		}

		//ods("DS&SIL - %d lights asked for, %d(all) items sorted, %d lights set.\n", nLights, m_nItemsDistanceSorted, nLightsSet) ;
	}

	int SortItemDistancesCallback( const VOID* arg1, const VOID* arg2 )
	{
		ITEM_SORT* i1 = (ITEM_SORT*)arg1 ;
		ITEM_SORT* i2 = (ITEM_SORT*)arg2 ;

		if (i1->d > i2->d) return +1 ;
		if (i1->d < i2->d) return -1 ;

		return 0 ;
	}

/*
 *
 * Per-frame LOD determination:
 *
 */
	CTerrain::DetermineRequiredLOD(CCamera *pCam)
	{
		signed int ndx = 0, x, y ;

	// get patch distances from camera:
		for (x = 0 ; x < m_nPatchesSQ ; x ++)
			for (y = 0 ; y < m_nPatchesSQ ; y ++)
			{
				float d ;
				xVec3 vD = pCam->m_vEyePt - m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_pBBox->m_vCenter ;
				d = D3DXVec3Length(&vD) ;

				m_psPatches[ndx].x = x ;
				m_psPatches[ndx].y = y ;
				m_psPatches[ndx].d = d ;
				ndx++ ;

			// default patch to minimal LOD;
				m_nPatchesRequiredLOD[TER_RC2ABS(x,y)] = m_nPatchMaxLODLevels - 1 ;
			}

	// QuickSort patches on distance from camera:
		qsort(m_psPatches, m_nPatchesSQ * m_nPatchesSQ, sizeof(PATCH_SORT), SortPatchDistancesCallback) ;

	//
	// Now we can prioritise exactly how many polygons to spend per patch:
	//
	// Get nearest patch, and set it AND its EIGHT adjacent neighbours to maximum LOD!
	// Set subsequent patches to ever decreasing LODs until we reach min. LOD.
	//
		int _x, _y, nCurLOD = 0, nRadialNo = 1 ;

	// get nearest patch and set it to max. LOD
		x = m_psPatches[0].x ;
		y = m_psPatches[0].y ;
		m_nPatchesRequiredLOD[TER_RC2ABS(x,y)] = nCurLOD ;

	// radiate out and set to decreasing LOD until we reach minimum LOD
		while (nCurLOD < m_nPatchMaxLODLevels)
		{
			for (_y = y - nRadialNo, _x = x - nRadialNo ; _y <= y + nRadialNo ; _y ++)
				if (_x >= 0 && _x < m_nPatchesSQ && _y >= 0 && _y < m_nPatchesSQ)
					m_nPatchesRequiredLOD/*[_x][_y]*/[TER_RC2ABS(_x,_y)] = nCurLOD ;
			for (_y = y - nRadialNo, _x = x + nRadialNo ; _y <= y + nRadialNo ; _y ++)
				if (_x >= 0 && _x < m_nPatchesSQ && _y >= 0 && _y < m_nPatchesSQ)
					m_nPatchesRequiredLOD/*[_x][_y]*/[TER_RC2ABS(_x,_y)] = nCurLOD ;

			for (_x = x - (nRadialNo-1), _y = y - nRadialNo ; _x <= x + (nRadialNo-1) ; _x ++)
				if (_x >= 0 && _x < m_nPatchesSQ && _y >= 0 && _y < m_nPatchesSQ)
					m_nPatchesRequiredLOD/*[_x][_y]*/[TER_RC2ABS(_x,_y)] = nCurLOD ;
			for (_x = x - (nRadialNo-1), _y = y + nRadialNo ; _x <= x + (nRadialNo-1) ; _x ++)
				if (_x >= 0 && _x < m_nPatchesSQ && _y >= 0 && _y < m_nPatchesSQ)
					m_nPatchesRequiredLOD/*[_x][_y]*/[TER_RC2ABS(_x,_y)] = nCurLOD ;
			
			nCurLOD++ ;
			nRadialNo++ ;
		}

	//
	// For each patch at LOD > 0, determine which edge strips to use,
	// either LOD or LOD+1, in order to tesselate correctly.
	//
		for (x = 0 ; x < m_nPatchesSQ ; x ++)
			for (y = 0 ; y < m_nPatchesSQ ; y ++)
			{
				int nThisLOD = m_nPatchesRequiredLOD[TER_RC2ABS(x,y)] ;
			
			// forget about max. LOD patches: we render them in one batch
				if (nThisLOD == 0)
					continue ;

			// default all edge strips to current LOD
				m_dsPatchesRequiredTopEdge[TER_RC2ABS(x,y)] = DS_TOP_EDGE ;
				m_dsPatchesRequiredLeftEdge[TER_RC2ABS(x,y)] = DS_LEFT_EDGE ;
				m_dsPatchesRequiredBottomEdge[TER_RC2ABS(x,y)] = DS_BOTTOM_EDGE ;
				m_dsPatchesRequiredRightEdge[TER_RC2ABS(x,y)] = DS_RIGHT_EDGE ;

			// is our ABOVE neighbour at a greater LOD?
				if (y > 0)
					if (m_nPatchesRequiredLOD/*[x][y-1]*/[TER_RC2ABS(x,y-1)] == nThisLOD - 1)
						m_dsPatchesRequiredTopEdge[TER_RC2ABS(x,y)] = DS_TOP_EDGE_LODPLUS1 ;

			// is our LEFT neighbour at a greater LOD?
				if (x > 0)
					if (m_nPatchesRequiredLOD/*[x-1][y]*/[TER_RC2ABS(x-1,y)] == nThisLOD - 1)
						m_dsPatchesRequiredLeftEdge[TER_RC2ABS(x,y)] = DS_LEFT_EDGE_LODPLUS1 ;

			// is our BOTTOM neighbour at a greater LOD?
				if (y < m_nPatchesSQ-1)
					if (m_nPatchesRequiredLOD/*[x][y+1]*/[TER_RC2ABS(x,y+1)] == nThisLOD - 1)
						m_dsPatchesRequiredBottomEdge[TER_RC2ABS(x,y)] = DS_BOTTOM_EDGE_LODPLUS1 ;

			// is our RIGHT neighbour at a greater LOD?
				if (x < m_nPatchesSQ-1)
					if (m_nPatchesRequiredLOD/*[x+1][y]*/[TER_RC2ABS(x+1,y)] == nThisLOD - 1)
						m_dsPatchesRequiredRightEdge[TER_RC2ABS(x,y)] = DS_RIGHT_EDGE_LODPLUS1 ;
			}
	}

	int SortPatchDistancesCallback(const VOID* arg1, const VOID* arg2)
	{
		PATCH_SORT* p1 = (PATCH_SORT*)arg1 ;
		PATCH_SORT* p2 = (PATCH_SORT*)arg2 ;

		if (p1->d > p2->d) return +1 ;
		if (p1->d < p2->d) return -1 ;

		return 0 ;
	}

/*
 *
 * Per-frame visibility determination:
 *
 */
	CTerrain::DetermineVisibility(LPDIRECT3DDEVICE8 pDev, CCamera *pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP)
	{
		m_nVisiblePatches = 0 ;

	// recurse quadtree
		for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
				m_pPatches[TER_RC2ABS(x,y)]->m_pPatchLOD[0]->m_bVisible = FALSE ;

		DetermineVisibilityForQuadtreeNode(m_pQT->m_pQT, pDev, pCam, pmatProj, pVP) ;

	// brute-force method
		/*for (int x = 0 ; x < m_nPatchesSQ ; x ++)
			for (int y = 0 ; y < m_nPatchesSQ ; y ++)
			{
				m_pPatches[x][y]->m_pPatchLOD[0]->GetBBoxVisibilityFlags(pDev, pmatProj, pCam, pVP) ;
				if (m_pPatches[x][y]->m_pPatchLOD[0]->m_bVisible)
					m_nVisiblePatches++ ;
			}*/
	}
	BOOL CTerrain::DetermineVisibilityForQuadtreeNode(CQuadtree::_QTNode *pQTNode, LPDIRECT3DDEVICE8 pDev, CCamera *pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP)
	{
	// is this node visible?
		if (!GetBBoxVisibility(pDev, *pmatProj, pCam->m_matView, pVP, pQTNode->vMin, pQTNode->vMax))
			return FALSE ;

	// mark visible terminal nodes
		if (pQTNode->pKids[0] == NULL)
		{
			m_pPatches/*[pQTNode->nX][pQTNode->nY]*/[TER_RC2ABS(pQTNode->nX,pQTNode->nY)]->m_pPatchLOD[0]->m_bVisible = TRUE ;
			m_nVisiblePatches++ ;
			return TRUE ;
		}

	// recurse visible non-terminal nodes
		for (int i = 0 ; i < 4 ; i ++)
			DetermineVisibilityForQuadtreeNode((CQuadtree::_QTNode*)pQTNode->pKids[i], pDev, pCam, pmatProj, pVP) ;

		return TRUE ;
	}

/*
 *
 * Renders game special items
 *
 */
	int CTerrain::RenderItem( LPDIRECT3DDEVICE8 pDev, CTextures* pTex, unsigned long *pulPolyCount, xMatUA* pmatCurWorld, GAMEITEM* pItem, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, CCamera* pCam, float fFPS, float d )
	{
		xMat matRot, matScale, matTrans, matFinal, m0, m1, m2, matTex ;
		float fItemAbsRadius = pItem->fRadius * ( m_fLenSQ / 160.0f ) * ( 1 + ( pItem->fShine / 2 ) );
		xVec3 vItemAbsPos = pItem->vPos + xVec3( 0, fItemAbsRadius, 0 );
		HRESULT hr ;

	// cull frustrum against the bbox of the bsphere;
		BOOL bVisible = GetBBoxVisibility( pDev, *pmatProj, pCam->m_matView, pVP,
										     (vItemAbsPos - xVec3(fItemAbsRadius, fItemAbsRadius, fItemAbsRadius)),
										     (vItemAbsPos + xVec3(fItemAbsRadius, fItemAbsRadius, fItemAbsRadius)) );
		if( !bVisible )
			return 0 ;

		pDev->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pDev->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

		SetTextureColorStage( pDev, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1,	D3DTA_CURRENT )
		SetTextureColorStage( pDev, 1, D3DTA_DIFFUSE, D3DTOP_MODULATE,		D3DTA_CURRENT )
		SetTextureColorStage( pDev, 2, D3DTA_CURRENT, D3DTOP_DISABLE,		D3DTA_CURRENT )
		SetTextureAlphaStage( pDev, 0, D3DTA_DIFFUSE, D3DTOP_SELECTARG1,	D3DTA_CURRENT )

	// ask for auto-gen'd tex. coords;
		pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

	// render;
//ods("1");
		switch( pItem->nType )
		{
			case IT_CHERRY:
			// draw cherry - already scaled to unit size; apply additional rotation, scale then position matrices
				pItem->fRot += (fFPS / 60.0f) * g_PI * 0.005f ;
				D3DXMatrixRotationY( &matRot, pItem->fRot );
				D3DXMatrixMultiply( &matFinal, &matRot, &m_pCherry->m_matScale );
				
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matScale );
				
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				m_pCherry->WorldSetMatrix( &matFinal, m_pCherry->m_fScale * fItemAbsRadius );
				pDev->SetMaterial( &m_mtrlCherry );
				pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+0 ] );
				m_pCherry->Render( pDev, pTex, pulPolyCount, FALSE, FALSE, pmatCurWorld );

			// draw a pretty sphere
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius);
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matTrans );
				D3DXMatrixMultiply( &matFinal, &matFinal, pmatCurWorld );
				pDev->SetTransform( D3DTS_WORLD, &matFinal );
				pDev->SetMaterial( &m_mtrlItemSphere );
				pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+1 ] );

					D3DXMatrixTranslation( &matTex, 0, pItem->fRot, 0 );	
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

				hr = m_pItemSphereMesh->DrawSubset( 0 );
				*pulPolyCount += m_pItemSphereMesh->GetNumFaces() ;
				assert( D3D_OK==hr );
				break ;
		 
			case IT_SHIELD:
			// draw shield - already scaled to unit size; apply additional rotation, scale then position matrices
				/*pItem->fRot += (fFPS / 60.0f) * g_PI * 0.005f ;
				D3DXMatrixRotationY( &matRot, pItem->fRot );
				D3DXMatrixMultiply( &matFinal, &matRot, &m_pShield->m_matScale );
				
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matScale );
				
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				m_pShield->WorldSetMatrix( &matFinal, m_pShield->m_fScale * fItemAbsRadius );
				//pDev->SetMaterial( &m_pShield );
				//pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+0 ] );
				pDev->SetTexture( 0, NULL );
				m_pShield->Render( pDev, pTex, pulPolyCount, TRUE, FALSE, pmatCurWorld );*/

			// draw a pretty sphere
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matTrans );
				D3DXMatrixMultiply( &matFinal, &matFinal, pmatCurWorld );
				pDev->SetTransform( D3DTS_WORLD, &matFinal );
				pDev->SetMaterial( &m_mtrlShield );
				pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+2 ] );

					D3DXMatrixRotationX( &m0, pItem->fX1 );
					D3DXMatrixRotationY( &m1, pItem->fX1 );
					D3DXMatrixRotationZ( &m2, pItem->fX1 );
					D3DXMatrixMultiply( &matTex, &m0, &m1 );
					D3DXMatrixMultiply( &matTex, &matTex, &m2 );
					pItem->fX1 += (0.01f) * (60.0f/fFPS) ;
					pItem->fY1 += (0.02f) * (60.0f/fFPS) ;
					pItem->fZ1 += (0.03f) * (60.0f/fFPS) ;
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

				hr = m_pItemSphereMesh->DrawSubset( 0 );
				*pulPolyCount += m_pItemSphereMesh->GetNumFaces() ;
				assert( D3D_OK==hr );
				break ;

			case IT_ENGINE:
			// draw a pretty sphere
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matTrans );
				D3DXMatrixMultiply( &matFinal, &matFinal, pmatCurWorld );
				pDev->SetTransform( D3DTS_WORLD, &matFinal );
				pDev->SetMaterial( &m_mtrlEngine );
				pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+2 ] );

					D3DXMatrixRotationX( &m0, pItem->fX1 );
					D3DXMatrixRotationY( &m1, pItem->fY1 );
					D3DXMatrixRotationZ( &m2, pItem->fZ1 );
					D3DXMatrixMultiply( &matTex, &m0, &m1 );
					D3DXMatrixMultiply( &matTex, &matTex, &m2 );
					pItem->fX1 += (0.01f) * (60.0f/fFPS) ;
					pItem->fY1 += (0.02f) * (60.0f/fFPS) ;
					pItem->fZ1 += (0.03f) * (60.0f/fFPS) ;
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

				hr = m_pItemSphereMesh->DrawSubset( 0 );
				*pulPolyCount += m_pItemSphereMesh->GetNumFaces() ;
				assert( D3D_OK==hr );
				break ;

			case IT_WEAPON:
			// draw weapon - already scaled to unit size; apply additional rotation, scale then position matrices
				/*pItem->fRot += (fFPS / 60.0f) * g_PI * 0.005f ;
				D3DXMatrixRotationY( &matRot, pItem->fRot );
				D3DXMatrixMultiply( &matFinal, &matRot, &m_pWeapon->m_matScale );
				
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matScale );
				
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				m_pWeapon->WorldSetMatrix( &matFinal, m_pCherry->m_fScale * fItemAbsRadius );
				//pDev->SetMaterial( &m_mtrlWeapon );
				//pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+0 ] );
				m_pWeapon->Render( pDev, pTex, pulPolyCount, TRUE, FALSE, pmatCurWorld );*/

			// draw a pretty sphere
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matTrans );
				D3DXMatrixMultiply( &matFinal, &matFinal, pmatCurWorld );
				pDev->SetTransform( D3DTS_WORLD, &matFinal );
				pDev->SetMaterial( &m_mtrlWeapon );
				pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+2 ] );

					D3DXMatrixRotationX( &m0, pItem->fX1 );
					D3DXMatrixRotationY( &m1, pItem->fY1 );
					D3DXMatrixRotationZ( &m2, pItem->fZ1 );
					D3DXMatrixMultiply( &matTex, &m0, &m1 );
					D3DXMatrixMultiply( &matTex, &matTex, &m2 );
					pItem->fX1 += (0.03f) * (60.0f/fFPS) ;
					pItem->fY1 += (0.02f) * (60.0f/fFPS) ;
					pItem->fZ1 += (0.01f) * (60.0f/fFPS) ;
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

				hr = m_pItemSphereMesh->DrawSubset( 0 );
				*pulPolyCount += m_pItemSphereMesh->GetNumFaces() ;
				assert( D3D_OK==hr );

			case IT_HEALTH:
			// draw heart - already scaled to unit size; apply additional rotation, scale then position matrices
				pItem->fRot += (fFPS / 60.0f) * g_PI * 0.005f ;
				D3DXMatrixRotationY( &matRot, pItem->fRot );
				D3DXMatrixMultiply( &matFinal, &matRot, &m_pHealth->m_matScale );
				
				D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matScale );
				
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				m_pHealth->WorldSetMatrix( &matFinal, m_pCherry->m_fScale * fItemAbsRadius );
				//pDev->SetMaterial( &m_mtrlWeapon );
				//pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+0 ] );
				m_pHealth->Render( pDev, pTex, pulPolyCount, TRUE, FALSE, pmatCurWorld );

			// draw a pretty sphere
				/*D3DXMatrixScaling( &matScale, fItemAbsRadius, fItemAbsRadius, fItemAbsRadius );
				D3DXMatrixTranslation( &matTrans, vItemAbsPos.x, vItemAbsPos.y, vItemAbsPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matTrans );
				D3DXMatrixMultiply( &matFinal, &matFinal, pmatCurWorld );
				pDev->SetTransform( D3DTS_WORLD, &matFinal );
				pDev->SetMaterial( &m_mtrlHealth );
				pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+2 ] );

					D3DXMatrixRotationX( &m0, X3 );
					D3DXMatrixRotationY( &m1, Y3 );
					D3DXMatrixRotationZ( &m2, Z3 );
					D3DXMatrixMultiply( &matTex, &m0, &m1 );
					D3DXMatrixMultiply( &matTex, &matTex, &m2 );
					X3+=(0.03f) * (60.0f/fFPS) ;
					Y3+=(0.02f) * (60.0f/fFPS) ;
					Z3+=(0.01f) * (60.0f/fFPS) ;
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

				hr = m_pItemSphereMesh->DrawSubset( 0 );
				*pulPolyCount += m_pItemSphereMesh->GetNumFaces() ;
				assert( D3D_OK==hr );*/
				break ;
		}

//ods("2");

		pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU  );

		pDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE) ;
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;
		pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW) ;
		pDev->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE) ;
		
		return 1 ;
	}
