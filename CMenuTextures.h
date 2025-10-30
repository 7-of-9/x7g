#ifndef __CMenuTextures__
#define __CMenuTextures__

#include <D3DX8Math.h>
#include <D3DX8Tex.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "tex.h"
#include "resource.h"

class CMenuTextures
{
public:

//
// Construction / Destruction:
//	
	CMenuTextures() ;
	~CMenuTextures() ;

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


