#pragma warning( disable : 4183 )

#include "CFileRenderObject.h"

DWORD dwUNCOLOREDVERTEX_vsdec[] = {
  D3DVSD_STREAM(0),
  D3DVSD_REG(0, D3DVSDT_FLOAT3 ),		// Pos: v0
  D3DVSD_REG(1, D3DVSDT_FLOAT3 ),		// Normal: v1
  D3DVSD_REG(2, D3DVSDT_FLOAT2 ),		// Tex1: v2
  D3DVSD_END()
};

/*
 *
 * Construction / Destruction.
 *
 */
	CFileRenderObject::CFileRenderObject( char *pszFile, int nResID, RENDER_DATA_INFO* pSharedRenderData )
	{
	//
	// RENDER_DATA_INFO:
	//
		m_pRenderData = NULL ;
		if( pSharedRenderData )
		{
			m_pRenderData = pSharedRenderData ;
			m_bRenderDataIsMine = FALSE ;
		}
		else
		{
			m_pRenderData = new RENDER_DATA_INFO ;
			m_bRenderDataIsMine = TRUE ;

			m_pRenderData->m_pMesh = NULL ;
			m_pRenderData->m_mtrlMeshMaterials = NULL ;
			m_pRenderData->m_dwNumMaterials = 0 ;
			m_pRenderData->m_dwNumFaces = 0 ;
			m_pRenderData->m_dwNumVertices = 0 ;
			m_pRenderData->m_pAdjacencyBuffer = NULL ;
			m_pRenderData->m_szFile[0] = '\x0' ;
			if( pszFile )
				strcpy( m_pRenderData->m_szFile, pszFile );
			m_pRenderData->m_nResID = nResID ;

			m_pRenderData->m_dwShaderRipple = 0 ;
		}

	//
	// Scale, bbox, rotation matrices:
	//
		m_pBBox = NULL ;
		m_matWorld = g_matIdent ;
		m_matRot = g_matIdent ;
		m_matScale = g_matIdent ;
		D3DUtil_InitMaterial(m_matDef, 1, 1, 1) ;
		m_fScale = 1.0f ;
		m_fLargestSide = 0.0f ;
	}
	CFileRenderObject::~CFileRenderObject()
	{
		if( m_bRenderDataIsMine )
			SAFE_DELETE( m_pRenderData )
	}

