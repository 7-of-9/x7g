#pragma warning( disable : 4183 )	// 'missing return type'

#include "CParticleSystem.h"

D3DXCOLOR g_ParticleColor[ NUM_PARTICLE_COLORS ] =
{
	D3DXCOLOR( 1.0f,   1.0f,   1.0f,   1.0f ),
	D3DXCOLOR( 1.0f,   0.5f,   0.4f,   1.0f ),
	D3DXCOLOR( 0.3f,   0.2f,   1.0f,   0.5f )
};
D3DXCOLOR g_ParticleColorFade[ NUM_PARTICLE_COLORS ] =
{
	D3DXCOLOR( 0.1f,  0.1f,   0.1f,   0.1f ),
	D3DXCOLOR( 0.1f,  0.1f,   0.1f,   0.1f ),
	D3DXCOLOR( 0.1f,  0.1f,   0.1f,   0.1f )
};

	CParticleSystem::CParticleSystem( DWORD dwFlush, DWORD dwDiscard, float fRadius )
	{
		m_fRadius        = fRadius;

		m_dwBase         = dwDiscard;
		m_dwFlush        = dwFlush;
		m_dwDiscard      = dwDiscard;

		m_dwParticles    = 0;
		m_dwParticlesLim = 1024 * 10;

		m_pParticles     = NULL;
		m_pParticlesFree = NULL;

		m_fTime			 = 0.0f ;
	}
	CParticleSystem::~CParticleSystem()
	{
		Clear() ;
	}
	
	CParticleSystem::Clear()
	{
		while( m_pParticles )
		{
			PARTICLE* pSpark = m_pParticles;
			m_pParticles = pSpark->m_pNext;
			delete pSpark;
		}
		m_pParticles = NULL ;
		m_dwParticles = 0 ;

		while( m_pParticlesFree )
		{
			PARTICLE *pSpark = m_pParticlesFree;
			m_pParticlesFree = pSpark->m_pNext;
			delete pSpark;
		}	
		m_pParticlesFree = NULL ;
	}

	HRESULT CParticleSystem::Render( LPDIRECT3DDEVICE8 pDev,
									 float fPointSize,
									 unsigned long* pulPolycount,
									 LPDIRECT3DVERTEXBUFFER8 pVB,
									 xMatUA* pmatWorld,
									 BOOL bMustFillVB,
									 xMatUA* pmatCurWorld )
	{
		HRESULT hr = S_OK ;
		xMat matWorld ;

		if( pmatWorld )
		{
			/*if( pmatCurWorld )
			{
				D3DXMatrixMultiply( &matWorld, pmatCurWorld, pmatWorld );
				pDev->SetTransform( D3DTS_WORLD, &matWorld );
			}
			else*/
				pDev->SetTransform( D3DTS_WORLD, pmatWorld );
		}
		else
		{
			/*if( pmatCurWorld )
			{
				D3DXMatrixMultiply( &matWorld, pmatCurWorld, pmatWorld );
				pDev->SetTransform( D3DTS_WORLD, &matWorld );
			}
			else*/
			{
				pDev->SetTransform( D3DTS_WORLD, &g_matIdent );
			}
		}

		pDev->SetRenderState( D3DRS_LIGHTING,  FALSE );
		pDev->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		pDev->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		pDev->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		pDev->SetRenderState( D3DRS_POINTSIZE,     FtoDW(fPointSize) );
		pDev->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
		/*pDev->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(1.00f) );
		pDev->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
		pDev->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(0.00f) );*/
	

		pDev->SetStreamSource( 0, pVB, sizeof( POINTSPRITEVERTEX ) );
		pDev->SetVertexShader( MYFVF_POINTSPRITEVERTEX );

		if( bMustFillVB )
		{
			PARTICLE*    pParticle = m_pParticles;
			POINTSPRITEVERTEX* pVertices;
			DWORD        dwNumParticlesToRender = 0;

			// Lock the vertex buffer.  We fill the vertex buffer in small
			// chunks, using D3DLOCK_NOOVERWRITE.  When we are done filling
			// each chunk, we call DrawPrim, and lock the next chunk.  When
			// we run out of space in the vertex buffer, we start over at
			// the beginning, using D3DLOCK_DISCARD.

			/*m_dwBase += m_dwFlush;

			if(m_dwBase >= m_dwDiscard)
				m_dwBase = 0;*/
			m_dwBase = 0 ;

			if(FAILED(hr = pVB->Lock(m_dwBase * sizeof(POINTSPRITEVERTEX), m_dwFlush * sizeof(POINTSPRITEVERTEX),
				(BYTE **) &pVertices, /*m_dwBase ? D3DLOCK_NOOVERWRITE :*/ D3DLOCK_DISCARD)))
			{
				goto done ;
			}

			// Render each particle
			while( pParticle )
			{
				xVec3 vPos(pParticle->m_vPos);
				xVec3 vVel(pParticle->m_vVel);
				FLOAT       fLengthSq = D3DXVec3LengthSq(&vVel);
				UINT        dwSteps;

				// Render each particle a bunch of times to get a blurring effect
				// to be enabled
				/*if( fLengthSq < 1.0f )        dwSteps = 2;
				else if( fLengthSq <  4.00f ) dwSteps = 3;
				else if( fLengthSq <  9.00f ) dwSteps = 4;
				else if( fLengthSq < 12.25f ) dwSteps = 5;
				else if( fLengthSq < 16.00f ) dwSteps = 6;
				else if( fLengthSq < 20.25f ) dwSteps = 7;
				else                          dwSteps = 8;*/
				dwSteps = 1 ;

				vVel *= -0.04f / (FLOAT)dwSteps;

				
				D3DXCOLOR clrDiffuse;
				//D3DXColorLerp(&clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, pParticle->m_fFade );

				//float fT = m_fTime - pParticle->m_fTime0;

				D3DXColorLerp(&clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse,
										pParticle->m_fAbsLifeTime > 0.0f ?
											1.0f - (pParticle->m_fAge / pParticle->m_fAbsLifeTime)
											: 1.0f 
											/*pParticle->m_fFade*/ );

				DWORD dwDiffuse = (DWORD) clrDiffuse;

				// Render each particle a bunch of times to get a blurring effect
				for( DWORD i = 0; i < dwSteps; i++ )
				{
					pVertices->v     = vPos;
					pVertices->color = dwDiffuse;
					pVertices++;

					if( ++dwNumParticlesToRender == m_dwFlush )
					{
						// Done filling this chunk of the vertex buffer.  Lets unlock and
						// draw this portion so we can begin filling the next chunk.

						pVB->Unlock();

						if(FAILED(hr = pDev->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender)))
							goto done ;
						*pulPolycount += dwNumParticlesToRender ;

						// Lock the next chunk of the vertex buffer.  If we are at the 
						// end of the vertex buffer, DISCARD the vertex buffer and start
						// at the beginning.  Otherwise, specify NOOVERWRITE, so we can
						// continue filling the VB while the previous chunk is drawing.
						m_dwBase += m_dwFlush;

						if(m_dwBase >= m_dwDiscard)
							m_dwBase = 0;

						if(FAILED(hr = pVB->Lock(m_dwBase * sizeof(POINTSPRITEVERTEX), m_dwFlush * sizeof(POINTSPRITEVERTEX), (BYTE **) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
						{
							goto done ;
						}

						dwNumParticlesToRender = 0;
					}

					vPos += vVel;
				}

				pParticle = pParticle->m_pNext;
			}

			// Unlock the vertex buffer
			pVB->Unlock();

			// Render any remaining particles
			if( dwNumParticlesToRender )
			{
				if(FAILED(hr = pDev->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender )))
					goto done ;
				*pulPolycount += dwNumParticlesToRender ;
			}
		}
		else	// HACK
				//
				// quicker render, relies on previous call to this function left the VB intact
				// and ready to render - the only change we can apply in this case is thru the world matrix
				//
				// NOTE: also will only work if the # of particles in the last call was less than m_dwDiscard,
				// i.e. that the *entire* particle system is sat in the VB.
		{
			hr = pDev->DrawPrimitive( D3DPT_POINTLIST, 0, m_dwParticles );
			assert( !FAILED(hr) );
			*pulPolycount += m_dwParticles ;
		}

done:

		pDev->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
		pDev->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

		pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	
		return hr ;
	}

	CParticleSystem::ImpactTerrain( xVec3* pvDir, float fFPS, float fLenSQ, float fImpactFXLifetime, float fSpeed )
	{
		PARTICLE *pParticle, **ppParticle ;
		float f = (fLenSQ / 1000000.0f) * ( 60.0f / fFPS ) * fSpeed ;

		ppParticle = &m_pParticles;

		while( *ppParticle )
		{
			pParticle = *ppParticle ;

			pParticle->m_vVel0 = xVec3( pvDir->x * rnd() * f,
											  pvDir->y * rnd() * f,
											  pvDir->z * rnd() * f );

			pParticle->m_fAge = 0.0f ;
			pParticle->m_fAbsLifeTime = fImpactFXLifetime ;

			pParticle->m_clrFade = pParticle->m_clrDiffuse ;
			pParticle->m_clrFade.a = 0 ;
			pParticle->m_clrFade.r *= 0.1f ;
			pParticle->m_clrFade.g *= 0.1f ;
			pParticle->m_clrFade.b *= 0.1f ;

			pParticle->m_clrDiffuse.a = 0.0f ;

			//pParticle->m_bFeelGravity = TRUE ;

			ppParticle = &pParticle->m_pNext ;
		}
	}

	CParticleSystem::ImpactFighter( xVec3* pvDir, float fFPS, float fLenSQ, float fImpactFXLifetime, float fSpeed, xVec3* pvVel )
	{
		PARTICLE *pParticle, **ppParticle ;
		float f = (fLenSQ / 1000000.0f) * ( 60.0f / fFPS ) * fSpeed ;

		ppParticle = &m_pParticles;

		while( *ppParticle )
		{
			pParticle = *ppParticle ;

			pParticle->m_vVel0 *= 0.0f ;
			pParticle->m_vVel0 += *pvVel ;
			pParticle->m_vVel0 += xVec3( pvDir->x * (rnd()-0.5f) * f,
											   pvDir->y * (rnd()-0.5f) * f,
											   pvDir->z * (rnd()-0.5f) * f );

			pParticle->m_fAge = 0.0f ;
			pParticle->m_fAbsLifeTime = fImpactFXLifetime ;

			pParticle->m_clrFade = pParticle->m_clrDiffuse ;
			pParticle->m_clrFade.a = 0.1f ;

			//pParticle->m_bFeelGravity = TRUE ;

			ppParticle = &pParticle->m_pNext ;
		}
	}

	CParticleSystem::CulledFrameMove( xVec3* pvMove )
	{
		BOOL bGotFirst = FALSE ;
		PARTICLE *pParticle, **ppParticle ;
		ppParticle = &m_pParticles;

		while( *ppParticle )
		{
			pParticle = *ppParticle ;

		// move particle; (for Vel in units/frame);
			pParticle->m_vPos += pParticle->m_vVel0 ;
			pParticle->m_vPos += *pvMove ;

		// track bbox;
			if( !bGotFirst )
			{
				m_vMin = pParticle->m_vPos ;
				m_vMax = pParticle->m_vPos ;
				bGotFirst = TRUE ;
			}
			else
			{
				if( pParticle->m_vPos.x < m_vMin.x )
					m_vMin.x = pParticle->m_vPos.x ;
				if( pParticle->m_vPos.y < m_vMin.y )
					m_vMin.y = pParticle->m_vPos.y ;
				if( pParticle->m_vPos.z < m_vMin.z )
					m_vMin.z = pParticle->m_vPos.z ;

				if( pParticle->m_vPos.x > m_vMax.x )
					m_vMax.x = pParticle->m_vPos.x ;
				if( pParticle->m_vPos.y > m_vMax.y )
					m_vMax.y = pParticle->m_vPos.y ;
				if( pParticle->m_vPos.z > m_vMax.z )
					m_vMax.z = pParticle->m_vPos.z ;
			}

			ppParticle = &pParticle->m_pNext ;
		}
	}

	HRESULT CParticleSystem::FrameMove( float fFPS,
										xVec3* pvMove,
										FLOAT fSecsPerFrame,
										DWORD dwNumParticlesToEmit,
										FLOAT fAbsLifeTime,
										BOOL  bFeelGravity,
										const D3DXCOLOR &clrEmitColor,
										const D3DXCOLOR &clrFadeColor,
										xVec3* pvEmitVel,
										xVec3* pvEmitPosition,
										BOOL bSphericalEmit,
										float fScatter,
										BOOL bTrackBoundingVolume )
	{
		PARTICLE *pParticle, **ppParticle;
		BOOL bGotFirst = FALSE ;
		m_fTime += fSecsPerFrame;

		ppParticle = &m_pParticles;

		while( *ppParticle )
		{
			pParticle = *ppParticle;

			float fT = m_fTime - pParticle->m_fTime0;
			/*float fGravity;*/

		// age & fade particle;
			//pParticle->m_fAge += ( 60.0f / fFPS );
			pParticle->m_fAge = fT ;
			/*if( pParticle->m_bSpark )
			{
				if( pParticle->m_bFeelGravity )
					fGravity = -5.0f;
				else
					fGravity = 0.0f;
				pParticle->m_fFade -= fSecsPerFrame * 5.25f;
			}
			else
			{
				if( pParticle->m_bFeelGravity )
					fGravity = -9.8f;
				else
					fGravity = 0.0f;
				pParticle->m_fFade -= fSecsPerFrame * 0.25f ; 
			}*/

			
		// move particle; (for Vel in units/frame);
			pParticle->m_vPos += pParticle->m_vVel0 ;
			pParticle->m_vPos += *pvMove ;

			/*if( pParticle->m_bFeelGravity )
			{
				float fGravity = -9.8f;
				pParticle->m_vPos.y += (0.5f * fGravity) * (fT * fT);
				pParticle->m_vVel.y  = pParticle->m_vVel0.y + fGravity * fT;
			}*/

			/*if( pParticle->m_fFade < 0.0f )
				pParticle->m_fFade = 0.0f;*/

		// kill particles;
			BOOL bParticleMustDie = FALSE ;

			if( pParticle->m_fAbsLifeTime > 0.0f || pParticle->m_bMarkedForDeath )
			{
				if( fT > pParticle->m_fAbsLifeTime )
					bParticleMustDie = TRUE ;
			}
			else
			{
				//if( pParticle->m_vPos.y < 0.0f )
				//	bParticleMustDie = TRUE ;
			}

			if( bParticleMustDie )
			{
				// Emit sparks
				/*if( !pParticle->m_bSpark )
				{
					for( int i=0; i<4; i++ )
					{
						PARTICLE *pSpark;

						if( m_pParticlesFree )
						{
							pSpark = m_pParticlesFree;
							m_pParticlesFree = pSpark->m_pNext;
						}
						else
						{
							if( NULL == ( pSpark = new PARTICLE ) )
								return E_OUTOFMEMORY;
						}

						pSpark->m_pNext = pParticle->m_pNext;
						pParticle->m_pNext = pSpark;

						pSpark->m_bSpark  = TRUE;
						pSpark->m_vPos0   = pParticle->m_vPos;
						pSpark->m_vPos0.y = m_fRadius;

						FLOAT fRand1 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 2.00f;
						FLOAT fRand2 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 0.25f;

						pSpark->m_vVel0.x  = pParticle->m_vVel.x * 0.25f + cosf(fRand1) * sinf(fRand2);
						pSpark->m_vVel0.z  = pParticle->m_vVel.z * 0.25f + sinf(fRand1) * sinf(fRand2);
						pSpark->m_vVel0.y  = cosf(fRand2);
						pSpark->m_vVel0.y *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * 1.5f;

						pSpark->m_vPos = pSpark->m_vPos0;
						pSpark->m_vVel = pSpark->m_vVel0;

						D3DXColorLerp( &pSpark->m_clrDiffuse, &pParticle->m_clrFade,
									   &pParticle->m_clrDiffuse, pParticle->m_fFade );
						pSpark->m_clrFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
						pSpark->m_fFade   = 1.0f;
						pSpark->m_m_fTime0  = m_fTime;
						pSpark->m_fAbsLifeTime = 3.0f;
						pSpark->m_bFeelGravity = TRUE;
					}
				}*/

				// Kill particle
				*ppParticle = pParticle->m_pNext;
				pParticle->m_pNext = m_pParticlesFree;
				m_pParticlesFree = pParticle;

				if(!pParticle->m_bSpark)
					m_dwParticles--;
			}
			else
			{
				if( bTrackBoundingVolume )
				{
					if( !bGotFirst )
					{
						m_vMin = pParticle->m_vPos ;
						m_vMax = pParticle->m_vPos ;
						bGotFirst = TRUE ;
					}
					else
					{
						if( pParticle->m_vPos.x < m_vMin.x )
							m_vMin.x = pParticle->m_vPos.x ;
						if( pParticle->m_vPos.y < m_vMin.y )
							m_vMin.y = pParticle->m_vPos.y ;
						if( pParticle->m_vPos.z < m_vMin.z )
							m_vMin.z = pParticle->m_vPos.z ;

						if( pParticle->m_vPos.x > m_vMax.x )
							m_vMax.x = pParticle->m_vPos.x ;
						if( pParticle->m_vPos.y > m_vMax.y )
							m_vMax.y = pParticle->m_vPos.y ;
						if( pParticle->m_vPos.z > m_vMax.z )
							m_vMax.z = pParticle->m_vPos.z ;
					}
				}

				ppParticle = &pParticle->m_pNext;
			}
		}

		// Emit new particles
		DWORD dwParticlesEmit = m_dwParticles + dwNumParticlesToEmit;
		while( m_dwParticles < m_dwParticlesLim && m_dwParticles < dwParticlesEmit )
		{
			if( m_pParticlesFree )
			{
				pParticle = m_pParticlesFree;
				m_pParticlesFree = pParticle->m_pNext;
			}
			else
			{
				if( NULL == ( pParticle = new PARTICLE ) )
					return E_OUTOFMEMORY;
			}

			pParticle->m_pNext = m_pParticles;
			pParticle->m_bMarkedForDeath = FALSE ;
			m_pParticles = pParticle;
			m_dwParticles++;

			// Emit new particle
			FLOAT fRand1 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 2.0f;
			FLOAT fRand2 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 0.25f;

			pParticle->m_bSpark = FALSE;
			pParticle->m_fAge = 0.0f ;
	
			if( bSphericalEmit )
			{
				pParticle->m_vPos0 = *pvEmitPosition + xVec3( 0.0f, m_fRadius, 0.0f );
				pParticle->m_vVel0.x  = cosf(fRand1) * sinf(fRand2) * 2.5f;
				pParticle->m_vVel0.z  = sinf(fRand1) * sinf(fRand2) * 2.5f;
				pParticle->m_vVel0.y  = cosf(fRand1) * sinf(fRand2) * 2.5f;

				pParticle->m_vVel0.x *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DXVec3Length(pvEmitVel);
				pParticle->m_vVel0.y *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DXVec3Length(pvEmitVel);
				pParticle->m_vVel0.z *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DXVec3Length(pvEmitVel);
				
				pParticle->m_vVel0 += *pvEmitVel ;		// ??
			}
			else
			{
				pParticle->m_vPos0 = *pvEmitPosition ;
				pParticle->m_vVel0 = *pvEmitVel +
								xVec3((rnd()-0.5f)*fScatter, (rnd()-0.5f)*fScatter, (rnd()-0.5f)*fScatter) ;
			}

			pParticle->m_vPos = pParticle->m_vPos0;
			pParticle->m_vVel = pParticle->m_vVel0;

			pParticle->m_clrDiffuse   = clrEmitColor;
			pParticle->m_clrFade      = clrFadeColor;
			pParticle->m_fFade        = 1.0f;
			pParticle->m_fTime0       = m_fTime;
			pParticle->m_fAbsLifeTime = fAbsLifeTime;
			pParticle->m_bFeelGravity = bFeelGravity;

			/*if( bTrackBoundingVolume )
				if( m_dwParticles == 1 )
				{
					m_vMin = pParticle->m_vPos ;
					m_vMax = pParticle->m_vPos ;
				}
				else
				{
					if( pParticle->m_vPos.x < m_vMin.x && pParticle->m_vPos.y < m_vMin.y && pParticle->m_vPos.z < m_vMin.z )
						m_vMin = pParticle->m_vPos ;
					if( pParticle->m_vPos.x > m_vMax.x && pParticle->m_vPos.y > m_vMax.y && pParticle->m_vPos.z > m_vMax.z )
						m_vMax = pParticle->m_vPos ;
				}*/
		}
		
		return S_OK ;
	}
