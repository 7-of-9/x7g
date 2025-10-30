#include "CRenderObject.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CRenderObject::CRenderObject()
	{
		m_nBaseX7LTs = 0 ;
		m_pX7LTs = NULL ;
		m_dwFVF = 0 ;
		
		m_nRSs = 0 ;
		for (int i = 0 ; i < 8 ; i ++)
		{
			m_ntvRS[i].dwNumVertices = 0 ;
			m_ntvRS[i].dwNumIndices = 0 ;
			m_ntvRS[i].pUnlitVertices = NULL ;
			m_ntvRS[i].pLitVertices = NULL ;
			m_ntvRS[i].pIndices = NULL ;

			m_ntvRS[i].pMesh = NULL ;
			m_ntvRS[i].pMeshAdjacency = NULL ;

			m_ntvRS[i].pPMesh = NULL ;

			m_ntvRS[i].pSPMesh = NULL ;
			m_ntvRS[i].pMesh1 = NULL ;
		}
	}
	CRenderObject::~CRenderObject()
	{
		if (m_pX7LTs)
		{
			X7LT *p, *p2 ;

			p = m_pX7LTs ;
			while (p->pNext)
			{
				p2 = (X7LT*)p->pNext ;
				free(p) ;
				p = p2 ;
			}
			free(p) ;
			m_pX7LTs = NULL ;
		}
		
		for (int i = 0 ; i < m_nRSs ; i ++)
		{
			if (m_ntvRS[i].pUnlitVertices)
			{
				delete m_ntvRS[i].pUnlitVertices ;
				m_ntvRS[i].pUnlitVertices = NULL ;
			}
			if (m_ntvRS[i].pLitVertices)
			{
				delete m_ntvRS[i].pLitVertices ;
				m_ntvRS[i].pLitVertices = NULL ;
			}
			if (m_ntvRS[i].pIndices)
			{
				delete m_ntvRS[i].pIndices ;
				m_ntvRS[i].pIndices = NULL ;
			}
		}
		InvalidateDeviceObjects() ;
	}