/*
 *
 * D3DX Mesh and PMesh Layer:
 *
 */
	HRESULT CFileRenderObject::MakeRenderDataAndBoundingData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "CFileRenderObject::MakeRenderDataAndBoundingData\n" );

		LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL ;
		HRESULT	  	 hr ;
		LPD3DXMESH   pTempMesh ;
		LPD3DXBUFFER pD3DXMtrlBuffer ;

	// load the mesh from the specified file:
		if( m_pRenderData->m_szFile[0] != '\x0' )
		{
			ods( "CFileRenderObject::MakeRenderDataAndBoundingData - D3DXLoadMeshFromX\n" );

			if( FAILED(hr = D3DXLoadMeshFromX(m_pRenderData->m_szFile,
											  D3DXMESH_SYSTEMMEM,
											  pDev,
											  &m_pRenderData->m_pAdjacencyBuffer,
											  &pD3DXMtrlBuffer,
											  &m_pRenderData->m_dwNumMaterials,
											  &pTempMesh)) )
			{
				Msg("Potential User-Error:\n\nCFileRenderObject::MakeMesh_PMesh: FAILED(D3DXLoadMeshFromX(...)) == TRUE") ;
				return hr ;
			}
		}
		else
		{
			if( FAILED(hr = LoadXMeshFromResource(pDev,
												  MAKEINTRESOURCE(m_pRenderData->m_nResID),
												  &pTempMesh,
												  &m_pRenderData->m_pAdjacencyBuffer,
												  &pD3DXMtrlBuffer,
												  &m_pRenderData->m_dwNumMaterials,
												  D3DXMESH_SYSTEMMEM)))
			{
				Msg("Potential User-Error:\n\nCFileRenderObject::MakeMesh_PMesh: FAILED(D3DXLoadMeshFromXResource(...)) == TRUE") ;
				return hr ;
			}
		}

		/*if (FAILED(hr = D3DXValidMesh(pTempMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
		{
			Msg("CFileRenderObject::MakeMesh_PMesh: FAILED(D3DXValidMesh(...)) == TRUE") ;

			SAFE_RELEASE(m_pAdjacencyBuffer)
			SAFE_RELEASE(pD3DXMtrlBuffer)
			m_dwNumMaterials = 0L ;
			return E_FAIL ;
		}*/

	// allocate a material array
		D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer() ;
		m_pRenderData->m_mtrlMeshMaterials = new D3DMATERIAL8[m_pRenderData->m_dwNumMaterials] ;

	// copy the materials
		for (DWORD i = 0 ; i < m_pRenderData->m_dwNumMaterials ; i++)
		{
			m_pRenderData->m_mtrlMeshMaterials[i] = d3dxMaterials[i].MatD3D ;
			m_pRenderData->m_mtrlMeshMaterials[i].Ambient = m_pRenderData->m_mtrlMeshMaterials[i].Diffuse ;
		}
		pD3DXMtrlBuffer->Release() ;

	// get the FVF
		/*m_dwFVF = pTempMesh->GetFVF() ;*/

	// get the number of vertices
		m_pRenderData->m_dwNumVertices = pTempMesh->GetNumVertices() ;

	// get the number of faces
		m_pRenderData->m_dwNumFaces = pTempMesh->GetNumFaces() ;

	// get the mesh's vertex buffer information
		/*LPDIRECT3DVERTEXBUFFER8 pVB ;
		hr = pTempMesh->GetVertexBuffer( &pVB );
		assert( !FAILED(hr) );*/

	// allocate a second adjacency buffer to store post attribute sorted adjacency
	    DWORD *rgdwAdjacencyTemp = NULL ;
		rgdwAdjacencyTemp = new DWORD[m_pRenderData->m_dwNumFaces * 3] ;
		assert( rgdwAdjacencyTemp );

		ods( "CFileRenderObject::MakeRenderDataAndBoundingData - OptimizeInplace\n" );

	// attribute sort + vertex cache sort
		hr = pTempMesh->OptimizeInplace(/*D3DXMESHOPT_COMPACT | */ D3DXMESHOPT_ATTRSORT /*| D3DXMESHOPT_VERTEXCACHE*/,
										NULL,/*(DWORD*)m_pRenderData->m_pAdjacencyBuffer->GetBufferPointer()*/
										NULL,/*rgdwAdjacencyTemp*/
										NULL,
										NULL) ;
		assert( !FAILED(hr) );

	// clone to temp readable mesh with known FVF
		LPD3DXMESH pTempMesh2 ;
		hr = pTempMesh->CloneMeshFVF( D3DXMESH_DYNAMIC, MYFVF_UNCOLOREDVERTEX, pDev, &pTempMesh2 );
		assert( !FAILED(hr) );
		SAFE_RELEASE( pTempMesh )
	// create bbox & bsphere from readable fixed FVF mesh
		MakeBoundingBoxAndSphere( pD3D, pDev, pTempMesh2 );

	// clone to global writeonly mesh
		hr = pTempMesh2->CloneMeshFVF( D3DXMESH_WRITEONLY, MYFVF_UNCOLOREDVERTEX, pDev, &m_pRenderData->m_pMesh );
		assert( !FAILED(hr) );
	
		SAFE_RELEASE( pTempMesh2 )
		SAFE_DELETE_ARRAY( rgdwAdjacencyTemp )

	//
	// Get attribute table:
	//
        hr = m_pRenderData->m_pMesh->GetAttributeTable( NULL, &m_pRenderData->m_dwNumAttr );
        assert( !FAILED(hr) );
        m_pRenderData->m_pAttrTable  = new D3DXATTRIBUTERANGE[m_pRenderData->m_dwNumAttr];
        assert( m_pRenderData->m_pAttrTable != NULL );
        hr = m_pRenderData->m_pMesh->GetAttributeTable( m_pRenderData->m_pAttrTable, NULL );
        assert( !FAILED(hr) );
	
	//
	// Clone mesh to ensure correct FVF:
	//
		/*if (pMesh->GetFVF() != m_dwFVF)
		{
			if (FAILED(hr = pMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM,
												m_dwFVF, 
												pDev,
												&pTempMesh)))
			{
				Msg("CFileRenderObject::MakeMesh_PMesh: FAILED(CloneMeshFVF(...)) == TRUE") ;
				return hr ;
			}

			if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
			{
				D3DXComputeNormals(pTempMesh) ;
			}

			pMesh->Release() ;
			pMesh = pTempMesh ;
		}

	//
	// Make progressive mesh:
	//
		hr = D3DXGeneratePMesh(pMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),
							   NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &m_pPMesh) ;
		pMesh->Release() ;
		if(FAILED(hr))
		{
			Msg("CFileRenderObject::MakeMesh_PMesh: FAILED(D3DXGeneratePMesh(...)) == TRUE") ;
			return hr ;
		}

		m_pPMesh->SetNumVertices(0xffffffff) ;*/

	//
	// Create vertex shaders:
	//
		LPD3DXBUFFER pErrs = NULL ;
		LPD3DXBUFFER pCode = NULL ;

	// assemble the vertex shader from file
		hr = D3DXAssembleShaderFromFile( "..\\resource\\tandl.vsh", 0, NULL, &pCode, &pErrs );
		if( FAILED(hr) )
		{
			Msg("CFileRenderObject::MakeRenderDataAndBoundingData - D3DXAssembleShaderFromFile() FAILED");
			if( pErrs )
				Msg((char*)pErrs->GetBufferPointer());

			SAFE_RELEASE( pCode )
			SAFE_RELEASE( pErrs )
			return E_FAIL ;
		}

	// create the vertex shader
		hr = pDev->CreateVertexShader( dwUNCOLOREDVERTEX_vsdec,
									   (DWORD*)pCode->GetBufferPointer(),
									   &m_pRenderData->m_dwShaderRipple,
									   0 );
		if( FAILED(hr) )
		{
			Msg("CFileRenderObject::MakeRenderDataAndBoundingData - CreateVertexShader() FAILED");
			return E_FAIL ;
		}
		SAFE_RELEASE( pCode )
		SAFE_RELEASE( pErrs )

		return S_OK ;
	}

