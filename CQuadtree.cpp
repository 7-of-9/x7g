#pragma warning( disable : 4183 )	// 'missing return type'

#include "CQuadtree.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CQuadtree::CQuadtree(xVec3 *pvMin, xVec3 *pvMax, int nRequiredDepth)
	{
		// memory usage monitor
		m_dwMem = 0 ;
		m_dwMem += sizeof(CQuadtree) ;

		// set static render data
		m_wIndices[0] = 0 ;	 // front face
		m_wIndices[1] = 1 ; 
		m_wIndices[2] = 2 ;
		m_wIndices[3] = 3 ;
		m_wIndices[4] = 2 ;
		m_wIndices[5] = 1 ; 
		m_wIndices[6] = 4 ;  // back face
		m_wIndices[7] = 5 ;
		m_wIndices[8] = 6 ;
		m_wIndices[9] = 7 ;
		m_wIndices[10] = 6 ;
		m_wIndices[11] = 5 ;
		m_wIndices[12] = 1 ; // top face
		m_wIndices[13] = 5 ;
		m_wIndices[14] = 7 ;
		m_wIndices[15] = 1 ;
		m_wIndices[16] = 3 ;
		m_wIndices[17] = 7 ;
		m_wIndices[18] = 0 ; // bottom face
		m_wIndices[19] = 4 ;
		m_wIndices[20] = 6 ;
		m_wIndices[21] = 0 ;
		m_wIndices[22] = 2 ;
		m_wIndices[23] = 6 ;
		m_wIndices[24] = 0 ; // left face
		m_wIndices[25] = 1 ;
		m_wIndices[26] = 5 ;
		m_wIndices[27] = 4 ;
		m_wIndices[28] = 5 ;
		m_wIndices[29] = 0 ;
		m_wIndices[30] = 2 ; // right face
		m_wIndices[31] = 3 ;
		m_wIndices[32] = 6 ;
		m_wIndices[33] = 3 ;
		m_wIndices[34] = 6 ;
		m_wIndices[35] = 7 ;

		D3DUtil_InitMaterial(m_Mtrl, 1, 1, 1) ;
		m_pVB = NULL ;
		m_pIB = NULL ;

		// build quadtree
		m_nQTNodes = 0 ;
		m_nQTDepth = 0 ;
		m_bQTBuilt = FALSE ;

		BuildQuadtree(nRequiredDepth, pvMin, pvMax) ;
	}
	CQuadtree::~CQuadtree()
	{
		if (m_bQTBuilt)
		{
			for (int i = 0 ; i < m_nQTNodes ; i ++)
				SAFE_DELETE(m_pQTNodes[i])
		}
	}

