#pragma warning( disable : 4183 )	// 'missing return type'

#include "CTextures.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CTextures::CTextures( char *pInfo1, char *pInfo2,
						  e_LightingType  ltLevelLightingType,
						  int			  nSkyboxFolder,
						  int			  nLevelTerrainTex1,
						  int			  nLevelTerrainTex2 )
	{
		m_pszInfo1 = pInfo1 ;
		m_pszInfo2 = pInfo2 ;

		for (unsigned i = 0 ; i < MAX_TEX ; i ++)
			m_pTex[i] = NULL ;
		m_nTex = 0 ;

		m_ltLevelLightingType = ltLevelLightingType ;
		m_nLevelTerrainTex1 = nLevelTerrainTex1 ;
		m_nLevelTerrainTex2 = nLevelTerrainTex2 ;
		m_nSkyboxFolder = nSkyboxFolder ;
	}
	CTextures::~CTextures()
	{
		InvalidateDeviceObjects() ;
	}

/*
 *
 * LPDIRECT3DTEXTURE8 Textures.
 *
 */
	HRESULT CTextures::LoadAllTex(LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr = S_OK ;
		char sz[200] ;
		int i ;
		
	//
	// Misc. textures:
	//
		for (i = 1 ; i <= 5 ; i ++)
		{
			sprintf(m_pszInfo2, "Misc. Texture %d [of 5]", i) ;

#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\misc\\%03d.DDS", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[ MISC_TEX_START + i - 1 ])))
				Msg("Potential User-Error:\n\nFAIL: tex_1") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDR_DDS_TRACK_001 + (i-1)),
											 &m_pTex[MISC_TEX_START + i-1],
											 "DDS" );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

	//
	// Sky textures:
	//
		char szSkyboxPath[ 256 ];
		switch( m_ltLevelLightingType )
		{
			case LT_DAY:	strcpy( szSkyboxPath, "day" ); break ;
			case LT_DUSK:	strcpy( szSkyboxPath, "dusk"); break ;
			case LT_NIGHT:	strcpy( szSkyboxPath, "night");  break ;
		}
		for (i = 1 ; i <= 6 ; i ++)
		{
			sprintf(m_pszInfo2, "Sky Texture %d [of 6]", i) ;

#ifdef LOAD_RES_FROM_FILE
			sprintf( sz, "..\\Resource\\tex\\sky\\%s\\%03d\\%03d.bmp.DDS", szSkyboxPath, m_nSkyboxFolder, (int)i ); 
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[SKY_TEX_START + i - 1])))
				Msg("Potential User-Error:\n\nFAIL: tex_2") ;
			else
				m_nTex++ ;
#else
			assert( false );
#endif
		}

	//
	// Terrain textures:
	//
		for (i = 1 ; i <= 18 ; i ++)
		{
			sprintf(m_pszInfo2, "Terrain Texture %d [of 2]", i) ;
			
#ifdef LOAD_RES_FROM_FILE
			if( m_nLevelTerrainTex1 == ( TERRAIN_TEX_START + i-1 ) ||
				m_nLevelTerrainTex2 == ( TERRAIN_TEX_START + i-1 ) )
			{
					sprintf(sz, "..\\Resource\\tex\\terrain\\%03d.DDS", (int)i) ;
					if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[TERRAIN_TEX_START + i - 1])))
						Msg("Potential User-Error:\n\nFAIL: tex_3") ;
					else
						m_nTex++ ;
			}
#else
			assert( false );
