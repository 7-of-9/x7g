#pragma warning( disable : 4183 )	// 'missing return type'

#include "CSky.h"

/*
 *
 * Constructor / Destructors.
 *
 */
	CSky::CSky( float f, BOOL bBlackSky )
	{
		HRESULT hr ;

	//
	// Render Object:
	//
		m_pRO = NULL ;
		m_pRO = new CProceduralRenderObject() ;
		assert( m_pRO );

		m_pRO->m_dwFVF = MYFVF_COLOREDVERTEX ; //WITHNORMAL2TEX ;
		/*m_pRO->m_dwMeshOptions = D3DXMESH_VB_DYNAMIC | D3DXMESH_IB_WRITEONLY ;*/
		
	//
	// Geometry:
	//
		hr = Create( bBlackSky );
		assert( hr==S_OK );
		
		m_pRO->ScaleX7LTs( f );
		//m_pRO->TranslateX7LTs(0.0f, -f / 4.0f, 0.0f) ;

		hr = m_pRO->IndexToList() ;
		assert( hr==S_OK );
	}
	CSky::~CSky()
	{
		SAFE_DELETE(m_pRO)
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT CSky::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev)
	{
		ods( "CSky::RestoreDeviceObjects\n" );

		if (FAILED(m_pRO->RestoreDeviceObjects(pD3D, pDev)))
			return E_FAIL ;

		return S_OK ;
	}
	HRESULT	CSky::InvalidateDeviceObjects()
	{
		m_pRO->InvalidateDeviceObjects() ;
		return S_OK ;
	}
    HRESULT CSky::Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, xMatUA *pmatCurWorld, xVec3* pvEyePt)
	{
		xMat matTrans, matWorld ;
		D3DXMatrixTranslation( &matTrans, pvEyePt->x, 0.0f, pvEyePt->z );
		D3DXMatrixMultiply( &matWorld, &matTrans, pmatCurWorld );

	// ask for auto-gen'd tex. coords;
		/*pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,
									   D3DTSS_TCI_CAMERASPACENORMAL );
									   //D3DTSS_TCI_CAMERASPACEPOSITION );*/

		m_pRO->Render( pDev, pTex, pulPolyCount, 0, &matWorld, false );

	// turn off auto-gen'd tex. coords;
		/*pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU  );*/

		return S_OK ;
	}
    HRESULT CSky::FrameMove(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP)
	{
		/*for (int nRS = 0 ; nRS < m_pRO->m_nRSs ; nRS ++)
		{
			LPCOLOREDVERTEXWITHNORMAL2TEX pVerts ;
			if (FAILED(m_pRO->m_ntvRS[nRS].pMesh->LockVertexBuffer(0, (BYTE**)&pVerts)))
			{
				Msg("CSky::FrameMove: FAILED(pMesh->LockVertexBuffer(...)) == TRUE") ;
				return E_FAIL ;
			}
			for (unsigned int i = 0 ; i < m_pRO->m_ntvRS[nRS].dwNumVertices ; i ++)
			{
				COLOREDVERTEXWITHNORMAL2TEX cv ;
				xVec3 v3 ;
				cv = *pVerts ;
				
				cv.tu1 += (60.0f / fFPS) / 1000.0f ;

				*pVerts = cv ;
				pVerts++ ;
			}
			m_pRO->m_ntvRS[nRS].pMesh->UnlockVertexBuffer() ;
		}*/

		return S_OK ;
	}

/*
 *
 * Creates a skydome with unit radius.
 *
 */
#define SD_MAX_VS 2048