/*
 *
 * Base X7LT Layer:
 *
 */
	X7LT *CRenderObject::AddX7LT(
								 UNLITVERTEX	*pVerts,
								 D3DCOLOR		*pColors,
								 D3DCOLOR		*pSpecularColors,
								 unsigned long	 ndxTex,
								 BOOL			 bIncludeInCDNet
								)
	{
		X7LT *pLast, *pNew ;
		
	//
	// First triangle in the linked list needs special attention:
	//
		if (!m_pX7LTs)
		{
			m_pX7LTs = (X7LT*)malloc(sizeof(X7LT)) ;
			if (m_pX7LTs == NULL)
			{
				Msg("alloc failure1") ;
				return NULL ;
			}
			m_pX7LTs->pPrevious = NULL ;
			m_pX7LTs->pNext = NULL ;
			m_pX7LTs->bColors = m_pX7LTs->bSpecularColors = FALSE ;
			pNew = m_pX7LTs ;
		}
	//
	// All others follow the same pattern:
	//
		else
		{
			pLast = LastX7LT() ;
			pLast->pNext = (X7LT*)malloc(sizeof(X7LT)) ;
			if (pLast->pNext == NULL)
			{
				Msg("alloc failure2") ;
				return NULL ;
			}
			pNew = (X7LT*)pLast->pNext ;
			pNew->pPrevious = pLast ;
			pNew->pNext = NULL ;
			pNew->bColors = pNew->bSpecularColors = FALSE ;
		}
		
	//
	// pNew is now the target address for the new vertex data:
	//
		for (int i = 0 ; i < 3 ; i ++)
		{
			memcpy((UNLITVERTEX*)&(pNew->Points[i]),
				   (UNLITVERTEX*)&(pVerts[i]),
					sizeof(UNLITVERTEX)) ;
			
			if (pColors)
			{
				pNew->Colors[i] = pColors[i] ;
				pNew->bColors = TRUE ;
			}

			if (pSpecularColors)
			{		
				pNew->SpecularColors[i] = pSpecularColors[i] ;
				pNew->bSpecularColors = TRUE ;
			}
		}
		pNew->ndxTex = ndxTex ;

		m_pFastLastX7LT = LastX7LT() ;
		m_nBaseX7LTs ++ ;

		return pNew ;
	}
	X7LT *CRenderObject::LastX7LT()
	{
		X7LT *p ;

		p = m_pX7LTs ;
		while (p->pNext)
			p = (X7LT*)p->pNext ;
		
		return p ;
	}
	X7LT *CRenderObject::FirstX7LT()
	{
		return m_pX7LTs ;
	}

	void CRenderObject::ScaleX7LTs(float s)
	{
		X7LT *p ;

		p = m_pX7LTs ;
		while (p)
		{
			for (int i = 0 ; i < 3 ; i ++)
			{
				p->Points[i].x *= s ;
				p->Points[i].y *= s ;
				p->Points[i].z *= s ;
			}
			p = (X7LT*)p->pNext ;
		}
	}
	void CRenderObject::RotateX7LTsY(float n, LPD3DXVECTOR3 pvO)
	{
		X7LT *p ;

		p = m_pX7LTs ;
		while (p)
		{
			for (int i = 0 ; i < 3 ; i ++)
			{
				D3DXVECTOR3 v ;
				
				v = D3DXVECTOR3(p->Points[i].x, p->Points[i].y, p->Points[i].z) ;
				RotateVectorY(&v, n, pvO) ;
				p->Points[i].x = v.x ;
				p->Points[i].y = v.y ;
				p->Points[i].z = v.z ;

				v = D3DXVECTOR3(p->Points[i].nx, p->Points[i].ny, p->Points[i].nz) ;
				RotateVectorY(&v, n, pvO) ;
				p->Points[i].nx = v.x ;
				p->Points[i].ny = v.y ;
				p->Points[i].nz = v.z ;
			}
			p = (X7LT*)p->pNext ;
		}
	}

/*
 *
 * Creates a list of distinct vertices and the indices for drawing as triangle lists.
 *
 */
	HRESULT CRenderObject::IndexToList(void)
	{
		unsigned	 i, j ;
		X7LT		*pTri ;
		BOOL		 bVertIsAlreadyThere ;

	//
	// Sanity check.
	// 
		if (m_dwFVF == 0)
		{
			Msg("CRenderObject::IndexToList(): m_dwFVF == 0") ;
		}

		m_pFastLastX7LT = LastX7LT() ;
		
		GetDistinctRenderStates() ;

	//
	// For each distinct render state, go get all the verts, & index them.
	//
		for (int nRS = 0 ; nRS < m_nRSs ; nRS ++)
		{
			m_ntvRS[nRS].bStrip = FALSE ;
			m_ntvRS[nRS].bList  = TRUE  ; 

		//
		// This will alloc. **WAY** too much memory for p[Lit]Vertices, but WTF!
		//
			if (m_dwFVF == MYFVF_UNLITVERTEX)
				m_ntvRS[nRS].pUnlitVertices = new UNLITVERTEX[m_nBaseX7LTs * 3] ;
			if (m_dwFVF == MYFVF_LITVERTEX)
				m_ntvRS[nRS].pLitVertices = new LITVERTEX[m_nBaseX7LTs * 3] ;
			m_ntvRS[nRS].pIndices = new WORD [m_nBaseX7LTs * 3] ;
		
		//
		// Get distinct vertices, and put into pUnlitVertices:
		//
			pTri = FirstX7LT() ;
			while (pTri)
			{
				if (pTri->ndxTex != m_ntvRS[nRS].ndxTex)
				{
					pTri = (X7LT*)pTri->pNext ;
					continue ;
				}

				for (i = 0 ; i < 3 ; i ++)
				{
				//
				// Do we already have this vertex?
				//
					bVertIsAlreadyThere = FALSE ;
					for (j = 0 ; j < m_ntvRS[nRS].dwNumVertices ; j ++)
						if (CompareIndexedVertex(j, pTri, i, nRS))
						{
							bVertIsAlreadyThere = TRUE ;
							break ;
						}

				//
				// If not, insert it into pUnlitVertices:
				//
					if (bVertIsAlreadyThere == FALSE)
					{
						AddIndexedVertex(m_ntvRS[nRS].dwNumVertices, pTri, i, nRS) ;
						m_ntvRS[nRS].dwNumVertices ++ ;
					}
				}
				pTri = (X7LT*)pTri->pNext ;
			}
			
		//
		// Now we've got all the distinct vertices, go through each triangle's verts.
		// and determine which index to use.
		//
			pTri = FirstX7LT() ;
			while (pTri)
			{
				if (pTri->ndxTex != m_ntvRS[nRS].ndxTex)
				{
					pTri = (X7LT*)pTri->pNext ;
					continue ;
				}
				for (i = 0 ; i < 3 ; i ++)
					for (j = 0 ; j < m_ntvRS[nRS].dwNumVertices ; j ++)
						if (CompareIndexedVertex(j, pTri, i, nRS))
							m_ntvRS[nRS].pIndices[m_ntvRS[nRS].dwNumIndices++] = j ;

				pTri = (X7LT*)pTri->pNext ;
			}
		}

		return S_OK ;
	}