/*
 *
 * Quadtree.
 *
 */
	void CQuadtree::BuildQuadtree(int nSubdivides, xVec3 *pvMin, xVec3 *pvMax)
	{
		if (m_bQTBuilt)
			return ;

		// create root node
		InitQuadtreeNode(&m_pQT, 0) ;
		m_pQT->vMin = *pvMin ;
		m_pQT->vMax = *pvMax ;

		// subdivide
		if (nSubdivides > 0)
			RecurseQuadtree(nSubdivides, m_pQT) ;

		m_nQTDepth = nSubdivides ;
		m_bQTBuilt = TRUE ;
	}
	void CQuadtree::InitQuadtreeNode(_QTNode **ppNode, int nDepth)
	{
		// create new node
		*ppNode = NULL ;
		*ppNode = new _QTNode ; 
		assert ( *ppNode );

		m_pQTNodes[m_nQTNodes] = *ppNode ;
		m_dwMem += sizeof(_QTNode) ;

		// keep track of the node's depth for rendering (debug) purposes
		(*ppNode)->nDepth = nDepth ;

		// custom data, used by CTerrain to store indices of associated patch in terminal node
		(*ppNode)->nX = -1 ;
		(*ppNode)->nY = -1 ;

		// default it to a terminal node
		for (int i = 0 ; i < 4 ; i ++)
			(*ppNode)->pKids[i] = NULL ;

		m_nQTNodes++ ;

		assert( m_nQTNodes < MAX_QT_NODES );
	}
	void CQuadtree::RecurseQuadtree(int nSubdivides, _QTNode *pNode)
	{
		_QTNode *p ;
		xVec3 vCenter ;
		int i ;

		m_nQTDepth++ ;
		
		// get the center of the current node
		vCenter.x = (pNode->vMax.x - pNode->vMin.x) / 2.0f + pNode->vMin.x ;
		vCenter.y = (pNode->vMax.y - pNode->vMin.y) / 2.0f + pNode->vMin.y ;
		vCenter.z = (pNode->vMax.z - pNode->vMin.z) / 2.0f + pNode->vMin.z ;

		// initialize the four children
		for (i = 0 ; i < 4 ; i ++)
			InitQuadtreeNode((_QTNode**)&pNode->pKids[i], m_nQTDepth) ;

		// populate the children with volume data
		p = (_QTNode*)pNode->pKids[0] ;
		p->vMin = xVec3(pNode->vMin.x, pNode->vMin.y, pNode->vMin.z) ;	// near left
		p->vMax = xVec3(vCenter.x, pNode->vMax.y, vCenter.z) ; 

		p = (_QTNode*)pNode->pKids[1] ;
		p->vMin = xVec3(vCenter.x, pNode->vMin.y, pNode->vMin.z) ;		// near right
		p->vMax = xVec3(pNode->vMax.x, pNode->vMax.y, vCenter.z) ;

		p = (_QTNode*)pNode->pKids[2] ;
		p->vMin = xVec3(pNode->vMin.x, pNode->vMin.y, vCenter.z) ;		// far left
		p->vMax = xVec3(vCenter.x, pNode->vMax.y, pNode->vMax.z) ; 

		p = (_QTNode*)pNode->pKids[3] ;
		p->vMin = xVec3(vCenter.x, pNode->vMin.y, vCenter.z) ;			// far right
		p->vMax = xVec3(pNode->vMax.x, pNode->vMax.y, pNode->vMax.z) ;

		// recurse if not at the desired level
		if (nSubdivides > 1)
		{
			for (i = 0 ; i < 4 ; i ++)
			{
				RecurseQuadtree(nSubdivides - 1, (_QTNode*)pNode->pKids[i]) ;
				m_nQTDepth-- ;
			}
		}
		else
		// build list of polygons in volume for terminal nodes...
		{
			;
		}
	}