#define SD_SQ 9

	HRESULT CSky::Create( BOOL bBlackSky )
	{
		UNCOLOREDVERTEX *pVertices ;
		DWORD	         dwNumVertices ;
		WORD			*pIndices ;
		DWORD			 dwNumIndices ;

	//
	// Skydome method 1:
	// FIXME: excessive allocation.
	//
		/*pVertices = (UNCOLOREDVERTEX*)malloc(sizeof(UNCOLOREDVERTEX) * SD_MAX_VS) ;
		if (!pVertices)
			return E_FAIL ;
		
		pIndices  = (WORD*)malloc(sizeof(WORD) * SD_MAX_VS * 3) ;
		if (!pIndices)
			return E_FAIL ;
		
		if (CreateHalfSphere ( pVertices,
							   &dwNumVertices,
							   pIndices,
 							   &dwNumIndices,
							   1,
							   10) != S_OK)
		  return E_FAIL ; 

		D3DCOLOR c[3], s[3] ;
		c[0]=c[1]=c[2] = D3DCOLOR_ARGB( 255,255,000,000 );	//D3DXCOLOR(1,1,0,1);
		s[0]=s[1]=s[2] = D3DCOLOR_ARGB( 255,255,255,255 );	//D3DXCOLOR(1,1,1,1);
		for( int i=0 ; i < dwNumIndices ; i+=3 )
		{
			UNCOLOREDVERTEX vs[3] ;
			vs[0] = pVertices[ pIndices[i+0] ];
			vs[1] = pVertices[ pIndices[i+1] ];
			vs[2] = pVertices[ pIndices[i+2] ];
			if( m_pRO->AddX7LT( vs,
								&c[0],
								&s[0],
								MISC_TEX_START+1,
								0,	
								0,	
								0	 ) == NULL )
				return E_FAIL ;
		}*/

	//
	// Skydome method 2:
	//
		 /*dwNumVertices = SD_SQ * SD_SQ ;
		pVertices = (UNCOLOREDVERTEX*)malloc(sizeof(UNCOLOREDVERTEX) * dwNumVertices) ;
		if (!pVertices)
		{
			Msg("CSky::Create: malloc[1] == NULL") ;
			return E_FAIL ;
		}
		dwNumIndices = (SD_SQ - 1) * (SD_SQ - 1) * 2 * 3 ;
		pIndices = (WORD*)malloc(sizeof(WORD) * dwNumIndices) ;
		if (!pIndices)
		{
			Msg("CSky::Create: malloc[2] == NULL") ;
			return E_FAIL ;
		}

		int nVert = 0, nIndex = 0 ;
		float fDomeCurve = 0.25f ;
		for (int y = 0 ; y < SD_SQ ; y ++)
		{
			for (int x = 0 ; x < SD_SQ ; x ++)
			{
				// build verts
				float fX, fY ;
				fX = (float)x / (float)(SD_SQ - 1) ;
				fY = (float)y / (float)(SD_SQ - 1) ;
				pVertices[nVert].tu1 = fX ;
				pVertices[nVert].tv1 = fY ;
				pVertices[nVert].x = fX - 0.5f ;
				pVertices[nVert].y = 0.0f + ((sin( g_PI * fX ) * fDomeCurve) + (sin( g_PI * fY ) * fDomeCurve) / 2.0f) ;
				pVertices[nVert].z = fY - 0.5f ;
				
				// build indices
				if (x < SD_SQ - 1 && y < SD_SQ - 1)
				{
					pIndices[nIndex++] = nVert ;
					pIndices[nIndex++] = nVert + 1 ;
					pIndices[nIndex++] = nVert + SD_SQ ;
				}
				if (x < SD_SQ - 1 && y > 0)
				{
					pIndices[nIndex++] = nVert ;
					pIndices[nIndex++] = nVert + 1 ;
					pIndices[nIndex++] = nVert + 1 - SD_SQ ;
				}
				
				nVert++ ;
			}
		}*/

	//
	// Sky box:
	// 
		dwNumVertices = 24 ;
		pVertices = (UNCOLOREDVERTEX*)malloc(sizeof(UNCOLOREDVERTEX) * dwNumVertices) ;
		assert( pVertices );

		dwNumIndices = 36 ;
		pIndices = (WORD*)malloc(sizeof(WORD) * dwNumIndices) ;
		assert( pIndices );

		float fStartTexCoord = 0.002f ;
		float fEndTexCoord = 0.998f ;

		// front face
		pVertices[0].x = -0.5f ;  pVertices[0].y = +0.5f ;  pVertices[0].z = +0.5f ;  pVertices[0].tu1 = fStartTexCoord ;  pVertices[0].tv1 = fStartTexCoord ;
		pVertices[1].x = +0.5f ;  pVertices[1].y = +0.5f ;  pVertices[1].z = +0.5f ;  pVertices[1].tu1 = fEndTexCoord ;  pVertices[1].tv1 = fStartTexCoord ;
		pVertices[2].x = -0.5f ;  pVertices[2].y = -0.5f ;  pVertices[2].z = +0.5f ;  pVertices[2].tu1 = fStartTexCoord ;  pVertices[2].tv1 = fEndTexCoord ;
		pVertices[3].x = +0.5f ;  pVertices[3].y = -0.5f ;  pVertices[3].z = +0.5f ;  pVertices[3].tu1 = fEndTexCoord ;  pVertices[3].tv1 = fEndTexCoord ;
		pIndices[0] = 0 ; pIndices[1] = 1 ; pIndices[2] = 2 ;
		pIndices[3] = 1 ; pIndices[4] = 2 ; pIndices[5] = 3 ;
		// right face
		pVertices[4].x = +0.5f ;  pVertices[4].y = +0.5f ;  pVertices[4].z = -0.5f ;  pVertices[4].tu1 = fEndTexCoord ;  pVertices[4].tv1 = fStartTexCoord ;
		pVertices[5].x = +0.5f ;  pVertices[5].y = +0.5f ;  pVertices[5].z = +0.5f ;  pVertices[5].tu1 = fStartTexCoord ;  pVertices[5].tv1 = fStartTexCoord ;
		pVertices[6].x = +0.5f ;  pVertices[6].y = -0.5f ;  pVertices[6].z = -0.5f ;  pVertices[6].tu1 = fEndTexCoord ;  pVertices[6].tv1 = fEndTexCoord ;
		pVertices[7].x = +0.5f ;  pVertices[7].y = -0.5f ;  pVertices[7].z = +0.5f ;  pVertices[7].tu1 = fStartTexCoord ;  pVertices[7].tv1 = fEndTexCoord ;
		pIndices[6] = 4 ; pIndices[7] = 5 ; pIndices[8] = 6 ;
		pIndices[9] = 5 ; pIndices[10] = 6 ; pIndices[11] = 7 ;
		// back face
		pVertices[8].x = -0.5f ;  pVertices[8].y = +0.5f ;	pVertices[8].z = -0.5f ;  pVertices[8].tu1 = fEndTexCoord ;  pVertices[8].tv1 = fStartTexCoord ;
		pVertices[9].x = +0.5f ;  pVertices[9].y = +0.5f ;	pVertices[9].z = -0.5f ;  pVertices[9].tu1 = fStartTexCoord ;  pVertices[9].tv1 = fStartTexCoord ;
		pVertices[10].x = -0.5f ; pVertices[10].y = -0.5f ; pVertices[10].z = -0.5f ; pVertices[10].tu1 = fEndTexCoord ; pVertices[10].tv1 = fEndTexCoord ;
		pVertices[11].x = +0.5f ; pVertices[11].y = -0.5f ; pVertices[11].z = -0.5f ; pVertices[11].tu1 = fStartTexCoord ; pVertices[11].tv1 = fEndTexCoord ;
		pIndices[12] = 8 ; pIndices[13] = 9 ; pIndices[14] = 10 ;
		pIndices[15] = 9 ; pIndices[16] = 10 ; pIndices[17] = 11 ;
		// left face
		pVertices[12].x = -0.5f ; pVertices[12].y = +0.5f ; pVertices[12].z = -0.5f ; pVertices[12].tu1 = fStartTexCoord ; pVertices[12].tv1 = fStartTexCoord ;
		pVertices[13].x = -0.5f ; pVertices[13].y = +0.5f ; pVertices[13].z = +0.5f ; pVertices[13].tu1 = fEndTexCoord ; pVertices[13].tv1 = fStartTexCoord ;
		pVertices[14].x = -0.5f ; pVertices[14].y = -0.5f ; pVertices[14].z = -0.5f ; pVertices[14].tu1 = fStartTexCoord ; pVertices[14].tv1 = fEndTexCoord ;
		pVertices[15].x = -0.5f ; pVertices[15].y = -0.5f ; pVertices[15].z = +0.5f ; pVertices[15].tu1 = fEndTexCoord ; pVertices[15].tv1 = fEndTexCoord ;
		pIndices[18] = 12 ; pIndices[19] = 13 ; pIndices[20] = 14 ;
		pIndices[21] = 13 ; pIndices[22] = 14 ; pIndices[23] = 15 ;
		// up face
		pVertices[16].x = -0.5f ; pVertices[16].y = +0.5f ; pVertices[16].z = -0.5f ; pVertices[16].tu1 = fStartTexCoord ; pVertices[16].tv1 = fEndTexCoord ;
		pVertices[17].x = +0.5f ; pVertices[17].y = +0.5f ; pVertices[17].z = -0.5f ; pVertices[17].tu1 = fStartTexCoord ; pVertices[17].tv1 = fStartTexCoord ;
		pVertices[18].x = -0.5f ; pVertices[18].y = +0.5f ; pVertices[18].z = +0.5f ; pVertices[18].tu1 = fEndTexCoord ; pVertices[18].tv1 = fEndTexCoord ;
		pVertices[19].x = +0.5f ; pVertices[19].y = +0.5f ; pVertices[19].z = +0.5f ; pVertices[19].tu1 = fEndTexCoord ; pVertices[19].tv1 = fStartTexCoord ;
		pIndices[24] = 16 ; pIndices[25] = 17 ; pIndices[26] = 18 ;
		pIndices[27] = 17 ; pIndices[28] = 18 ; pIndices[29] = 19 ;
		// down face
		pVertices[20].x = -0.5f ; pVertices[20].y = -0.5f ; pVertices[20].z = -0.5f ; pVertices[20].tu1 = fStartTexCoord ; pVertices[20].tv1 = fEndTexCoord ;
		pVertices[21].x = +0.5f ; pVertices[21].y = -0.5f ; pVertices[21].z = -0.5f ; pVertices[21].tu1 = fStartTexCoord ; pVertices[21].tv1 = fStartTexCoord ;
		pVertices[22].x = -0.5f ; pVertices[22].y = -0.5f ; pVertices[22].z = +0.5f ; pVertices[22].tu1 = fEndTexCoord ; pVertices[22].tv1 = fEndTexCoord ;
		pVertices[23].x = +0.5f ; pVertices[23].y = -0.5f ; pVertices[23].z = +0.5f ; pVertices[23].tu1 = fEndTexCoord ; pVertices[23].tv1 = fStartTexCoord ;
		pIndices[30] = 20 ; pIndices[31] = 21 ; pIndices[32] = 22 ;
		pIndices[33] = 21 ; pIndices[34] = 22 ; pIndices[35] = 23 ;
		
	//
	// xfer vertex data to underlying RenderObject:
	//
		r1 = 1 ; //rnd() ;
		r2 = 1 ; //rnd() ;
		int nTex ;
		if( bBlackSky )
			r1 = r2 = 0 ;

		for (unsigned i = 0 ; i < dwNumIndices ; i += 3)
		{
			UNCOLOREDVERTEX v[3] ;
			memcpy(&v[0], &pVertices[pIndices[i+0]], sizeof(UNCOLOREDVERTEX)) ;
			memcpy(&v[1], &pVertices[pIndices[i+1]], sizeof(UNCOLOREDVERTEX)) ;
			memcpy(&v[2], &pVertices[pIndices[i+2]], sizeof(UNCOLOREDVERTEX)) ;
			
			D3DCOLOR c[3];
			float y, a ;
			
			y = (v[0].y / 1.0f) ;
			a = y * 64.0f ;
			c[0] = D3DCOLOR_ARGB((int)a, (int)((1-y) * r1 * 128), (int)((1-y)/r1 * r2 * 128), (int)(y/r2 * 128)) ;

			y = (v[1].y / 1.0f) ;
			a = y * 64.0f ;
			c[1] = D3DCOLOR_ARGB((int)a, (int)((1-y) * r1 * 128), (int)((1-y)/r1 * r2 * 128), (int)(y/r2 * 128)) ;

			y = (v[2].y / 1.0f) ;
			a = y * 64.0f ;
			c[2] = D3DCOLOR_ARGB((int)a, (int)((1-y) * r1 * 128), (int)((1-y)/r1 * r2 * 128), (int)(y/r2 * 128)) ;

			D3DCOLOR s[3] ;
			s[0] = D3DCOLOR_ARGB(0,0,0,0) ;
			s[1] = D3DCOLOR_ARGB(0,0,0,0) ;
			s[2] = D3DCOLOR_ARGB(0,0,0,0) ;

			// second tex. coords.
			float fTU2[3], fTV2[3] ;
			fTU2[0] = v[0].tu1 * 2.0f ;
			fTU2[1] = v[1].tu1 * 2.0f ;
			fTU2[2] = v[2].tu1 * 2.0f ;
			fTV2[0] = v[0].tv1 * 2.0f ;
			fTV2[1] = v[1].tv1 * 2.0f ;
			fTV2[2] = v[2].tv1 * 2.0f ;

			if (i == 0) nTex = SKY_TEX_START + 0 ;
			if (i == 6) nTex = SKY_TEX_START + 1 ;
			if (i == 12) nTex = SKY_TEX_START + 2 ;
			if (i == 18) nTex = SKY_TEX_START + 3 ;
			if (i == 24) nTex = SKY_TEX_START + 4 ;
			if (i == 30) nTex = SKY_TEX_START + 5 ;

			if (m_pRO->AddX7LT(v, &c[0], &s[0], nTex, 0, fTU2, fTV2) == NULL)
				return E_FAIL ;
		}

	//
	// Release mem.:
	//
		free(pVertices) ;
		free(pIndices) ;

		return S_OK ;
	}

