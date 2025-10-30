#include "CExplosion.h"

EXPLOSION_TYPE_DETAILS g_Explosions[] = 
{
	{ EXP2_TEX_START,	34 },	// ET_ALPHA
	{ EXP_TEX_START,	15 }	// ET_BETA
} ;

/*
 *
 * Construction / Destruction.
 * 
 */
	CExplosion::CExplosion( float fLifespan, BOOL bShockwave, BOOL bParticles, int nSubexplosions )
	{
		m_fLifespan = fLifespan ;
		m_nSubExps = 0 ;
		m_fAge = 0.0f ;

		InitSubExplosion( ET_ALPHA, fLifespan * 1.00f, 0.5f + rnd() * 0.5f, 0.0f, &xVec2( 0.0f, 0.0f ) );
		
		for( int i=0 ; i<nSubexplosions ; i++ )
			InitSubExplosion(
							  rnd() < 0.5f ? ET_ALPHA : ET_BETA,
							  rnd() * fLifespan * 0.5f,					// lifespan
							  rnd() * 0.5f,								// size
							  rnd() * fLifespan * 0.5f,					// delay
							  &xVec2( rnd()-0.5f, rnd()-0.5f )	// positional offset; -0.5 to +0.5
							);

		m_pBB = NULL ;
		m_pBB = new CBillboard( 1.0f, 1.0f, FALSE, &D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f) );
		assert( m_pBB );

		m_pBB_Shockwave = NULL ;
		if( bShockwave )
		{
			m_pBB_Shockwave = new CBillboard( 1.0f, 1.0f, TRUE, &D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.1f) );
			assert( m_pBB_Shockwave );
		}
		
		m_pPS_Explosion = NULL ;
		if( bParticles )
		{
			m_pPS_Explosion = new CParticleSystem( 512, 2048, 0.0f );
			assert( m_pPS_Explosion );
		}
	}
	CExplosion::~CExplosion()
	{
		SAFE_DELETE( m_pBB )
		SAFE_DELETE( m_pPS_Explosion )
		SAFE_DELETE( m_pBB_Shockwave )
	}

	CExplosion::InitSubExplosion( e_ExpTypes Type, float fLifespan, float fRelSize, float fDelay, xVec2* pvRelCenter )
	{
		assert( m_nSubExps < MAX_EXPLOSION_SUBEXPS );

		m_SubExp[ m_nSubExps ].fDelay = fDelay ;
		m_SubExp[ m_nSubExps ].bActive = FALSE ;
		m_SubExp[ m_nSubExps ].vRelCenter = *pvRelCenter ;
		m_SubExp[ m_nSubExps ].fRelSize = fRelSize ;
		m_SubExp[ m_nSubExps ].ndxTex = g_Explosions[ Type ].ndxTex ;
		m_SubExp[ m_nSubExps ].ndxMaxTexFrame = g_Explosions[ Type ].ndxMaxTex ;
		m_SubExp[ m_nSubExps ].ndxCurTexFrame = 0 ;
		m_SubExp[ m_nSubExps ].fLifespan = fLifespan ;
		m_SubExp[ m_nSubExps ].fCurFrameAge = 0.0f ;
		m_nSubExps++ ;
	}

	CExplosion::Reset()
	{
	// resets size, position, lifetime and delay of subexplosions
	// note that the number of type of subexplosions is fixed in the constructor
		m_fAge = 0.0f ;
		for( int i=0 ; i < m_nSubExps ; i++ )
		{
			if( 0==i )
			{	// ensure primary is larger
				m_SubExp[ i ].fRelSize = 0.5f + rnd() * 0.5f ;
			}
			if( i > 0 )
			{
				m_SubExp[ i ].fRelSize = rnd() * 0.5f ;
				m_SubExp[ i ].fLifespan = rnd() * m_fLifespan * 0.5f ;
				m_SubExp[ i ].fDelay = rnd() * m_fLifespan * 0.5f ;
				m_SubExp[ i ].vRelCenter = xVec2( rnd()-0.5f, rnd()-0.5f );
			}
			m_SubExp[ i ].ndxCurTexFrame = 0 ;
			m_SubExp[ i ].fCurFrameAge = 0.0f ;
			m_SubExp[ i ].bActive = FALSE ;
		}

		if( m_pPS_Explosion )
			m_pPS_Explosion->Clear() ;
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT CExplosion::RestoreDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		ods( "CExplosion::RestoreDeviceObjects\n" );

		m_pBB->RestoreDeviceObjects( pDev );
		if( m_pBB_Shockwave )
			m_pBB_Shockwave->RestoreDeviceObjects( pDev );

		return S_OK ;
	}
	HRESULT	CExplosion::InvalidateDeviceObjects()
	{
		m_pBB->InvalidateDeviceObjects();
		if( m_pBB_Shockwave )
			m_pBB_Shockwave->InvalidateDeviceObjects() ;

		return S_OK ;
	}
	HRESULT CExplosion::FrameMove( LPDIRECT3DDEVICE8 pDev, float fFPS, xVec3* pvSourceVelocity, xVec3* pvSource )
	{
		m_fAge += ( 1.0f / fFPS );

	// advance sub-explosion animations
		for( int i=0 ; i < m_nSubExps ; i++ )
		{
			if( m_fAge > m_SubExp[i].fDelay )
				m_SubExp[i].bActive = TRUE ;

			if( m_SubExp[i].ndxCurTexFrame != -1 && m_SubExp[i].bActive)
			{
				m_SubExp[i].fCurFrameAge += ( 1.0f / fFPS );
				if( m_SubExp[i].fCurFrameAge >=
						(m_SubExp[i].fLifespan / (m_SubExp[i].ndxMaxTexFrame + 1) ))
				{
					m_SubExp[i].fCurFrameAge = 0.0f ;
					if( ++m_SubExp[i].ndxCurTexFrame == m_SubExp[i].ndxMaxTexFrame - 1 )
						m_SubExp[i].ndxCurTexFrame = -1 ;
				}
			}
		}

	// advance particles
		if( m_pPS_Explosion )
			m_pPS_Explosion->FrameMove( fFPS,
										pvSourceVelocity,
										m_fAge,
										(m_fAge > m_fLifespan * 0.5f) ? 0 : (int)(15.0f * (60.0f/fFPS)),
										m_fLifespan * 0.25f,
										FALSE,
										g_ParticleColor[ (int)(rnd()*3) ],
										g_ParticleColorFade[ (int)(rnd()*3) ],
										(xVec3*)&( ( 20.0f * (1.0f/fFPS) * xVec3( rnd() - 0.5f, 0.0f, rnd() - 0.5f ) ) ),
										pvSource,
										TRUE,
										0.0f,
										FALSE );

		return S_OK ;
	}

