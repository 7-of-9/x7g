#pragma warning( disable : 4183 )	// 'missing return type'

#include "CProceduralRenderObject.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CProceduralRenderObject::CProceduralRenderObject()
	{
	//
	// Base procedural geometry and FVF:
	//
		m_nBaseX7LTs = 0 ;
		m_pX7LTs = NULL ;
		m_pFastLastX7LT = NULL ;
		m_dwFVF = 0 ;
		
	//
	// Render States:
	//
		m_nRSs = 0 ;
		for (int i = 0 ; i < 8 ; i ++)
		{
			m_ntvRS[i].dwNumVertices = 0 ;
			m_ntvRS[i].dwNumIndices = 0 ;

			m_ntvRS[i].pUnlitVertices = NULL ;
			m_ntvRS[i].pLitVertices = NULL ;
			m_ntvRS[i].pLitVertices2Tex = NULL ;
			m_ntvRS[i].pLitVerticesWithNormals = NULL ;
			m_ntvRS[i].pLitVerticesWithNormals2Tex = NULL ;

			m_ntvRS[i].pIndices = NULL ;
			m_ntvRS[i].pMesh = NULL ;
			
			m_ntvRS[i].pVB = NULL ;
			m_ntvRS[i].pIB = NULL ;
				
			m_ntvRS[i].nDSs = 0 ;
			
			ZeroMemory(m_ntvRS[i].dwSS_MinIndex, sizeof(DWORD) * MAX_DS) ;
			ZeroMemory(m_ntvRS[i].dwSS_MaxIndex, sizeof(DWORD) * MAX_DS) ;
			ZeroMemory(m_ntvRS[i].dwSS_NumVerts, sizeof(DWORD) * MAX_DS) ;
			ZeroMemory(m_ntvRS[i].dwSS_StartIndex, sizeof(DWORD) * MAX_DS) ;
			ZeroMemory(m_ntvRS[i].dwSS_PrimCount, sizeof(DWORD) * MAX_DS) ;

			D3DUtil_InitMaterial(m_ntvRS[i].mtrlMaterial, 1, 1, 1) ;
		}

	//
	// D3D resources options:
	//
		m_dwMeshOptions = D3DXMESH_WRITEONLY ;
		m_dwUsageOptions = D3DUSAGE_WRITEONLY ;
		m_bUseD3DXMesh = TRUE ;
		m_bUse32bitIndexesIfAvailable = false ;

	//
	// Bounding box and world matrix:
	//
		m_pBBox = NULL ;
		m_matWorld = g_matIdent ;

	//
	// Memory Usage Monitors:
	//	
		m_dwMem = 0 ;
		m_dwMem += sizeof(CProceduralRenderObject) ;
		m_dwMem_X7LTs = 0 ;
		m_dwMem_IndexedVerts = 0 ;
		m_dwMem_RenderData = 0 ;
		m_dwMem_BBox = 0 ;
	}
	CProceduralRenderObject::~CProceduralRenderObject()
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
			if (m_ntvRS[i].pLitVertices2Tex)
			{
				delete m_ntvRS[i].pLitVertices2Tex ;
				m_ntvRS[i].pLitVertices2Tex = NULL ;
			}
			if (m_ntvRS[i].pLitVerticesWithNormals)
			{
				delete m_ntvRS[i].pLitVerticesWithNormals ;
				m_ntvRS[i].pLitVerticesWithNormals = NULL ;
			}
			if (m_ntvRS[i].pLitVerticesWithNormals2Tex)
			{
				delete m_ntvRS[i].pLitVerticesWithNormals2Tex ;
				m_ntvRS[i].pLitVerticesWithNormals2Tex = NULL ;
			}

			if (m_ntvRS[i].pIndices)
			{
				delete m_ntvRS[i].pIndices ;
				m_ntvRS[i].pIndices = NULL ;
			}
		}
		
		SAFE_DELETE(m_pBBox)
	}