#endif
		}

	//
	// HUD Textures:
	//
		for (i = 1 ; i <= 4 ; i ++)
		{
			sprintf(m_pszInfo2, "HUD Texture %d [of 4]", i) ;

#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\HUD\\%03d.DDS", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[HUD_TEX_START + i - 1])))
				Msg("Potential User-Error:\n\nFAIL: tex_5") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDR_DDS_HUD_001 + (i-1)),
											 &m_pTex[HUD_TEX_START + i-1],
											 "DDS" );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

	//
	// Explosion Textures:
	//
		for (i = 1 ; i <= 18 ; i ++)
		{
			sprintf(m_pszInfo2, "Explosion Texture %d [of 18]", i) ;
			
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\exp\\exp%02d.DDS", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[EXP_TEX_START + i - 1])))
				Msg("Potential User-Error:\n\nFAIL: tex_6") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDB_BMP_EXP_001 + (i-1)),
											 &m_pTex[EXP_TEX_START + i-1],
											 RT_BITMAP );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

	//
	// Explosion(2) Textures:
	//
		for (i = 1 ; i <= 35 ; i ++)
		{
			sprintf(m_pszInfo2, "Explosion(2) Texture %d [of 35]", i) ;
			
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\exp2\\expl%03d.bmp.dds", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[EXP2_TEX_START + i - 1])))
				Msg("Potential User-Error:\n\nFAIL: tex_7") ;
			else
				m_nTex++ ;
#else
			Msg( "FATAL: not put exp2 textures into resource section yet! ");
#endif
		}

	//
	// Ocean Textures:
	//
		/*for (i = 1 ; i <= 1 ; i ++)
		{
			//sprintf(m_pszInfo2, "Ocean Texture %d [of 1]", i) ;
			//sprintf(sz, "..\\Resource\\tex\\ocean\\%03d.DDS", (int)i) ;
			//if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[OCEAN_TEX_START + i - 1])))
			//	Msg("Potential User-Error:\n\nFAIL: tex_7") ;
			//else
			//	m_nTex++ ;

			//sprintf( sz, "IDR_DDS_OCEAN_%03d", i );
			//hr = LoadDDSTextureFromResource( pDev, MAKEINTRESOURCE(IDR_DDS_OCEAN_001), &m_pTex[OCEAN_TEX_START + i - 1] );
			//assert( S_OK==hr );
		}*/

	//
	// Caustic Textures:
	//
		for (i = 1 ; i <= 32 ; i ++)
		{
			sprintf( m_pszInfo2, "Caustic Texture %d [of 32]", i );
			
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\caustic\\caust%02d.tga", (int)i-1) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[CAUSTIC_TEX_START + i-1])))
				Msg("Potential User-Error:\n\nFAIL: tex_8") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDB_BMP_CAUST_001 + (i-1)),
											 &m_pTex[CAUSTIC_TEX_START + i-1],
											 RT_BITMAP );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

	//
	// Caustic(2) Textures:
	//
		for (i = 1 ; i <= 32 ; i ++)
		{
			sprintf( m_pszInfo2, "Caustic(2) Texture %d [of 32]", i );
			
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\caustic2\\caus%02d.bmp.dds", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[CAUSTIC2_TEX_START + i-1])))
				Msg("Potential User-Error:\n\nFAIL: tex_9") ;
			else
				m_nTex++ ;
#else
			Msg( "FATAL: not put caustic2 textures into resource section yet! ");
#endif
		}

	//
	// Spherical Textures:
	//
		for (i = 1 ; i <= 1 ; i ++)
		{
			sprintf(m_pszInfo2, "Spherical Texture %d [of 1]", i) ;
			
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\sphere\\%03d.dds", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[SPHERE_TEX_START + i - 1])))
				Msg("Potential User-Error:\n\nFAIL: tex_10") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDR_DDS_SPHERE_001 + (i-1)),
											 &m_pTex[SPHERE_TEX_START + i-1],
											 "DDS" );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

	//
	// Particle Textures:
	//
		for (i = 1 ; i <= 3 ; i ++)
		{
			sprintf(m_pszInfo2, "Particle Texture %d [of 3]", i) ;
			
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\particle\\%03d.dds", (int)i) ;
			if( FAILED(D3DXCreateTextureFromFileEx( pDev, sz, 
                D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
                D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0x00, NULL, NULL, &m_pTex[PARTICLE_TEX_START + i - 1] )))
				Msg("Potential User-Error:\n\nFAIL: tex_11") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDB_BMP_PARTICLE_001 + (i-1)),
											 &m_pTex[PARTICLE_TEX_START + i-1],
											 RT_BITMAP );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

		m_pszInfo2[0] = '\x0' ;
		return S_OK ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D IDirect3DTexture8 *s.
 *
 */
	HRESULT CTextures::RestoreDeviceObjects(LPDIRECT3D8	pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "CTextures::RestoreDeviceObjects\n" );

		strcpy(m_pszInfo1, "m_pTextures") ;
		return LoadAllTex(pDev) ;
	}
	HRESULT	CTextures::InvalidateDeviceObjects()
	{
		for (unsigned long i = 0 ; i < MAX_TEX ; i ++)
		{
			if (m_pTex[i])
				m_pTex[i]->Release() ;
			m_pTex[i] = NULL ;
		}
		m_nTex = 0 ;
		return S_OK ;
	}