/*
 *
 * IndexToList helper functions.
 *
 */
	void CRenderObject::GetDistinctRenderStates(void)
	{
		X7LT	*pTri ;
		BOOL     bTexIsAlreadyThere ;
		unsigned ndxDistinctTex[8] ;
		unsigned nDistinctTex = 0 ;

		pTri = FirstX7LT() ;
		while (pTri)
		{
			bTexIsAlreadyThere = FALSE ;
			for (unsigned i = 0 ; i < nDistinctTex ; i ++)
				if (pTri->ndxTex == ndxDistinctTex[i])
				{
					bTexIsAlreadyThere = TRUE ;
					m_ntvRS[i].nPolys ++ ;
					break ;
				}

			if (bTexIsAlreadyThere == FALSE)
			{
				m_ntvRS[nDistinctTex].nPolys = 1 ;
				m_ntvRS[nDistinctTex].ndxTex = pTri->ndxTex ;
				ndxDistinctTex[nDistinctTex ++] = pTri->ndxTex ;
			}

			pTri = (X7LT*)pTri->pNext ;
		}
		m_nRSs = nDistinctTex ;
	}
	BOOL CRenderObject::CompareIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS)
	{
		if (m_dwFVF == MYFVF_UNLITVERTEX)
		{
			if (m_ntvRS[nRS].pUnlitVertices[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pUnlitVertices[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pUnlitVertices[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pUnlitVertices[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pUnlitVertices[ndx].tv1 == tri->Points[v].tv1)
				return TRUE ;
		}
		if (m_dwFVF == MYFVF_LITVERTEX)
		{
			if (m_ntvRS[nRS].pLitVertices[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pLitVertices[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pLitVertices[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pLitVertices[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pLitVertices[ndx].tv1 == tri->Points[v].tv1 &&
				m_ntvRS[nRS].pLitVertices[ndx].color == tri->Colors[v])
				return TRUE ;
		}
		return FALSE ;
	}
	void CRenderObject::AddIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS)
	{
		if (m_dwFVF == MYFVF_UNLITVERTEX)
			memcpy ((UNLITVERTEX*)&(m_ntvRS[nRS].pUnlitVertices[ndx]),
					(UNLITVERTEX*)&tri->Points[v],
					sizeof(UNLITVERTEX)) ;

		if (m_dwFVF == MYFVF_LITVERTEX)
		{
			LITVERTEX *lv = &(m_ntvRS[nRS].pLitVertices[ndx]) ;
			lv->color = tri->Colors[v] ;
			lv->specular = tri->SpecularColors[v] ;
			lv->tu1 = tri->Points[v].tu1 ;
			lv->tv1 = tri->Points[v].tv1 ;
			lv->x = tri->Points[v].x ;
			lv->y = tri->Points[v].y ;
			lv->z = tri->Points[v].z ;
		}
	}

/*
 *
 * D3DX Mesh and PMesh Layer:
 *
 */
	HRESULT CRenderObject::MakeMesh_PMesh(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr ;
		
		for (int nRS = 0 ; nRS < m_nRSs ; nRS ++)
		{
		//
		// Create an empty mesh:
		//
			hr = D3DXCreateMeshFVF(m_ntvRS[nRS].dwNumIndices / 3,
								   m_ntvRS[nRS].dwNumVertices,
								   D3DXMESH_WRITEONLY,
								   m_dwFVF,
								   pDev,
								   &m_ntvRS[nRS].pMesh) ;
			if (FAILED(hr))
			{
				m_ntvRS[nRS].pMesh = NULL ;
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(D3DXCreateMeshFVF(...)) == TRUE") ;
				return hr ;
			}

		//
		// Fill the mesh's VB:
		//
			BYTE *pVerts ;
			hr = m_ntvRS[nRS].pMesh->LockVertexBuffer(0/*D3DLOCK_DISCARD*/, &pVerts) ;
			if (FAILED(hr))
			{
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(pMesh->LockVertexBuffer(...)) == TRUE") ;
				return hr ;
			}

			if (m_dwFVF == MYFVF_UNLITVERTEX)
				memcpy(pVerts, m_ntvRS[nRS].pUnlitVertices, sizeof(UNLITVERTEX) * m_ntvRS[nRS].dwNumVertices) ;

			if (m_dwFVF == MYFVF_LITVERTEX)
				memcpy(pVerts, m_ntvRS[nRS].pLitVertices, sizeof(LITVERTEX) * m_ntvRS[nRS].dwNumVertices) ;

			m_ntvRS[nRS].pMesh->UnlockVertexBuffer() ;

		//
		// Fill the mesh's IB:
		//
			BYTE *pIndices ;
			hr = m_ntvRS[nRS].pMesh->LockIndexBuffer(0/*D3DLOCK_DISCARD*/, &pIndices) ;
			if (FAILED(hr))
			{
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(pMesh->LockIndexBuffer(...)) == TRUE") ;
				return hr ;
			}

			memcpy(pIndices, m_ntvRS[nRS].pIndices, sizeof(WORD) * m_ntvRS[nRS].dwNumIndices) ;

			m_ntvRS[nRS].pMesh->UnlockIndexBuffer() ;

		//
		// Get adjacency data:
		//
			m_ntvRS[nRS].pMeshAdjacency = (DWORD*)malloc(sizeof(DWORD) * 3 * (m_ntvRS[nRS].pMesh->GetNumFaces())) ;
			if (!m_ntvRS[nRS].pMeshAdjacency)
			{
				Msg("CRenderObject::MakeMesh_PMesh: (m_ntvRS[nRS].pMeshAdjacency = malloc(...)) == NULL") ;
				return S_FALSE ;
			}

			hr = m_ntvRS[nRS].pMesh->GenerateAdjacency(0.0f, m_ntvRS[nRS].pMeshAdjacency) ;
			if (hr != D3D_OK)
			{
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(GenerateAdjacency(...)) == TRUE") ;
				return hr ;
			}

		//
		// Generate a progressive mesh:
		//
			hr = D3DXGeneratePMesh(m_ntvRS[nRS].pMesh,
								   m_ntvRS[nRS].pMeshAdjacency,
								   NULL,
								   NULL,
								   1/*m_ntvRS[nRS].pMesh->GetNumVertices() - 3*/,
								   D3DXMESHSIMP_VERTEX,
								   &m_ntvRS[nRS].pPMesh) ;
			if (hr != D3D_OK)
			{
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(D3DXGeneratePMesh(...)) == TRUE") ;
				return hr ;
			}

			m_ntvRS[nRS].pPMesh->SetNumVertices(m_ntvRS[nRS].pMesh->GetNumVertices()) ;

		//
		// Generate a simplification mesh:
		//
			hr = D3DXCreateSPMesh(m_ntvRS[nRS].pMesh,
								  m_ntvRS[nRS].pMeshAdjacency,
								  NULL,
								  NULL,
								  &m_ntvRS[nRS].pSPMesh) ;
			if (hr != D3D_OK)
			{
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(D3DXCreateSPMesh(...)) == TRUE") ;
				return hr ;
			}

		//
		// Clone simplification mesh:
		//
			/*m_ntvRS[nRS].pSPMesh->CloneMeshFVF(D3DXMESH_WRITEONLY,
											   m_dwFVF,
											   pDev,
											   m_ntvRS[nRS].pMeshAdjacency,
											   (DWORD*)m_ntvRS[nRS].pIndices,
											   &m_ntvRS[nRS].pMesh1) ;
			if (hr != D3D_OK)
			{
				Msg("CRenderObject::MakeMesh_PMesh: FAILED(pSPMesh->CloneMeshFVF(...)) == TRUE") ;
				return hr ;
			}*/
		}
		
		return S_OK ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh and PMesh Layers.
 *
 */
	HRESULT CRenderObject::RestoreDeviceObjects(LPDIRECT3D8	pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr ;

		if ((hr = MakeMesh_PMesh(pD3D, pDev)) != S_OK)
		{
			Msg("CRenderObject::RestoreDeviceObjects: MakeMesh_PMesh(...) != S_OK") ;
			return hr ;
		}

		return hr ;
	}
	HRESULT	CRenderObject::InvalidateDeviceObjects()
	{
		for (int i = 0 ; i < m_nRSs ; i ++)
		{
			if (m_ntvRS[i].pMesh)
			{
				m_ntvRS[i].pMesh->Release() ;
				m_ntvRS[i].pMesh = NULL ;
			}
			if (m_ntvRS[i].pMeshAdjacency)
			{
				free(m_ntvRS[i].pMeshAdjacency) ;
				m_ntvRS[i].pMeshAdjacency = NULL ;
			}
			if (m_ntvRS[i].pPMesh)
			{
				m_ntvRS[i].pPMesh->Release() ;
				m_ntvRS[i].pPMesh = NULL ;
			}
			if (m_ntvRS[i].pSPMesh)
			{
				m_ntvRS[i].pSPMesh->Release() ;
				m_ntvRS[i].pSPMesh = NULL ;
			}
			if (m_ntvRS[i].pMesh1)
			{
				m_ntvRS[i].pMesh1->Release() ;
				m_ntvRS[i].pMesh1 = NULL ;
			}
		}

		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CRenderObject::Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount)
	{
		for (int i = 0 ; i < m_nRSs ; i ++)
		{
			LPDIRECT3DVERTEXBUFFER8 pVB ;
			LPDIRECT3DINDEXBUFFER8 pIB ;

			m_ntvRS[i].pMesh->GetVertexBuffer(&pVB) ;
			m_ntvRS[i].pMesh->GetIndexBuffer(&pIB) ;

			pDev->SetStreamSource(0, pVB, m_dwFVF == MYFVF_LITVERTEX ? sizeof(LITVERTEX) : sizeof(UNLITVERTEX)) ;
			pDev->SetIndices(pIB, 0) ;

			pDev->SetVertexShader(m_dwFVF) ;
			pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
			pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
			pDev->SetTexture(0, pTex->m_pTex[m_ntvRS[i].ndxTex]) ;
			
			pDev->DrawIndexedPrimitive(
							D3DPT_TRIANGLELIST,
							0,
							m_ntvRS[i].dwNumVertices,
							0,
							m_ntvRS[i].dwNumIndices / 3) ;
			
			pVB->Release() ;
			pIB->Release() ;

			*pulPolyCount += m_ntvRS[i].dwNumIndices / 3 ;
		}
		return S_OK ;
	}


/*
 *
 * Optimized D3D8 VB & IB Layer. Not needed w/ Mesh.
 *
 */
/*
	HRESULT CRenderObject::MakeVBs_IBs(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr ;

		for (int nRS = 0 ; nRS < m_nRSs ; nRS ++)
		{
		//
		// Create & populate a write-only, non-dynamic (driver-optimal, VIDMEM) Vertex Buffer:
		//
			hr = pDev->CreateVertexBuffer(m_ntvRS[nRS].dwNumVertices *
										  (
											m_dwFVF == MYFVF_UNLITVERTEX ? sizeof(UNLITVERTEX) :
			 								m_dwFVF ==   MYFVF_LITVERTEX ? sizeof(LITVERTEX)   :
											0
										  ),
										  D3DUSAGE_WRITEONLY,
										  m_dwFVF,
 										  D3DPOOL_DEFAULT,
										  &(m_ntvRS[nRS].pVB)) ;
			if (FAILED(hr))
			{
				Msg("CRenderObject::MakeVBs_IBs: FAIELD(CreateVertexBuffer(...)) == TRUE") ;
				return hr ;
			}

			UNLITVERTEX *pVB ;
			hr = m_ntvRS[nRS].pVB->Lock(0, 0, (BYTE**)&pVB, 0) ;
			if (FAILED(hr))
			{
				Msg("CRenderObject::MakeVBs_IBs: pVB->Lock != S_OK") ;
				return hr ;
			}
			
			if (m_dwFVF == MYFVF_UNLITVERTEX)
				memcpy(pVB, m_ntvRS[nRS].pUnlitVertices, sizeof(UNLITVERTEX) * m_ntvRS[nRS].dwNumVertices) ;

			if (m_dwFVF == MYFVF_LITVERTEX)
				memcpy(pVB, m_ntvRS[nRS].pLitVertices, sizeof(LITVERTEX) * m_ntvRS[nRS].dwNumVertices) ;

			m_ntvRS[nRS].pVB->Unlock() ;
		
		//
		// Create & populate a write-only, non-dynamic (driver-optimal, VIDMEM) Index Buffer:
		//
			hr = pDev->CreateIndexBuffer(m_ntvRS[nRS].dwNumIndices *
										  (
										  sizeof(WORD)
										  ),
										  D3DUSAGE_WRITEONLY,
										  D3DFMT_INDEX16,
 										  D3DPOOL_DEFAULT,
										  &(m_ntvRS[nRS].pIB)) ;
			if (FAILED(hr))
			{
				Msg("CRenderObject::MakeVBs_IBs: FAILED(CreateIndexBuffer(...)) == TRUE") ;
				return hr ;
			}

			VOID* pIndices ;

			hr = m_ntvRS[nRS].pIB->Lock(0, sizeof(WORD) * m_ntvRS[nRS].dwNumIndices, (BYTE**)&pIndices, 0) ;
			if (FAILED(hr))
			{
				Msg("CRenderObject::MakeVBs_IBs: pIB->Lock != S_OK") ;
				return hr ;
			}

			memcpy(pIndices, m_ntvRS[nRS].pIndices, sizeof(WORD) * m_ntvRS[nRS].dwNumIndices) ;

			m_ntvRS[nRS].pIB->Unlock() ;
		}

		return S_OK ;
	}
*/
