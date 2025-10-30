#include "CTextRenderObject.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CTextRenderObject::CTextRenderObject(char *pszText, char *pszFontName, DWORD dwFontHeight)
	{
		m_dwFVF = 0 ;
		m_pMesh = NULL ;
		m_dwNumFaces = 0 ;
		m_dwNumVertices = 0 ;
		
		m_szText[0] = '\x0' ;
		m_szFontName[0] = '\x0' ;
		m_dwFontHeight = 0 ;

		m_pBBox = NULL ;

		D3DXMatrixIdentity(&m_matWorld) ;
		D3DUtil_InitMaterial(m_matDef, 1, 1, 1) ;
		
		strcpy(m_szText, pszText) ;
		strcpy(m_szFontName, pszFontName) ;
		m_dwFontHeight = dwFontHeight ;
	}
	CTextRenderObject::~CTextRenderObject()
	{
		InvalidateDeviceObjects() ;
	}

/*
 *
 * D3DX Mesh and PMesh Layer:
 *
 */
	HRESULT CTextRenderObject::MakeRenderDataAndBoundingData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL ;
		HRESULT	  	 hr ;
		LPD3DXMESH   pTempMesh ;
		HDC			 hDC ;
		HFONT		 hFont ;
		INT			 nHeight ;

		hDC = CreateCompatibleDC(NULL) ;

	    nHeight = -MulDiv( m_dwFontHeight, 
        (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * 1.0f), 72 );

		hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE,
                           FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                           VARIABLE_PITCH, m_szFontName) ;
		if (!hFont)
			Msg("CTextRenderObject::MakeMesh_PMesh: NULL==(CreateFont(...)) == TRUE") ;
	    SelectObject( hDC, hFont );

	// create the mesh from the specified string:
		if (FAILED(hr = D3DXCreateTextA(pDev, hDC, m_szText, 0.0f, 1.0f, &pTempMesh, NULL)))
		{
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(D3DXCreateTextA(...)) == TRUE") ;
			return hr ;
		}

		DeleteDC( hDC );
		DeleteObject( hFont );

		/*if (FAILED(hr = D3DXValidMesh(pTempMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
		{
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(D3DXValidMesh(...)) == TRUE") ;

			SAFE_RELEASE(m_pAdjacencyBuffer)
			m_dwNumMaterials = 0L ;
			return E_FAIL ;
		}*/

	// allocate a material array
		/*D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer() ;
		m_mtrlMeshMaterials = new D3DMATERIAL8[m_dwNumMaterials] ;

	// copy the materials
		for (DWORD i = 0 ; i < m_dwNumMaterials ; i++)
		{
			m_mtrlMeshMaterials[i] = d3dxMaterials[i].MatD3D ;
			m_mtrlMeshMaterials[i].Ambient = m_mtrlMeshMaterials[i].Diffuse ;
		}
		pD3DXMtrlBuffer->Release() ;*/

	// get the FVF
		m_dwFVF = pTempMesh->GetFVF() ;

	// get the number of vertices
		m_dwNumVertices = pTempMesh->GetNumVertices() ;

	// get the number of faces
		m_dwNumFaces = pTempMesh->GetNumFaces() ;

	// get the mesh's vertex buffer information
		LPDIRECT3DVERTEXBUFFER8 pVB ;
		if (FAILED(pTempMesh->GetVertexBuffer(&pVB)))
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(GetVertexBuffer(...))") ;
		else
		{
			if (FAILED(pVB->GetDesc(&m_vbdVBDesc)))
				Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(pVB->GetDesc(...))") ;
			pVB->Release() ;
		}

	// allocate a second adjacency buffer to store post attribute sorted adjacency
	    DWORD *rgdwAdjacencyTemp = NULL ;
		rgdwAdjacencyTemp = new DWORD[m_dwNumFaces * 3] ;
		if (rgdwAdjacencyTemp == NULL)
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(new DWORD[...])") ;

	// attribute sort + vertex cache sort
		/*hr = pTempMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
									 (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),
									 rgdwAdjacencyTemp, NULL, NULL) ;
		if (FAILED(hr))
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(OptimizeInplace1(...))") ;*/

	// use readable temp mesh to create bounding box
		MakeBoundingBoxAndSphere(pD3D, pDev, pTempMesh) ;
	
	// clone to global mesh & release
		hr = pTempMesh->CloneMeshFVF(D3DXMESH_WRITEONLY, pTempMesh->GetFVF(), pDev, &m_pMesh) ;
		if (FAILED(hr))
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(CloneMeshFVF(...))") ;
		SAFE_RELEASE(pTempMesh)
			
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
				Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(CloneMeshFVF(...)) == TRUE") ;
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
			Msg("CTextRenderObject::MakeMesh_PMesh: FAILED(D3DXGeneratePMesh(...)) == TRUE") ;
			return hr ;
		}

		m_pPMesh->SetNumVertices(0xffffffff) ;*/

		return S_OK ;
	}

