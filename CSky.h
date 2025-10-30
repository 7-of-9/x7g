#ifndef __CSky__
#define __CSky__

	#include "CProceduralRenderObject.h"
	#include "tex.h"

class CSky
{
public:

//
// Construction / Destruction:
//
	CSky( float f, BOOL bBlackSky );
	~CSky() ;

//
// Creation:
//
	CProceduralRenderObject *m_pRO ;
	HRESULT Create( BOOL bBlackSky ); 
	void MoveClouds(float fps) ;
	float r1, r2 ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev ); 
	HRESULT	InvalidateDeviceObjects() ;
    HRESULT Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, xMatUA *pmatCurWorld, xVec3* pvEyePt );
    HRESULT FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP );

protected:

	HRESULT CreateHalfSphere (UNCOLOREDVERTEX* pVertices,
									    DWORD*	pdwNumVertices,
										WORD*	pIndices,
										DWORD*	pdwNumIndices,
										FLOAT	fRadius,
										DWORD	dwNumRings) ;
	HRESULT CreateSphere( UNCOLOREDVERTEX* pVertices,
                                DWORD* pdwNumVertices,
                                WORD* pIndices, DWORD* pdwNumIndices,
                                FLOAT fRadius, DWORD dwNumRings ) ;

	
};

#endif
