#pragma warning( disable : 4183 )	// 'missing return type'

#include "COcean.h"

/*
 *
 * Constructor / Destructors.
 *
 */
	COcean::COcean( float f, int nTex, D3DXCOLOR* pColour )
	{
	//
	// Render Object:
	//

		m_pRO = NULL ;
		m_pRO = new CProceduralRenderObject() ;
		assert( m_pRO );

		m_pRO->m_dwFVF = MYFVF_COLOREDVERTEX/*2TEX*/ ;
		m_pRO->m_dwMeshOptions = D3DXMESH_VB_WRITEONLY | D3DXMESH_IB_WRITEONLY ;
		
		//m_pRO->m_bUseD3DXMesh = FALSE ;
		//m_pRO->m_dwUsageOptions = D3DUSAGE_WRITEONLY ;

		m_nTex = nTex ;
		m_Colour = *pColour ;
		
	//
	// Geometry:
	//
		HRESULT hr ;
		hr = Create() ;
		assert( hr==S_OK );
		
		m_pRO->ScaleX7LTs( f );

		hr = m_pRO->IndexToList() ; 
		assert( hr==S_OK );

	//
	// Tide:
	//
		m_fTideDirection = +1.0f ;
		m_fTideHeight = 0.0f ;
		m_fTideMaxHeight = +f / 2000.0f ;
		m_fTideMinHeight = -f / 2000.0f ;

	//
	// Texture Movement:
	//
		m_vTexVel = xVec2( rnd(), rnd() );
		m_ndxCurTex = 0 ;
	}
	COcean::~COcean()
	{
		SAFE_DELETE(m_pRO)
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT COcean::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "COcean::RestoreDeviceObjects\n" );

		if (FAILED(m_pRO->RestoreDeviceObjects(pD3D, pDev)))
			return E_FAIL ;

		return S_OK ;
	}
	HRESULT	COcean::InvalidateDeviceObjects()
	{
		m_pRO->InvalidateDeviceObjects() ;
		return S_OK ;
	}
    HRESULT COcean::Render(LPDIRECT3DDEVICE8 pDev,
						   CTextures *pTex,
						   unsigned long *pulPolyCount,
						   xMatUA *pmatCurWorld,
						   xVec3* pvEyePt)
	{
		xMat matTrans, matWorld ;
		D3DXMatrixTranslation( &matTrans, pvEyePt->x * 0.75f, 0.0f, pvEyePt->z  * 0.75f );
		D3DXMatrixMultiply( &matWorld, &matTrans, pmatCurWorld );

		m_pRO->Render( pDev, pTex, pulPolyCount, 0, &matWorld, false );

		return S_OK ;
	}
    HRESULT COcean::FrameMove(LPDIRECT3DDEVICE8 pDev,
							  CUserInput *pInput,
							  float fFPS,
							  CCamera *pCamera,
							  xMatUA *pmatProj,
							  D3DVIEWPORT8 *pVP)
	{


		static float fTime = 0.0f ;

	//
	// Modulate tide height:
	//
		/*m_fTideHeight += 0.2f * (60.0f / fFPS) * m_fTideDirection ;
		if (m_fTideHeight >= m_fTideMaxHeight || m_fTideHeight <= m_fTideMinHeight)
			m_fTideDirection *= -1.0f ;
		D3DXMatrixTranslation(&m_pRO->m_matWorld, 0.0f, m_fTideHeight, 0.0f) ;*/

	//
	// Rotate caustic texture:
	//
		m_ndxCurTex = (int)((fTime+=(0.5f/fFPS))*32.0f)%31;
		m_pRO->m_ntvRS[0].ndxTex = m_nTex/*CAUSTIC2_TEX_START*/ + m_ndxCurTex ;

		// move texture offsets
		//
		// should do this with texture animation;
		/*for (int nRS = 0 ; nRS < m_pRO->m_nRSs ; nRS ++)
		{
			LPCOLOREDVERTEX pVerts ;
			if (FAILED(m_pRO->m_ntvRS[nRS].pMesh->LockVertexBuffer(0, (BYTE**)&pVerts)))
			{
				Msg("COcean::FrameMove: FAILED(pMesh->LockVertexBuffer(...)) == TRUE") ;
				return E_FAIL ;
			}
			for (unsigned int i = 0 ; i < m_pRO->m_ntvRS[nRS].dwNumVertices ; i ++)
			{
				COLOREDVERTEX cv ;
				xVec3 v3 ;
				cv = *pVerts ;
				cv.tu1 += (60.0f / fFPS) * m_vTexVel.x * 0.001f ;
				cv.tv1 += (60.0f / fFPS) * m_vTexVel.y * 0.001f ;
				*pVerts = cv ;
				pVerts++ ;
			}
			m_pRO->m_ntvRS[nRS].pMesh->UnlockVertexBuffer() ;
		}
		m_vTexVel.x *= 0.99f ;
		m_vTexVel.y *= 0.99f ;
		if (fabs(m_vTexVel.x) < 0.01f || fabs(m_vTexVel.y) < 0.01f)
			m_vTexVel = xVec2(rnd() - 0.5f, rnd() - 0.5f) ;*/

		return S_OK ;
	}

