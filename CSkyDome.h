#ifndef __CSkyDome__
#define __CSkyDome__

#include "CTextures.h"

class CSkyDome
{
public:

//
// Construction / Destruction:
//
	CSkyDome( float f, bool bBlackSky, int nStacksAndSlices );
	~CSkyDome() ;

	float		m_fDiam ;
	int			m_nStacksAndSlices ;
	LPD3DXMESH	m_pSkyMesh ;

	LPD3DXMESH CreateMappedSphere( LPDIRECT3DDEVICE8 pDev, float fDiam, UINT slices, UINT stacks );

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev ); 
	HRESULT	InvalidateDeviceObjects() ;
    HRESULT Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, xMatUA *pmatCurWorld, xVec3* pvEyePt, float fFPS );
    HRESULT FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP );

};

#endif
