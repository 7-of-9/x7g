#pragma warning( disable : 4183 )

#include "CBoundingBox.h"

/*
 *
 * Construction / Destruction.
 *
 */
	CBoundingBox::CBoundingBox( xVec3* pvMin, xVec3* pvMax )	// for 'axis-aligned' bbox
	{
		m_dwMem = 0 ;
		m_dwMem += sizeof(CBoundingBox) ;

	// get maximal and minimal vertices
		m_vMin = m_vInitialMin = xVec3(pvMin->x, pvMin->y, pvMin->z) ;
		m_vMax = m_vInitialMax = xVec3(pvMax->x, pvMax->y, pvMax->z) ;

	// get all eight corner vertices
		m_vVerts[0] = m_vInitialVerts[0] = m_vMin ;										// near bottom left	: 0
		m_vVerts[1] = m_vInitialVerts[1] = xVec3(m_vMin.x, m_vMax.y, m_vMin.z) ;	// near top left	: 1
		m_vVerts[2] = m_vInitialVerts[2] = xVec3(m_vMax.x, m_vMin.y, m_vMin.z) ;	// near bottom right: 2
		m_vVerts[3] = m_vInitialVerts[3] = xVec3(m_vMax.x, m_vMax.y, m_vMin.z) ;	// near top right	: 3
		m_vVerts[4] = m_vInitialVerts[4] = xVec3(m_vMin.x, m_vMin.y, m_vMax.z) ;	// far bottom left	: 4
		m_vVerts[5] = m_vInitialVerts[5] = xVec3(m_vMin.x, m_vMax.y, m_vMax.z) ;	// far top left		: 5
		m_vVerts[6] = m_vInitialVerts[6] = xVec3(m_vMax.x, m_vMin.y, m_vMax.z) ;	// far bottom right	: 6
		m_vVerts[7] = m_vInitialVerts[7] = m_vMax ;										// far top right	: 7

	// get geometric center vertex
		m_vCenter.x = m_vInitialCenter.x = (m_vMax.x - m_vMin.x) / 2 + m_vMin.x ;
		m_vCenter.y = m_vInitialCenter.y = (m_vMax.y - m_vMin.y) / 2 + m_vMin.y ;
		m_vCenter.z = m_vInitialCenter.z = (m_vMax.z - m_vMin.z) / 2 + m_vMin.z ;

	// octree
		m_dwMem_OT = 0 ;
		m_nOTNodes = 0 ;
		m_bOTBuilt = FALSE ;

	// set static render data
		m_pMesh = NULL ;
		m_pVB = NULL ;
		m_pIB = NULL ;
		m_pVBProcessed = NULL ;
		InitRenderData() ;

		m_matWorld = g_matIdent ;
	}

	CBoundingBox::CBoundingBox( xVec3* pvVerts )	// for 'oriented' bbox
	{
		m_dwMem = 0 ;
		m_dwMem += sizeof(CBoundingBox) ;

	// get maximal and minimal vertices
		m_vMin = pvVerts[ 0 ];
		m_vMax = pvVerts[ 0 ];

	// get all eight corner vertices
		for( int i=0; i<8; i++ )
		{
			m_vVerts[ i ] = m_vInitialVerts[ i ] = pvVerts[ i ];
			if( pvVerts[ i ].x < m_vMin.x && pvVerts[ i ].y < m_vMin.y && pvVerts[ i ].z < m_vMin.z )
				m_vMin = pvVerts[ i ];
			if( pvVerts[ i ].x > m_vMax.x && pvVerts[ i ].y > m_vMax.y && pvVerts[ i ].z > m_vMax.z )
				m_vMax = pvVerts[ i ];
		}

	// get geometric center vertex
		m_vCenter.x = m_vInitialCenter.x = (m_vMax.x - m_vMin.x) / 2 + m_vMin.x ;
		m_vCenter.y = m_vInitialCenter.y = (m_vMax.y - m_vMin.y) / 2 + m_vMin.y ;
		m_vCenter.z = m_vInitialCenter.z = (m_vMax.z - m_vMin.z) / 2 + m_vMin.z ;

	// octree
		m_dwMem_OT = 0 ;
		m_nOTNodes = 0 ;
		m_bOTBuilt = FALSE ;

	// set static render data
		m_pMesh = NULL ;
		m_pVB = NULL ;
		m_pIB = NULL ;
		m_pVBProcessed = NULL ;
		InitRenderData() ;

		m_matWorld = g_matIdent ;
	}

	CBoundingBox::InitRenderData()
	{
#ifdef _DEBUG
		DWORD color = D3DCOLOR_ARGB(64, 128, 0, 0) ;
		DWORD specular = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		m_fvfVerts[0] = COLOREDVERTEX(m_vVerts[0].x, m_vVerts[0].y, m_vVerts[0].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[1] = COLOREDVERTEX(m_vVerts[1].x, m_vVerts[1].y, m_vVerts[1].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[2] = COLOREDVERTEX(m_vVerts[2].x, m_vVerts[2].y, m_vVerts[2].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[3] = COLOREDVERTEX(m_vVerts[3].x, m_vVerts[3].y, m_vVerts[3].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[4] = COLOREDVERTEX(m_vVerts[4].x, m_vVerts[4].y, m_vVerts[4].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[5] = COLOREDVERTEX(m_vVerts[5].x, m_vVerts[5].y, m_vVerts[5].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[6] = COLOREDVERTEX(m_vVerts[6].x, m_vVerts[6].y, m_vVerts[6].z, 0.0f, 1.0f, color, specular) ;
		m_fvfVerts[7] = COLOREDVERTEX(m_vVerts[7].x, m_vVerts[7].y, m_vVerts[7].z, 0.0f, 1.0f, color, specular) ;
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
#endif
	}
	CBoundingBox::~CBoundingBox()
	{
		SAFE_RELEASE( m_pMesh )
		SAFE_RELEASE( m_pVB )
		SAFE_RELEASE( m_pIB )
		SAFE_RELEASE( m_pVBProcessed )

		if( m_bOTBuilt )
		{
			for( int i=0 ; i<m_nOTNodes ; i++ )
				SAFE_DELETE( m_pOTNodes[i] )
		}
	}

/*
 *
 * Octree.
 *
 */
	void CBoundingBox::BuildOctree( int nSubdivides )
	{
		if (m_bOTBuilt)
			return ;

		// create root node
		InitOctreeNode(&m_pOT) ;
		m_pOT->vMin = m_vMin ;
		m_pOT->vMax = m_vMax ;

		// subdivide
		if (nSubdivides > 0)
			RecurseOctree(nSubdivides, m_pOT) ;

		m_bOTBuilt = TRUE ;
	}
	void CBoundingBox::InitOctreeNode( _OTNode **ppNode )
	{
		// create new node
		*ppNode = NULL ;
		*ppNode = new _OTNode ; 
		assert( *ppNode );

		(*ppNode)->pTris = NULL ;
		m_pOTNodes[m_nOTNodes] = *ppNode ;
		m_dwMem_OT += sizeof(_OTNode) ;

		// default it to a terminal node
		for (int i = 0 ; i < 8 ; i ++)
			(*ppNode)->pKids[i] = NULL ;

		m_nOTNodes ++ ;

		assert( m_nOTNodes < MAX_OT_NODES );
	}
	void CBoundingBox::RecurseOctree( int		nSubdivides,
									  _OTNode* pNode )
	{
		_OTNode *p ;
		xVec3 vCenter ;
		int i ;

		// get the center of the current node
		vCenter.x = (pNode->vMax.x - pNode->vMin.x) / 2 + pNode->vMin.x ;
		vCenter.y = (pNode->vMax.y - pNode->vMin.y) / 2 + pNode->vMin.y ;
		vCenter.z = (pNode->vMax.z - pNode->vMin.z) / 2 + pNode->vMin.z ;

		// build the eight children
		for (i = 0 ; i < 8 ; i ++)
			InitOctreeNode((_OTNode**)&pNode->pKids[i]) ;

		// populate the children with volume data
		p = (_OTNode*)pNode->pKids[0] ;
		p->vMin = xVec3(pNode->vMin.x, pNode->vMin.y, pNode->vMin.z) ;	// near bottom left
		p->vMax = xVec3(vCenter.x, vCenter.y, vCenter.z) ; 
			
		p = (_OTNode*)pNode->pKids[1] ;
		p->vMin = xVec3(vCenter.x, pNode->vMin.y, pNode->vMin.z) ;		// near bottom right
		p->vMax = xVec3(pNode->vMax.x, vCenter.y, vCenter.z) ;

		p = (_OTNode*)pNode->pKids[2] ;
		p->vMin = xVec3(pNode->vMin.x, vCenter.y, pNode->vMin.z) ;		// near top left
		p->vMax = xVec3(vCenter.x, pNode->vMax.y, vCenter.z) ;

		p = (_OTNode*)pNode->pKids[3];
		p->vMin = xVec3(pNode->vMin.x, vCenter.y, pNode->vMin.z) ;		// near top right
		p->vMax = xVec3(vCenter.x, pNode->vMax.y, vCenter.z) ;

		p = (_OTNode*)pNode->pKids[4];
		p->vMin = xVec3(pNode->vMin.x, pNode->vMin.y, vCenter.z) ;		// near bottom left
		p->vMax = xVec3(vCenter.x, vCenter.y, pNode->vMax.z) ; 
			
		p = (_OTNode*)pNode->pKids[5] ;
		p->vMin = xVec3(vCenter.x, pNode->vMin.y, vCenter.z) ;			// far bottom right
		p->vMax = xVec3(pNode->vMax.x, vCenter.y, pNode->vMax.z) ;

		p = (_OTNode*)pNode->pKids[6] ;
		p->vMin = xVec3(pNode->vMin.x, vCenter.y, vCenter.z) ;			// far top left
		p->vMax = xVec3(vCenter.x, pNode->vMax.y, pNode->vMax.z) ;

		p = (_OTNode*)pNode->pKids[7] ;
		p->vMin = xVec3(pNode->vMin.x, vCenter.y, vCenter.z) ;			// far top right
		p->vMax = xVec3(vCenter.x, pNode->vMax.y, pNode->vMax.z) ;

		// recurse if not at the desired level
		if (nSubdivides > 1)
			for (i = 0 ; i < 8 ; i ++)
				RecurseOctree(nSubdivides - 1, (_OTNode*)pNode->pKids[i]) ;
		else
		// build list of polygons in volume for terminal nodes...
		{

		}
	}

/*
 *
 * World Matrix
 *
 */
	void CBoundingBox::ApplyMatrix( xMatUA* m )						// iterative applications
	{
		xVec4 v ;

	// apply to bounding vertices;
		D3DXVec3Transform( &v, &m_vMin, m );
		m_vMin.x = v.x ; m_vMin.y = v.y ; m_vMin.z = v.z ;

		D3DXVec3Transform( &v, &m_vMax, m );
		m_vMax.x = v.x ; m_vMax.y = v.y ; m_vMax.z = v.z ;

		D3DXVec3Transform( &v, &m_vCenter, m );
		m_vCenter.x = v.x ; m_vCenter.y = v.y ; m_vCenter.z = v.z ;

		for( int i=0; i<8; i++ )
		{
			D3DXVec3Transform( &v, &m_vVerts[ i ], m );
			m_vVerts[ i ].x = v.x ; m_vVerts[ i ].y = v.y ; m_vVerts[ i ].z = v.z ;
		}

//#ifdef _DEBUG
	// apply to world matrix;
		D3DXMatrixMultiply( &m_matWorld, &m_matWorld, m );
//#endif
	}

	void CBoundingBox::ApplyMatrixFromInitialValues( xMatUA* m )	// one-off matrix set
	{
		xVec4 v ;

	// apply to bounding vertices;
		D3DXVec3Transform( &v, &m_vInitialMin, m );
		m_vMin.x = v.x ; m_vMin.y = v.y ; m_vMin.z = v.z ;

		D3DXVec3Transform( &v, &m_vInitialMax, m );
		m_vMax.x = v.x ; m_vMax.y = v.y ; m_vMax.z = v.z ;

		D3DXVec3Transform( &v, &m_vInitialCenter, m );
		m_vCenter.x = v.x ; m_vCenter.y = v.y ; m_vCenter.z = v.z ;

		for( int i=0; i<8; i++ )
		{
			D3DXVec3Transform( &v, &m_vInitialVerts[ i ], m );
			m_vVerts[ i ].x = v.x ; m_vVerts[ i ].y = v.y ; m_vVerts[ i ].z = v.z ;
		}

//#ifdef _DEBUG
	// apply to world matrix;
		m_matWorld = *m ;
//#endif
	}

/*
 *
 * Render Data Creation.
 *
 */
	HRESULT CBoundingBox::MakeRenderData( LPDIRECT3DDEVICE8 pDev )
	{
	//
	// Create & fill the VB:
	//
		HRESULT hr ;
		UINT iFVFSize = sizeof(COLOREDVERTEX) ;
		hr = pDev->CreateVertexBuffer(8 * iFVFSize, D3DUSAGE_WRITEONLY/*D3DUSAGE_SOFTWAREPROCESSING*/, MYFVF_COLOREDVERTEX, D3DPOOL_SYSTEMMEM, &m_pVB) ;
		assert( !FAILED(hr) );

		BYTE *pVerts ;
		hr = m_pVB->Lock(0, 0, &pVerts, 0) ;
		assert( !FAILED(hr) );

		memcpy(pVerts, m_fvfVerts, sizeof(COLOREDVERTEX) * 8) ;
		m_dwMem += sizeof(COLOREDVERTEX) * 8 ;
		m_pVB->Unlock() ;

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
 * Collision Detection:
 *
 * Note that pvDir is any second point along the ray (not a *normalized* direction),
 * and fDistance is how far down the ray to test.
 *
 */

/*
FIXME:
	CBoundingBox::LineIntersect()
	Doesn't seeem to work on bounding boxes that have had their positions modified with
	WorldRotateXYZ() fns. [but seems to work when I use WorldTranslate()!]
*/
	BOOL CBoundingBox::LineIntersects( xVec3* pvLineStart,
									   xVec3* pvDir,
									   float		fDistance )
	{
		xVec3 v ;
		
		D3DXVec3Subtract(&v, pvDir, pvLineStart) ;
		D3DXVec3Normalize(&v, &v) ;
		v = v * fDistance ;
		return D3DXBoxBoundProbe(&m_vMin, &m_vMax, pvLineStart, &v) ;
	}

/*
 *
 * D3DFrame Event Hooks. Maintain the device-dependent D3D VBs and IBs, and D3DX Mesh objects.
 *
 */
	HRESULT CBoundingBox::RestoreDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		HRESULT hr = S_OK ;
#ifdef FULL_DBG_TRACE
		ods( "CBoundingBox::RestoreDeviceObjects\n" );
#endif

#ifdef _DEBUG
		hr = MakeRenderData( pDev );
		assert( hr==S_OK );
#endif

		return hr ;
	}
	HRESULT	CBoundingBox::InvalidateDeviceObjects()
	{

#ifdef _DEBUG
		SAFE_RELEASE( m_pMesh )
		SAFE_RELEASE( m_pVB )
		SAFE_RELEASE( m_pIB )
		SAFE_RELEASE( m_pVBProcessed )
#endif

		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CBoundingBox::Render( LPDIRECT3DDEVICE8 pDev, unsigned long* pulPolyCount, xMatUA* pmatWorld )
	{
#ifdef _DEBUG
		LPDIRECT3DVERTEXBUFFER8 pVB ;
		LPDIRECT3DINDEXBUFFER8 pIB ;
		
		pDev->SetRenderState(D3DRS_LIGHTING, FALSE) ;
		pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) ;
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
		pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;

		if( pmatWorld )
			pDev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)pmatWorld );
		else
			pDev->SetTransform( D3DTS_WORLD, &m_matWorld );

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
#endif

		return S_OK ;
	}
