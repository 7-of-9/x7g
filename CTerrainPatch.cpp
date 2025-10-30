#pragma warning( disable : 4183 )	// 'missing return type'

#include "CTerrainPatch.h"



#define _ROWCOLTOINDEX(r,c) ( ((r)*(nPatchSQ+1)) + (c) ) ;

#define _ADD_TRIANGLE(ndxA, ndxB, ndxC)				\
	pRO->m_ntvRS[0].pIndices[nCurIndex++] = ndxA ;	\
	pRO->m_ntvRS[0].pIndices[nCurIndex++] = ndxB ;	\
	pRO->m_ntvRS[0].pIndices[nCurIndex++] = ndxC ;	\
	AddFaceNormalData(pRO, ndxA, ndxB, ndxC) ;

#define _MAINTAIN_MIN_MAX_INDEX_ATOE(ndxA, ndxB, ndxC, ndxD, ndxE, nDS)	\
	if (ndxA < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxA ;	\
	if (ndxB < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxB ;	\
	if (ndxC < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxC ;	\
	if (ndxD < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxD ;	\
	if (ndxE < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxE ;	\
	if (ndxA > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxA ;	\
	if (ndxB > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxB ;	\
	if (ndxC > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxC ;	\
	if (ndxD > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxD ;	\
	if (ndxE > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxE ;	

#define _MAINTAIN_MIN_MAX_INDEX_ATOD(ndxA, ndxB, ndxC, ndxD, nDS)	\
	if (ndxA < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxA ;	\
	if (ndxB < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxB ;	\
	if (ndxC < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxC ;	\
	if (ndxD < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxD ;	\
	if (ndxA > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxA ;	\
	if (ndxB > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxB ;	\
	if (ndxC > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxC ;	\
	if (ndxD > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxD ;	\

#define _MAINTAIN_MIN_MAX_INDEX_ATOC(ndxA, ndxB, ndxC, nDS)	\
	if (ndxA < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxA ;	\
	if (ndxB < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxB ;	\
	if (ndxC < pRO->m_ntvRS[0].dwSS_MinIndex[nDS]) pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = ndxC ;	\
	if (ndxA > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxA ;	\
	if (ndxB > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxB ;	\
	if (ndxC > pRO->m_ntvRS[0].dwSS_MaxIndex[nDS]) pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = ndxC ;	\

/*
 *
 * Construction / Destruction.
 *
 */
	CTerrainPatch::CTerrainPatch( int			nPatchMaxLODSQ,
								  xVec3*		pvTL,
								  float			fWidth,
								  float			fDepth,
								  float*		pfHeightmap,
								  D3DXCOLOR*	pColormap,
								  unsigned long ulNextRowHeightmapDelta,
								  unsigned long ulMaxExtent,
								  float			fMinY,
								  float			fMaxY,
								  xVec3*		pvSpawnPos,
								  float		    fTerrainLenSQ,
								  float			fCherriesReq,
								  float			fShieldPwrUpsReq,
								  float			fEnginePwrUpsReq,
								  float			fWeaponPwrUpsReq,
								  float			fHealthPwrUpsReq,
								  int			nTex1,
								  int			nTex2 )
	{
		int nCurLODPatchSQ ;

	//
	// Memory Usage Monitor:
	//
		m_dwMem = sizeof(CTerrainPatch) ;
		m_dwTotalNumVertices = 0 ;
		m_dwTotalNumIndices = 0 ;

	//
	// Basic Parameters:
	//
		m_nPatchMaxLODSQ = nPatchMaxLODSQ ;
		m_nPatchMaxLODLevels = Pow2( m_nPatchMaxLODSQ ) - 1 ;
		nCurLODPatchSQ = m_nPatchMaxLODSQ ;
		m_fTerrainLenSQ = fTerrainLenSQ ;

	//
	// Items:
	//
		m_pItems = NULL ;
		m_nItems = 0 ;

		m_nCherries = 0 ;
		m_fCherriesReq = fCherriesReq ;
		m_fCherryProb = m_fCherriesReq / (float)((m_nPatchMaxLODSQ-2) * (m_nPatchMaxLODSQ-2)) ;
		
		m_nShieldPwrUps = 0 ;
		m_fShieldPwrUpsReq = fShieldPwrUpsReq ;
		m_fShieldPwrUpProb = m_fShieldPwrUpsReq / (float)((m_nPatchMaxLODSQ-2) * (m_nPatchMaxLODSQ-2)) ;

		m_nEnginePwrUps = 0 ;
		m_fEnginePwrUpsReq = fEnginePwrUpsReq ;
		m_fEnginePwrUpProb = m_fEnginePwrUpsReq / (float)((m_nPatchMaxLODSQ-2) * (m_nPatchMaxLODSQ-2)) ;

		m_nWeaponPwrUps = 0 ;
		m_fWeaponPwrUpsReq = fWeaponPwrUpsReq ;
		m_fWeaponPwrUpProb = m_fWeaponPwrUpsReq / (float)((m_nPatchMaxLODSQ-2) * (m_nPatchMaxLODSQ-2)) ;

		m_nHealthPwrUps = 0 ;
		m_fHealthPwrUpsReq = fHealthPwrUpsReq ;
		m_fHealthPwrUpProb = m_fHealthPwrUpsReq / (float)((m_nPatchMaxLODSQ-2) * (m_nPatchMaxLODSQ-2)) ;

		m_fItemsReq = m_fCherriesReq +
					  m_fShieldPwrUpsReq + 
					  m_fEnginePwrUpsReq + 
					  m_fWeaponPwrUpsReq +
					  m_fHealthPwrUpsReq ;

		m_nItemsActuallyAllocated = (int)ceil( m_fItemsReq ) + 10 ;	//KLUDGE: +10
		m_pItems = new GAMEITEM[ m_nItemsActuallyAllocated ];
		assert( m_pItems );
	
	//
	// Allocate dynamic memory array:
	// 
		m_pPatchLOD = new CProceduralRenderObject*[ m_nPatchMaxLODLevels ];
		assert( m_pPatchLOD );

	//
	// Create all LODs for this patch:
	//
		for (int i = 0 ; i < m_nPatchMaxLODLevels ; i ++)
		{
		// create a new render object:
			m_pPatchLOD[i] = NULL ;
			m_pPatchLOD[i] = new CProceduralRenderObject ;
			assert( m_pPatchLOD[i] );
			m_pPatchLOD[i]->m_dwFVF = MYFVF_COLOREDVERTEXWITHNORMAL/*2TEX*/ ;
			m_pPatchLOD[i]->m_nRSs = 1 ;
			m_pPatchLOD[i]->m_bUseD3DXMesh = FALSE ;
			m_pPatchLOD[i]->m_bUse32bitIndexesIfAvailable = true ;

		// make patch without using X7LTs layer:
			CreateFastPatch(
							nCurLODPatchSQ,
							pvTL,
							fWidth,
							fDepth,
							m_pPatchLOD[i],
							pfHeightmap,
							pColormap,
							ulNextRowHeightmapDelta,
							ulMaxExtent,
							fMinY,
							fMaxY,
							i > 0,
							(i==0) ? pvSpawnPos : NULL,
							nTex1, nTex2
						   ) ;
			
			nCurLODPatchSQ /= 2 ;
		}
	}
	CTerrainPatch::~CTerrainPatch()
	{
		for (int i = 0 ; i < m_nPatchMaxLODLevels ; i ++)
			SAFE_DELETE( m_pPatchLOD[i] ) 

		SAFE_DELETE_ARRAY( m_pPatchLOD )
		SAFE_DELETE_ARRAY( m_pItems )
	}
	DWORD CTerrainPatch::QueryMemUsage()
	{
		DWORD dwMem = m_dwMem ;
		
		for (int i = 0 ; i < m_nPatchMaxLODLevels ; i ++)
			dwMem += (m_pPatchLOD[i]->m_dwMem_IndexedVerts + 
					  m_pPatchLOD[i]->m_dwMem_RenderData + 
					  m_pPatchLOD[i]->m_dwMem_X7LTs + 
					  m_pPatchLOD[i]->m_dwMem + 
					  m_pPatchLOD[i]->m_dwMem_BBox) ;
		
		return dwMem ;
	}

/*
 *
 * Terrain Generator.
 *
 */
	HRESULT CTerrainPatch::CreateFastPatch( int nPatchSQ, xVec3* pvTL, float fWidth, float fDepth, CProceduralRenderObject *pRO, float *pfHeightmap, D3DXCOLOR* pColormap, unsigned long ulNextRowHeightmapDelta, unsigned long ulMaxExtent, float fMinY, float fMaxY, BOOL bBuildEdgeConnectors, xVec3* pvSpawnPos, int nTex1, int nTex2 )
	{
	// items can effectively increase the patch's bounding vol. max. Y;
		float fItemMaxY = 0.0f ;

	// save whether spawn pos generation is required
		BOOL bSpawnPosRequired = ( pvSpawnPos != NULL );
		BOOL bSpawnPosDone = FALSE ;

	// save start point in height & color map
		float *pfHMStart = pfHeightmap ;
		D3DXCOLOR *pCMStart = pColormap ;

	// for main evenly spaced subset including edges at LOD
		unsigned nQuads = nPatchSQ * nPatchSQ ;
		unsigned nVerts = (nPatchSQ + 1) * (nPatchSQ + 1) ;
		unsigned nIndices = nQuads * 2 * 3 ;
	
	// 4 sides of patch at LOD+1
		unsigned nEdgeConnectorVerts = nPatchSQ * 4 ;
	// two tris in each corner = 12 indices
	// for the rest, each quad is drawn as three tris = 9 indices
		unsigned nEdgeConnectorIndices = (12 + (nPatchSQ - 2) * 9) * 4 ;
		
	// true total no. of verts & indices
		unsigned nTotalVerts, nTotalIndices ;

	// bounding volume
		xVec3 vMin, vMax ;

	// default to a patch with no edge connectors at LOD+1
		nTotalVerts = nVerts ;
		nTotalIndices = nIndices ;

	//
	// Allocate extra room for edge connectors:
	//
		if (bBuildEdgeConnectors)
		{
			nTotalVerts += nEdgeConnectorVerts ;
			nTotalIndices += nEdgeConnectorIndices ;
		}
		
	//
	// Allocate memory for vertices and indices:
	//
		pRO->m_ntvRS[0].ndxTex = nTex1 ;
		pRO->m_ntvRS[0].ndxTex2 = nTex2 ;

		pRO->m_ntvRS[0].dwNumVertices = nTotalVerts ;
		pRO->m_ntvRS[0].dwNumIndices = nTotalIndices ;

	//
	// Memory Usage Monitor:
	//
		m_dwTotalNumVertices += pRO->m_ntvRS[0].dwNumVertices ;
		m_dwTotalNumIndices += pRO->m_ntvRS[0].dwNumIndices ;

		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEX)
		{
			pRO->m_ntvRS[0].pLitVertices = new COLOREDVERTEX[pRO->m_ntvRS[0].dwNumVertices] ;
			pRO->m_dwMem_IndexedVerts += sizeof(COLOREDVERTEX) * pRO->m_ntvRS[0].dwNumVertices ;
			assert( pRO->m_ntvRS[0].pLitVertices );
		}
		if (pRO->m_dwFVF == MYFVF_UNCOLOREDVERTEX)
		{
			pRO->m_ntvRS[0].pUnlitVertices = new UNCOLOREDVERTEX[pRO->m_ntvRS[0].dwNumVertices] ;
			pRO->m_dwMem_IndexedVerts += sizeof(UNCOLOREDVERTEX) * pRO->m_ntvRS[0].dwNumVertices ;
			assert( pRO->m_ntvRS[0].pUnlitVertices );
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
		{
			pRO->m_ntvRS[0].pLitVerticesWithNormals = new COLOREDVERTEXWITHNORMAL[pRO->m_ntvRS[0].dwNumVertices] ;
			pRO->m_dwMem_IndexedVerts += sizeof(COLOREDVERTEXWITHNORMAL) * pRO->m_ntvRS[0].dwNumVertices ;
			assert( pRO->m_ntvRS[0].pLitVerticesWithNormals );
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
		{
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex = new COLOREDVERTEXWITHNORMAL2TEX[pRO->m_ntvRS[0].dwNumVertices] ;
			pRO->m_dwMem_IndexedVerts += sizeof(COLOREDVERTEXWITHNORMAL2TEX) * pRO->m_ntvRS[0].dwNumVertices ;
			assert( pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex );
		}

		pRO->m_ntvRS[0].pIndices = new WORD[pRO->m_ntvRS[0].dwNumIndices] ;
		pRO->m_dwMem_IndexedVerts += sizeof(WORD) * pRO->m_ntvRS[0].dwNumIndices ;
		assert( pRO->m_ntvRS[0].pIndices );

	//
	// Build list of distinct vertices (uniform spaced grid):
	//
		int   nX = 0, nY = 0 ;
		float fX, fY ;
		for (unsigned nVert = 0 ; nVert < nVerts ; nVert ++)
		{
			float r, g, b, a ;

			int nHeightmapDelta = m_nPatchMaxLODSQ / nPatchSQ ;

			if (nVert % (nPatchSQ + 1) == 0)
			{
				nX = 0 ;
				if (nVert > 0)
				{
					nY++ ;
					// drop down one row in the heightmap:
					pfHeightmap += (ulNextRowHeightmapDelta + (ulMaxExtent * (nHeightmapDelta-1)) ) ;
					pColormap += (ulNextRowHeightmapDelta + (ulMaxExtent * (nHeightmapDelta-1)) ) ;
				}
			}
			else
				nX++ ;

			fX = (float)nX / (float)nPatchSQ ;
			fY = (float)nY / (float)nPatchSQ ;

		// assign colours
			r = pColormap->r ; g = pColormap->g ; b = pColormap->b ; a = pColormap->a ;
			//TerrainVertexColours(&r, &g, &b, &a, *pfHeightmap, fMaxY, fMinY) ;

			float fVertexX = pvTL->x + fX * fWidth,
				  fVertexY = *pfHeightmap,
				  fVertexZ = pvTL->z + fY * fDepth ;

		// we must supply bounding box data manually for non-D3DXMesh Render Objects:
			if (nVert == 0)
				vMin = vMax = xVec3(fVertexX, fVertexY, fVertexZ) ;
			if (fVertexX > vMax.x) vMax.x = fVertexX ;
			if (fVertexY > vMax.y) vMax.y = fVertexY ;
			if (fVertexZ > vMax.z) vMax.z = fVertexZ ;

			if (fVertexX < vMin.x) vMin.x = fVertexX ;
			if (fVertexY < vMin.y) vMin.y = fVertexY ;
			if (fVertexZ < vMin.z) vMin.z = fVertexZ ;
			
		// add the vertex to the render object
			AddVertex(pRO, nVert, fVertexX, fVertexY, fVertexZ, fX, fY, a, r, g, b) ;

		//
		// Items:
		//
			if( m_nPatchMaxLODSQ == nPatchSQ && 
				nX > 0 && nX < nPatchSQ-1 && nY > 0 && nY < nPatchSQ-1 )
			{
			// cherries
				if( rnd() < m_fCherryProb )
					if( (float)m_nCherries < m_fCherriesReq )
					{
						m_pItems[ m_nItems ].nType = IT_CHERRY ;
						m_pItems[ m_nItems ].fRadius = 1.0f ;
						m_pItems[ m_nItems ].vPos = xVec3( fVertexX, fVertexY + m_pItems[ m_nItems ].fRadius, fVertexZ );
//ods( "dropped a cherry @(%0.1f, %0.1f, %0.1f)\n", m_pItems[ m_nItems ].vPos.x, m_pItems[ m_nItems ].vPos.y, m_pItems[ m_nItems ].vPos.z );
						m_pItems[ m_nItems ].fRot = 0 ;
						m_pItems[ m_nItems ].fShine = 0 ;
						m_pItems[ m_nItems ].fShineModSpeed = 0.75f ;
						m_pItems[ m_nItems ].fX1 = 0 ;
						m_pItems[ m_nItems ].fY1 = 0 ;
						m_pItems[ m_nItems ].fZ1 = 0 ;
						if( m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius > fItemMaxY )
							fItemMaxY = m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius ;
						m_nCherries++ ;
						m_nItems++ ;
						assert( m_nItems <= m_nItemsActuallyAllocated );
					}

			// shield pwr ups
				if( rnd() < m_fShieldPwrUpProb )
					if( (float)m_nShieldPwrUps < m_fShieldPwrUpsReq )
					{
						m_pItems[ m_nItems ].nType = IT_SHIELD ;
						m_pItems[ m_nItems ].fRadius = 1.0f ; //rnd() * 4.0f ;
						m_pItems[ m_nItems ].vPos = xVec3( fVertexX, fVertexY, fVertexZ );
						m_pItems[ m_nItems ].fRot = 0 ;
						m_pItems[ m_nItems ].fShine = 1 ;
						m_pItems[ m_nItems ].fShineModSpeed = 0 ;
						m_pItems[ m_nItems ].fX1 = 0 ;
						m_pItems[ m_nItems ].fY1 = 0 ;
						m_pItems[ m_nItems ].fZ1 = 0 ;
						if( m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius > fItemMaxY )
							fItemMaxY = m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius ;
						m_nShieldPwrUps++ ;
						m_nItems++ ;
						assert( m_nItems <= m_nItemsActuallyAllocated );
					}

			// engine pwr ups
				if( rnd() < m_fEnginePwrUpProb )
					if( (float)m_nEnginePwrUps < m_fEnginePwrUpsReq )
					{
						m_pItems[ m_nItems ].nType = IT_ENGINE ;
						m_pItems[ m_nItems ].fRadius = 1.0f ; //rnd() * 4.0f ;
						m_pItems[ m_nItems ].vPos = xVec3( fVertexX, fVertexY, fVertexZ );
						m_pItems[ m_nItems ].fRot = 0 ;
						m_pItems[ m_nItems ].fShine = 0 ;
						m_pItems[ m_nItems ].fShineModSpeed = 0.25f ;
						m_pItems[ m_nItems ].fX1 = 0 ;
						m_pItems[ m_nItems ].fY1 = 0 ;
						m_pItems[ m_nItems ].fZ1 = 0 ;
						if( m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius > fItemMaxY )
							fItemMaxY = m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius ;
						m_nEnginePwrUps++ ;
						m_nItems++ ;
						assert( m_nItems <= m_nItemsActuallyAllocated );
					}

			// weapon pwr ups
				if( rnd() < m_fWeaponPwrUpProb )
					if( (float)m_nWeaponPwrUps < m_fWeaponPwrUpsReq )
					{
						m_pItems[ m_nItems ].nType = IT_WEAPON ;
						m_pItems[ m_nItems ].fRadius = 1.0f ; //rnd() * 4.0f ;
						m_pItems[ m_nItems ].vPos = xVec3( fVertexX, fVertexY, fVertexZ );
						m_pItems[ m_nItems ].fRot = 0 ;
						m_pItems[ m_nItems ].fShine = 0 ;
						m_pItems[ m_nItems ].fShineModSpeed = 0.25f ;
						m_pItems[ m_nItems ].fX1 = 0 ;
						m_pItems[ m_nItems ].fY1 = 0 ;
						m_pItems[ m_nItems ].fZ1 = 0 ;
						if( m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius > fItemMaxY )
							fItemMaxY = m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius ;
						m_nWeaponPwrUps++ ;
						m_nItems++ ;
						assert( m_nItems <= m_nItemsActuallyAllocated );
					}

			// health pwr ups
				if( rnd() < m_fHealthPwrUpProb )
					if( (float)m_nHealthPwrUps < m_fHealthPwrUpsReq )
					{
						m_pItems[ m_nItems ].nType = IT_HEALTH ;
						m_pItems[ m_nItems ].fRadius = 1.0f ; //rnd() * 4.0f ;
						m_pItems[ m_nItems ].vPos = xVec3( fVertexX, fVertexY, fVertexZ );
						m_pItems[ m_nItems ].fRot = 0 ;
						m_pItems[ m_nItems ].fShine = 1 ;
						m_pItems[ m_nItems ].fShineModSpeed = 0.5f ;
						m_pItems[ m_nItems ].fX1 = 0 ;
						m_pItems[ m_nItems ].fY1 = 0 ;
						m_pItems[ m_nItems ].fZ1 = 0 ;
						if( m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius > fItemMaxY )
							fItemMaxY = m_pItems[ m_nItems ].vPos.y + m_pItems[ m_nItems ].fRadius ;
						m_nHealthPwrUps++ ;
						m_nItems++ ;
						assert( m_nItems <= m_nItemsActuallyAllocated );
					}
			}

			if (nHeightmapDelta > 1)
			{
				// we need to be +*one* over the vertex in the code above, in order
				// to drop one full row correctly. this ensures that we are exactly
				// one over, when we're striding > 1:
				if ((nVert+1) % (nPatchSQ+1) == 0)
				{
					pfHeightmap ++ ;
					pColormap ++ ;
				}
				else
				{
					pfHeightmap += nHeightmapDelta ;
					pColormap += nHeightmapDelta ;
				}
			}
			else
			{
				pfHeightmap += nHeightmapDelta ;
				pColormap += nHeightmapDelta ;
			}
		}

	//
	// Build list of additional LOD+1 edge vertices (tagged onto the end
	// of the main list of vertices) - edge connectors:
	//
		if (bBuildEdgeConnectors)
		{
			//char sz[256] ;
			//sprintf(sz, "Building edge connectors, on patch sq. %d", nPatchSQ) ;
			//Msg(sz) ;

			int nHMDelta = m_nPatchMaxLODSQ / nPatchSQ, i ;
			float* pfHM, fX, fY, r, g, b, a, fVertexX, fVertexY, fVertexZ ;
			D3DXCOLOR* pCM ;

		// top edge
			pfHM = pfHMStart + nHMDelta / 2 ;
			pCM = pCMStart + nHMDelta / 2 ;
			for (i = 0 ; i < nPatchSQ ; i ++)
			{
				fX = (float)i / (float)nPatchSQ + 1.0f / (float)(nPatchSQ * 2) ;
				fY = 0.0f ;
				fVertexX = pvTL->x + fX * fWidth ;
				fVertexY = *pfHM ;
				fVertexZ = pvTL->z + fY * fDepth ;
				r = pCM->r ; g = pCM->g ; b = pCM->b ; a = pCM->a ;
				//TerrainVertexColours(&r, &g, &b, &a, *pfHM, fMaxY, fMinY) ;
				AddVertex(pRO, nVert++, fVertexX, fVertexY, fVertexZ, fX, fY, a, r, g, b) ;
				pfHM += nHMDelta ;
				pCM += nHMDelta ;
			}
		// left edge
			pfHM = pfHMStart + (ulMaxExtent * (nHMDelta / 2)) ;
			pCM = pCMStart + (ulMaxExtent * (nHMDelta / 2)) ;
			for (i = 0 ; i < nPatchSQ ; i ++)
			{
				fX = 0.0f ;
				fY = (float)i / (float)nPatchSQ + 1.0f / (float)(nPatchSQ * 2) ;
				fVertexX = pvTL->x + fX * fWidth ;
				fVertexY = *pfHM ;
				fVertexZ = pvTL->z + fY * fDepth ;
				r = pCM->r ; g = pCM->g ; b = pCM->b ; a = pCM->a ;
				//TerrainVertexColours(&r, &g, &b, &a, *pfHM, fMaxY, fMinY) ;
				AddVertex(pRO, nVert++, fVertexX, fVertexY, fVertexZ, fX, fY, a, r, g, b) ;
				pfHM += (ulMaxExtent * nHMDelta) ;
				pCM += (ulMaxExtent * nHMDelta) ;
			}
		// bottom edge
			pfHM = pfHMStart + (ulMaxExtent * m_nPatchMaxLODSQ) + nHMDelta / 2 ;
			pCM = pCMStart + (ulMaxExtent * m_nPatchMaxLODSQ) + nHMDelta / 2 ;
			for (i = 0 ; i < nPatchSQ ; i ++)
			{
				fX = (float)i / (float)nPatchSQ + 1.0f / (float)(nPatchSQ * 2) ;
				fY = 1.0f ;
				fVertexX = pvTL->x + fX * fWidth ;
				fVertexY = *pfHM ;
				fVertexZ = pvTL->z + fY * fDepth ;
				r = pCM->r ; g = pCM->g ; b = pCM->b ; a = pCM->a ;
				//TerrainVertexColours(&r, &g, &b, &a, *pfHM, fMaxY, fMinY) ;
				AddVertex(pRO, nVert++, fVertexX, fVertexY, fVertexZ, fX, fY, a, r, g, b) ;
				pfHM += nHMDelta ;
				pCM += nHMDelta ;
			}
		// right edge
			pfHM = pfHMStart + (ulMaxExtent * (nHMDelta / 2)) + m_nPatchMaxLODSQ ;
			pCM = pCMStart + (ulMaxExtent * (nHMDelta / 2)) + m_nPatchMaxLODSQ ;
			for (i = 0 ; i < nPatchSQ ; i ++)
			{
				fX = 1.0f ;
				fY = (float)i / (float)nPatchSQ + 1.0f / (float)(nPatchSQ * 2) ;
				fVertexX = pvTL->x + fX * fWidth ;
				fVertexY = *pfHM ;
				fVertexZ = pvTL->z + fY * fDepth ;
				r = pCM->r ; g = pCM->g ; b = pCM->b ; a = pCM->a ;
				//TerrainVertexColours(&r, &g, &b, &a, *pfHM, fMaxY, fMinY) ;
				AddVertex(pRO, nVert++, fVertexX, fVertexY, fVertexZ, fX, fY, a, r, g, b) ;
				pfHM += (ulMaxExtent * nHMDelta) ;
				pCM += (ulMaxExtent * nHMDelta) ;
			}			
		}

	//
	// Supply bounding volume data to render object (necessary for ROs that don't use D3DXMesh):
	//
		pRO->m_vMin = vMin ;
		pRO->m_vMax = vMax ;
	//
	// gameitems effectively increase the max. vector; affects frustrum culling.
	// this effectively rebounds the patche's bbox to include the items!
	//
		if( fItemMaxY > pRO->m_vMax.y )
			pRO->m_vMax.y = fItemMaxY ;

	//
	// Set the no. of drawing subsets:
	//
		pRO->m_ntvRS[0].nDSs = 6 ;

	//
	// Special DS, all in one go!
	//
		pRO->m_ntvRS[0].dwSS_StartIndex[DS_ALL] = 0 ;
		pRO->m_ntvRS[0].dwSS_MinIndex[DS_ALL] = 0 ;
		pRO->m_ntvRS[0].dwSS_MaxIndex[DS_ALL] = nVerts - 1 ;
		pRO->m_ntvRS[0].dwSS_PrimCount[DS_ALL] = nQuads * 2 ;
		pRO->m_ntvRS[0].dwSS_NumVerts[DS_ALL] = nVerts	;
		
	//
	// Assemble tri. lists of indices for main uniform grid:
	//
		DWORD nCurIndex = 0 ;
		DWORD nMaxVert = nVert ;
		for (int nDS = DS_CENTER ; nDS <= DS_RIGHT_EDGE ; nDS ++)
		{
		// maintain Drawing Subset data:
			pRO->m_ntvRS[0].dwSS_StartIndex[nDS] = nCurIndex ;
			pRO->m_ntvRS[0].dwSS_MinIndex[nDS] = nMaxVert ;
			pRO->m_ntvRS[0].dwSS_MaxIndex[nDS] = 0 ;
			pRO->m_ntvRS[0].dwSS_PrimCount[nDS] = 0 ;
			switch (nDS)
			{
				case DS_CENTER:		pRO->m_ntvRS[0].dwSS_NumVerts[nDS] = (nPatchSQ - 1) * (nPatchSQ - 1) ; break ;
				case DS_TOP_EDGE:	
				case DS_LEFT_EDGE:
				case DS_BOTTOM_EDGE:
				case DS_RIGHT_EDGE: pRO->m_ntvRS[0].dwSS_NumVerts[nDS] = (nPatchSQ + 1) + (nPatchSQ - 2) + 1 ;
			}
			
			for (int nRow = 0 ; nRow <= nPatchSQ ; nRow ++)
			{
				nVert = nRow * (nPatchSQ + 1) ;

				for (int nCol = 0 ; nCol < nPatchSQ ; nCol ++)
				{
					WORD ndx1, ndx2, ndx3, ndxA, ndxB, ndxC ;
					
				// determine if this point is in the current subset
					BOOL bPointIsInDSForUpBuild = 1, bPointIsInDSForDownBuild = 1 ;
					
					switch (nDS)
					{
						case DS_CENTER:
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = FALSE ;
							if (nRow == 0 || nCol == 0 || nRow == nPatchSQ)	break ;			// trivially reject all extreme edge points
								
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = TRUE ;
							if (nRow == 1) bPointIsInDSForUpBuild = FALSE ;					// reject top points for upbuilds
							if (nRow == nPatchSQ - 1) bPointIsInDSForDownBuild = FALSE ;	// reject bottom points of downbuilds
							if (nCol == nPatchSQ - 1) bPointIsInDSForDownBuild = bPointIsInDSForUpBuild = FALSE ;// reject right points for upbuilds and downbuilds
							break ;
						
						case DS_TOP_EDGE:
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = FALSE ;
							if (nRow > 1) break ;											// trivially reject all points below second row
							if (nRow == 1 && nCol == 0) break ;
							
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = TRUE ;
							if (nRow == 1) bPointIsInDSForDownBuild = FALSE ;
							if (nRow == 1 && nCol == nPatchSQ - 1) bPointIsInDSForUpBuild = FALSE ;
							break ;

						case DS_LEFT_EDGE:
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = FALSE ;
							if (nCol > 0) break ;											// trivially reject all points to the right of the first col
							if (nCol == 0 && nRow == 0) break ;
							if (nCol == 0 && nRow == nPatchSQ) break ;

							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = TRUE ;
							break ;

						case DS_BOTTOM_EDGE:
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = FALSE ;
							if (nRow < nPatchSQ - 1) break ;
							if (nRow == nPatchSQ - 1 && (nCol == 0)) break ;
							
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = TRUE ;
							if (nRow == nPatchSQ - 1) bPointIsInDSForUpBuild = FALSE ;
							if (nRow == nPatchSQ - 1 && nCol == nPatchSQ - 1) bPointIsInDSForDownBuild = FALSE ;
							break ;

						case DS_RIGHT_EDGE:
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = FALSE ;
							if (nCol < nPatchSQ - 1) break ;
							
							bPointIsInDSForUpBuild = bPointIsInDSForDownBuild = TRUE ;							
							if (nRow == 0) bPointIsInDSForDownBuild = FALSE ;
							if (nRow == nPatchSQ) bPointIsInDSForUpBuild = FALSE ;
					}
					
				// if it is, then insert indices for one primitive
					if (nRow != nPatchSQ && bPointIsInDSForDownBuild)
					{	//
						// downbuild	A B 
						//				C
						//

						if ((nRow == 0 && nCol == 0) || (nRow == nPatchSQ - 1 && nCol == nPatchSQ - 1))
						{	// special case on corners
							ndx1 = nVert ;
							ndx2 = nVert + 1 ;
							ndx3 = nVert + nPatchSQ + 2 ;
						}
						else
						{
							ndx1 = nVert ;
							ndx2 = nVert + 1 ;
							ndx3 = nVert + nPatchSQ + 1 ;
						}
						ndxA = ndx1 ; ndxB = ndx2 ; ndxC = ndx3 ;
						_ADD_TRIANGLE( ndx1, ndx2, ndx3 )

					// maintain Drawing Subset data:
						pRO->m_ntvRS[0].dwSS_PrimCount[nDS] += 1 ;
						_MAINTAIN_MIN_MAX_INDEX_ATOC( ndx1, ndx2, ndx3, nDS )
					}

					if (nRow != 0 && bPointIsInDSForUpBuild)
					{	//
						// upbuild		  C
						//				A B
						//

						if ((nRow == 1 && nCol == 0) || (nRow == nPatchSQ && nCol == nPatchSQ - 1))
						{	// special case on corners
							ndx1 = nVert ;
							ndx2 = nVert + 1 ;
							ndx3 = nVert - nPatchSQ - 1 ;
						}
						else
						{
							ndx1 = nVert ;
							ndx2 = nVert + 1 ;
							ndx3 = nVert - nPatchSQ ;
						}
						ndxA = ndx3 ; ndxB = ndx2 ; ndxC = ndx1 ;
						_ADD_TRIANGLE( ndx3, ndx2, ndx1 )

					// maintain Drawing Subset data:
						pRO->m_ntvRS[0].dwSS_PrimCount[nDS] += 1 ;
						_MAINTAIN_MIN_MAX_INDEX_ATOC( ndx1, ndx2, ndx3, nDS )
					}

				//
				// Spawn Positions:
				//
					float fVertexX, fVertexY, fVertexZ ;
					xVec3 vN, v ;
					if (pRO->m_dwFVF == MYFVF_COLOREDVERTEX)
					{
						fVertexX = pRO->m_ntvRS[0].pLitVertices[nVert].x ;
						fVertexY = pRO->m_ntvRS[0].pLitVertices[nVert].y ;
						fVertexZ = pRO->m_ntvRS[0].pLitVertices[nVert].z ;
						Msg( "FATAL: Using bad FVF in CTerrainPatch; no normal data! ");
					}
					if (pRO->m_dwFVF == MYFVF_UNCOLOREDVERTEX)
					{
						fVertexX = pRO->m_ntvRS[0].pUnlitVertices[nVert].x ;
						fVertexY = pRO->m_ntvRS[0].pUnlitVertices[nVert].y ;
						fVertexZ = pRO->m_ntvRS[0].pUnlitVertices[nVert].z ;
						vN.x = pRO->m_ntvRS[0].pUnlitVertices[nVert].nx ;
						vN.y = pRO->m_ntvRS[0].pUnlitVertices[nVert].ny ;
						vN.z = pRO->m_ntvRS[0].pUnlitVertices[nVert].nz ;
					}
					if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
					{
						fVertexX = pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].x ;
						fVertexY = pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].y ;
						fVertexZ = pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].z ;
						vN.x = pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].nx ;
						vN.y = pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].ny ;
						vN.z = pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].nz ;
					}
					if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
					{
						fVertexX = pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].x ;
						fVertexY = pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].y ;
						fVertexZ = pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].z ;
						vN.x = pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].nx ;
						vN.y = pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].ny ;
						vN.z = pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].nz ;
					}

					if( bSpawnPosRequired && ! bSpawnPosDone )
						if( fVertexY >= fMaxY * 0.15f && 
							fVertexY <= fMaxY && vN.y > 0.5f )
						{
							*pvSpawnPos = xVec3( fVertexX, fVertexY, fVertexZ );
							bSpawnPosDone = TRUE ;
							ods( "dropped a SPAWNPOS @(%0.1f, %0.1f, %0.1f)\n", pvSpawnPos->x, pvSpawnPos->y, pvSpawnPos->z );
						}

					nVert++ ;
				}
			}
		}
		
	//
	// Assemble additional tri. lists for LOD+1 edge connectors.
	//
	// B is the additional LOD+1 vertex; corners have two tris, 
	// other 'quads' have three tris.
	//
	// A *B* C		A *B* C		 A *B* C
	//
	//  	 D		D	  E		 D
	//
		if (bBuildEdgeConnectors)
		{
			int nRow, nCol ;
		//
		// DS_TOP_EDGE_LODPLUS1
		//
			pRO->m_ntvRS[0].nDSs++ ;
			pRO->m_ntvRS[0].dwSS_StartIndex[DS_TOP_EDGE_LODPLUS1] = nCurIndex ;
			pRO->m_ntvRS[0].dwSS_MinIndex[DS_TOP_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_MaxIndex[DS_TOP_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_PrimCount[DS_TOP_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_NumVerts[DS_TOP_EDGE_LODPLUS1] = 6 * (nPatchSQ - 2) ;
			for (nCol = 1, nRow = 1 ; nCol <= nPatchSQ - 1 ; nCol ++)
			{
				if (nCol == 1)
				{ // left corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow - 1, nCol - 1 )
					ndxB = nVerts + nCol - 1 ;
					ndxC = _ROWCOLTOINDEX( nRow - 1, nCol	  )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_TOP_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD(ndxA, ndxB, ndxC, ndxD, DS_TOP_EDGE_LODPLUS1)
				}
				if (nCol == nPatchSQ - 1)
				{ // right corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow - 1, nCol     )
					ndxB = nVerts + nCol ;
					ndxC = _ROWCOLTOINDEX( nRow - 1, nCol + 1 )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_TOP_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_TOP_EDGE_LODPLUS1 )
					continue ;
				}
				// normal 'quad'
					WORD ndxA, ndxB, ndxC, ndxD, ndxE ;
					ndxA = _ROWCOLTOINDEX( nRow - 1, nCol     )
					ndxB = nVerts + nCol ;
					ndxC = _ROWCOLTOINDEX( nRow - 1, nCol + 1 )
					ndxD = _ROWCOLTOINDEX( nRow	   , nCol 	  )
					ndxE = _ROWCOLTOINDEX( nRow	   , nCol + 1 )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxD, ndxB, ndxE )
					_ADD_TRIANGLE( ndxB, ndxC, ndxE )
					_MAINTAIN_MIN_MAX_INDEX_ATOE(ndxA, ndxB, ndxC, ndxD, ndxE, DS_TOP_EDGE_LODPLUS1)
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_TOP_EDGE_LODPLUS1] += 3 ;
			}

		//
		// DS_LEFT_EDGE_LODPLUS1
		//
			pRO->m_ntvRS[0].nDSs++ ;
			pRO->m_ntvRS[0].dwSS_StartIndex[DS_LEFT_EDGE_LODPLUS1] = nCurIndex ;
			pRO->m_ntvRS[0].dwSS_MinIndex[DS_LEFT_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_MaxIndex[DS_LEFT_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_PrimCount[DS_LEFT_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_NumVerts[DS_LEFT_EDGE_LODPLUS1] = 6 * (nPatchSQ - 2) ;
			for (nRow = 1, nCol = 1 ; nRow <= nPatchSQ - 1 ; nRow ++)
			{
				if (nRow == 1)
				{ // top corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow    , nCol - 1 )
					ndxB = nVerts + nPatchSQ ;
					ndxC = _ROWCOLTOINDEX( nRow - 1, nCol - 1 )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_LEFT_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_LEFT_EDGE_LODPLUS1 )
				}
				if (nRow == nPatchSQ - 1)
				{ // bottom corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow + 1, nCol - 1 )
					ndxB = nVerts + nPatchSQ + nPatchSQ - 1 ;
					ndxC = _ROWCOLTOINDEX( nRow    , nCol - 1 )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_LEFT_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_LEFT_EDGE_LODPLUS1 )
					continue ;
				}
				// normal 'quad'
					WORD ndxA, ndxB, ndxC, ndxD, ndxE ;
					ndxA = _ROWCOLTOINDEX( nRow + 1, nCol - 1 )
					ndxB = nVerts + nPatchSQ + 1 + (nRow - 1) ;
					ndxC = _ROWCOLTOINDEX( nRow    , nCol - 1 )
					ndxD = _ROWCOLTOINDEX( nRow	+ 1, nCol     )
					ndxE = _ROWCOLTOINDEX( nRow	   , nCol     )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxD, ndxB, ndxE )
					_ADD_TRIANGLE( ndxB, ndxC, ndxE )
					_MAINTAIN_MIN_MAX_INDEX_ATOE( ndxA, ndxB, ndxC, ndxD, ndxE, DS_LEFT_EDGE_LODPLUS1 )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_LEFT_EDGE_LODPLUS1] += 3 ;
			}

		//
		// DS_BOTTOM_EDGE_LODPLUS1
		//
			pRO->m_ntvRS[0].nDSs++ ;
			pRO->m_ntvRS[0].dwSS_StartIndex[DS_BOTTOM_EDGE_LODPLUS1] = nCurIndex ;
			pRO->m_ntvRS[0].dwSS_MinIndex[DS_BOTTOM_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_MaxIndex[DS_BOTTOM_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_PrimCount[DS_BOTTOM_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_NumVerts[DS_BOTTOM_EDGE_LODPLUS1] = 6 * (nPatchSQ - 2) ;
			for (nRow = nPatchSQ - 1, nCol = 1 ; nCol <= nPatchSQ - 1 ; nCol ++)
			{
				if (nCol == 1)
				{ // top corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow + 1, nCol     )
					ndxB = nVerts + nPatchSQ + nPatchSQ ;
					ndxC = _ROWCOLTOINDEX( nRow + 1, nCol - 1 )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_BOTTOM_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_BOTTOM_EDGE_LODPLUS1 )
				}
				if (nCol == nPatchSQ - 1)
				{ // bottom corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow + 1, nCol + 1 )
					ndxB = nVerts + nPatchSQ + nPatchSQ + nPatchSQ - 1 ;
					ndxC = _ROWCOLTOINDEX( nRow + 1, nCol     )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_BOTTOM_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_BOTTOM_EDGE_LODPLUS1 )
					continue ;
				}
				// normal 'quad'
				WORD ndxA, ndxB, ndxC, ndxD, ndxE ;
				ndxA = _ROWCOLTOINDEX( nRow + 1, nCol + 1 )
				ndxB = nVerts + nPatchSQ + nPatchSQ + nCol ;
				ndxC = _ROWCOLTOINDEX( nRow + 1, nCol     )
				ndxD = _ROWCOLTOINDEX( nRow	   , nCol + 1 )
				ndxE = _ROWCOLTOINDEX( nRow	   , nCol     )
				_ADD_TRIANGLE( ndxA, ndxB, ndxD )
				_ADD_TRIANGLE( ndxD, ndxB, ndxE )
				_ADD_TRIANGLE( ndxB, ndxC, ndxE )
				_MAINTAIN_MIN_MAX_INDEX_ATOE( ndxA, ndxB, ndxC, ndxD, ndxE, DS_BOTTOM_EDGE_LODPLUS1 )
				pRO->m_ntvRS[0].dwSS_PrimCount[DS_BOTTOM_EDGE_LODPLUS1] += 3 ;
			}
	
		//
		// DS_RIGHT_EDGE_LODPLUS1
		//
			pRO->m_ntvRS[0].nDSs++ ;
			pRO->m_ntvRS[0].dwSS_StartIndex[DS_RIGHT_EDGE_LODPLUS1] = nCurIndex ;
			pRO->m_ntvRS[0].dwSS_MinIndex[DS_RIGHT_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_MaxIndex[DS_RIGHT_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_PrimCount[DS_RIGHT_EDGE_LODPLUS1] = 0 ;
			pRO->m_ntvRS[0].dwSS_NumVerts[DS_RIGHT_EDGE_LODPLUS1] = 6 * (nPatchSQ - 2) ;
			for (nCol = nPatchSQ - 1, nRow = 1 ; nRow <= nPatchSQ - 1 ; nRow ++)
			{
				if (nRow == 1)
				{ // top corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow - 1, nCol + 1 )
					ndxB = nVerts + nPatchSQ + nPatchSQ + nPatchSQ ;
					ndxC = _ROWCOLTOINDEX( nRow    , nCol + 1 )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_RIGHT_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_RIGHT_EDGE_LODPLUS1 )
				}
				if (nRow == nPatchSQ - 1)
				{ // bottom corner
					WORD ndxA, ndxB, ndxC, ndxD ;
					ndxA = _ROWCOLTOINDEX( nRow    , nCol + 1 )
					ndxB = nVerts + nPatchSQ + nPatchSQ + nPatchSQ + nPatchSQ - 1 ;
					ndxC = _ROWCOLTOINDEX( nRow + 1, nCol + 1 )
					ndxD = _ROWCOLTOINDEX( nRow    , nCol	  )
					_ADD_TRIANGLE( ndxA, ndxB, ndxD )
					_ADD_TRIANGLE( ndxB, ndxC, ndxD )
					pRO->m_ntvRS[0].dwSS_PrimCount[DS_RIGHT_EDGE_LODPLUS1] += 2 ;
					_MAINTAIN_MIN_MAX_INDEX_ATOD( ndxA, ndxB, ndxC, ndxD, DS_RIGHT_EDGE_LODPLUS1 )
					continue ;
				}
				// normal 'quad'
				WORD ndxA, ndxB, ndxC, ndxD, ndxE ;
				ndxA = _ROWCOLTOINDEX( nRow    , nCol + 1 )
				ndxB = nVerts + nPatchSQ + nPatchSQ + nPatchSQ + nRow ;
				ndxC = _ROWCOLTOINDEX( nRow + 1, nCol + 1 )
				ndxD = _ROWCOLTOINDEX( nRow	   , nCol     )
				ndxE = _ROWCOLTOINDEX( nRow	+ 1, nCol     )
				_ADD_TRIANGLE( ndxA, ndxB, ndxD )
				_ADD_TRIANGLE( ndxD, ndxB, ndxE )
				_ADD_TRIANGLE( ndxB, ndxC, ndxE )
				_MAINTAIN_MIN_MAX_INDEX_ATOE( ndxA, ndxB, ndxC, ndxD, ndxE, DS_RIGHT_EDGE_LODPLUS1 )
				pRO->m_ntvRS[0].dwSS_PrimCount[DS_RIGHT_EDGE_LODPLUS1] += 3 ;
			}
		}

		return S_OK ;
	}

	/*void inline CTerrainPatch::TerrainVertexColours(float *r, float *g, float *b, float *a, float fY, float fMaxY, float fMinY)
	{
		float fMidY = (fMaxY - fMinY) / 2.0f + fMinY ;
		float fHalfRange = fMaxY - fMidY ;
		float fFullRange = fHalfRange * 2.0f ;

		*r = (fabsf(fMidY - fY) / fHalfRange) ;	// 1 - 0 - 1

		*g = 1.00f ;							// 1

		*b = (fY + fabsf(fMinY)) / fFullRange ;	// 0 - 1

		*a = (fY + fabsf(fMinY)) / fFullRange ;	// 0 - 1

		assert( *a >= 0.0f && *a <= 1.0f );
		
		(*r) *= 255.0f ;
		(*g) *= 255.0f ;
		(*b) *= 255.0f ;
		(*a) *= 255.0f ;
	}*/

	void inline CTerrainPatch::AddVertex(CProceduralRenderObject *pRO, int nVert, float fVertexX, float fVertexY, float fVertexZ, float fX, float fY, float a, float r, float g, float b)
	{
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEX)
		{
			pRO->m_ntvRS[0].pLitVertices[nVert].x   = fVertexX ;
			pRO->m_ntvRS[0].pLitVertices[nVert].y   = fVertexY ;
			pRO->m_ntvRS[0].pLitVertices[nVert].z   = fVertexZ ;
			pRO->m_ntvRS[0].pLitVertices[nVert].tu1 = fX * 10.0f ;
			pRO->m_ntvRS[0].pLitVertices[nVert].tv1 = fY * 10.0f ;
			pRO->m_ntvRS[0].pLitVertices[nVert].color = D3DCOLOR_ARGB((int)a, (int)r, (int)g, (int)b) ;
			pRO->m_ntvRS[0].pLitVertices[nVert].specular = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		}
		if (pRO->m_dwFVF == MYFVF_UNCOLOREDVERTEX)
		{
			pRO->m_ntvRS[0].pUnlitVertices[nVert].x   = fVertexX ;
			pRO->m_ntvRS[0].pUnlitVertices[nVert].y   = fVertexY ;
			pRO->m_ntvRS[0].pUnlitVertices[nVert].z   = fVertexZ ;
			pRO->m_ntvRS[0].pUnlitVertices[nVert].tu1 = fX * 10.0f ;
			pRO->m_ntvRS[0].pUnlitVertices[nVert].tv1 = fY * 10.0f ;
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
		{
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].x   = fVertexX ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].y   = fVertexY ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].z   = fVertexZ ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].tu1 = fX * 10.0f ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].tv1 = fY * 10.0f ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].color = D3DCOLOR_ARGB((int)a, (int)r, (int)g, (int)b) ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals[nVert].specular = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
		{
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].x   = fVertexX ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].y   = fVertexY ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].z   = fVertexZ ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].tu1 = fX * 10.0f ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].tv1 = fY * 10.0f ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].tu2 = fX * 30.0f ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].tv2 = fY * 30.0f ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].color = D3DCOLOR_ARGB((int)a, (int)r, (int)g, (int)b) ;
			pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[nVert].specular = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		}
	}

	void inline CTerrainPatch::AddFaceNormalData(CProceduralRenderObject *pRO, int ndxA, int ndxB, int ndxC)
	{
	// add face normal data:
		xVec3 v1, v2, v3, vd1, vd2, vcp, vn ;

		if (pRO->m_dwFVF == MYFVF_UNCOLOREDVERTEX)
		{
			UNCOLOREDVERTEX *pv ;
			pv = &pRO->m_ntvRS[0].pUnlitVertices[ndxA] ;
			v1 = xVec3(pv->x, pv->y, pv->z) ;
			pv = &pRO->m_ntvRS[0].pUnlitVertices[ndxB] ;
			v2 = xVec3(pv->x, pv->y, pv->z) ;
			pv = &pRO->m_ntvRS[0].pUnlitVertices[ndxC] ;
			v3 = xVec3(pv->x, pv->y, pv->z) ;
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
		{
			COLOREDVERTEXWITHNORMAL *pv ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals[ndxA] ;
			v1 = xVec3(pv->x, pv->y, pv->z) ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals[ndxB] ;
			v2 = xVec3(pv->x, pv->y, pv->z) ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals[ndxC] ;
			v3 = xVec3(pv->x, pv->y, pv->z) ;
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
		{
			COLOREDVERTEXWITHNORMAL2TEX *pv ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[ndxA] ;
			v1 = xVec3(pv->x, pv->y, pv->z) ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[ndxB] ;
			v2 = xVec3(pv->x, pv->y, pv->z) ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[ndxC] ;
			v3 = xVec3(pv->x, pv->y, pv->z) ;
		}

// compute face normal
		D3DXVec3Subtract(&vd1, &v3, &v2) ;
		D3DXVec3Subtract(&vd2, &v3, &v1) ;
		D3DXVec3Cross(&vcp, &vd1, &vd2) ;
		D3DXVec3Normalize(&vn, &vcp) ;

// compute a new alpha-value based on the normal
		float fA = fabsf(vn.y) / 1.0f ;
		DWORD dwA = (DWORD)(fA * 255.0f) ;

		if (pRO->m_dwFVF == MYFVF_UNCOLOREDVERTEX)
		{
			UNCOLOREDVERTEX *pv ;
			pv = &pRO->m_ntvRS[0].pUnlitVertices[ndxA] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			pv = &pRO->m_ntvRS[0].pUnlitVertices[ndxB] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			pv = &pRO->m_ntvRS[0].pUnlitVertices[ndxC] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL)
		{
			COLOREDVERTEXWITHNORMAL *pv ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals[ndxA] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals[ndxB] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals[ndxC] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
		}
		if (pRO->m_dwFVF == MYFVF_COLOREDVERTEXWITHNORMAL2TEX)
		{
			COLOREDVERTEXWITHNORMAL2TEX *pv ;

		// note; also reassigns alpha values
			DWORD r, g, b, a ;
			
			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[ndxA] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			a = pv->color & 0xff000000 ;
			r = pv->color & 0x00ff0000 ;
			g = pv->color & 0x0000ff00 ;
			b = pv->color & 0x000000ff ;
			a = dwA << 24 ;
			pv->color = a + r + g + b ;

			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[ndxB] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			a = pv->color & 0xff000000 ;
			r = pv->color & 0x00ff0000 ;
			g = pv->color & 0x0000ff00 ;
			b = pv->color & 0x000000ff ;
			a = dwA << 24 ;
			pv->color = a + r + g + b ;


			pv = &pRO->m_ntvRS[0].pLitVerticesWithNormals2Tex[ndxC] ;
			pv->nx = vn.x ; pv->ny = vn.y ; pv->nz = vn.z ;
			a = pv->color & 0xff000000 ;
			r = pv->color & 0x00ff0000 ;
			g = pv->color & 0x0000ff00 ;
			b = pv->color & 0x000000ff ;
			a = dwA << 24 ;
			pv->color = a + r + g + b ;

		}
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT CTerrainPatch::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
#ifdef FULL_DBG_TRACE
		ods( "CTerrainPatch::RestoreDeviceObjects\n" );
#endif

		for (int i = 0 ; i < m_nPatchMaxLODLevels ; i ++)
			if (m_pPatchLOD[i])
			{
				if (FAILED(m_pPatchLOD[i]->RestoreDeviceObjects(pD3D, pDev)))
					return E_FAIL ;

			//
			// Make an octree on the highest LOD:
			//
				/*if (i == 0)
				{
					m_pPatchLOD[0]->m_pBBox->BuildOctree(4) ;
				}*/
			}
		
		return S_OK ;
	}
	HRESULT	CTerrainPatch::InvalidateDeviceObjects()
	{
		for (int i = 0 ; i < m_nPatchMaxLODLevels ; i ++)
			if (m_pPatchLOD[i])
				m_pPatchLOD[i]->InvalidateDeviceObjects() ;

		return S_OK ;
	}
	HRESULT CTerrainPatch::Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int nLOD, int dsTopEdge, int dsLeftEdge, int dsBottomEdge, int dsRightEdge, xMatUA *pmatCurWorld)
	{
		if (m_pPatchLOD[nLOD])
		{
			if (
				nLOD == 0 ||
			    (dsTopEdge == DS_TOP_EDGE &&
			     dsLeftEdge == DS_LEFT_EDGE &&
				 dsBottomEdge == DS_BOTTOM_EDGE &&
				 dsRightEdge == DS_RIGHT_EDGE)
			   )
			{
			// fastest render, all in one batch, either when at max. LOD,
			// or when no edge tesselation is specified;
				m_pPatchLOD[nLOD]->Render( pDev, pTex, pulPolyCount, DS_ALL, pmatCurWorld, false );
			}
			else
			{
			// slower render, 5 batches, draws specified edge strips for tesselation;
				m_pPatchLOD[nLOD]->Render( pDev, pTex, pulPolyCount, DS_CENTER, pmatCurWorld, false );
				m_pPatchLOD[nLOD]->Render( pDev, pTex, pulPolyCount, dsTopEdge, pmatCurWorld, false );
				m_pPatchLOD[nLOD]->Render( pDev, pTex, pulPolyCount, dsLeftEdge, pmatCurWorld, false );
				m_pPatchLOD[nLOD]->Render( pDev, pTex, pulPolyCount, dsBottomEdge, pmatCurWorld, false );
				m_pPatchLOD[nLOD]->Render( pDev, pTex, pulPolyCount, dsRightEdge, pmatCurWorld, false );
			}

		/*
			if( nLOD==0 )
			{
			// draw detail pass;
				pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pPatchLOD[nLOD]->RenderDetailPass( pDev, pTex->m_pTex[ TERRAIN_TEX_START + 1 ], pulPolyCount, DS_ALL, pmatCurWorld );
				pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		*/
		}
		
		return S_OK ;
	}
	HRESULT CTerrainPatch::FrameMove(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS)
	{
		return S_OK ;
	}