/*
 *
 * Sizing:
 *
 */
	void CFileRenderObject::SetLargestSide( float fLargestSide )
	{
		xVec3 vSize ;
		
	// get current largest side
		vSize = m_pBBox->m_vMax - m_pBBox->m_vMin ;
		m_fLargestSide = (vSize.x >= vSize.y && vSize.x >= vSize.z) ? vSize.x :
						 (vSize.y >= vSize.x && vSize.y >= vSize.z) ? vSize.y :
						 (vSize.z >= vSize.y && vSize.z >= vSize.x) ? vSize.z : 0.0f ;
		assert( m_fLargestSide != 0.0f );
		
	// get degree of magnification or minifaction requried
		m_fScale = fLargestSide / m_fLargestSide ;

	// apply scaling to largest side
		m_fScaledLargestSide = m_fLargestSide * m_fScale ;

	// apply scaling to bounding sphere radius
		m_pRenderData->m_fBSphereRadius = m_pRenderData->m_fInitialBSphereRadius * m_fScale ;

	// build matrix
		D3DXMatrixScaling(&m_matScale, m_fScale, m_fScale, m_fScale) ;
	}

/*
 *
 * Bounding Box & Sphere.
 *
 */
	HRESULT CFileRenderObject::MakeBoundingBoxAndSphere(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, LPD3DXMESH pReadableMesh)
	{
		HRESULT hr ;
		xVec3 vMin, vMax, vCenter ;

	// lock VB
		BYTE *pVerts ;
		hr = pReadableMesh->LockVertexBuffer( 0, &pVerts );
		assert( hr==D3D_OK );

	// get raw bounding volume
		hr = D3DXComputeBoundingBox( pVerts, m_pRenderData->m_dwNumVertices, MYFVF_UNCOLOREDVERTEX, &vMin, &vMax );
		assert( !FAILED(hr) );

	// may have to walk through the vertices and move them so that
	// the mesh is centered at 0,0,0.
		vCenter.x = (vMax.x - vMin.x) / 2.0f + vMin.x ;
		vCenter.y = (vMax.y - vMin.y) / 2.0f + vMin.y ;
		vCenter.z = (vMax.z - vMin.z) / 2.0f + vMin.z ;
		if( vCenter.x != 0.0f || vCenter.y != 0.0f || vCenter.z != 0.0f ) 
		{
			UNCOLOREDVERTEX* pV = (UNCOLOREDVERTEX*)pVerts ;
			for( DWORD v=0 ; v < m_pRenderData->m_dwNumVertices ; v++ )
			{
				pV->x -= vCenter.x ;
				pV->y -= vCenter.y ;
				pV->z -= vCenter.z ;
				pV++ ;
			}
		// commit changes, and recompute bounding volume
			hr = pReadableMesh->UnlockVertexBuffer() ;
			assert( !FAILED(hr) );
			hr = pReadableMesh->LockVertexBuffer( 0, &pVerts );
			assert( hr==D3D_OK );
			hr = D3DXComputeBoundingBox( pVerts, m_pRenderData->m_dwNumVertices, MYFVF_UNCOLOREDVERTEX, &vMin, &vMax );
			assert( !FAILED(hr) );
		}

	// get bounding sphere radius
	// NOTE: vCenter returned here is *completely different* to center computed by bbox class!
		hr = D3DXComputeBoundingSphere( pVerts,
									    m_pRenderData->m_dwNumVertices,
										MYFVF_UNCOLOREDVERTEX,
										&vCenter,
										&m_pRenderData->m_fInitialBSphereRadius );
		assert( !FAILED(hr) );
		m_pRenderData->m_fBSphereRadius = m_pRenderData->m_fInitialBSphereRadius ;

	// save bbox data;
		m_pRenderData->m_vInitialMin = vMin ;
		m_pRenderData->m_vInitialMax = vMax ;

	// unlock VB
		pReadableMesh->UnlockVertexBuffer() ;

		return S_OK ;
	}