/*
 *
 * Render Methods.
 *
 */
	HRESULT CExplosion::Render( LPDIRECT3DDEVICE8 pDev, CTextures* pTex, unsigned long *pulPolyCount, CCamera *pCam, float fSize, xVec3* pvCenter, float fShockwaveRadius, LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB, int SB, int DB )
	{
		pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );	
		pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

	// render subexplosions at specified positions and sizes
		for( int i=0 ; i < m_nSubExps ; i++ )
		{
			if( m_SubExp[i].ndxCurTexFrame != -1 && m_SubExp[i].bActive )
			{
				xVec3 vPos ;
				D3DXVec3Lerp( &vPos, pvCenter, &pCam->m_vEyePt, 0.01f * i );
				vPos.x -= m_SubExp[i].vRelCenter.x * fSize * 0.25f ;
				vPos.z -= m_SubExp[i].vRelCenter.x * fSize * 0.25f ;
				vPos.y -= m_SubExp[i].vRelCenter.y * fSize * 0.25f ;

				pDev->SetTexture( 0, pTex->m_pTex[ m_SubExp[i].ndxTex + m_SubExp[i].ndxCurTexFrame ] );
				
				/*if( i > 0 ) {
					pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );	
					pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				}*/

				m_pBB->Render( pDev,
							   pulPolyCount,
							   (xVec3*)&vPos,
							   pCam,
							   fSize * m_SubExp[ i ].fRelSize ,
							   TRUE );
			}
		}

	// particles
		if( m_pPS_Explosion )
		{
			pDev->SetTexture( 0, pTex->m_pTex[ PARTICLE_TEX_START+0 ] );
			m_pPS_Explosion->Render( pDev, fSize / 4.0f, pulPolyCount, pSharedParticleSystemVB, NULL, TRUE, NULL );
		}

	// shockwave, rendered with gadget blender;
		if( m_pBB_Shockwave )
		{
			float i = m_fAge / m_fLifespan ;
			pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
			pDev->SetTexture( 0, pTex->m_pTex[ EXP_TEX_START+17 ] );
			m_pBB_Shockwave->Render( pDev, pulPolyCount, (xVec3*)pvCenter, pCam, fShockwaveRadius * i, FALSE );
		}

	// all done
		pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		return S_OK ;
	}

