#ifndef __COcean__
#define __COcean__

	#include "CProceduralRenderObject.h"
	#include "tex.h"

class COcean
{
public:

//
// Construction / Destruction:
//
	COcean( float f, int nTex, D3DXCOLOR* pColour );
	~COcean() ;

//
// Creation:
//
	CProceduralRenderObject *m_pRO ;
	HRESULT Create() ; 

//
// Tide:
//
	float m_fTideDirection ;
	float m_fTideHeight ;
	float m_fTideMaxHeight ;
	float m_fTideMinHeight ;

//
// Texture, Colour & Movement:
//
	xVec2 m_vTexVel ;
	int			m_nTex ;
	D3DXCOLOR	m_Colour ;

//
// Caustic Texture Rotation:
//
	int		m_ndxCurTex ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev );
	HRESULT	InvalidateDeviceObjects() ;
    HRESULT Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, xMatUA *pmatCurWorld, xVec3* pvEyePt );
    HRESULT FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP );

};

#endif
