#pragma warning( disable : 4183 )

#include "CBillboard.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CBillboard::CBillboard( float	   fHeight,
						    float	   fWidth,
							BOOL	   bBuildFlat,
							D3DXCOLOR* pMaterialColor )
	{
		m_vCenter = xVec3(0.0f, 0.0f, 0.0f) ;
		m_fHeight = fHeight ;
		m_fWidth = fWidth ;

		DWORD color = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
		DWORD specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
		if( !bBuildFlat )
		{
			m_fvfVerts[0] = COLOREDVERTEX(m_vCenter.x - fWidth/2, m_vCenter.y - fHeight/2, m_vCenter.z, 0.0f, 0.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
			m_fvfVerts[1] = COLOREDVERTEX(m_vCenter.x + fWidth/2, m_vCenter.y - fHeight/2, m_vCenter.z, 0.0f, 1.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
			m_fvfVerts[2] = COLOREDVERTEX(m_vCenter.x - fWidth/2, m_vCenter.y + fHeight/2, m_vCenter.z, 1.0f, 0.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
			m_fvfVerts[3] = COLOREDVERTEX(m_vCenter.x + fWidth/2, m_vCenter.y + fHeight/2, m_vCenter.z, 1.0f, 1.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
		}
		else
		{
			m_fvfVerts[0] = COLOREDVERTEX(m_vCenter.x - fWidth/2, m_vCenter.y, m_vCenter.z - fHeight/2, 0.0f, 0.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
			m_fvfVerts[1] = COLOREDVERTEX(m_vCenter.x + fWidth/2, m_vCenter.y, m_vCenter.z - fHeight/2, 0.0f, 1.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
			m_fvfVerts[2] = COLOREDVERTEX(m_vCenter.x - fWidth/2, m_vCenter.y, m_vCenter.z + fHeight/2, 1.0f, 0.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
			m_fvfVerts[3] = COLOREDVERTEX(m_vCenter.x + fWidth/2, m_vCenter.y, m_vCenter.z + fHeight/2, 1.0f, 1.0f, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ), specular) ;
		}
		
		D3DUtil_InitMaterial( m_Mtrl, pMaterialColor->r, pMaterialColor->g, pMaterialColor->b, pMaterialColor->a );
		m_pVB = NULL ;
	}
	CBillboard::~CBillboard()
	{
	}

/*
 *
 * Render Data Creation.
 *
 */
	HRESULT CBillboard::MakeRenderData( const LPDIRECT3DDEVICE8 pDev )
	{
		HRESULT hr ;

	//
	// Create & fill the VB:
	//
		UINT iFVFSize = sizeof(COLOREDVERTEX) ;
		hr = pDev->CreateVertexBuffer(4 * iFVFSize,
									  D3DUSAGE_WRITEONLY/*D3DUSAGE_SOFTWAREPROCESSING*/,
									  MYFVF_COLOREDVERTEX,
									  D3DPOOL_MANAGED/*D3DPOOL_DEFAULT*/,
									  &m_pVB) ;
		assert( !FAILED(hr) );

		BYTE *pVerts ;
		hr = m_pVB->Lock(0, 0, &pVerts, 0) ;
		assert( !FAILED(hr) );

		memcpy(pVerts, m_fvfVerts, sizeof(COLOREDVERTEX) * 4) ;
		m_pVB->Unlock() ;

		return S_OK ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh objects.
 *
 */
	HRESULT CBillboard::RestoreDeviceObjects( const LPDIRECT3DDEVICE8 pDev )
	{
		ods( "CBillboard::RestoreDeviceObjects\n" );

		HRESULT hr ;
		hr = MakeRenderData( pDev );
		assert( hr==S_OK );

		return hr ;
	}
	HRESULT	CBillboard::InvalidateDeviceObjects()
	{
		SAFE_RELEASE( m_pVB )
		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CBillboard::Render( LPDIRECT3DDEVICE8	pDev,
								unsigned long*		pulPolyCount,
								xVec3*				pvPos,
								CCamera*			pCam,
								float				fScale,
								BOOL				bBillboard )
	{
		xMat matWorld, matTrans ;

		D3DXMatrixScaling( &matWorld, fScale, fScale, fScale );
		
		if( bBillboard )
			BillboardLookAt( &matWorld, pvPos,
							 (const xVec3*)&pCam->m_vEyePt,
							 (const xVec3*)&pCam->m_vUpVec );
		else
		{
			D3DXMatrixTranslation( &matTrans, pvPos->x, pvPos->y, pvPos->z );
			D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
		}

		pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		pDev->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pDev->SetTransform( D3DTS_WORLD, &matWorld );
		pDev->SetMaterial( &m_Mtrl );

		//pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		//pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		pDev->SetStreamSource( 0, m_pVB, sizeof(COLOREDVERTEX) );
		pDev->SetVertexShader( MYFVF_COLOREDVERTEX );
		pDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		*pulPolyCount += 2 ;
		return S_OK ;
	}

	void CBillboard::BillboardLookAt( xMatUA*		    pmatOut,
									  const xVec3*  pvBillboardPosition,
									  const xVec3*  pvCameraPosition,
									  const xVec3*  pvUp )
	/*void CBillboard::BillboardLookAt( xMat*		 pmatOut,
									  const xVec3* pvBillboardPosition,
									  const xVec3* pvCameraPosition,
									  const xVec3* pvUp )*/
	{
	//
	// http://64.33.37.114/billboards.htm
	//
		xMat Matrix ;
		xVec3 View = *pvCameraPosition - *pvBillboardPosition ;
		D3DXVec3Normalize( &View, &View );

		float angle = D3DXVec3Dot( pvUp, &View );
		xVec3 Up = *pvUp - angle*View ;
		D3DXVec3Normalize( &Up, &Up );

		xVec3 Right ;
		D3DXVec3Cross( &Right, &Up, &View );

		xMat mat ;
		mat._11 = Right.x; mat._21 = Up.x; mat._31 = View.x;
		mat._12 = Right.y; mat._22 = Up.y; mat._32 = View.y;
		mat._13 = Right.z; mat._23 = Up.z; mat._33 = View.z;

		mat._41=pvBillboardPosition->x;
		mat._42=pvBillboardPosition->y;
		mat._43=pvBillboardPosition->z;

		mat._14=0.0f;
		mat._24=0.0f;
		mat._34=0.0f;
		mat._44=1.0f;

		D3DXMatrixMultiply( pmatOut, pmatOut, &mat );
	}