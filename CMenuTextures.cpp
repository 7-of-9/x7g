#pragma warning( disable : 4183 )	// 'missing return type'

#include "CMenuTextures.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CMenuTextures::CMenuTextures()
	{
		for (unsigned i = 0 ; i < MAX_TEX ; i ++)
			m_pTex[i] = NULL ;
		m_nTex = 0 ;
	}
	CMenuTextures::~CMenuTextures()
	{
		InvalidateDeviceObjects() ;
	}

/*
 *
 * LPDIRECT3DTEXTURE8 Textures.
 *
 */
	HRESULT CMenuTextures::LoadAllTex(LPDIRECT3DDEVICE8 pDev)
	{
		char sz[200] ;
		HRESULT hr = S_OK ;
		int i ;
		
	//
	// Menu Textures:
	//
		for (i = 1 ; i <= 8 ; i ++)
		{
#ifdef LOAD_RES_FROM_FILE
			sprintf(sz, "..\\Resource\\tex\\menu\\%03d.DDS", (int)i) ;
			if (FAILED(D3DXCreateTextureFromFileA(pDev, sz, &m_pTex[MENU_TEX_START + i - 1])))
				Msg("Potential User-Error:\n\nFAIL: menu_tex_1") ;
			else
				m_nTex++ ;
#else
			hr = LoadDDSTextureFromResource( pDev,
											 MAKEINTRESOURCE(IDR_DDS_MENU_001 + (i-1)),
											 &m_pTex[MENU_TEX_START + i - 1],
											 "DDS" );
			assert( S_OK==hr );
			m_nTex++ ;
#endif
		}

		return S_OK ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D IDirect3DTexture8 *s.
 *
 */
	HRESULT CMenuTextures::RestoreDeviceObjects(LPDIRECT3D8	pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "CMenuTextures::RestoreDeviceObjects\n" );

		return LoadAllTex(pDev) ;
	}
	HRESULT	CMenuTextures::InvalidateDeviceObjects()
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