/*
 *
 * World Matrix Methods.
 *
 */
	/*void CFileRenderObject::WorldRotateX(float r, xVec3 *pv)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationX(&m1, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		D3DXMatrixIdentity(&m) ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;
	
		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CFileRenderObject::WorldRotateY(float r, xVec3 *pv)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationY(&m1, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		D3DXMatrixIdentity(&m) ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;
	
		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CFileRenderObject::WorldRotateZ(float r, xVec3 *pv)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationZ(&m1, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		D3DXMatrixIdentity(&m) ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;
	
		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CFileRenderObject::WorldTranslate(xVec3 *pv)
	{
		xMat m ;
		
		// build desired translation matrix
		D3DXMatrixTranslation(&m, pv->x, pv->y, pv->z) ;

		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CFileRenderObject::WorldRotateAxis(float r, xVec3 *pv, xVec3 *pvAxis)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationAxis(&m1, pvAxis, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		D3DXMatrixIdentity(&m) ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;

		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}*/
	void CFileRenderObject::WorldSetMatrix(xMatUA *m, float fScale)
	{
	// apply scaling to bounding sphere radius
		m_pRenderData->m_fBSphereRadius = m_pRenderData->m_fInitialBSphereRadius * fScale ;

	// apply full matrix to the object's bounding box
		m_pBBox->ApplyMatrixFromInitialValues(m) ;

	// apply full matrix to object's world matrix
		D3DXMatrixIdentity(&m_matWorld) ;
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m) ;
	}

	void CFileRenderObject::WorldSetMatrixIncremental(xMatUA *pmatFull,
													  xMatUA *pmatTransDeltaInc,
													  xMatUA *pmatRotAboutCenter,
													  float fScale)
	{

	// apply full matrix to the object's bounding box
		m_pBBox->ApplyMatrixFromInitialValues(pmatFull) ;

	// apply incremental *translation* matrix to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, pmatTransDeltaInc) ;

	// save rotation matrix for render-time
		m_matRot = *pmatRotAboutCenter ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh and PMesh Layers.
 *
 */
	HRESULT CFileRenderObject::RestoreDeviceObjects(LPDIRECT3D8	pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "CFileRenderObject::RestoreDeviceObjects\n" );

		if( m_bRenderDataIsMine )
		{
			HRESULT hr = MakeRenderDataAndBoundingData( pD3D, pDev );
			assert( !FAILED(hr) );
		}

	// initialise bounding box class
		m_pBBox = new CBoundingBox( &m_pRenderData->m_vInitialMin, &m_pRenderData->m_vInitialMax );
		m_pBBox->RestoreDeviceObjects( pDev );			// NOTE: in debug build this does nothing

		return S_OK ;
	}
	HRESULT	CFileRenderObject::InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		if( m_bRenderDataIsMine )
		{
			SAFE_RELEASE(m_pRenderData->m_pMesh)
			SAFE_RELEASE(m_pRenderData->m_pAdjacencyBuffer)
			SAFE_DELETE_ARRAY(m_pRenderData->m_mtrlMeshMaterials)

			m_pRenderData->m_dwNumMaterials = 0 ;
			m_pRenderData->m_dwNumVertices = 0 ;
			m_pRenderData->m_dwNumFaces = 0 ;

	        delete[] m_pRenderData->m_pAttrTable ;
			m_pRenderData->m_dwNumAttr = 0 ;

			pDev->DeleteVertexShader( m_pRenderData->m_dwShaderRipple );
		}

		SAFE_DELETE(m_pBBox)
		m_matWorld = g_matIdent ;

		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CFileRenderObject::Render(LPDIRECT3DDEVICE8 pDev,
									  CTextures *pTex,
									  unsigned long *pulPolyCount,
									  BOOL bUseMeshMaterials,
									  BOOL bUseDefMaterial,
									  xMatUA* pmatCurWorld)
	{
		xMat matCombinedWorld ;

	//
	// Set a world-matrix:
	//
		matCombinedWorld = m_matWorld ;
		D3DXMatrixMultiply(&matCombinedWorld, &matCombinedWorld, &m_matRot) ;
		if (pmatCurWorld)
			D3DXMatrixMultiply(&matCombinedWorld, &matCombinedWorld, pmatCurWorld) ;
		pDev->SetTransform(D3DTS_WORLD, &matCombinedWorld) ;

    //
	// Set def. material:
	//
		if( bUseDefMaterial )
			pDev->SetMaterial( &m_matDef );

	//
	// Render Subsets:
	//
		if( m_pRenderData->m_pMesh )
        {
            for (DWORD i = 0 ; i < m_pRenderData->m_dwNumMaterials ; i++)
            {
				if( bUseMeshMaterials )
					pDev->SetMaterial( &m_pRenderData->m_mtrlMeshMaterials[i] );

				m_pRenderData->m_pMesh->DrawSubset( i );
            }
			
			*pulPolyCount += m_pRenderData->m_dwNumFaces ;
        }

		//m_pBBox->Render( pDev, pulPolyCount, &matCombinedWorld );
		return S_OK ;
	}

	HRESULT CFileRenderObject::RenderVS( LPDIRECT3DDEVICE8 pDev,
										 float fTime,
										 unsigned long *pulPolyCount,
										 BOOL bUseMeshMaterials,
										 BOOL bUseDefMaterial,
										 xMatUA* pmatCurWorld,
										 const xMatUA& matView,
										 const xMatUA& matProj,
										 CCamera* pCam,
										 const xVec3& vLightDir )
	{
		HRESULT hr ;
		xMat matCombinedWorld ;
		xMat matTemp, matWorldViewProj, matWorldI, matWorldIT, matWorldT ;
		float fD ;

	//
	// Set a world-matrix:
	//
		matCombinedWorld = m_matWorld ;
		D3DXMatrixMultiply(&matCombinedWorld, &matCombinedWorld, &m_matRot) ;
		if (pmatCurWorld)
			D3DXMatrixMultiply(&matCombinedWorld, &matCombinedWorld, pmatCurWorld) ;
		pDev->SetTransform(D3DTS_WORLD, &matCombinedWorld) ;

    //
	// Set def. material:
	//
		if( bUseDefMaterial )
			pDev->SetMaterial( &m_matDef );

	//
	// Grab VB and IB:
	//
		LPDIRECT3DVERTEXBUFFER8 pVB ;
		LPDIRECT3DINDEXBUFFER8 pIB ;
		hr = m_pRenderData->m_pMesh->GetVertexBuffer( &pVB );
		assert( !FAILED(hr) );
		hr = m_pRenderData->m_pMesh->GetIndexBuffer( &pIB );
		assert( !FAILED(hr) );

	//
	// Set VS constants for TandL.vsh
	//

	// c0-c3; mat.view.proj transposed
		D3DXMatrixMultiply(&matTemp, &matCombinedWorld, &matView);
		D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
		D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
		pDev->SetVertexShaderConstant(  0, &matWorldViewProj(0,0),  4 );

	// c4-c6; word inverse transposed
		pDev->SetVertexShaderConstant(  4, &matWorldViewProj(0,0),  3 );

    // c11; halfway (between eye & light) vector in model space
		D3DXMatrixInverse( &matWorldI, &fD, &matCombinedWorld );
		D3DXMatrixTranspose( &matWorldIT, &matWorldIT );
		xVec3 vToEye( pCam->m_vEyePt - pCam->m_vLookAtPt );
		D3DXVec3Normalize( &vToEye, &vToEye );
		xVec3 vHalfway( vLightDir + vToEye );
		D3DXVec3TransformNormal( &vHalfway, &vHalfway, &matWorldI );
		D3DXVec3Normalize( &vHalfway, &vHalfway );
		pDev->SetVertexShaderConstant( 11, &vHalfway, 1 );

	// c12; specular power constants
		pDev->SetVertexShaderConstant( 12, &xVec4( 0,10,50,100 ), 1 );
		
	// c13; points to the eye in model space
		D3DXVec3TransformNormal( &vToEye, &vToEye, &matWorldI );
		D3DXVec3Normalize( &vToEye, &vToEye );
		vToEye = -vToEye;
		pDev->SetVertexShaderConstant( 13, &vToEye.x, 1 );

	// c14-17; world matrix
		D3DXMatrixTranspose( &matWorldT, &matCombinedWorld );
		pDev->SetVertexShaderConstant( 14, &matWorldT, 4 );

	// c18: eye point
		pDev->SetVertexShaderConstant( 18, &pCam->m_vEyePt, 1 );

	// c19: tex. coord offset
		pDev->SetVertexShaderConstant( 19, &xVec4( 0,0,0,0 ), 1 );

	//
	// Set vertex shader:
	//
		pDev->SetVertexShader( m_pRenderData->m_dwShaderRipple );

		hr = pDev->SetStreamSource( 0, pVB, D3DXGetFVFVertexSize(m_pRenderData->m_pMesh->GetFVF()) );
		assert( !FAILED(hr) );
		hr = pDev->SetIndices( pIB, 0 );
		assert( !FAILED(hr) );

	//
	// Render Subsets:
	//
		for( DWORD i=0 ; i<m_pRenderData->m_dwNumAttr ; i++ )
		{
			if( !bUseDefMaterial)
			// c9; material colour;
				pDev->SetVertexShaderConstant( 9, &m_pRenderData->m_mtrlMeshMaterials[i].Diffuse, 1 );

			pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                       m_pRenderData->m_pAttrTable[i].VertexStart,
                                       m_pRenderData->m_pAttrTable[i].VertexCount,
                                       m_pRenderData->m_pAttrTable[i].FaceStart * 3,
                                       m_pRenderData->m_pAttrTable[i].FaceCount );
			assert( !FAILED(hr) );
		}

		*pulPolyCount += m_pRenderData->m_dwNumFaces ;
		pVB->Release() ;
		pIB->Release() ;

		return S_OK ;
	}



		/*
    DWORD        m_dwShader;
    m_dwShader         = 0;
	
	FrameMove: 
		set vertex shader constants, e.g.
		m_pd3dDevice->SetVertexShaderConstant(  0, &mat,  4 );

    Render:
		m_pd3dDevice->SetVertexShader( m_dwShader );
        m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(xVec2) );
        m_pd3dDevice->SetIndices( m_pIB, 0 );
        m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumVertices,
                                            0, m_dwNumIndices/3 );

	RestoreDeviceObjects:
		// Create vertex shader
		{
			TCHAR        strVertexShaderPath[512];
			LPD3DXBUFFER pCode;

			DWORD dwDecl[] =
			{
				D3DVSD_STREAM(0),
				D3DVSD_REG(D3DVSDE_POSITION,  D3DVSDT_FLOAT2),
				D3DVSD_END()
			};

			// Find the vertex shader file
			DXUtil_FindMediaFile( strVertexShaderPath, _T("Ripple.vsh") );

			// Assemble the vertex shader from the file
			if( FAILED( hr = D3DXAssembleShaderFromFile( strVertexShaderPath, 
														0, NULL, &pCode, NULL ) ) )
				return hr;

			// Create the vertex shader
			hr = m_pd3dDevice->CreateVertexShader( dwDecl, (DWORD*)pCode->GetBufferPointer(),
												&m_dwShader, 0 );
			pCode->Release();
			if( FAILED(hr) )
				return hr;
		}

	InvalidateDeviceObjects:
	    m_pd3dDevice->DeleteVertexShader( m_dwShader );

		*/