/*
 *
 * Render Data Creation.
 *
 */
	HRESULT CQuadtree::MakeRenderData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		HRESULT hr ;

	//
	// Create an empty VB:
	//
		UINT iFVFSize = sizeof(COLOREDVERTEX) ;
		hr = pDev->CreateVertexBuffer(8 * iFVFSize, D3DUSAGE_DYNAMIC, MYFVF_COLOREDVERTEX, D3DPOOL_SYSTEMMEM, &m_pVB) ;
		assert( !FAILED(hr) );
		/*BYTE *pVerts ;
		hr = m_pVB->Lock(0, 0, &pVerts, 0) ;
		if (FAILED(hr))
		{
			Msg("CQuadtree::MakeRenderData: FAILED(pMesh->LockVertexBuffer(...)) == TRUE") ;
			return hr ;
		}
		memcpy(pVerts, m_fvfVerts, sizeof(COLOREDVERTEX) * 8) ;
		m_dwMem += sizeof(COLOREDVERTEX) * 8 ;
		m_pVB->Unlock() ;*/

	//
	// Create & fill the IB:
	//
		hr = pDev->CreateIndexBuffer(sizeof(WORD) * 36, D3DUSAGE_WRITEONLY/*D3DUSAGE_SOFTWAREPROCESSING*/, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB) ;
		assert( !FAILED(hr) );
		
		BYTE *pIndices ;
		hr = m_pIB->Lock(0, 0, &pIndices, 0) ;
		assert( !FAILED(hr) );

		memcpy(pIndices, m_wIndices, sizeof(WORD) * 36) ;
		m_dwMem += sizeof(WORD) * 36 ;
		m_pIB->Unlock() ;

		return S_OK ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh objects.
 *
 */
	HRESULT CQuadtree::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "CQuadtree::RestoreDeviceObjects\n" );

		HRESULT hr ;

		hr = MakeRenderData( pD3D, pDev );
		assert( hr==S_OK );
	
		return hr ;
	}
	HRESULT	CQuadtree::InvalidateDeviceObjects()
	{
		SAFE_RELEASE(m_pVB)
		SAFE_RELEASE(m_pIB)

		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CQuadtree::Render(LPDIRECT3DDEVICE8 pDev, unsigned long *pulPolyCount, xMatUA* pmatWorld, int nNodeDepth)
	{
		HRESULT hr ;
		int nNodes2Draw = 4 ^ nNodeDepth ;
		int nNodeNo = 0 ;
		float j ;

		for (int i = 0 ; i < m_nQTNodes ; i ++)
		{
			if (m_pQTNodes[i]->nDepth == nNodeDepth)
			{
				nNodeNo++ ;
				//j = (float)nNodeNo / (float)nNodes2Draw ;
				j =	(float)nNodeDepth/(float)m_nQTDepth ;

				xVec3 vVerts[8] ;

			// get all eight corner vertices
				vVerts[0] = m_pQTNodes[i]->vMin ;																// near bottom left	: 0
				vVerts[1] = xVec3(m_pQTNodes[i]->vMin.x, m_pQTNodes[i]->vMax.y, m_pQTNodes[i]->vMin.z) ;	// near top left	: 1
				vVerts[2] = xVec3(m_pQTNodes[i]->vMax.x, m_pQTNodes[i]->vMin.y, m_pQTNodes[i]->vMin.z) ;	// near bottom right: 2
				vVerts[3] = xVec3(m_pQTNodes[i]->vMax.x, m_pQTNodes[i]->vMax.y, m_pQTNodes[i]->vMin.z) ;	// near top right	: 3
				vVerts[4] = xVec3(m_pQTNodes[i]->vMin.x, m_pQTNodes[i]->vMin.y, m_pQTNodes[i]->vMax.z) ;	// far bottom left	: 4
				vVerts[5] = xVec3(m_pQTNodes[i]->vMin.x, m_pQTNodes[i]->vMax.y, m_pQTNodes[i]->vMax.z) ;	// far top left		: 5
				vVerts[6] = xVec3(m_pQTNodes[i]->vMax.x, m_pQTNodes[i]->vMin.y, m_pQTNodes[i]->vMax.z) ;	// far bottom right	: 6
				vVerts[7] = m_pQTNodes[i]->vMax ;
				
			// set static render data
				DWORD color = D3DCOLOR_ARGB(64, (int)(j * 255.0f), (int)((1-j) * 255.0f), 255) ;
				DWORD specular = D3DCOLOR_ARGB(0, 0, 0, 0) ;
				COLOREDVERTEX fvfVerts[8] ;
				fvfVerts[0] = COLOREDVERTEX(vVerts[0].x, vVerts[0].y, vVerts[0].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[1] = COLOREDVERTEX(vVerts[1].x, vVerts[1].y, vVerts[1].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[2] = COLOREDVERTEX(vVerts[2].x, vVerts[2].y, vVerts[2].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[3] = COLOREDVERTEX(vVerts[3].x, vVerts[3].y, vVerts[3].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[4] = COLOREDVERTEX(vVerts[4].x, vVerts[4].y, vVerts[4].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[5] = COLOREDVERTEX(vVerts[5].x, vVerts[5].y, vVerts[5].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[6] = COLOREDVERTEX(vVerts[6].x, vVerts[6].y, vVerts[6].z, 0.0f, 1.0f, color, specular) ;
				fvfVerts[7] = COLOREDVERTEX(vVerts[7].x, vVerts[7].y, vVerts[7].z, 0.0f, 1.0f, color, specular) ;

			// fill VB
				BYTE *pVerts ;
				hr = m_pVB->Lock(0, 0, &pVerts, 0) ;
				assert( !FAILED(hr) );

				memcpy(pVerts, fvfVerts, sizeof(COLOREDVERTEX) * 8) ;
				m_dwMem += sizeof(COLOREDVERTEX) * 8 ;
				m_pVB->Unlock() ;

			// render
				LPDIRECT3DVERTEXBUFFER8 pVB ;
				LPDIRECT3DINDEXBUFFER8 pIB ;
				
				pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
				pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
				pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;

				if (pmatWorld)
					pDev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)pmatWorld) ;

				pVB = m_pVB ;
				pIB = m_pIB ;

				pDev->SetStreamSource(0, pVB, sizeof(COLOREDVERTEX)) ;
				pDev->SetIndices(pIB, 0) ;
				pDev->SetVertexShader(MYFVF_COLOREDVERTEX) ;
				pDev->SetTexture(0, NULL) ;
				pDev->SetMaterial(&m_Mtrl) ;

				SetTextureColorStage(pDev, 0, D3DTA_CURRENT, D3DTOP_SELECTARG1,	D3DTA_CURRENT)
				SetTextureColorStage(pDev, 1, D3DTA_CURRENT, D3DTOP_DISABLE,	D3DTA_CURRENT)
				
				pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 8, 0, 12) ;
				
				*pulPolyCount += 12 ;
			}
		}

		return S_OK ;
	}