/*
 *
 * Base X7LT Layer:
 *
 */
	X7LT *CProceduralRenderObject::AddX7LT(
											 UNCOLOREDVERTEX	*pVerts,
											 D3DCOLOR			*pColors,
											 D3DCOLOR			*pSpecularColors,
											 unsigned long		 ndxTex,
											 unsigned long		 ndxTex2,
											 float				*pfTU2,
											 float				*pfTV2
										  )
	{
		X7LT *pLast, *pNew ;
		
		m_dwMem_X7LTs += sizeof(X7LT) ;

	//
	// First triangle in the linked list needs special attention:
	//
		if (!m_pX7LTs)
		{
			m_pX7LTs = (X7LT*)malloc(sizeof(X7LT)) ;
			assert( m_pX7LTs );
			m_pX7LTs->pPrevious = NULL ;
			m_pX7LTs->pNext = NULL ;
			m_pX7LTs->bColors = m_pX7LTs->bSpecularColors = FALSE ;
			m_pX7LTs->fTU2[0] = m_pX7LTs->fTU2[1] = m_pX7LTs->fTU2[2] = 0.0f ;
			m_pX7LTs->fTV2[0] = m_pX7LTs->fTV2[1] = m_pX7LTs->fTV2[2] = 0.0f ;
			m_pX7LTs->ndxTex2 = 0 ;
			pNew = m_pX7LTs ;
		}
	//
	// All others follow the same pattern:
	//
		else
		{
			pLast = LastX7LT() ;
			pLast->pNext = (X7LT*)malloc(sizeof(X7LT)) ;
			assert( pLast->pNext );

			pNew = (X7LT*)pLast->pNext ;
			pNew->pPrevious = pLast ;
			pNew->pNext = NULL ;
			pNew->bColors = pNew->bSpecularColors = FALSE ;
			pNew->fTU2[0] = pNew->fTU2[1] = pNew->fTU2[2] = 0.0f ;
			pNew->fTV2[0] = pNew->fTV2[1] = pNew->fTV2[2] = 0.0f ;
			pNew->ndxTex2 = 0 ;
		}
		
	//
	// pNew is now the target address for the new vertex data:
	//
		for (int i = 0 ; i < 3 ; i ++)
		{
			memcpy((UNCOLOREDVERTEX*)&(pNew->Points[i]),
				   (UNCOLOREDVERTEX*)&(pVerts[i]),
					sizeof(UNCOLOREDVERTEX)) ;
			
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

			if (pfTU2)
			{
				pNew->fTU2[i] = pfTU2[i] ;
				pNew->b2Tex = TRUE ;
			}
			if (pfTV2)
			{
				pNew->fTV2[i] = pfTV2[i] ;
				pNew->b2Tex = TRUE ;
			}
		}
		pNew->ndxTex = ndxTex ;
		pNew->ndxTex2 = ndxTex2 ;

		m_pFastLastX7LT = LastX7LT() ;
		m_nBaseX7LTs ++ ;

		return pNew ;
	}
	X7LT *CProceduralRenderObject::LastX7LT()
	{
		X7LT *p ;

		p = m_pX7LTs ;
		while (p->pNext)
			p = (X7LT*)p->pNext ;
		
		return p ;
	}
	X7LT *CProceduralRenderObject::FirstX7LT()
	{
		return m_pX7LTs ;
	}

	void CProceduralRenderObject::TranslateX7LTs(float x, float y, float z)
	{
		X7LT *p ;

		p = m_pX7LTs ;
		while (p)
		{
			for (int i = 0 ; i < 3 ; i ++)
			{
				p->Points[i].x += x ;
				p->Points[i].y += y ;
				p->Points[i].z += z ;
			}
			p = (X7LT*)p->pNext ;
		}
	}
	void CProceduralRenderObject::ScaleX7LTs(float s)
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
	void CProceduralRenderObject::RotateX7LTsY(float n, xVec3* pvO)
	{
		X7LT *p ;

		p = m_pX7LTs ;
		while (p)
		{
			for (int i = 0 ; i < 3 ; i ++)
			{
				xVec3 v ;
				
				v = xVec3(p->Points[i].x, p->Points[i].y, p->Points[i].z) ;
				RotateVectorY(&v, n, pvO) ;
				p->Points[i].x = v.x ;
				p->Points[i].y = v.y ;
				p->Points[i].z = v.z ;

				v = xVec3(p->Points[i].nx, p->Points[i].ny, p->Points[i].nz) ;
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
	HRESULT CProceduralRenderObject::IndexToList(void)
	{
		unsigned	 i, j ;
		X7LT		*pTri ;
		BOOL		 bVertIsAlreadyThere ;

		assert( m_dwFVF != 0 );

		m_pFastLastX7LT = LastX7LT() ;
		GetDistinctRenderStates() ;

	//
	// For each distinct render state, go get all the verts, & index them.
	//
		for (int nRS = 0 ; nRS < m_nRSs ; nRS ++)
		{
			/*m_ntvRS[nRS].bStrip = FALSE ;
			m_ntvRS[nRS].bList  = TRUE  ; */

		//
		// This will alloc. **WAY** too much memory for p[Lit]Vertices, but WTF!
		//
			if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
			{
				m_ntvRS[nRS].pUnlitVertices = new UNCOLOREDVERTEX[m_nBaseX7LTs * 3] ;
				m_dwMem_IndexedVerts += sizeof(UNCOLOREDVERTEX) * m_nBaseX7LTs * 3 ;
			}
			if (m_dwFVF == MYFVF_COLOREDVERTEX)
			{
				m_ntvRS[nRS].pLitVertices = new COLOREDVERTEX[m_nBaseX7LTs * 3] ;
				m_dwMem_IndexedVerts += sizeof(COLOREDVERTEX) * m_nBaseX7LTs * 3 ;
			}
			if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
			{
				m_ntvRS[nRS].pLitVerticesWithNormals = new COLOREDVERTEXWITHNORMAL[m_nBaseX7LTs * 3] ;
				m_dwMem_IndexedVerts += sizeof(COLOREDVERTEXWITHNORMAL) * m_nBaseX7LTs * 3 ;
			}
			if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
			{
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex = new COLOREDVERTEXWITHNORMAL2TEX[m_nBaseX7LTs * 3] ;
				m_dwMem_IndexedVerts += sizeof(COLOREDVERTEXWITHNORMAL2TEX) * m_nBaseX7LTs * 3 ;
			}
			if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
			{
				m_ntvRS[nRS].pLitVertices2Tex = new COLOREDVERTEX2TEX[m_nBaseX7LTs * 3] ;
				m_dwMem_IndexedVerts += sizeof(COLOREDVERTEX2TEX) * m_nBaseX7LTs * 3 ;
			}
			m_ntvRS[nRS].pIndices = new WORD [m_nBaseX7LTs * 3] ;
			m_dwMem_IndexedVerts += sizeof(WORD) * m_nBaseX7LTs * 3 ;

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

		//
		// Build list of drawing subsets -- only 1!
		//
			m_ntvRS[nRS].nDSs = 1 ;
			m_ntvRS[nRS].dwSS_NumVerts[0] = m_ntvRS[nRS].dwNumVertices ;
			m_ntvRS[nRS].dwSS_MinIndex[0] = 0 ;
			m_ntvRS[nRS].dwSS_MaxIndex[0] = m_ntvRS[nRS].dwNumVertices - 1 ;
			m_ntvRS[nRS].dwSS_StartIndex[0] = 0 ;
			m_ntvRS[nRS].dwSS_PrimCount[0] = m_ntvRS[nRS].dwNumIndices / 3 ;
		}

		return S_OK ;
	}
/*
 *
 * IndexToList helper functions.
 *
 */
	void CProceduralRenderObject::GetDistinctRenderStates(void)
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
					/*m_ntvRS[i].nPolys ++ ;*/
					break ;
				}

			if (bTexIsAlreadyThere == FALSE)
			{
				/*m_ntvRS[nDistinctTex].nPolys = 1 ;*/
				m_ntvRS[nDistinctTex].ndxTex = pTri->ndxTex ;
				m_ntvRS[nDistinctTex].ndxTex2 = pTri->ndxTex2 ;
				ndxDistinctTex[nDistinctTex ++] = pTri->ndxTex ;
			}

			pTri = (X7LT*)pTri->pNext ;
		}
		m_nRSs = nDistinctTex ;
	}
	BOOL CProceduralRenderObject::CompareIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS)
	{
		if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
		{
			if (m_ntvRS[nRS].pUnlitVertices[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pUnlitVertices[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pUnlitVertices[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pUnlitVertices[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pUnlitVertices[ndx].tv1 == tri->Points[v].tv1)
				return TRUE ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEX)
		{
			if (m_ntvRS[nRS].pLitVertices[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pLitVertices[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pLitVertices[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pLitVertices[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pLitVertices[ndx].tv1 == tri->Points[v].tv1 &&
				m_ntvRS[nRS].pLitVertices[ndx].color == tri->Colors[v])
				return TRUE ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
		{
			if (m_ntvRS[nRS].pLitVertices2Tex[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].tv1 == tri->Points[v].tv1 &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].tu2 == tri->fTU2[v] &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].tv2 == tri->fTV2[v] &&
				m_ntvRS[nRS].pLitVertices2Tex[ndx].color == tri->Colors[v])
				return TRUE ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
		{
			if (m_ntvRS[nRS].pLitVerticesWithNormals[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].tv1 == tri->Points[v].tv1 &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].nx == tri->Points[v].nx &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].ny == tri->Points[v].ny &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].nz == tri->Points[v].nz &&
				m_ntvRS[nRS].pLitVerticesWithNormals[ndx].color == tri->Colors[v])
				return TRUE ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
		{
			if (m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].x == tri->Points[v].x &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].y == tri->Points[v].y &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].z == tri->Points[v].z &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].tu1 == tri->Points[v].tu1 &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].tv1 == tri->Points[v].tv1 &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].tu2 == tri->fTU2[v] &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].tv2 == tri->fTV2[v] &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].nx == tri->Points[v].nx &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].ny == tri->Points[v].ny &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].nz == tri->Points[v].nz &&
				m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx].color == tri->Colors[v])
				return TRUE ;
		}
		return FALSE ;
	}
	void CProceduralRenderObject::AddIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS)
	{
		if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
		{
			memcpy ((UNCOLOREDVERTEX*)&(m_ntvRS[nRS].pUnlitVertices[ndx]),
					(UNCOLOREDVERTEX*)&tri->Points[v],
					sizeof(UNCOLOREDVERTEX)) ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEX)
		{
			COLOREDVERTEX *lv = &(m_ntvRS[nRS].pLitVertices[ndx]) ;
			lv->color = tri->Colors[v] ;
			lv->specular = tri->SpecularColors[v] ;
			lv->tu1 = tri->Points[v].tu1 ;
			lv->tv1 = tri->Points[v].tv1 ;
			lv->x = tri->Points[v].x ;
			lv->y = tri->Points[v].y ;
			lv->z = tri->Points[v].z ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
		{
			COLOREDVERTEX2TEX *lv = &(m_ntvRS[nRS].pLitVertices2Tex[ndx]) ;
			lv->color = tri->Colors[v] ;
			lv->specular = tri->SpecularColors[v] ;
			lv->tu1 = tri->Points[v].tu1 ;
			lv->tv1 = tri->Points[v].tv1 ;
			lv->tu1 = tri->fTU2[v] ;
			lv->tv1 = tri->fTV2[v] ;
			lv->x = tri->Points[v].x ;
			lv->y = tri->Points[v].y ;
			lv->z = tri->Points[v].z ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
		{
			COLOREDVERTEXWITHNORMAL *lvn = &(m_ntvRS[nRS].pLitVerticesWithNormals[ndx]) ;
			lvn->color = tri->Colors[v] ;
			lvn->specular = tri->SpecularColors[v] ;
			lvn->tu1 = tri->Points[v].tu1 ;
			lvn->tv1 = tri->Points[v].tv1 ;
			lvn->x = tri->Points[v].x ;
			lvn->y = tri->Points[v].y ;
			lvn->z = tri->Points[v].z ;
			lvn->nx = tri->Points[v].nx ;
			lvn->ny = tri->Points[v].ny ;
			lvn->nz = tri->Points[v].nz ;
		}
		if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
		{
			COLOREDVERTEXWITHNORMAL2TEX *lvn = &(m_ntvRS[nRS].pLitVerticesWithNormals2Tex[ndx]) ;
			lvn->color = tri->Colors[v] ;
			lvn->specular = tri->SpecularColors[v] ;
			lvn->tu1 = tri->Points[v].tu1 ;
			lvn->tv1 = tri->Points[v].tv1 ;
			lvn->tu2 = tri->fTU2[v] ;
			lvn->tv2 = tri->fTV2[v] ;
			lvn->x = tri->Points[v].x ;
			lvn->y = tri->Points[v].y ;
			lvn->z = tri->Points[v].z ;
			lvn->nx = tri->Points[v].nx ;
			lvn->ny = tri->Points[v].ny ;
			lvn->nz = tri->Points[v].nz ;
		}
	}

/*
 *
 * D3DX Mesh and PMesh Layer:
 *
 */
	HRESULT CProceduralRenderObject::MakeRenderData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr ;
		
		for (int nRS = 0 ; nRS < m_nRSs ; nRS ++)
		{
			if (m_bUseD3DXMesh)
			{
			//
			// Create an empty mesh:
			//
				hr = D3DXCreateMeshFVF(m_ntvRS[nRS].dwNumIndices / 3,
									   m_ntvRS[nRS].dwNumVertices,
									   m_dwMeshOptions,
									   m_dwFVF,
									   pDev,
									   &m_ntvRS[nRS].pMesh) ;
				assert( !FAILED(hr) );

			//
			// Fill the mesh's VB:
			//
				BYTE *pVerts ;
				hr = m_ntvRS[nRS].pMesh->LockVertexBuffer(0, &pVerts) ;
				assert( !FAILED(hr) );

				if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pUnlitVertices, sizeof(UNCOLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(UNCOLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVertices, sizeof(COLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVertices2Tex, sizeof(COLOREDVERTEX2TEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEX2TEX) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVerticesWithNormals, sizeof(COLOREDVERTEXWITHNORMAL) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEXWITHNORMAL) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVerticesWithNormals2Tex, sizeof(COLOREDVERTEXWITHNORMAL2TEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEXWITHNORMAL2TEX) * m_ntvRS[nRS].dwNumVertices ;
				}

				m_ntvRS[nRS].pMesh->UnlockVertexBuffer() ;

			//
			// Fill the mesh's IB:
			//
				BYTE *pIndices ;
				hr = m_ntvRS[nRS].pMesh->LockIndexBuffer(0, &pIndices) ;
				assert( !FAILED(hr) );

				memcpy(pIndices, m_ntvRS[nRS].pIndices, sizeof(WORD) * m_ntvRS[nRS].dwNumIndices) ;
				m_dwMem_RenderData += sizeof(WORD) * m_ntvRS[nRS].dwNumIndices ;

				m_ntvRS[nRS].pMesh->UnlockIndexBuffer() ;

			//
			// Compute vertex normals from face normals:
			//
			// this function seems to *reverse* the x values of the normals!
			//
				/*if (m_dwFVF == MYFVF_UNCOLOREDVERTEX || m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
				{
					if (FAILED(D3DXComputeNormals(m_ntvRS[nRS].pMesh)))
					{
						Msg("CProceduralRenderObject::MakeRenderData: FAILED(D3DXComputeNormals) == TRUE") ;
						return E_FAIL ;
					}
				}*/
			}
			else
			{
			//
			// Create the VB:
			//
				UINT iFVFSize ;

				if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
					iFVFSize = sizeof(UNCOLOREDVERTEX) ;
				if (m_dwFVF == MYFVF_COLOREDVERTEX)
					iFVFSize = sizeof(COLOREDVERTEX) ;
				if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
					iFVFSize = sizeof(COLOREDVERTEX2TEX) ;
				if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
					iFVFSize = sizeof(COLOREDVERTEXWITHNORMAL) ;
				if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
					iFVFSize = sizeof(COLOREDVERTEXWITHNORMAL2TEX) ;

				hr = pDev->CreateVertexBuffer(m_ntvRS[nRS].dwNumVertices * iFVFSize,
											  m_dwUsageOptions,
											  m_dwFVF,
											  D3DPOOL_MANAGED,
											  &m_ntvRS[nRS].pVB) ;
				assert( !FAILED(hr) );

			//
			// Fill the VB:
			//
				BYTE *pVerts ;
				hr = m_ntvRS[nRS].pVB->Lock(0, 0, &pVerts, 0/*D3DLOCK_DISCARD*/) ;
				assert( !FAILED(hr) );

				if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pUnlitVertices, sizeof(UNCOLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(UNCOLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVertices, sizeof(COLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEX) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVertices2Tex, sizeof(COLOREDVERTEX2TEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEX2TEX) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVerticesWithNormals, sizeof(COLOREDVERTEXWITHNORMAL) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEXWITHNORMAL) * m_ntvRS[nRS].dwNumVertices ;
				}
				if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
				{
					memcpy(pVerts, m_ntvRS[nRS].pLitVerticesWithNormals2Tex, sizeof(COLOREDVERTEXWITHNORMAL2TEX) * m_ntvRS[nRS].dwNumVertices) ;
					m_dwMem_RenderData += sizeof(COLOREDVERTEXWITHNORMAL2TEX) * m_ntvRS[nRS].dwNumVertices ;
				}

				hr = m_ntvRS[nRS].pVB->Unlock() ;					
				assert( !FAILED(hr) );

			//
			// 16-bit Index Buffer:
			//
				// create;
					hr = pDev->CreateIndexBuffer( sizeof(WORD) * m_ntvRS[nRS].dwNumIndices,
												m_dwUsageOptions,
												D3DFMT_INDEX16,
												D3DPOOL_MANAGED,
												&m_ntvRS[nRS].pIB );
					assert( !FAILED(hr) );
				// fill ;
					WORD* pIndices ;
					hr = m_ntvRS[nRS].pIB->Lock(0, 0, (BYTE**)&pIndices, 0 );//D3DLOCK_DISCARD
					assert( !FAILED(hr) );
					//memcpy(pIndices, m_ntvRS[nRS].pIndices, sizeof(WORD) * m_ntvRS[nRS].dwNumIndices) ;
					for( DWORD ndx=0 ; ndx < m_ntvRS[ nRS ].dwNumIndices ; ndx++ )
						pIndices[ ndx ] = m_ntvRS[nRS].pIndices[ ndx ];
					m_dwMem_RenderData += sizeof(WORD) * m_ntvRS[nRS].dwNumIndices ;
					m_ntvRS[nRS].pIB->Unlock() ;

			//
			// 32-bit Index Buffer:
			//
				// create;
					/*hr = pDev->CreateIndexBuffer( sizeof(DWORD) * m_ntvRS[nRS].dwNumIndices,
												  m_dwUsageOptions,
												  D3DFMT_INDEX32,
												  D3DPOOL_MANAGED,
												  &m_ntvRS[nRS].pIB );
					assert( !FAILED(hr) );
				// fill ;
					DWORD* pIndices ;
					hr = m_ntvRS[nRS].pIB->Lock(0, 0, (BYTE**)&pIndices, 0 ); //D3DLOCK_DISCARD
					assert( !FAILED(hr) );
					//memcpy(pIndices, m_ntvRS[nRS].pIndices, sizeof(WORD) * m_ntvRS[nRS].dwNumIndices) ;
					for( DWORD ndx=0 ; ndx < m_ntvRS[ nRS ].dwNumIndices ; ndx++ )
						pIndices[ ndx ] = (DWORD)m_ntvRS[nRS].pIndices[ ndx ];
					m_dwMem_RenderData += sizeof(DWORD) * m_ntvRS[nRS].dwNumIndices ;
					hr = m_ntvRS[nRS].pIB->Unlock() ;
					assert( !FAILED(hr) );*/

			}
		}

		return S_OK ;
	}

#pragma message(" ")
#pragma message("NOTE: BBox creation only happens on geom. for m_ntvRS[0] !")
#pragma message(" ")
	HRESULT CProceduralRenderObject::MakeBoundingBox(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr ;
		xVec3 vBBMin, vBBMax ;
		
		// determine the bounding volume as appropriate.
		if (m_bUseD3DXMesh)
		{
			hr = D3DXComputeBoundingBox
				 (
					m_dwFVF == MYFVF_UNCOLOREDVERTEX ? (PVOID)m_ntvRS[0].pUnlitVertices :
					m_dwFVF == MYFVF_COLOREDVERTEX ? (PVOID)m_ntvRS[0].pLitVertices :
					m_dwFVF == MYFVF_COLOREDVERTEX2TEX ? (PVOID)m_ntvRS[0].pLitVertices2Tex :
					m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL ? (PVOID)m_ntvRS[0].pLitVerticesWithNormals :
					m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX ? (PVOID)m_ntvRS[0].pLitVerticesWithNormals2Tex : NULL,
					m_ntvRS[0].dwNumVertices,
					m_dwFVF,
					&vBBMin,
					&vBBMax
				 );
			assert( !FAILED(hr) );
		}
		else
		{
			vBBMin = m_vMin ;
			vBBMax = m_vMax ;
		}

		m_pBBox = new CBoundingBox(&vBBMin, &vBBMax) ;
		m_dwMem_BBox += m_pBBox->m_dwMem ; 
		
		return S_OK ;
	}

	
/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh and PMesh Layers.
 *
 */
	HRESULT CProceduralRenderObject::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
#ifdef FULL_DBG_TRACE
		ods( "CProceduralRenderObject::RestoreDeviceObjects\n" );
#endif

		HRESULT hr1 = S_OK, hr2 = S_OK ;

		hr1 = MakeRenderData(pD3D, pDev) ;
		hr2 = MakeBoundingBox(pD3D, pDev) ;
		assert( hr1==S_OK && hr2==S_OK );

		m_pBBox->RestoreDeviceObjects( pDev );		// NOTE: in release build this does nothing

		return S_OK ;
	}
	HRESULT	CProceduralRenderObject::InvalidateDeviceObjects()
	{
		// destroy the bounding box
		m_pBBox->InvalidateDeviceObjects() ;
		SAFE_DELETE(m_pBBox)
		m_matWorld = g_matIdent ;

		for (int i = 0 ; i < m_nRSs ; i ++)
		{
			SAFE_RELEASE(m_ntvRS[i].pMesh)
			SAFE_RELEASE(m_ntvRS[i].pVB)
			SAFE_RELEASE(m_ntvRS[i].pIB)
		}

		m_dwMem_RenderData = 0 ;
		m_dwMem_BBox = 0 ;

		return S_OK ;
	}

/*
 *
 * World Matrix Methods:
 *
 */
	void CProceduralRenderObject::WorldRotateX(float r, xVec3 *pv)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationX(&m1, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		m = g_matIdent ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;
	
		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CProceduralRenderObject::WorldRotateY(float r, xVec3 *pv)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationY(&m1, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		m = g_matIdent ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;
	
		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CProceduralRenderObject::WorldRotateZ(float r, xVec3 *pv)
	{
		xMat m, m0, m1, m2 ;
		
		// build desired rotation about point matrix
		D3DXMatrixTranslation(&m0, -pv->x, -pv->y, -pv->z) ;
		D3DXMatrixRotationZ(&m1, r) ;
		D3DXMatrixTranslation(&m2, +pv->x, +pv->y, +pv->z) ;
		m = g_matIdent ;
		D3DXMatrixMultiply(&m, &m, &m0) ;
		D3DXMatrixMultiply(&m, &m, &m1) ;
		D3DXMatrixMultiply(&m, &m, &m2) ;
	
		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}
	void CProceduralRenderObject::WorldTranslate(xVec3 *pv)
	{
		xMat m ;
		
		// build desired translation matrix
		D3DXMatrixTranslation(&m, pv->x, pv->y, pv->z) ;

		// apply to the object's bounding box
		m_pBBox->ApplyMatrix(&m) ;

		// apply to object's world matrix
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m) ;
	}

/*
 *
 * Meta-data:
 *
 */
	CProceduralRenderObject::GetBBoxVisibilityFlags(LPDIRECT3DDEVICE8 pDev, xMatUA *pmatProj, CCamera *pCam, D3DVIEWPORT8 *pVP)
	{
		/*xVec3 vA, vB, vC ;
		xVec3 vpA, vpB, vpC ;
		xMat matView ;
		D3DXPLANE plnFrustrumPlane ;
		
		D3DXPLANE plnNear, plnLeft, plnRight, plnUp, plnDown ;
		int nRS, i ;

		matView = xMat(pCam->m_matView) ;

	//
	// Get viewing frustrum's near plane:
	//
		vpA = xVec3(0, 0, 0) ;
		vpB = xVec3((float)pVP->Width/2, 0, 0) ;
		vpC = xVec3((float)pVP->Width-1, (float)pVP->Height, 0) ;
		D3DXVec3Unproject(&vA, &vpA, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vB, &vpB, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vC, &vpC, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXPlaneFromPoints(&plnNear, &vA, &vB, &vC) ;

	//
	// Get viewing frustrum's left plane:
	//
		vpA = xVec3(0, 0, 0) ;
		vpB = xVec3(0, (float)pVP->Height/2, 0) ;
		vpC = xVec3(0, (float)pVP->Height-1, 0.1f) ;
		D3DXVec3Unproject(&vA, &vpA, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vB, &vpB, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vC, &vpC, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXPlaneFromPoints(&plnLeft, &vA, &vB, &vC) ;

	//
	// Get viewing frustrum's right plane:
	//
		vpA = xVec3((float)pVP->Width-1, 0, 0) ;
		vpB = xVec3((float)pVP->Width-1, (float)pVP->Height/2, 0) ;
		vpC = xVec3((float)pVP->Width-1, (float)pVP->Height-1, 0.1f) ;
		D3DXVec3Unproject(&vA, &vpA, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vB, &vpB, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vC, &vpC, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXPlaneFromPoints(&plnRight, &vA, &vB, &vC) ;

	//
	// Get viewing frustrum's upper plane:
	//
		vpA = xVec3(0, 0, 0) ;
		vpB = xVec3((float)pVP->Width/2, 0, 0.1f) ;
		vpC = xVec3((float)pVP->Width-1, 0, 0) ;
		D3DXVec3Unproject(&vA, &vpA, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vB, &vpB, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vC, &vpC, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXPlaneFromPoints(&plnUp, &vA, &vB, &vC) ;

	//
	// Get viewing frustrum's lower plane:
	//
		vpA = xVec3(0, (float)pVP->Height-1, 0) ;
		vpB = xVec3((float)pVP->Width/2, (float)pVP->Height-1, 0.1f) ;
		vpC = xVec3((float)pVP->Width-1, (float)pVP->Height-1, 0) ;
		D3DXVec3Unproject(&vA, &vpA, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vB, &vpB, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXVec3Unproject(&vC, &vpC, pVP, pmatProj, &matView, &m_matWorld) ; 
		D3DXPlaneFromPoints(&plnDown, &vA, &vB, &vC) ;

	//
	// Loop through all of the vBBox[] vectors and test each against each plane.
	//
		for (i = 0 ; i < 8 ; i ++)
		{
			for (nRS = 0 ; nRS < m_nRSs ; nRS ++)
			{
				if (i==1)
				{
					fDE = D3DXPlaneDotCoord(&plnUp, &m_ntvRS[nRS].vBBox[i]) ;
					fDA = plnUp.a ;
					fDB = plnUp.b ;
					fDC = plnUp.c ;
					fDD = plnUp.d ;
				}

				m_ntvRS[nRS].bBBoxInFrontOfNearFrustrumPlane[i] = D3DXPlaneDotCoord(&plnNear, &m_ntvRS[nRS].vBBox[i]) < 0 ;
				m_ntvRS[nRS].bBBoxRightOfLeftFrustrumPlane[i] = D3DXPlaneDotCoord(&plnLeft, &m_ntvRS[nRS].vBBox[i]) < 0 ;
				m_ntvRS[nRS].bBBoxLeftOfRightFrustrumPlane[i] = D3DXPlaneDotCoord(&plnRight, &m_ntvRS[nRS].vBBox[i]) > 0 ;
				m_ntvRS[nRS].bBBoxBelowUpperFrustrumPlane[i] = D3DXPlaneDotCoord(&plnUp, &m_ntvRS[nRS].vBBox[i]) < 0 ;
				m_ntvRS[nRS].bBBoxAboveLowerFrustrumPlane[i] = D3DXPlaneDotCoord(&plnDown, &m_ntvRS[nRS].vBBox[i]) > 0 ;
				m_ntvRS[nRS].bBBoxInFrontOfNearFrustrumPlane[i] = D3DXPlaneDotCoord(&plnNear, &m_ntvRS[nRS].vBBox[i]) < 0 ;
			}
		}
		
	//
	// Results:
	//
		for (nRS = 0 ; nRS < m_nRSs ; nRS ++)
		{
		//
		// Decide if it's visible or not:
		//
			m_ntvRS[nRS].bBBoxAllLeftOfLeft = TRUE ;
			m_ntvRS[nRS].bBBoxAllRightOfRight = TRUE ;
			m_ntvRS[nRS].bBBoxAllAboveUpper = TRUE ;
			m_ntvRS[nRS].bBBoxAllBelowLower = TRUE ;
			m_ntvRS[nRS].bBBoxAllBehindNear = TRUE ;

			for (i = 0 ; i < 8 ; i ++)
			{
				if (m_ntvRS[nRS].bBBoxRightOfLeftFrustrumPlane[i])
					m_ntvRS[nRS].bBBoxAllLeftOfLeft = FALSE ;
				if (m_ntvRS[nRS].bBBoxLeftOfRightFrustrumPlane[i])
					m_ntvRS[nRS].bBBoxAllRightOfRight = FALSE ;
				if (m_ntvRS[nRS].bBBoxBelowUpperFrustrumPlane[i])
					m_ntvRS[nRS].bBBoxAllAboveUpper = FALSE ;
				if (m_ntvRS[nRS].bBBoxAboveLowerFrustrumPlane[i])
					m_ntvRS[nRS].bBBoxAllBelowLower = FALSE ;
				if (m_ntvRS[nRS].bBBoxInFrontOfNearFrustrumPlane[i])
					m_ntvRS[nRS].bBBoxAllBehindNear = FALSE ;
			}
			
			m_ntvRS[nRS].bVisible = TRUE ;

			if (   m_ntvRS[nRS].bBBoxAllLeftOfLeft
				|| m_ntvRS[nRS].bBBoxAllRightOfRight
				|| m_ntvRS[nRS].bBBoxAllAboveUpper
				|| m_ntvRS[nRS].bBBoxAllBelowLower
				|| m_ntvRS[nRS].bBBoxAllBehindNear)
				m_ntvRS[nRS].bVisible = FALSE ;
		}*/

/*
m_matTransform is the view matrix but I must admit I should have found 
a
better name!

Laury MICHEL

"Well, it all started the day before today.  I remember it just like it 
was
yesturday."
  --Moleman from Sam & Max Hit The Road


-----Message d'origine-----
De : Dominic Morris [mailto:dominicmorris2@yahoo.com]
Envoye : lundi 26 mars 2001 16:46
A : Laury Michel
Objet : RE: how to get view frustrum planes


Laury,

thanks very much for this! is m_matTransform the
object's world matrix, or is it the view matrix, or is
it both multiplied together!?

thanks again,
dom

--- Laury Michel <michel@montecristogames.com> wrote:
> The following works fine for me!
>
> void Camera::ComputeFrustum()
> {
>   xMat matClip;
>   D3DXMatrixMultiply(&matClip, &m_matTransform,
> &m_matProjection);
>
> 	// Extract the RIGHT clipping plane
> 	m_frustumRight=D3DXPLANE(
>     matClip._14 - matClip._11,
>     matClip._24 - matClip._21,
>     matClip._34 - matClip._31,
>     matClip._44 - matClip._41);
>
>   // Normalize it
>   D3DXPlaneNormalize(&m_frustumRight,
> &m_frustumRight);
>
> 	// Extract the LEFT clipping plane
> 	m_frustumLeft=D3DXPLANE(
>     matClip._14 + matClip._11,
>     matClip._24 + matClip._21,
>     matClip._34 + matClip._31,
>     matClip._44 + matClip._41);
>
>   // Normalize it
>   D3DXPlaneNormalize(&m_frustumLeft,
> &m_frustumLeft);
>
> 	// Extract the BOTTOM clipping plane
> 	m_frustumBottom=D3DXPLANE(
>     matClip._14 + matClip._12,
>     matClip._24 + matClip._22,
>     matClip._34 + matClip._32,
>     matClip._44 + matClip._42);
>
>   // Normalize it
>   D3DXPlaneNormalize(&m_frustumBottom,
> &m_frustumBottom);
>
> 	// Extract the TOP clipping plane
> 	m_frustumTop=D3DXPLANE(
>     matClip._14 - matClip._12,
>     matClip._24 - matClip._22,
>     matClip._34 - matClip._32,
>     matClip._44 - matClip._42);
>
>   // Normalize it
>   D3DXPlaneNormalize(&m_frustumTop, &m_frustumTop);
>
> 	// Extract the FAR clipping plane
> 	m_frustumFar=D3DXPLANE(
>     matClip._14 - matClip._13,
>     matClip._24 - matClip._23,
>     matClip._34 - matClip._33,
>     matClip._44 - matClip._43);
>
>   // Normalize it
>   D3DXPlaneNormalize(&m_frustumFar, &m_frustumFar);
>
> 	// Extract the NEAR clipping plane.  This is last
> on purpose (see
> pointinfrustum() for reason)
> 	m_frustumNear=D3DXPLANE(
>     matClip._14 + matClip._13,
>     matClip._24 + matClip._23,
>     matClip._34 + matClip._33,
>     matClip._44 + matClip._43);
>
>   // Normalize it
>   D3DXPlaneNormalize(&m_frustumNear,
> &m_frustumNear);
> }
*/


	//-----------------------------------------------
	//  See if this node is in the view frustrum
	//-----------------------------------------------
		int i, t, nOut[8] ;
		//xMat matView = xMat(pCam->m_matView) ;
		xMat matView = pCam->m_matView ;
		xVec3 v[8] ;
		xVec4 v4[8] ;

		for (i = 0 ; i < 8 ; i ++)
		{
			v[i] = m_pBBox->m_vVerts[i] ;
			//D3DXVec3TransformCoord(&v[i], &v[i], &m_matWorld) ;
			D3DXVec3TransformCoord(&v[i], &v[i], &matView) ;
			D3DXVec3Transform(&v4[i], &v[i], pmatProj) ;
			nOut[i] = 0 ;
		}

		m_bVisible = TRUE ;
		t = 0;
		for ( i = 0; i < 8; i++ )
			if (v4[i].z < 0)
			{
				nOut[i] += 1 ;
				t++ ;
			}
		if ( t >= 8 ) m_bVisible = FALSE ;

		t = 0;
		for ( i = 0; i < 8; i++ )
			if (v4[i].x > v4[i].w)
			{
				nOut[i] += 2 ;
				t++ ;
			}
		if ( t >= 8 ) m_bVisible = FALSE ;

		t = 0;
		for ( i = 0; i < 8; i++ )
			if ( v4[i].x < -v4[i].w )
			{
				nOut[i] += 4 ;
				t++ ;
			}
		if ( t >= 8 ) m_bVisible = FALSE ;

		t = 0;
		for ( i = 0; i < 8; i++ )
			if ( v4[i].y > v4[i].w )
			{
				nOut[i] += 8 ;
				t++;
			}
		if ( t >= 8 ) m_bVisible = FALSE ;

		t = 0;
		for ( i = 0; i < 8; i++ )
			if ( v4[i].y < -v4[i].w )
			{
				nOut[i] += 16 ;
				t++;
			}
		if ( t >= 8 ) m_bVisible = FALSE ;

		// process bbox verts
		/*pDev->GetClipStatus(&m_csVis) ;
		pDev->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE) ;
		pDev->SetStreamSource(0, m_pBBox->m_pVB, sizeof(COLOREDVERTEX)) ;
		pDev->SetIndices(m_pBBox->m_pIB, 0) ;
		pDev->SetVertexShader(MYFVF_COLOREDVERTEX) ;
		if (pDev->ProcessVertices(0, 0, 8, m_pBBox->m_pVBProcessed, 0) != D3D_OK)
			Msg("GetBBoxVisibilityFlags ProcessVertices FAILS") ;
		pDev->GetClipStatus(&m_csVis) ;
		pDev->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE) ;*/
		/*BYTE *pVerts ;
		if (FAILED(m_pBBox->m_pVBProcessed->Lock(0, 0, &pVerts, 0)))
			Msg("GetBBoxVisibilityFlags Lock FAILS") ;
		for (i = 0 ; i < 8 ; i ++) 
		{
			PROCESSEDVERTEX *pV ;
			pV = (PROCESSEDVERTEX*)pVerts ;
			m_vP[i].x = pV->x ;
			m_vP[i].y = pV->y ;
			m_vP[i].z = pV->z ;
			pVerts += sizeof(PROCESSEDVERTEX) ;
		}
		m_pBBox->m_pVBProcessed->Unlock() ;
		
		// decide if all verts are clipped
		int t ; 
		m_bVisible = TRUE ;
		for (i = t = 0 ; i < 8 ; i ++)
			if (m_vP[i].x > (float)pVP->Width) t ++ ;
		if (t == 8) m_bVisible = FALSE ;

		for (i = t = 0 ; i < 8 ; i ++)
			if (m_vP[i].x < 0.0f) t ++ ;
		if (t == 8) m_bVisible = FALSE ;

		for (i = t = 0 ; i < 8 ; i ++)
			if (m_vP[i].y > (float)pVP->Height) t ++ ;
		if (t == 8) m_bVisible = FALSE ;

		for (i = t = 0 ; i < 8 ; i ++)
			if (m_vP[i].y < 0.0f) t ++ ;
		if (t == 8) m_bVisible = FALSE ;*/
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CProceduralRenderObject::Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int nDS, xMatUA* pmatCurWorld, bool bCallerWantsAutoGendTexCoords )
	{
		HRESULT hr ;
		xMat matCombinedWorld ;

		matCombinedWorld = m_matWorld ;
		if (pmatCurWorld)
			D3DXMatrixMultiply(&matCombinedWorld, &matCombinedWorld, pmatCurWorld) ;
		pDev->SetTransform(D3DTS_WORLD, &matCombinedWorld) ;

		for (int i = 0 ; i < m_nRSs ; i ++)
		{
			LPDIRECT3DVERTEXBUFFER8 pVB ;
			LPDIRECT3DINDEXBUFFER8 pIB ;

			if (m_bUseD3DXMesh)
			{
				m_ntvRS[i].pMesh->GetVertexBuffer(&pVB) ;
				m_ntvRS[i].pMesh->GetIndexBuffer(&pIB) ;
			}
			else
			{
				pVB = m_ntvRS[i].pVB ;
				pIB = m_ntvRS[i].pIB ;
			}

			if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
				pDev->SetStreamSource(0, pVB, sizeof(UNCOLOREDVERTEX)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEX)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEX)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEX2TEX)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEXWITHNORMAL)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEXWITHNORMAL2TEX)) ;

			pDev->SetIndices(pIB, 0) ;
			pDev->SetVertexShader(m_dwFVF) ;
			pDev->SetTexture(0, pTex->m_pTex[m_ntvRS[i].ndxTex]) ;
			
			if (m_ntvRS[i].ndxTex2)
			{
			// set for single-pass alpha-blend of two textures;
				if( !bCallerWantsAutoGendTexCoords )
					pDev->SetTexture( 1, pTex->m_pTex[ m_ntvRS[i].ndxTex2 ] );
				
				pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
				pDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

				SetTextureColorStage( pDev, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1,		D3DTA_CURRENT )
				SetTextureColorStage( pDev, 1, D3DTA_TEXTURE, D3DTOP_BLENDDIFFUSEALPHA,	D3DTA_CURRENT )
				SetTextureColorStage( pDev, 2, D3DTA_DIFFUSE, D3DTOP_MODULATE,			D3DTA_CURRENT )
				
			// shit card terrain colour test
			/*
				SetTextureColorStage(pDev, 0, D3DTA_DIFFUSE, D3DTOP_SELECTARG1, D3DTA_CURRENT)
				pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;
			*/
			}
			else
			{
			// set for single texture;
				if( !bCallerWantsAutoGendTexCoords )
					pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

				SetTextureColorStage(pDev, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1,			D3DTA_CURRENT)
				SetTextureColorStage(pDev, 1, D3DTA_DIFFUSE, D3DTOP_MODULATE,			D3DTA_CURRENT)
				SetTextureColorStage(pDev, 2, D3DTA_CURRENT, D3DTOP_DISABLE,			D3DTA_CURRENT)

				SetTextureAlphaStage(pDev, 0, D3DTA_DIFFUSE, D3DTOP_SELECTARG1,			D3DTA_CURRENT)
			}

            pDev->SetMaterial(&m_ntvRS[i].mtrlMaterial) ;
			
		//
		// OK, after much investigation;
		// 
		// NumVertices (DIP() param. 3) is NOT:
		//		The no. of verts to be rendered.
		//		OR
		//		The no. of distinct vertices used in drawn primitives.
		//
		//	It IS:
		//		Starting from MinIndex, the total no.
		//		of sequential vertices in the VB that
		//		the DIP() call needs to care about.
		//		
		//		It doesn't matter whether the verts. 
		//		are indexed by the IB (i.e. rendered);
		//		If the vert. lies in the range (MaxNdx-MinNdx)
		//		then it counts towards NumVertices.
		//
			hr = 
				pDev->DrawIndexedPrimitive(

								D3DPT_TRIANGLELIST,
								m_ntvRS[i].dwSS_MinIndex[nDS],
								m_ntvRS[i].dwSS_MaxIndex[nDS] - 
								m_ntvRS[i].dwSS_MinIndex[nDS] + 1,
								m_ntvRS[i].dwSS_StartIndex[nDS],
								m_ntvRS[i].dwSS_PrimCount[nDS]

										  );
			assert( hr==D3D_OK );
			if( D3D_OK != hr )
			{
				Msg2( "ERR" );
			}

			if (m_bUseD3DXMesh)
			{
				pVB->Release() ;
				pIB->Release() ;
			}

			*pulPolyCount += m_ntvRS[i].dwSS_PrimCount[nDS] ;
		}
		return S_OK ;
	}


	HRESULT CProceduralRenderObject::RenderDetailPass( LPDIRECT3DDEVICE8	pDev,
													   LPDIRECT3DTEXTURE8	pTexA,
													   unsigned long*		pulPolyCount,
													   int					nDS,
													   xMatUA*				pmatCurWorld )
	{
		HRESULT hr ;
		xMat matCombinedWorld ;

		matCombinedWorld = m_matWorld ;
		if (pmatCurWorld)
			D3DXMatrixMultiply(&matCombinedWorld, &matCombinedWorld, pmatCurWorld) ;
		pDev->SetTransform(D3DTS_WORLD, &matCombinedWorld) ;

		for (int i = 0 ; i < m_nRSs ; i ++)
		{
			LPDIRECT3DVERTEXBUFFER8 pVB ;
			LPDIRECT3DINDEXBUFFER8 pIB ;

			if (m_bUseD3DXMesh)
			{
				m_ntvRS[i].pMesh->GetVertexBuffer(&pVB) ;
				m_ntvRS[i].pMesh->GetIndexBuffer(&pIB) ;
			}
			else
			{
				pVB = m_ntvRS[i].pVB ;
				pIB = m_ntvRS[i].pIB ;
			}

			if (m_dwFVF == MYFVF_UNCOLOREDVERTEX)
				pDev->SetStreamSource(0, pVB, sizeof(UNCOLOREDVERTEX)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEX)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEX)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEX2TEX)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEX2TEX)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEXWITHNORMAL)) ;
			if (m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEXWITHNORMAL2TEX)) ;

			pDev->SetIndices(pIB, 0) ;
			pDev->SetVertexShader(m_dwFVF) ;
			pDev->SetTexture(0, pTexA) ;
			
		// set for single texture;
			pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );

			pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			pDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x11ffffff );

			SetTextureColorStage( pDev, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_CURRENT )
			SetTextureColorStage( pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	 D3DTA_CURRENT )
			
			SetTextureAlphaStage( pDev, 0, D3DTA_TFACTOR, D3DTOP_SELECTARG1, D3DTA_CURRENT )
			SetTextureAlphaStage( pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	 D3DTA_CURRENT )

            pDev->SetMaterial(&m_ntvRS[i].mtrlMaterial) ;

		// draw;
			hr = 
				pDev->DrawIndexedPrimitive(
								D3DPT_TRIANGLELIST,
								m_ntvRS[i].dwSS_MinIndex[nDS],
								m_ntvRS[i].dwSS_MaxIndex[nDS] - 
								m_ntvRS[i].dwSS_MinIndex[nDS] + 1,
								m_ntvRS[i].dwSS_StartIndex[nDS],
								m_ntvRS[i].dwSS_PrimCount[nDS]
										  );
			assert( hr==D3D_OK );

			if (m_bUseD3DXMesh)
			{
				pVB->Release() ;
				pIB->Release() ;
			}

		// reset RS;
			/*pDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
			SetTextureColorStage( pDev, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1,		D3DTA_CURRENT )
			SetTextureColorStage( pDev, 1, D3DTA_TEXTURE, D3DTOP_BLENDDIFFUSEALPHA,	D3DTA_CURRENT )
			SetTextureColorStage( pDev, 2, D3DTA_DIFFUSE, D3DTOP_MODULATE,			D3DTA_CURRENT )*/

			*pulPolyCount += m_ntvRS[i].dwSS_PrimCount[nDS] ;
		}
		return S_OK ;
	}