/*
 *
 * Adapted from Boids.cpp
 *
 * FIXME: texturing of top vertices.
 *
 */
	HRESULT CSky::CreateHalfSphere (UNCOLOREDVERTEX* pVertices,
										DWORD*		 pdwNumVertices,
										WORD*		 pIndices,
										DWORD*		 pdwNumIndices,
										FLOAT		 fRadius,
										DWORD		 dwNumRings)
	{
		DWORD      dwNumVertices = (dwNumRings*(2*dwNumRings+1)+2);
		DWORD      dwNumIndices  = 6*(dwNumRings*2)*((dwNumRings-1)+1);

		assert( dwNumVertices < SD_MAX_VS );
		assert( dwNumIndices < SD_MAX_VS * 3 );

		// Counters
		WORD x, y, vtx = 0, index = 0;

		// Angle deltas for constructing the sphere's vertices
		FLOAT fDAng   = g_PI / dwNumRings ;
		FLOAT fDAngY0 = fDAng;

		float t = 8 ;

		// Make top
		xVec3 vy ;
		vy.x = 0 ; vy.y = 2 ; vy.z = 0 ;

		WORD wNorthVtx = vtx;
		*pVertices++ = UNCOLOREDVERTEX( (fRadius*vy/2).x,
									(fRadius*vy/2).y,
									(fRadius*vy/2).z,
									vy.x, vy.y, vy.z,
									0.0f, 1.0f );
		vtx++;

		for( x=0; x<(dwNumRings*2)+1; x++ )
		{
			WORD p1 = wNorthVtx;
			WORD p2 = (WORD)( (0)*(dwNumRings*2+1) + (x+1) );
			WORD p3 = (WORD)( (0)*(dwNumRings*2+1) + (x+0) );

			*pIndices++ = p1;
			*pIndices++ = p3;
			*pIndices++ = p2;
			index += 3;
		}

		// Make the middle of the sphere
		for( y=0; y<(float)dwNumRings * 0.5f ; y++)
		{
			FLOAT y0 = (FLOAT)cos(fDAngY0);
			FLOAT r0 = (FLOAT)sin(fDAngY0);
			FLOAT tv = (1.0f - y0)/2;

			for( x=0; x<(dwNumRings*2)+1; x++ )
			{
				FLOAT fDAngX0 = x*fDAng;
        
				xVec3 v( r0*(FLOAT)sin(fDAngX0), y0, r0*(FLOAT)cos(fDAngX0) );
				FLOAT tu = 1.0f - x/(dwNumRings*2.0f);

				v =-v ;
				*pVertices++ = UNCOLOREDVERTEX( (fRadius*v).x,
											(fRadius*v).y,
											(fRadius*v).z,
											v.x, v.y, v.z,
											tu *t, tv *t );
				vtx ++;
			}
			fDAngY0 += fDAng;
		}

		for( y=0; y < (float)dwNumRings * 0.5f -1; y++ )
		{
			for( x=0; x<(dwNumRings*2); x++ )
			{
				*pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+0) +1);
				*pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) +1);
				*pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) +1);
				*pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) +1);
				*pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) +1); 
				*pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+1) +1);
				index += 6;
			}
		}

		*pdwNumVertices = vtx;
		*pdwNumIndices  = index;

		return S_OK ;
	}