/*
 *
 * 
 *
 */
	HRESULT COcean::Create()
	{
		UNCOLOREDVERTEX *pVertices ;
		DWORD	         dwNumVertices ;
		WORD			*pIndices ;
		DWORD			 dwNumIndices ;

	//
	// Allocate and create geom.
	//
		/*dwNumVertices = 4 ;
		pVertices = (UNCOLOREDVERTEX*)malloc(sizeof(UNCOLOREDVERTEX) * dwNumVertices) ;
		if (!pVertices)
		{
			Msg("COcean::Create: malloc[1] == NULL") ;
			return E_FAIL ;
		}
		dwNumIndices = 6 ;
		pIndices = (WORD*)malloc(sizeof(WORD) * dwNumIndices) ;
		if (!pIndices)
		{
			Msg("COcean::Create: malloc[2] == NULL") ;
			return E_FAIL ;
		}
		// single quad
		float t = 150.0f ;
		pVertices[0].x = -0.5f ; pVertices[0].y = +0.0f ; pVertices[0].z = -0.5f ; pVertices[0].tu1 = 0.00f * t ; pVertices[0].tv1 = 1.00f * t  ;
		pVertices[1].x = +0.5f ; pVertices[1].y = +0.0f ; pVertices[1].z = -0.5f ; pVertices[1].tu1 = 0.00f * t ; pVertices[1].tv1 = 0.00f * t  ;
		pVertices[2].x = -0.5f ; pVertices[2].y = +0.0f ; pVertices[2].z = +0.5f ; pVertices[2].tu1 = 1.00f * t ; pVertices[2].tv1 = 1.00f * t  ;
		pVertices[3].x = +0.5f ; pVertices[3].y = +0.0f ; pVertices[3].z = +0.5f ; pVertices[3].tu1 = 1.00f * t ; pVertices[3].tv1 = 0.00f * t  ;
		pIndices[0] = 0 ; pIndices[1] = 1 ; pIndices[2] = 2 ;
		pIndices[3] = 1 ; pIndices[4] = 2 ; pIndices[5] = 3 ;*/

		const int SQ = 8 ;
		float t = 150.0f ;
		dwNumVertices = SQ * SQ ;
		pVertices = (UNCOLOREDVERTEX*)malloc(sizeof(UNCOLOREDVERTEX) * dwNumVertices) ;
		assert( pVertices );

		dwNumIndices = (SQ - 1) * (SQ - 1) * 2 * 3 ;
		pIndices = (WORD*)malloc(sizeof(WORD) * dwNumIndices) ;
		assert( pIndices );

		int nVert = 0, nIndex = 0 ;
		for (int y = 0 ; y < SQ ; y ++)
		{
			for (int x = 0 ; x < SQ ; x ++)
			{
				// build verts
				float fX, fY ;
				fX = (float)x / (float)(SQ - 1) ;
				fY = (float)y / (float)(SQ - 1) ;
				pVertices[nVert].tu1 = fX * t ;
				pVertices[nVert].tv1 = fY * t ;
				pVertices[nVert].x = fX - 0.5f ;
				pVertices[nVert].y = 0.0f ;
				pVertices[nVert].z = fY - 0.5f ;
				
				// build indices
				if (x < SQ - 1 && y < SQ - 1)
				{
					pIndices[nIndex++] = nVert ;
					pIndices[nIndex++] = nVert + 1 ;
					pIndices[nIndex++] = nVert + SQ ;
				}
				if (x < SQ - 1 && y > 0)
				{
					pIndices[nIndex++] = nVert ;
					pIndices[nIndex++] = nVert + 1 ;
					pIndices[nIndex++] = nVert + 1 - SQ ;
				}
				
				nVert++ ;
			}
		}
		
	//
	// xfer vertex data to underlying RenderObject:
	//
		/*float r = rnd(), g = rnd(), b = rnd() ;*/

		for (unsigned i = 0 ; i < dwNumIndices ; i += 3)
		{
			UNCOLOREDVERTEX v[3] ;
			memcpy(&v[0], &pVertices[pIndices[i+0]], sizeof(UNCOLOREDVERTEX)) ;
			memcpy(&v[1], &pVertices[pIndices[i+1]], sizeof(UNCOLOREDVERTEX)) ;
			memcpy(&v[2], &pVertices[pIndices[i+2]], sizeof(UNCOLOREDVERTEX)) ;
			
			D3DCOLOR c[3] ;
			D3DCOLOR s[3] ;
			c[ 0 ] = c[ 1 ] = c[ 2 ] = /*D3DXCOLOR( 1, 0, 0, 0.5f );*/	m_Colour ;
			s[ 0 ] = s[ 1 ] = s[ 2 ] = D3DXCOLOR( 1, 1, 1, 0 );	

			float fTU2[3], fTV2[3] ;
			fTU2[0] = v[0].tu1 ; fTV2[0] = v[0].tv1 ;
			fTU2[1] = v[1].tu1 ; fTV2[1] = v[1].tv1 ;
			fTU2[2] = v[2].tu1 ; fTV2[2] = v[2].tv1 ;

			if (m_pRO->AddX7LT( v,
								&c[0],
								&s[0],
								/*CAUSTIC2_TEX_START*/m_nTex + 0,
								NULL/*TRACK_TEX_START + 1*/,
								NULL/*fTU2*/,
								NULL/*fTV*/
							   ) == NULL)
				return E_FAIL ;
		}

	//
	// Release mem.:
	//
		free( pVertices );
		free( pIndices );

		return S_OK ;
	}

