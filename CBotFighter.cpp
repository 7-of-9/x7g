#include "CFighters.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CBotFighter::CBotFighter( float fSkill,
							  int nLevel,
							  int/*e_FighterTypes*/ ftType,
							  float fMyBSRadius,
							  CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData )
							  :
		CFighter( fSkill, ftType, fMyBSRadius, pSharedRenderData, FALSE )
	{
		m_fDistanceToTargetLastFrame = 0 ;
		
		m_fThrustMagSkillModifier = 0.75f + rnd()*0.25f + fSkill ;
		m_bBotIsDead = false ;
		m_bBotWasKilledByProjectile = false ;

		m_nBotLives = nLevel ;
	}
	CBotFighter::~CBotFighter()
	{
	}

/*
 *
 * Flight Controls: (AI)
 *
 */
	void CBotFighter::FC_Set( LPDIRECT3DDEVICE8 pDev, CRace* pRace, float fFPS )
	{
		float		fFlightSpeed = 0.75f ;	// relative to user's fighter
		float		fTurnSpeed = 0.75f ;	// relative to user's fighter
		BOOL		bTargetVisible ;
		float		fTargetDistance, fAngleX, fAngleY ;
		xMat		matMyView ;

	// dbg;
		/*if( m_dwID % 2 == 1 )
			m_MovementMode = BFMM_STATIONARY ;
		else*/
			m_MovementMode = BFMM_HUNTING ;

	// have we just hit the deck? if so, enter terrain collision response mode;
		if( m_bTerrainCollision )
		{
			m_PrevMovementMode = m_MovementMode ;
			m_MovementMode = BFMM_TERRAINCOLLISIONRESPONSE ;
			m_fTerrainCollisionResponseTime = 0 ;
		}

	// determine target's visibility;
		xVec3 vX = ( m_vOrientation * m_pFRO->m_pRenderData->m_fBSphereRadius );
		xVec3 vEyePt = m_pFRO->m_pBBox->m_vCenter + (vX * 0.0f) ;
		xVec3 vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX * 0.5f) ;
		D3DXMatrixLookAtLH( &matMyView, &vEyePt, &vLookAtPt, &xVec3( 0,1,0 ) );
		bTargetVisible = GetBBoxVisibility( pDev,
											pRace->m_pX7->m_matProj,
											matMyView,
											&pRace->m_pX7->m_vpViewport,
											pRace->m_pFighter->m_pFRO->m_pBBox->m_vMin,
											pRace->m_pFighter->m_pFRO->m_pBBox->m_vMax );

	// set null input;
		FC_Zero() ;

	// get target's distance;
		fTargetDistance = fabsf( D3DXVec3Length( &( m_vPosition - pRace->m_pFighter->m_vPosition ) ) );

	// don't do anything first frame!
		if( m_fDistanceToTargetLastFrame == 0 )
		{
			m_fDistanceToTargetLastFrame = fTargetDistance ;
			return ;
		}

	//
	// Move according to current movement mode:
	//
		switch( m_MovementMode )
		{
		//
		// Stationary:
		//
			case BFMM_STATIONARY:
				break ;

		//
		// Terrain Collision Response:
		// Point straight up and power on for fixed length of time!
		//
			case BFMM_TERRAINCOLLISIONRESPONSE:
			// align directly up;
				FC_AlignWithTarget( xVec3( m_vPosition.x, m_vPosition.y + 1, m_vPosition.z ),
									&fAngleX, &fAngleY, fTurnSpeed );

			// power on;
				//m_FlightControls.fAccelerate = MAX_FLIGHTMAP * fFlightSpeed ;

			// stay in this mode for fixed length of time;
				m_fTerrainCollisionResponseTime += ( 1 / fFPS );
				if( m_fTerrainCollisionResponseTime > 30.5f )
				{
					m_MovementMode = m_PrevMovementMode ;
				}
				break ;

		//
		// Forward facing zombie tracking, with no terrain avoidance:
		//
			case BFMM_HUNTING:
			// align with target;
				FC_AlignWithTarget( pRace->m_pFighter->m_vPosition, &fAngleX, &fAngleY, fTurnSpeed );

			// fire if target is visible and we're nearly aligned with it;
				if( bTargetVisible &&
					fabsf( fAngleX ) < g_PI * 0.1f &&
					fabsf( fAngleY ) < g_PI * 0.1f )
				{
				// fire rate proportional to target's speed;
					float i = pRace->m_pFighter->m_fThrustMag
								/ pRace->m_pFighter->m_fMaxThrustMag ;
					if( i < 0.05f )	i = 0 ;
					float fMinProb = 0.0f ;//+ ( 0.15f * m_fSkill );
					float fMaxProb = 0.5f + ( 0.45f * m_fSkill );
					float fProb = fMinProb + ( fMaxProb - fMinProb ) * i ;
					if( rnd() < fProb )
						m_FlightControls.bFireA = TRUE ;
					if( rnd() < 0.1f )
						m_FlightControls.bNextWeapon = TRUE ;
				}

			// we should never enter the exclusion zone;
				float fExclusionZone = 6 ;		// BS radii
			// the scale zone is outside of the exclusion zone and when in it we scale our acceleration by our
			// distance through the scale zone, 1 as we enter, 0 as we leave and enter the exclusion zone.
				float fScaleZone = 100 ;		// BS radii

				fExclusionZone *= m_pFRO->m_pRenderData->m_fBSphereRadius ;
				fScaleZone *= m_pFRO->m_pRenderData->m_fBSphereRadius ;

				if( fTargetDistance < fExclusionZone )
				{
				// in exclusion zone, brake scaled to thrust.
					//ods("in exclusion zone.\n");

					m_FlightControls.fBrake = MAX_FLIGHTMAP * fFlightSpeed * 
						( m_fThrustMag * m_fThrustMagSkillModifier / m_fMaxThrustMag );
				}
				else if( fTargetDistance < ( fExclusionZone + fScaleZone ) )
				{
				// in scale zone, apply scaled acceleration.
					float i = ( fTargetDistance - fExclusionZone ) / fScaleZone ;
					//ods("in scale zone: %0.1f\n", i);

					if( fTargetDistance > m_fDistanceToTargetLastFrame )
						m_FlightControls.fAccelerate = MAX_FLIGHTMAP * fFlightSpeed * i ;
				}
				else
				{
				// outside of brake zone, full acceleration always.
					//ods("in void zone\n");

					m_FlightControls.fAccelerate = MAX_FLIGHTMAP * fFlightSpeed ;
				}

			// DEPRECATED in favour of above:
			//
			// if we're nearly in line with the target, then accelerate towards it
			// until we get to a certain distance, then back off the accelerator
				/*if( (fAngleX + fAngleY) / 2 < 0.5f )
				{
					D3DXPLANE plnFacingWall ; float fDot ;
					D3DXPlaneFromPoints( &plnFacingWall,
			 							&( m_vPosition + m_vLeft ), // one click left
										&( m_vPosition - m_vLeft ), // one click right
										&( m_vPosition + m_vUp ) ); // one click up
					fDot = D3DXPlaneDotCoord( &plnFacingWall, &pRace->m_pFighter->m_vPosition );

				// compute relative approach velocity;
					xVec3 vRelVel = 
 					  ( m_vOrientation * m_fSpeed ) - 
					  ( pRace->m_pFighter->m_vOrientation * pRace->m_pFighter->m_fSpeed );
					float fRelSpeed = D3DXVec3Length( &vRelVel );
					float fRelVel = fabsf( D3DXVec3Length( &vRelVel ) );
					float fRelVelI ;
					if( fRelVel < 0 )
						fRelVel = 0 ;
					fRelVelI = fRelVel / m_fMaxThrustMag ;
					fRelSpeed /= m_fMaxThrustMag ;

				// compute distance to target inside which we need to brake;
					//float fBrakeDistance = m_pFRO->m_pRenderData->m_fBSphereRadius * 100 * fRelVelI ;
					float fBrakeDistance = m_pFRO->m_pRenderData->m_fBSphereRadius * 10 ;

					if( fTargetDistance > fBrakeDistance )
					{
						//if( fDot < 0 )
						//	m_FlightControls.fBrake = MAX_FLIGHTMAP * 0.5f ;
						//if( fDot > 0 )
							m_FlightControls.fAccelerate = MAX_FLIGHTMAP * fFlightSpeed ;
					}
					else
					{
						if( m_fThrustMag > 0 )
							m_FlightControls.fBrake = MAX_FLIGHTMAP * fFlightSpeed ;
					}
				}*/
				break ;
		}
		m_fDistanceToTargetLastFrame = fTargetDistance ;
	}
	void CBotFighter::FC_Zero()
	{
		m_FlightControls.bFireA = FALSE ;
		m_FlightControls.bFireB = FALSE ;
		m_FlightControls.bNextWeapon = FALSE ;
		m_FlightControls.bPrevWeapon = FALSE ;
		m_FlightControls.fAccelerate = 0 ;
		m_FlightControls.fBrake = 0 ;
		m_FlightControls.fClimb = 0 ;
		m_FlightControls.fDive = 0 ;
		m_FlightControls.fRollLeft = 0 ;
		m_FlightControls.fRollRight = 0 ;
		m_FlightControls.fSteerLeft = 0 ;
		m_FlightControls.fSteerRight = 0 ;
		m_FlightControls.fStrafeLeft = 0 ;
		m_FlightControls.fStrafeRight = 0 ;
		m_FlightControls.bSteerAndRoll = FALSE ;
	}
	void CBotFighter::FC_AlignWithTarget( const xVec3& vTarget, float* fAngleX, float* fAngleY, float fTurnSpeed )
	{
		D3DXPLANE plnGround, plnAhead ;
		xVec3 vTargetNorm ;
		float fDot, fExtent ;

		vTargetNorm = vTarget - m_vPosition ;
		D3DXVec3Normalize( &vTargetNorm, &vTargetNorm );

	// compute flat 'ground' plane;
		D3DXPlaneFromPoints( &plnGround,
							 &( m_vPosition + m_vOrientation ), // straight ahead, one click
							 &( m_vPosition + m_vLeft ),		// one click left
							 &( m_vPosition - m_vLeft ) );		// one click right
	// compute 'ahead' plane;
		D3DXPlaneFromPoints( &plnAhead,
							 &( m_vPosition + m_vOrientation ), // straight ahead, one click
							 &( m_vPosition ),					// center
							 &( m_vPosition + m_vUp ) );		// one click up

	// is target below or above?
		fDot = D3DXPlaneDotNormal( &plnGround, &vTargetNorm );
		*fAngleY = fabsf( fDot );
		fExtent = ( *fAngleY / 1 ) * 10 ;						// this smoothes out the motion nicely!

		if( fDot > 0 )
			m_FlightControls.fDive = MAX_FLIGHTMAP * fTurnSpeed * fExtent ;
		if( fDot < 0 )
			m_FlightControls.fClimb = MAX_FLIGHTMAP * fTurnSpeed * fExtent ;

	// is target to the left or the right?
		fDot = D3DXPlaneDotNormal( &plnAhead, &vTargetNorm );
		*fAngleX = fabsf( fDot );
		fExtent = ( *fAngleX / 1 ) * 10 ;

		if( fDot < 0 )
			m_FlightControls.fSteerLeft = MAX_FLIGHTMAP * fTurnSpeed * fExtent ;
		if( fDot > 0 )
			m_FlightControls.fSteerRight= MAX_FLIGHTMAP * fTurnSpeed * fExtent ;
	}

