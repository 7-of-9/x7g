#ifndef __CTextures__
#define __CTextures__

#include <D3DX8Math.h>
#include <D3DX8Tex.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "resource.h"
#include "tex.h"

enum e_LightingType
{
	LT_DAY,
	LT_DUSK,
	LT_NIGHT
};

class CTextures
{
public:

//
// Construction / Destruction:
//	
	CTextures( char *pInfo1, char *pInfo2,
			   e_LightingType ltLevelLightingType,
			   int			  nSkyboxFolder,
			   int			  nLevelTerrainTex1,
			   int			  nLevelTerrainTex2 );
	~CTextures() ;

	char*			m_pszInfo1 ;
	char*			m_pszInfo2 ;
	e_LightingType  m_ltLevelLightingType ;
	int			    m_nLevelTerrainTex1 ;
	int			    m_nLevelTerrainTex2 ;
	int				m_nSkyboxFolder ;

//
// LPDIRECT3DTEXTURE8 Textures:
//
	unsigned long m_nTex ;
	LPDIRECT3DTEXTURE8 m_pTex[MAX_TEX] ;
	
	HRESULT LoadAllTex(LPDIRECT3DDEVICE8 pDev) ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT	InvalidateDeviceObjects() ;

private:
} ;

#endif


