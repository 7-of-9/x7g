#ifndef __CBillboard__
#define __CBillboard__

class CBillboard ;

#include <D3DX8Math.h>
#include "CCamera.h"
#include "x7_primitives.h"
#include "x7_utils.h"
#include "tex.h"

class CBillboard
{
public:
//
// Construction / Destruction:
//
	explicit CBillboard( float		 fHeight,
						 float		 fWidth,
						 BOOL		 bBuildFlat,
						 D3DXCOLOR*  pMaterialColor );

	~CBillboard() ;

//
// Geometric Data:
//
	xVec3						m_vCenter ;
	float						m_fHeight ;
	float						m_fWidth ;

//
// Render Data:
//
	COLOREDVERTEX				m_fvfVerts[ 4 ];
	D3DMATERIAL8				m_Mtrl ;
	LPDIRECT3DVERTEXBUFFER8		m_pVB ;

	HRESULT MakeRenderData( LPDIRECT3DDEVICE8 pDev );

//
// Billboard Construction:
//
	void BillboardLookAt( xMatUA*	   pmatOut,
			 			  const xVec3* pvBillboardPosition,
						  const xVec3* pvCameraPosition,
						  const xVec3* pvUp );

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( const LPDIRECT3DDEVICE8 pDev );
	HRESULT InvalidateDeviceObjects() ;
	HRESULT Render( LPDIRECT3DDEVICE8	pDev,
					unsigned long*		pulPolyCount,
					xVec3*				pvPos,
					CCamera*			pCam,
					float				fScale,
					BOOL				bBillboard );
} ;

#endif