/*
 *
 * Bounding Box & Sphere.
 *
 */
	HRESULT CTextRenderObject::MakeBoundingBoxAndSphere(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, LPD3DXMESH pReadableMesh)
	{
		D3DXVECTOR3 vMin, vMax ;

	// lock VB
		BYTE *pVerts ;
		if (pReadableMesh->LockVertexBuffer(0, &pVerts))
		{
			Msg("CTextRenderObject::MakeBoundingBoxAndSphere: FAILED(pReadableMesh->LockVertexBuffer)") ;
			return E_FAIL ;
		}

	// get bounding volume
		if (FAILED(D3DXComputeBoundingBox(pVerts,
									      m_dwNumVertices,
										  m_dwFVF,
										  &vMin,
										  &vMax)))
		{
			Msg("CTextRenderObject::MakeBoundingBoxAndSphere: FAILED(D3DXComputeBoundingBox)") ;
			pReadableMesh->UnlockVertexBuffer() ;
			return E_FAIL ;
		}

	// get bounding sphere radius
		D3DXVECTOR3 vCenter ;
		if (FAILED(D3DXComputeBoundingSphere(pVerts,
											 m_dwNumVertices,
											 m_dwFVF,
											 &vCenter,
											 &m_fBSphereRadius)))
		{
			Msg("CTextRenderObject::MakeBoundingBoxAndSphere: FAILED(D3DXComputeBoundingSphere)") ;
			pReadableMesh->UnlockVertexBuffer() ;
			return E_FAIL ;
		}

	// initialise bounding box class
		m_pBBox = new CBoundingBox(&vMin, &vMax) ;

	// unlock VB
		pReadableMesh->UnlockVertexBuffer() ;

		return S_OK ;
	}

/*
 *
 * World Matrix Methods.
 *
 */
	void CTextRenderObject::WorldRotateX(float r, D3DXVECTOR3 *pv)
	{
		D3DXMATRIX m, m0, m1, m2 ;
		
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
	void CTextRenderObject::WorldRotateY(float r, D3DXVECTOR3 *pv)
	{
		D3DXMATRIX m, m0, m1, m2 ;
		
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
	void CTextRenderObject::WorldRotateZ(float r, D3DXVECTOR3 *pv)
	{
		D3DXMATRIX m, m0, m1, m2 ;
		
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
	void CTextRenderObject::WorldTranslate(D3DXVECTOR3 *pv)
	{
		D3DXMATRIX m ;
		
		// build desired translation matrix
		D3DXMatrixTranslation(&m, pv->x, pv->y, pv->z) ;

		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CTextRenderObject::WorldRotateAxis(float r, D3DXVECTOR3 *pv, D3DXVECTOR3 *pvAxis)
	{
		D3DXMATRIX m, m0, m1, m2 ;
		
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
	}
	void CTextRenderObject::WorldSetMatrix(D3DXMATRIX *m)
	{

		// apply to the object's bounding box
		m_pBBox->ApplyMatrixFromInitialValues(m) ;

		// apply to object's world matrix
		D3DXMatrixIdentity(&m_matWorld) ;
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m) ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh and PMesh Layers.
 *
 */
	HRESULT CTextRenderObject::RestoreDeviceObjects(LPDIRECT3D8	pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		if (FAILED(MakeRenderDataAndBoundingData(pD3D, pDev)))
		{
			Msg("CTextRenderObject::RestoreDeviceObjects: FAILED(MakeRenderData(...))") ;
			return E_FAIL ;
		}

		/*if (FAILED(MakeBoundingBox(pD3D, pDev)))
		{
			Msg("CTextRenderObject::RestoreDeviceObjects: FAILED(MakeBoundingBox(...))") ;
			return E_FAIL ;
		}*/

		m_pBBox->RestoreDeviceObjects(pD3D, pDev) ;

		return S_OK ;
	}
	HRESULT	CTextRenderObject::InvalidateDeviceObjects()
	{
		SAFE_RELEASE(m_pMesh)
		SAFE_DELETE(m_pBBox)
		m_dwNumVertices = 0 ;
		m_dwNumFaces = 0 ;
		m_dwFVF = 0 ;
		D3DXMatrixIdentity(&m_matWorld) ;

		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CTextRenderObject::Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount)
	{
		pDev->SetTransform(D3DTS_WORLD, &m_matWorld) ;

		pDev->SetMaterial(&m_matDef) ;
		if (m_pMesh)
        {
			m_pMesh->DrawSubset(0) ;
			*pulPolyCount += m_dwNumFaces ;
        }

		return S_OK ;
	}