/*
 *
 * D3DFrame Event Hooks:
 *
 */
	int CBotFighter::FrameMove( LPDIRECT3DDEVICE8 pDev,
								CUserInput *pInput,
								float fFPS,
								CCamera *pCamera,
								xMatUA *pmatProj,
								D3DVIEWPORT8 *pVP,
								BOOL bManageCamera,
								CTerrain *pTerrain,
								int ctCamMode,
								float fElapsedTime,
								CRace* pRace )
	{

	//
	// Decide what to do:
	//
		FC_Set( pDev, pRace, fFPS );

	//
	// Process Flight Controls (Physics):
	//
		/*if( m_dwID % 2 == 0 )
			ProcessPhysicsFromFlightControls( &pInput->m_FlightControls, fFPS, pTerrain, TRUE, pRace, pDev, fElapsedTime, NULL );
		else*/
			ProcessPhysicsFromFlightControls( &m_FlightControls, fFPS, pTerrain, TRUE, pRace, pDev, fElapsedTime, NULL );

	//
	// Baseclass FrameMove:
	//
		int ret ;
		ret = CFighter::FrameMove( pDev, pInput, fFPS, pCamera, pmatProj, pVP, bManageCamera, pTerrain, ctCamMode, fElapsedTime, pRace );

	//
	// Respawn or Die:
	//
		if( m_bJustFinishedExploding )
		{
			m_bJustFinishedExploding = false ;

			if( m_bBotWasKilledByProjectile )
			{
				if( --m_nBotLives == 0 )
					m_bBotIsDead = true ;
				else
					Respawn( pTerrain );
			}
			else 
				Respawn( pTerrain );
		}

		return ret ;
	}

	HRESULT CBotFighter::RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM, CTextures* pTex, float fAspect )
	{		CFighter::RestoreDeviceObjects( pD3D, pDev, pOM, pTex, fAspect );

		ods( "CBotFighter::RestoreDeviceObjects\n" );

		return S_OK ;
	}

	HRESULT	CBotFighter::InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev ) 
	{
		CFighter::InvalidateDeviceObjects( pDev );
		return S_OK ;
	}
