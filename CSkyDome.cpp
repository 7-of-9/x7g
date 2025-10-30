#include "CSkyDome.h"

CSkyDome::CSkyDome( float f, bool bBlackSky, int nStacksAndSlices )
{
	m_pSkyMesh = NULL ;
	m_fDiam = f ;
	m_nStacksAndSlices = nStacksAndSlices ;
}

CSkyDome::~CSkyDome()
{
}

HRESULT CSkyDome::RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev )
{
	ods( "CSkyDome::RestoreDeviceObjects\n" );

	assert( NULL == m_pSkyMesh );
	m_pSkyMesh = CreateMappedSphere( pDev, 1, m_nStacksAndSlices, m_nStacksAndSlices );
	assert( NULL != m_pSkyMesh );

	return S_OK ;
}

HRESULT	CSkyDome::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pSkyMesh )
	return S_OK ;
}

HRESULT CSkyDome::Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, xMatUA *pmatCurWorld, xVec3* pvEyePt, float fFPS )
{
	xMat matScale ; D3DXMatrixScaling( &matScale, m_fDiam, m_fDiam, m_fDiam );
	pDev->SetTransform( D3DTS_WORLD, &matScale );
	
	pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START + 1 ] );

	/*static float X0 = 0 ;
	pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	xMat matTex ; D3DXMatrixTranslation( &matTex, 0, X0, 0 );	
	X0 += 0.005f * ( 60 / fFPS );
	pDev->SetTransform( D3DTS_TEXTURE0, &matTex );*/

	/*pDev->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,
									//D3DTSS_TCI_CAMERASPACEPOSITION
									//D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR
									D3DTSS_TCI_CAMERASPACENORMAL );*/

	m_pSkyMesh->DrawSubset( 0 );
	*pulPolyCount += m_pSkyMesh->GetNumFaces() ;

	/*pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

	pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );*/

	return S_OK ;
}

HRESULT CSkyDome::FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP )
{
	return S_OK ;
}

// 10x 2 rich DX MVP
LPD3DXMESH CSkyDome::CreateMappedSphere( LPDIRECT3DDEVICE8 pDev, float fDiam, UINT slices, UINT stacks )
{
	float fRad = fDiam / 2 ;
    // create the sphere
    LPD3DXMESH mesh;
    if (FAILED(D3DXCreateSphere(pDev,fRad,slices,stacks,&mesh,NULL)))
        return NULL;

	LPD3DXMESH pMesh ;

    // create a copy of the mesh with texture coordinates,
    // since the D3DX function doesn't include them
    if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM,MYFVF_UNCOLOREDVERTEX,pDev,&pMesh)))
        return NULL ;

    // finished with the original mesh, release it
    mesh->Release();

    // lock the vertex buffer
    UNCOLOREDVERTEX* pVerts;
    if (SUCCEEDED(pMesh->LockVertexBuffer(0,(BYTE **) &pVerts))) {

        // get vertex count
        int numVerts=pMesh->GetNumVertices();

        // loop through the vertices
        for (int i=0;i<numVerts;i++) {

            // calculate texture coordinates
			
		// 1
			float t = 10 ;
            //pVerts->tu1= ( ( asinf(pVerts->nx)/g_PI ) * t ) + t/2 ;
            //pVerts->tv1= ( ( asinf(pVerts->nz)/g_PI ) * t ) + t/2 ;

        // 2
			//pVerts->tu1= pVerts->nx / 2 + 0.5f ;
            //pVerts->tv1= pVerts->nz / 2 + 0.5f ;

		// 3
			xVec3 v = xVec3( pVerts->x, pVerts->y, pVerts->z );
			D3DXVec3Normalize( &v, &v );
			pVerts->tu1 = ( asinf( v.x ) / g_PI ) * t + t/2 ;
			pVerts->tv1 = ( asinf( v.y ) / g_PI ) * t + t/2 ;

            // go to next vertex
            pVerts++;
        }

        // unlock the vertex buffer
        pMesh->UnlockVertexBuffer();
    }
    
    return pMesh ;
}
