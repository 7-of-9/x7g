#pragma warning( disable : 4183 )

#include "CFighters.h"


/*
 *
 * Construction / Destruction:
 *
 */
CFighter::CFighter( float fSkill,
					int/*e_FighterTypes*/ ftType,
					float fMyBSRadius,
					CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData,
					BOOL bContrails )
	{
		int i ;
		m_Type = ftType ;
		m_dwID = 0 ;	// internal dbg use only

		m_bUserFighter = FALSE ;

	//
	// Skill Modifier:
	//
		m_fSkill = fSkill ;

	//
	// Render Object Instantiation:
	//
		m_pFRO = NULL ;
#ifdef LOAD_RES_FROM_FILE
		char szRelativePath[ 512 ];
		sprintf( szRelativePath, "..\\resource\\mod0\\%s", g_Fighters[ ftType ].szFilename );
		m_pFRO = new CFileRenderObject( szRelativePath,	0, pSharedRenderData );
#else
		m_pFRO = new CFileRenderObject( NULL, g_Fighters[ ftType ].nResID, pSharedRenderData );
#endif
		assert( m_pFRO );

	//
	// Fighter Render Option:
	//
		m_rtFighterMode = /*FRT_NORMAL*/FRT_GLASS ;

	//
	// Explosion FX System:
	//
		m_fExpLifespan = 1.5f ;
		m_fExpAge = 0 ;
		m_pExp = NULL ;
		m_pExp = new CExplosion( m_fExpLifespan, TRUE, FALSE, 5 );

	//
	// Contrail FX System:
	//
		if( bContrails )
		{
			m_nContrailEmitters = g_Fighters[ ftType ].nContrailEmitters ;
			for( i=0 ; i < m_nContrailEmitters ; i++ )
				m_pPS_Contrails[ i ] = NULL ;
			for( i=0 ; i < m_nContrailEmitters ; i++ )
			{
				m_vContrailEmitters[ i ] = (xVec3)g_Fighters[ ftType ].vContrailEmitters[ i ];
				m_pPS_Contrails[ i ] = new CParticleSystem( 512, 2048, 0 );

				m_pPS_Contrails[ i ]->m_vMin = xVec3( 0,0,0 );
				m_pPS_Contrails[ i ]->m_vMax = xVec3( 0,0,0 );

				assert( m_pPS_Contrails[ i ] );
			}
			m_clrContrailEmitColour = D3DXCOLOR( rnd(), rnd(), rnd(), rnd() * 0.1f + 0.1f );
			m_clrContrailFadeColour = D3DXCOLOR( rnd(), rnd(), rnd(), 0.05f );
			m_fContrailSize = 1 ; //0.5f + rnd() ;
			m_fContrailScatter = 1 + rnd() * m_fContrailSize ;
		}
		else
			m_nContrailEmitters = 0 ;

	//
	// Weapons & Projectiles System:
	//
		m_Weapon = WT_GREENLASER ;
		m_nWeaponEmitters = g_Fighters[ ftType ].nWeaponEmitters ;
		m_fBaseWeaponEmmisionSpeedModifier = g_Fighters[ ftType ].fBaseWeaponEmmisionSpeedModifier ;
		m_fLastProjectileEmmitedTime = 0.0f ;
		m_ndxCurWeaponEmitter = 0 ;
		for( i=0 ; i < m_nWeaponEmitters ; i++ )
			m_vWeaponEmitters[ i ] = (xVec3)g_Fighters[ ftType ].vWeaponEmitters[ i ];
		m_pProjectiles = NULL ;
		m_nProjectiles = 0 ;

	//
	// Shield:
	//
		m_bShowingShieldEffect = FALSE ;
		m_fShieldEffectLifetime = 0.0f ;
		m_fShieldEffectAge = 0.0f ;
		m_bShieldLevelChanged = FALSE ;
		m_bJustFinishedExploding = FALSE ;

	//
	// Physics & initial orientation & position:
	//
		m_fsState = FS_NORMAL ;
		m_fSpeed = 0.0f ;

		//m_fMaxThrustMag = fTerrainLenSQ / 500.0f ;
		//m_fThrustDelta = ( fTerrainLenSQ / 100000.0f ) * 60.0f ;
		m_fMaxThrustMag = fMyBSRadius * 8 ;
		m_fThrustDelta = m_fMaxThrustMag / 2 ;
		m_fThrustMag = 0 ;
		m_fStrafeThrustMag = 0 ;

		m_vOrientation = xVec3(0.0f, 0.0f, -1.0f) ;
		m_vUp = xVec3( 0,1,0 );
		m_vInitialOrientation = xVec3(0.0f, 0.0f, -1.0f) ;

		m_vDirectionOfTravel = xVec3(0.0f, 0.0f, 0.0f) ;

		m_bJustBouncedOffTerrain = FALSE ;

		m_fSteeringSensitivity = 6.0f ;
		m_fRollSensitivity = 3.0f ;

		m_vExternalForce = xVec3(0.0f, 0.0f, 0.0f) ;

		m_vPosition = xVec3(0.0f, 0.0f, 0.0f) ;

		m_fPitch = m_fYaw = m_fRoll = 0.0f ;
		m_matTrans = g_matIdent ;
		m_matRot = g_matIdent ;

		m_fDistanceToGround = -1 ;

	//
	// Terrain Force:
	//
		m_fMaxTerrainForce = 0 ;				// zero for no terrain force! CUserFighter overrides this.
		m_fTerrainForceBalanceAltitude = 0 ;
		m_fTerrainForce = 0 ;

	//
	// Systems:
	//
		m_fShieldLevel = 1.0f ;
		m_fWeaponsLevel = 0.1f ;
		m_fEngineLevel = 0.1f ;

	//
	// Dynamic Camera:
	//
		for( i=0 ; i<THRUSTMAG_HISTORY_BUF ; i++ )
			m_fThrustMagHistory[ i ] = 0 ;
	}
	CFighter::~CFighter()
	{
		SAFE_DELETE( m_pFRO )
		
		for( int i=0; i<m_nContrailEmitters ; i++ )
			SAFE_DELETE( m_pPS_Contrails[ i ] )

		SAFE_DELETE( m_pExp )

		//Msg2( "deleting %d projectiles", m_nProjectiles );
		FIGHTER_PROJECTILE* pProjectile, *pDeletedProjectile ;
		pProjectile = m_pProjectiles ;
		while( pProjectile )
		{
			pDeletedProjectile = pProjectile ;
			pProjectile = pProjectile->pNext ;
			SAFE_DELETE( pDeletedProjectile->pPS )
			SAFE_DELETE( pDeletedProjectile->pBBox )
			delete pDeletedProjectile ;
		}
		m_pProjectiles = NULL ;
		m_nProjectiles = 0 ;
	}

/*
 *
 * Flight Controls Processing:
 *
 */
	void CFighter::ProcessPhysicsFromFlightControls( FLIGHT_CONTROLS* pFC, float fFPS, CTerrain *pTerrain, BOOL bAcceptInput, CRace* pRace, LPDIRECT3DDEVICE8 pDev, float fElapsedTime, float* pfYawDelta )
	{
		xMat		m0, m1, matRes ;
		xVec3		vDelta ;
		xVec4		v ;
		float		fTestPitch = m_fPitch,
					fTestYaw = m_fYaw,
					fTestRoll = m_fRoll,
					fTestThrustMag = m_fThrustMag,
					fTestStrafeThrustMag = m_fStrafeThrustMag ;
		xVec3		vTestOrientation = m_vOrientation ;
		xVec3		vTestPosition = m_vPosition ;
		xMat		matTestTrans = m_matTrans ;
		xMat		matTestRot = m_matRot ;
		float		fMovementMod = /*1*/ 0.25f + m_fEngineLevel * 0.75f ;
		float		fPitchMod = 0,
					fYawMod = 0,
					fRollMod = 0 ;
		float		fPitchDelta = 0,
					fYawDelta = 0,
					fAutoRollDelta = 0,
					fManualRollDelta = 0 ;
		float		fEffectiveSteeringSensitivity = m_fSteeringSensitivity ;
		float		fEffectiveMaxThrustMag = m_fMaxThrustMag * fMovementMod ;
		xVec4		vTestBBVerts[ 8 ];
		xMat		matTransDelta ;

		m_matLastTransDelta = g_matIdent ;

	//
	// (FLIGHT_CONTROLS) Update thrust mag: (friction & input)
	//
		if( bAcceptInput )
		{
			fTestThrustMag *= 1.0f - (0.005f * (60.0f / fFPS)) ;
			if( pFC->fAccelerate != 0 )
			{
				fTestThrustMag += pFC->fAccelerate * m_fThrustDelta * (1.0f / fFPS) ;
				if (fTestThrustMag > +m_fMaxThrustMag/*fEffectiveMaxThrustMag*/)
					fTestThrustMag = +m_fMaxThrustMag/*fEffectiveMaxThrustMag*/ ;
			}
			if( pFC->fBrake != 0 )
			{
				fTestThrustMag -= pFC->fBrake * m_fThrustDelta * (1.0f / fFPS) * 2.0f ;
				if (fTestThrustMag < -m_fMaxThrustMag/*-fEffectiveMaxThrustMag*/)
					fTestThrustMag = -m_fMaxThrustMag/*-fEffectiveMaxThrustMag*/ ;
			}
		}

	//
	// (FLIGHT_CONTROLS) Update strafe thrust mag: (friction & input)
	//
		if( bAcceptInput )
		{
			fTestStrafeThrustMag *= 1.0f - (0.005f * (60.0f / fFPS)) ;
			if( pFC->fStrafeLeft != 0)
			{
				if( fTestStrafeThrustMag < 0 )
					fTestStrafeThrustMag = 0 ;
				fTestStrafeThrustMag += pFC->fStrafeLeft * m_fThrustDelta * (1.0f / fFPS) ;
				if( fTestStrafeThrustMag > +m_fMaxThrustMag/*fEffectiveMaxThrustMag*/ )
					fTestStrafeThrustMag = +m_fMaxThrustMag/*fEffectiveMaxThrustMag*/ ;

				fTestThrustMag *= 1.0f - (0.01f * (60.0f / fFPS)) ;
			}
			if( pFC->fStrafeRight != 0)
			{
				if( fTestStrafeThrustMag > 0 )
					fTestStrafeThrustMag = 0 ;
				fTestStrafeThrustMag -= pFC->fStrafeRight * m_fThrustDelta * (1.0f / fFPS) ;
				if( fTestStrafeThrustMag < -m_fMaxThrustMag/*fEffectiveMaxThrustMag*/ )
					fTestStrafeThrustMag = -m_fMaxThrustMag/*fEffectiveMaxThrustMag*/ ;

				fTestThrustMag *= 1.0f - (0.01f * (60.0f / fFPS)) ;
			}
		}

	//
	// Update external force: (friction only)
	//
		m_vExternalForce *= 1.0f - (0.005f * (60.0f / fFPS)) ;

	//
	// Terrain Force:
	//
		xVec3 vTerrainForce( 0,0,0 );
		
		//pFC->fDive = pFC->fClimb = 0 ;	// Dbg
		/*if( m_fMaxTerrainForce != 0 )
		{
			if( m_fDistanceToGround != -1 )
			{
				//if( m_fDistanceToGround > m_fTerrainForceBalanceAltitude * 2 )
					;
				//else
				{
				// grav
					//if( m_fDistanceToGround > m_fTerrainForceBalanceAltitude )
					{
						float i = m_fDistanceToGround / m_fTerrainForceBalanceAltitude * 2 ;
						if( i > 1 ) i=1 ;
						m_fTerrainForce -= m_fMaxTerrainForce * i * ( 1 / fFPS );
					}
					
				// antigrav
					//if( m_fDistanceToGround < m_fTerrainForceBalanceAltitude )
					{
						float i = 1 - ( m_fDistanceToGround / m_fTerrainForceBalanceAltitude );
						if( i < 0 ) i=0 ;
						m_fTerrainForce += m_fMaxTerrainForce * i * ( 1 / fFPS );
					}

					vTerrainForce = xVec3( 0, m_fTerrainForce, 0 );
				}
			}
		}*/

	//
	// Apply test movement vector, build trans. matrix:
	//
		vDelta =  fMovementMod * fTestThrustMag * m_vOrientation * (60 / fFPS) ;
		vDelta += fMovementMod * fTestStrafeThrustMag * m_vLeft * (60 / fFPS) ;
		vDelta += m_vExternalForce * (60 / fFPS) ;
		vDelta += vTerrainForce * (60 / fFPS) ; 
		vTestPosition += vDelta ;
		D3DXMatrixTranslation( &matTestTrans, vTestPosition.x, vTestPosition.y, vTestPosition.z );
		
	//
	// (FLIGHT_CONTROLS) Update pitch, yaw and roll:
	//
		if( bAcceptInput )
		{
		// read FLIGHT_CONTROLS;
			if( pFC->fSteerLeft != 0 )		fYawMod -= pFC->fSteerLeft ;
			if( pFC->fSteerRight != 0 )		fYawMod += pFC->fSteerRight ;

			if( pFC->fDive != 0 )			fPitchMod -= pFC->fDive ;
			if( pFC->fClimb != 0 )			fPitchMod += pFC->fClimb ;
	
			if( pFC->fRollLeft != 0 )		fRollMod -= pFC->fRollLeft ;
			if( pFC->fRollRight != 0 )		fRollMod += pFC->fRollRight ;

			// Dropped the SHIFT / joystick rotate modifier for rolling, more realistic.
			//
			// This REALLY fucks up the bots FC_AlignWithTarget fn., so only allow
			// it for the user fighter.
			//
			if( /*pFC->bSteerAndRoll*/ m_bUserFighter )
			{
				if( pFC->fSteerLeft != 0 )  fRollMod -= pFC->fSteerLeft ;
				if( pFC->fSteerRight != 0 ) fRollMod += pFC->fSteerRight ;
			}

		// allow quicker turning when manual roll is applied;
			if( fRollMod != 0 ) fEffectiveSteeringSensitivity *= 0.8f ;

		// *********** MAIN ROTATION DELTAS
		// derive deltas;
			fPitchDelta		= fMovementMod * (fPitchMod / (fEffectiveSteeringSensitivity * 120)) * (60 / fFPS) ;
			fYawDelta		= fMovementMod * (fYawMod	/ (fEffectiveSteeringSensitivity * 120)) * (60 / fFPS) ;
			fAutoRollDelta 	= fMovementMod * (fYawMod	/ (2.0f							 * 30))  * (60 / fFPS) ;
			fManualRollDelta= fMovementMod * (fRollMod  / (m_fRollSensitivity			 * 40))	 * (60 / fFPS) ;
			
		// trap very large inputs; these cause problems;
			if( fPitchDelta > g_PI || fYawDelta > g_PI ) return ;
			
		// apply pitch & yaw;
			fTestPitch  += fPitchDelta ;
			fTestYaw	+= fYawDelta ;

		// apply ManualRoll value;
			fTestRoll += fManualRollDelta ;

		// cap yaw, pitch & roll angles;
			if (fTestRoll > +g_PI * 2.0f) fTestRoll -= +g_PI * 2.0f ;
			if (fTestRoll < -g_PI * 2.0f) fTestRoll -= -g_PI * 2.0f ;
			///float fMaxRoll = g_PI * 0.75f ;
			//if( fTestRoll > +fMaxRoll ) fTestRoll = +fMaxRoll ;
			//if( fTestRoll < -fMaxRoll ) fTestRoll = -fMaxRoll ;

			if (fTestPitch > +g_PI * 2.0f) fTestPitch -= +g_PI * 2.0f ;
			if (fTestPitch < -g_PI * 2.0f) fTestPitch -= -g_PI * 2.0f ;

			if (fTestYaw > +g_PI * 2.0f) fTestYaw -= +g_PI * 2.0f ;
			if (fTestYaw < -g_PI * 2.0f) fTestYaw -= -g_PI * 2.0f ;

		// apply AutoRoll;
			/*if( fTestRoll > -g_PI * 0.1f && fTestRoll < +g_PI * 0.1f )
			{
				fTestRoll += fAutoRollDelta ;
				//if( fTestRoll > +g_PI * 0.1f ) fTestRoll -= fAutoRollDelta ;
				//if( fTestRoll < -g_PI * 0.1f ) fTestRoll += fAutoRollDelta ;
			}*/

		// unroll over time;
			//if( fAutoRollDelta == 0 )
			{
				if( fTestRoll > 0 )
				{
					float fUnrollSpeed = 0.5f ;
					fTestRoll -= fMovementMod * 1.0f / (fUnrollSpeed * 2.0f * 30.0f) * (60.0f / fFPS) ;
					if (fTestRoll < 0.0f) fTestRoll = 0 ;
				}
				if( fTestRoll < 0 )
				{
					float fUnrollSpeed = 0.5f ;
					fTestRoll += fMovementMod * 1.0f / (fUnrollSpeed * 2.0f * 30.0f) * (60.0f / fFPS) ;
					if (fTestRoll > 0.0f) fTestRoll = 0 ;
				}
			}
		}

	//
	// Build the correct world matrix (scale, rot. around origin, then translation):
	//
		matRes = g_matIdent ;
		D3DXMatrixRotationYawPitchRoll( &matTestRot, fTestYaw, fTestPitch, fTestRoll );
#ifdef FULL_DBG_TRACE
	ods( "\t\tPPFC 1\n" );
#endif
		D3DXMatrixMultiply( &matRes, &matRes, &m_pFRO->m_matScale );		
		//xMat _m1, _m2 ;
		//_m1 = matRes ; _m2 = m_pFRO->m_matScale ;
		//D3DXMatrixMultiply( &_m1, &_m1, &_m2 );
		//matRes = _m1 ;

#ifdef FULL_DBG_TRACE
	ods( "\t\tPPFC 2\n" );
#endif
		D3DXMatrixMultiply( &matRes, &matRes, &matTestRot );
#ifdef FULL_DBG_TRACE
	ods( "\t\tPPFC 3\n" );
#endif
		D3DXMatrixMultiply( &matRes, &matRes, &matTestTrans );
#ifdef FULL_DBG_TRACE
	ods( "\t\tPPFC 4\n" );
#endif
	// also build an *incremental* translation-only matrix;
		D3DXMatrixTranslation( &matTransDelta, vDelta.x, vDelta.y, vDelta.z );
#ifdef FULL_DBG_TRACE
	ods( "\t\tPPFC 5\n" );
#endif

	//
	// Test the updated bounding volume for terrain collision:
	//
		for( int i = 0 ; i < 8 ; i++ )
			D3DXVec3Transform( &vTestBBVerts[i], &m_pFRO->m_pBBox->m_vInitialVerts[i], &matRes );
		if( !DetectAndHandleTerrainCollision( vTestBBVerts, pTerrain, pRace ) )
		{
		// update globals
			m_fPitch = fTestPitch ;
			m_fRoll = fTestRoll ;
			m_fYaw = fTestYaw ;
			m_fThrustMag = fTestThrustMag ;
			m_fStrafeThrustMag = fTestStrafeThrustMag ;
			m_vPosition = vTestPosition ;
			m_vOrientation = vTestOrientation ;
			m_matTrans = matTestTrans ;
			m_matRot = matTestRot ;
			m_fSpeed = D3DXVec3Length( &vDelta );
			if( pfYawDelta ) *pfYawDelta = fYawDelta ;
			D3DXVec3Normalize( &m_vDirectionOfTravel, &vDelta );
			m_matLastTransDelta = matTransDelta ;

		// velocity in m/s
			m_vVelocityMS = vDelta * pRace->m_fWorldSpaceToM ;	// m/frame
			m_vVelocityMS *= fFPS ;								// m/s
			m_fSpeedMS = D3DXVec3Length( &m_vVelocityMS );

		// apply to render object
			m_pFRO->WorldSetMatrix( &matRes, m_pFRO->m_fScale );

		// apply rotation matrix to unit orientation vector
			D3DXVec3Transform(&v, &m_vInitialOrientation, &m_matRot) ;
			m_vOrientation.x = v.x ; m_vOrientation.y = v.y ; m_vOrientation.z = v.z ;

		// apply rot. to unit up vec.
			D3DXVec3Transform( &v, &xVec3( 0,1,0 ), &m_matRot );
			m_vUp.x = v.x ; m_vUp.y = v.y ; m_vUp.z = v.z ;

		// appy rot. to unit right vec.
			D3DXVec3Transform( &v, &xVec3( 1,0,0 ), &m_matRot );
			m_vLeft.x = v.x ; m_vLeft.y = v.y ; m_vLeft.z = v.z ;
		}
		else
		{
			m_fThrustMag = 0 ;
			m_fStrafeThrustMag = 0 ;
			m_fSpeed = 0 ;	// KLUDGE; stops contrail going strange when colliding.
			if( pfYawDelta ) *pfYawDelta = 0 ;
		}

	//
	// Reduce external force one full tick after we've reacted to a terrain collision:
	//
		if( m_bJustBouncedOffTerrain )
		{
			m_vExternalForce *= 0.15f ;
			m_bJustBouncedOffTerrain = FALSE ;
		}

	//
	// (FLIGHT_CONTROLS) Handle weapons systems:
	//
		if( bAcceptInput )
		{
		// cycle;
			if( pFC->bNextWeapon )
				if( ++m_Weapon == MAX_WEAPON_TYPES ) m_Weapon = 0 ;
			if( pFC->bPrevWeapon )
				if( --m_Weapon == -1 ) m_Weapon = MAX_WEAPON_TYPES - 1 ;

		// fire;
			if( pFC->bFireA )
				LaunchProjectile( pDev, fFPS, fElapsedTime, (e_WeaponTypes)m_Weapon, pRace );
		}
	}


/*
 *
 * Weapons & Projectiles:
 *
 */
	void CFighter::LaunchProjectile( LPDIRECT3DDEVICE8 pDev, float fFPS, float fElapsedTime, /*e_WeaponTypes*/int Type, CRace* pRace )
	{
		float fSize ;
		int nParticles ;

	// make sure that we don't fire too fast;
		if( m_nProjectiles > 0 )
			if( fElapsedTime - m_fLastProjectileEmmitedTime
					<
				g_Weapons[ Type ].fEmmisionMinInterval * m_fBaseWeaponEmmisionSpeedModifier )
				return ;

		m_fLastProjectileEmmitedTime = fElapsedTime ;

	// cycle through emision points;
		xVec3 vEmitterPos ;
		vEmitterPos = m_vWeaponEmitters[ m_ndxCurWeaponEmitter ] * m_pFRO->m_pRenderData->m_fBSphereRadius ;
		m_ndxCurWeaponEmitter++ ;
		if( m_ndxCurWeaponEmitter == m_nWeaponEmitters )
			m_ndxCurWeaponEmitter = 0 ;

	// create new projectile info struct
		FIGHTER_PROJECTILE* pNewProjectile ;
		FIGHTER_PROJECTILE* pLastProjectile ;
		if( 0==m_nProjectiles )
		{
			m_pProjectiles = new FIGHTER_PROJECTILE ;
			m_pProjectiles->pPrev = NULL ;
			m_pProjectiles->pNext = NULL ;
			pNewProjectile = m_pProjectiles ;
		}
		else
		{
			pLastProjectile = m_pProjectiles ;
			while( pLastProjectile->pNext )
				pLastProjectile = pLastProjectile->pNext ;

			pNewProjectile = new FIGHTER_PROJECTILE ;
			pLastProjectile->pNext = pNewProjectile ;
			pNewProjectile->pPrev = pLastProjectile ;
			pNewProjectile->pNext = NULL ;
		}

		pNewProjectile->Type = (e_WeaponTypes)Type ;

		float fSpeed ;
	// init. struct def. values
		pNewProjectile->bStaticPS = TRUE ;
		pNewProjectile->fAge = 0.0f ;
		pNewProjectile->fDamage = 1.0f ;								// overridden below
		pNewProjectile->fRenderSize = 0.0f ;							// overridden below
		pNewProjectile->fRenderSizeWhenImpacting = 0.0f ;				// overridden below
		pNewProjectile->fLifetime = 1.0f ;								// overridden below
		pNewProjectile->vLeadingPos = xVec3( 0,0,0 );				// overridden below
		pNewProjectile->vTrailingPos = xVec3( 0,0,0 );			// overridden below
		pNewProjectile->fLength = 0.0f ;								// overridden below
		pNewProjectile->fImpactFXLifetime = 0.0f ;						// overridden below
		pNewProjectile->fImpactFXSpeed = 0.0f ;							// overridden below

		pNewProjectile->bImpacting = FALSE ;
		pNewProjectile->fImpactFXAge = 0.0f ;

	// init. particle system
		pNewProjectile->pPS = new CParticleSystem( 512, 2048, 0.0f );

	// define type-specfic data
		pNewProjectile->fDamage = g_Weapons[ Type ].fDamage ;
		pNewProjectile->fLifetime = g_Weapons[ Type ].fLifetime ;
		D3DXCOLOR colA, colB, col ;
		if( WT_GREENLASER==Type )
		{
			colA = D3DXCOLOR( 0.1f, 0.9f, 0.1f, 0.15f );
			colB = D3DXCOLOR( 0.1f, 0.9f, 0.1f, 0.15f );
			pNewProjectile->fRenderSize = m_pFRO->m_fScaledLargestSide / 10 ;
			pNewProjectile->fRenderSizeWhenImpacting = pNewProjectile->fRenderSize * 10 /*3*/ ;
			
			pNewProjectile->fImpactFXLifetime = 2.5f ;
			pNewProjectile->fImpactFXSpeed = 80/*10*/ ;

			pNewProjectile->fAccel = 0.0f ; //1.0f / 50.0f ;
			fSize = 30.0f ;
			fSpeed = 0.5f ;
			nParticles = 75 ;
		}
		if( WT_BLUELASER==Type )
		{
			colA = D3DXCOLOR( 0.1f, 0.1f, 0.9f, /*0.15f*/0 );
			colB = D3DXCOLOR( 0.1f, 0.1f, 0.9f, 0.15f );
			pNewProjectile->fRenderSize = m_pFRO->m_fScaledLargestSide / 5 ;
			pNewProjectile->fRenderSizeWhenImpacting = pNewProjectile->fRenderSize * 15 /*3*/ ;
			
			pNewProjectile->fImpactFXLifetime = 2.5f ;
			pNewProjectile->fImpactFXSpeed = 150/*10*/ ;

			pNewProjectile->fAccel = 0.0f ; //1.0f / 50.0f ;
			fSize = 30.0f ;
			fSpeed = 0.75f ;
			nParticles = 100 ;
		}
		if( WT_BULLET==Type )
		{
			colA = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.0f );
			colB = D3DXCOLOR( 0.8f, 0.4f, 0.2f, 0.1f );
			pNewProjectile->fRenderSize = m_pFRO->m_fScaledLargestSide / 20 ;
			pNewProjectile->fRenderSizeWhenImpacting = m_pFRO->m_fScaledLargestSide ;

			pNewProjectile->fImpactFXLifetime = 2.5f ;
			pNewProjectile->fImpactFXSpeed = 100/*10*/  ;

			pNewProjectile->fAccel = 0.0f ;
			fSize = 30.0f ;
			fSpeed = 1.5f ;
			nParticles = 75 ;
		}

		pNewProjectile->vVel = ( m_fSpeed * m_vOrientation ) +			// projectile speed
							   m_vOrientation * ( m_fMaxThrustMag * fSpeed * (60.0f / fFPS) ) + 
							   m_vExternalForce * (60.0f / fFPS );
		//ods(" pNewProjectile->vVel: %0.1f\n", D3DXVec3Length( &pNewProjectile->vVel ) );
	
	//
	// Scale all projectile parameters according to fighter's core weapon level:
	//
		float fMultiplier = ( m_fWeaponsLevel / 0.1f ) ;	// 2.5 times, at max weapons level
		pNewProjectile->fDamage *= fMultiplier ;
		pNewProjectile->fRenderSize *= fMultiplier ;
		pNewProjectile->fRenderSizeWhenImpacting *= fMultiplier ;
		pNewProjectile->fImpactFXLifetime *= fMultiplier ;
		pNewProjectile->fImpactFXSpeed /= fMultiplier ;
		fSize *= fMultiplier ;
		fSpeed *= fMultiplier ;
		
	// projectiles with 'static'-type particle systems need a base world matrix defined;
		if( pNewProjectile->bStaticPS )
		{
			xMat mT ;
			
			D3DXMatrixTranslation( &pNewProjectile->matWorld, vEmitterPos.x, vEmitterPos.y, vEmitterPos.z );
			D3DXMatrixMultiply( &pNewProjectile->matWorld, &pNewProjectile->matWorld, &m_matRot );
				
			D3DXMatrixTranslation( &mT, m_vPosition.x, m_vPosition.y, m_vPosition.z );
			D3DXMatrixMultiply( &pNewProjectile->matWorld, &pNewProjectile->matWorld, &mT );
		}

	// build projectile particle system;
		xVec3 vOBBox[ 8 ];

		for( int intI=0; intI<=nParticles; intI++ )
		{	float i = (float)intI / (float)nParticles ;

		// lerp between start and end colors
			D3DXColorLerp( &col, &colA, &colB, i );

		// determine where to spawn the particle
		// NOTE: the difference between the 'dynamic' and 'static' -type particle systems here;
		// 'static' particle systems spawn from the world-origin, and extend _straight_ into the z plane
		// 'dynamic' particle systems spawn from the absolute emmiter position
			xVec3 vAdd, vEmitterRotatedPos, vParticlePos ;
			float fAddStraight ;
			fAddStraight = m_pFRO->m_pRenderData->m_fBSphereRadius * i * fSize ;
			vAdd = fAddStraight * m_vOrientation ;
			
			GetFighterAbsPosFromRelative( (xVec3*)&(vEmitterPos + m_vPosition), &vEmitterRotatedPos );

			if( pNewProjectile->bStaticPS )
				vParticlePos = xVec3( 0, 0, -fAddStraight );
			else
				vParticlePos = vEmitterRotatedPos + vAdd ;

		// keep track of the effect's bounding volume, length and leading & trailing points
			if( i==0.0f )
			{
				vOBBox[ 0 ] = xVec3( -pNewProjectile->fRenderSize / 2.0f, -pNewProjectile->fRenderSize / 2.0f, 0 );
				vOBBox[ 1 ] = xVec3( +pNewProjectile->fRenderSize / 2.0f, -pNewProjectile->fRenderSize / 2.0f, 0 );
				vOBBox[ 2 ] = xVec3( -pNewProjectile->fRenderSize / 2.0f, +pNewProjectile->fRenderSize / 2.0f, 0 );
				vOBBox[ 3 ] = xVec3( +pNewProjectile->fRenderSize / 2.0f, +pNewProjectile->fRenderSize / 2.0f, 0 );
				pNewProjectile->vTrailingPos = vEmitterRotatedPos + vAdd ;
			}
			if( i==1.0f )
			{
				vOBBox[ 4 ] = xVec3( -pNewProjectile->fRenderSize / 2.0f, -pNewProjectile->fRenderSize / 2.0f, -fAddStraight );
				vOBBox[ 5 ] = xVec3( +pNewProjectile->fRenderSize / 2.0f, -pNewProjectile->fRenderSize / 2.0f, -fAddStraight );
				vOBBox[ 6 ] = xVec3( -pNewProjectile->fRenderSize / 2.0f, +pNewProjectile->fRenderSize / 2.0f, -fAddStraight );
				vOBBox[ 7 ] = xVec3( +pNewProjectile->fRenderSize / 2.0f, +pNewProjectile->fRenderSize / 2.0f, -fAddStraight );
				pNewProjectile->vLeadingPos = vEmitterRotatedPos + vAdd ;
				pNewProjectile->fLength = fAddStraight ;
			}

		// spawn the particle in the right place, with the right color, with zero intrinsic velocity;
			pNewProjectile->pPS->FrameMove(	fFPS,
											&xVec3( 0,0,0 ),			// intrinsic vel.
											1.0f / fFPS,
											1,							// no. to emit
											0.0f,						// particle lifetime
											FALSE,						// gravity
											col,
											col,
											&xVec3( 0,0,0 ),			// emit velocity
											&vParticlePos,				// emit position
											FALSE,						// spherical emit
											0.0f,						// scatter
											FALSE						// compute bounding volume
										  );
		}

	// frustrum culling for projectiles;
		pNewProjectile->pBBox = new CBoundingBox( &vOBBox[ 0 ] );
		pNewProjectile->pBBox->RestoreDeviceObjects( pDev );			// NOTE: in release build this does nothing
		pNewProjectile->pBBox->ApplyMatrix( &pNewProjectile->matWorld );

		m_nProjectiles++ ;

	// let there be sound!
	// NOTE: look out for origin-based positions creeping in here. sounds OK at the moment, though!
		//ods( "new proj. sound {%0.1f,%0.1f,%0.1f}\n", pNewProjectile->vTrailingPos.x,pNewProjectile->vTrailingPos.y,pNewProjectile->vTrailingPos.z );
		switch( Type )
		{
			case WT_BULLET:
				pNewProjectile->fVol = 0.25f ;
				break ;
			case WT_BLUELASER:
				pNewProjectile->fVol = 0.35f ;
				break ;
			case WT_GREENLASER:
				pNewProjectile->fVol = 0.30f ;
				break ;
		}
		if( !m_bUserFighter ) 
			pNewProjectile->fVol /= 2 ;
		pNewProjectile->nSoundChannel = 
			pRace->m_pX7->m_pSound->PlaySFX3D( g_Weapons[ Type ].sfx,  &pNewProjectile->vTrailingPos, NULL, pNewProjectile->fVol );
		
		//ods(" {%0.1f,%0.1f,%0.1f}: {%0.1f,%0.1f,%0.1f}\n",
		//	pNewProjectile->vTrailingPos.x, pNewProjectile->vTrailingPos.y, pNewProjectile->vTrailingPos.z,
		//	m_vPosition.x, m_vPosition.y, m_vPosition.z );
	}

	void CFighter::AnimateProjectiles( float fFPS, float fElapsedTime, CTerrain* pTerrain, CRace* pRace )
	{
		xMat m ;
		FIGHTER_PROJECTILE* pProjectile = m_pProjectiles ;
		int nParticlesToEmit = 0 ;
		BOOL bStillEmitting = FALSE ;
		xVec3 vd1, vd2, vcp, vn1, vn2, vnAvg, vTerrainVerts[4] ;
		BOOL bAboveTerrain ;//, bCollision ;
		float fMinTerrainY ;
		xVec3 A, N, X ;

		while( pProjectile )
		{
			if( pProjectile->fAge > 0.0f )
			{
			// test projectile's instantaneous collision-detection point for collisions
				if( !pProjectile->bImpacting )
				{
				// terrain collision;
					pTerrain->LERPdY( pProjectile->vLeadingPos.x, pProjectile->vLeadingPos.z, &fMinTerrainY, &bAboveTerrain, vTerrainVerts );
					if( pProjectile->vLeadingPos.y < fMinTerrainY && bAboveTerrain )
					{
						/*GetAvgNormalFrom4TerrainVerts( &vnAvg, vTerrainVerts );
						D3DXVec3Normalize( &N, &vnAvg );
						D3DXVec3Normalize( &A, &pProjectile->vVel );
						X = (2.0f * N) + A ;*/

					// play particle system's impact effect;
						X = xVec3( 1,1,1 );
						pProjectile->pPS->ImpactTerrain( &X,
														 fFPS, pTerrain->m_fLenSQ,
														 pProjectile->fImpactFXLifetime,
														 pProjectile->fImpactFXSpeed );
						pProjectile->bImpacting = TRUE ;
					}

				// collisions with other ships;
					CFighter* pHit ;
					if( pRace->DetectAndHandleProjectileAgainstAllFighters( (void*)pProjectile, (void*)this, (void**)&pHit ) )
					{
						D3DXVec3Normalize( &X, &pProjectile->vVel );
						X.x = ( X.x + 1 ) / 2;
						X.y = ( X.y + 1 ) / 2;
						X.z = ( X.z + 1 ) / 2;

					// play particle system's impact effect;
						pProjectile->pPS->ImpactFighter( &X,
														 fFPS, pTerrain->m_fLenSQ,
														 pProjectile->fImpactFXLifetime,
														 pProjectile->fImpactFXSpeed,
														 (xVec3*)&(pHit->m_fSpeed * pHit->m_vOrientation) );
						pProjectile->bImpacting = TRUE ;
					}
				}

			// kill projectiles after they've finished their impact effect
				if( pProjectile->fImpactFXAge >= pProjectile->fImpactFXLifetime )
				{
					pProjectile = KillProjectile( pProjectile );
					continue ;
				}

			// make all projectiles die without effect after a certain amount of life
				if( pProjectile->fAge > pProjectile->fLifetime )
				{
					pProjectile = KillProjectile( pProjectile );
					continue ;
				}

			// make all projectiles die without effect when they are a certain distance from the origin.
			// NOTE: this leads to strange 'fast-fire' effects when firing at the extreme of the 'life-zone' for projectiles.
				/*if( fabsf( D3DXVec3Length(&pProjectile->vLeadingPos) ) > pTerrain->m_fLenSQ * 0.5f )
				{
					pProjectile = KillProjectile( pProjectile );
					continue ;
				}*/

			// advance dynamic particles animation
				if( !pProjectile->bStaticPS )
					pProjectile->pPS->FrameMove( fFPS,
												&pProjectile->vVel,
												1.0f / fFPS,
												0,						// no. to emit
												0.0f,					// particle lifetime
												FALSE,					// gravity
												g_ParticleColor[ 0 ],
												g_ParticleColor[ 0 ],
												&xVec3( 0,0,0 ),	// emit velocity:
												NULL,					// emit position:
												FALSE,					// spherical emit
												0.0f,					// scatter
												FALSE					// compute bounding volume
												);

			// advance impact animation effect
				if( pProjectile->bImpacting )
				{
					float fTimeRemaining = pProjectile->fImpactFXLifetime - pProjectile->fImpactFXAge ;
					float f = pTerrain->m_fLenSQ / 1000000.0f ;
					pProjectile->pPS->FrameMove( fFPS,
												&xVec3( 0,0,0 ),
												1.0f / fFPS,
																		// no. to emit
												/*fTimeRemaining > pProjectile->fImpactFXLifetime * 0.8f
													? (DWORD)( 50.0f * ( 60.0f / fFPS ) ) :*/ 0,

												fTimeRemaining,			// particle lifetime
												FALSE,					// gravity
												D3DXCOLOR(0,1,0,0.5f),
												D3DXCOLOR(1,0,0.75f,0),
																		// emit velocity:
												&xVec3( rnd() * f * ( rnd() > 0.5f ? +1 : -1 ),
															  rnd() * f * ( rnd() > 0.5f ? +1 : -1 ),
															  rnd() * f * ( rnd() > 0.5f ? +1 : -1 ) ),

																		// emit position:
												&xVec3( rnd() * f * ( rnd() > 0.5f ? +1 : -1 ),
															  rnd() * f * ( rnd() > 0.5f ? +1 : -1 ),
															  rnd() * f * ( rnd() > 0.5f ? +1 : -1 ) ),

												FALSE,					// spherical emit
												0.01f,					// scatter
												TRUE					// compute bounding volume
												);
					
					pProjectile->fImpactFXAge += ( 1.0f / fFPS );
				}
				else
				{
				// advance bbox
					D3DXMatrixTranslation( &m, pProjectile->vVel.x, pProjectile->vVel.y, pProjectile->vVel.z );
					pProjectile->pBBox->ApplyMatrix( &m );

				// projectiles with a 'static'-type particle system also need their world matrix updated;
					D3DXMatrixMultiply( &pProjectile->matWorld, &pProjectile->matWorld, &m );
				
				// advance projectile's instantaneous collision-detection points
					pProjectile->vLeadingPos += pProjectile->vVel ;
					pProjectile->vTrailingPos += pProjectile->vVel ;

				// accelerate particles (they start off quite slow, so we can actually see as they are launched!)
					pProjectile->vVel *= 1.0f + (( 60.0f / fFPS ) * pProjectile->fAccel );

				// update FMOD
					/*pRace->m_pX7->m_pSound->UpdateSFX3D( pProjectile->nSoundChannel,
														 &pProjectile->vTrailingPos,
														 &(pProjectile->vVel * pRace->m_fWorldSpaceToM), // m/s
														 pProjectile->fVol );*/
				}
			}
			pProjectile->fAge += ( 1.0f / fFPS );
			pProjectile = pProjectile->pNext ;
		}
	}

CFighter::FIGHTER_PROJECTILE*
	CFighter::KillProjectile( FIGHTER_PROJECTILE* pProjectileToDie )
	{
		FIGHTER_PROJECTILE* pNextProjectile = pProjectileToDie->pNext ;
		
	// restitch linked list;
		if( pProjectileToDie->pPrev )
			pProjectileToDie->pPrev->pNext = pProjectileToDie->pNext ;
		else
			m_pProjectiles = pProjectileToDie->pNext ;
		if( pProjectileToDie->pNext )
			pProjectileToDie->pNext->pPrev = pProjectileToDie->pPrev ;
		
	// delete list entry;
		if( pProjectileToDie->pBBox )
			pProjectileToDie->pBBox->InvalidateDeviceObjects() ;			// NOTE: in release build this does nothing
		SAFE_DELETE( pProjectileToDie->pPS )
		SAFE_DELETE( pProjectileToDie->pBBox )
		delete pProjectileToDie ;

	// return next in list;
		m_nProjectiles-- ;
		return pNextProjectile ;
	}

/*
 *
 * Manual Positioning:
 *
 */
	void CFighter::SetPositionFromValues(float x, float y, float z)
	{
		m_vPosition = xVec3(x, y, z) ;
		D3DXMatrixTranslation(&m_matTrans, x, y, z) ;
		m_pFRO->WorldSetMatrix(&m_matTrans, m_pFRO->m_fScale) ;
	}

/*
 *
 * Physics:
 *
 */

	BOOL CFighter::DetectAndHandleTerrainCollision( xVec4* pvTestBBVerts, CTerrain* pTerrain, CRace* pRace )
	{
		xVec3 vTerrainVerts[4] ;
		float fMinTerrainY[8] ;
		BOOL bAboveTerrain[8] ;
		int i ;

	// test each against heightmap
		for	(i = 0 ; i < 8 ; i ++)
		{
			pTerrain->LERPdY(pvTestBBVerts[i].x, pvTestBBVerts[i].z, &fMinTerrainY[i], &bAboveTerrain[i], &vTerrainVerts[0]) ;	

		// save this for future reference!
		// note, not quite accurate, but good enough?
			if( i==0 )
				if( bAboveTerrain[ 0 ] )
					m_fDistanceToGround = m_vPosition.y - fMinTerrainY[ 0 ];
				else
					m_fDistanceToGround = -1 ;

		// collision
			if (pvTestBBVerts[i].y < fMinTerrainY[i] && bAboveTerrain[i])
			{
				xVec3 vd1, vd2, vd3, vcp, vn1, vn2, vnAvg ;

				m_bTerrainCollision = TRUE ;
				ods("TC!\n") ;

				/*ods( "vTerrainVerts[0].x=(%0.1f,%0.1f,%0.1f)\n", vTerrainVerts[0].x, vTerrainVerts[0].y, vTerrainVerts[0].z );
				ods( "vTerrainVerts[1].x=(%0.1f,%0.1f,%0.1f)\n", vTerrainVerts[1].x, vTerrainVerts[1].y, vTerrainVerts[1].z );
				ods( "vTerrainVerts[2].x=(%0.1f,%0.1f,%0.1f)\n", vTerrainVerts[2].x, vTerrainVerts[2].y, vTerrainVerts[2].z );
				ods( "vTerrainVerts[3].x=(%0.1f,%0.1f,%0.1f)\n", vTerrainVerts[3].x, vTerrainVerts[3].y, vTerrainVerts[3].z );*/
				GetAvgNormalFrom4TerrainVerts( &vnAvg, vTerrainVerts );

				/*D3DXVec3Subtract(&vd1, &vTerrainVerts[3], &vTerrainVerts[2]) ;
				D3DXVec3Subtract(&vd2, &vTerrainVerts[3], &vTerrainVerts[1]) ;
				D3DXVec3Cross(&vcp, &vd1, &vd2) ;
				D3DXVec3Normalize(&vn1, &vcp) ;
				D3DXVec3Subtract(&vd1, &vTerrainVerts[4], &vTerrainVerts[3]) ;
				D3DXVec3Subtract(&vd2, &vTerrainVerts[4], &vTerrainVerts[2]) ;
				D3DXVec3Cross(&vcp, &vd1, &vd2) ;
				D3DXVec3Normalize(&vn2, &vcp) ;
				//vnAvg.x = (vn1.z + vn2.z) / 2.0f ;	// must reverse x and z here! -- caused by LERPdY's logic?
				//vnAvg.y = (vn1.y + vn2.y) / 2.0f ;
				//vnAvg.z = (vn1.x + vn2.x) / 2.0f ;
				vnAvg = vn1 ;*/

			// apply rebound force;
			// FIXME: this sometimes seems to get 'stuck'
				xVec3 A, N, X ;
				D3DXVec3Normalize( &N, &vnAvg );
				D3DXVec3Normalize( &A, &m_vDirectionOfTravel );
				X = (2.0f * N) + A ;
				m_vExternalForce = X * m_fSpeed ;
				m_bJustBouncedOffTerrain = TRUE ;

				m_TCD_vN = vnAvg ;
				m_TCD_vA = m_vDirectionOfTravel ;

				ods( "vnAvg=%0.1f,%0.1f,%0.1f\n", vnAvg.x, vnAvg.y, vnAvg.z );
				ods( "X=%0.1f,%0.1f,%0.1f\n", X.x, X.y, X.z );
				ods( "\n" );
				
			// this works in all cases, but isn't as physically accurate
				/*m_vExternalForce = vnAvg * m_fSpeed * 0.25f ;*/

			//
			// DO THE DAMAGE!
			//

			// if this is a user fighter: scale the damage from 0.5 base to 1.5 base according to user's skill level
			// is this is a bot: set a constant 0.5 base modifier.
				float fModifier ;
				fModifier = 0.5f ;
				if( m_bUserFighter )
					fModifier = 0.5f + ( 0.5f * m_fSkill );

				float fDamage = ( m_fThrustMag / m_fMaxThrustMag ) * fModifier ;
				m_fShieldLevel -= fDamage ;
				if( m_fShieldLevel < 0 )
					m_fShieldLevel = 0 ;
				else
					StartShieldEffect( fDamage, pRace );
				m_bShieldLevelChanged = TRUE ;

				return TRUE ;
			}
		}

		m_bTerrainCollision = FALSE ;
		return FALSE ;
	}

/*
 *
 * Dynamic Camera Management:
 *
 */
	void CFighter::ManageCamera(LPDIRECT3DDEVICE8 pDev, CCamera *pCamera, CUserInput *pIn, int ctCamMode)
	{
		xVec3 vX = ( m_vOrientation * m_pFRO->m_pRenderData->m_fBSphereRadius );

		if (ctCamMode == CT_BEHIND_FIXED1)
		{
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter - (vX * 3.0f) ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX) ;
		}
		if (ctCamMode == CT_BEHIND_FIXED2)
		{
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter - (vX * 9.0f) ;
			pCamera->m_vEyePt.y += m_pFRO->m_pRenderData->m_fBSphereRadius * 1.5f ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX) ;
		}
		if (ctCamMode == CT_BEHIND_FIXED3)
		{
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter - (vX * 24.0f) ;
			pCamera->m_vEyePt.y += m_pFRO->m_pRenderData->m_fBSphereRadius * 1.5f ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX) ;
		}

		if (ctCamMode == CT_BEHIND_VARIABLE1)
		{
			float fI = 1.0f - (m_fThrustMag / m_fMaxThrustMag) ;
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter - (vX * (1.5f + (5.0f * fI)) ) ;
			pCamera->m_vEyePt.y += m_pFRO->m_pRenderData->m_fBSphereRadius * (2.0f * fI) ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX) ;
		}
	
	// smoothed, variable distance;
		if (ctCamMode == CT_BEHIND_VARIABLE2)
		{
			float fI = 1.0f - (m_fThrustMag / m_fMaxThrustMag) ;

			// move historical data through the buffer
			for(int i = THRUSTMAG_HISTORY_BUF - 2 ; i >= 0 ; i-- )
				m_fThrustMagHistory[i+1] = m_fThrustMagHistory[i] ;

			// drop the new data into the front of the history buffer
			m_fThrustMagHistory[0] = fI ;

			// get weighted average from the buffer
			float fA = 0, fWeight = 1, fTotalWeight = 0 ;
			for( i=0 ; i<THRUSTMAG_HISTORY_BUF ; i++ )
			{
				fA += m_fThrustMagHistory[i] * fWeight ;
				fTotalWeight += fWeight ;
				fWeight *= THRUSTMAG_SMOOTH_WEIGHT_D ;
			}
			fA /= fTotalWeight ;

			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter - vX - (vX * (5.0f * fA) ) ;
			pCamera->m_vEyePt.y += m_pFRO->m_pRenderData->m_fBSphereRadius * (2.0f * fA) ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX) ;
		}

		if (ctCamMode == CT_BEHIND_VARIABLE3)
		{
			float fI = 1.0f - (m_fThrustMag / m_fMaxThrustMag) ;
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter - (vX * (1.5f + (5.0f * fI)) ) ;
			pCamera->m_vEyePt.y += m_pFRO->m_pRenderData->m_fBSphereRadius * (2.0f * fI) ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX) ;
		}

		if (ctCamMode == CT_COCKPIT_FIXED)
		{
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter + (vX * 0.0f) ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX * 0.5f) ;
		}
		if (ctCamMode == CT_COCKPIT_ROLL)
		{
			pCamera->m_vEyePt = m_pFRO->m_pBBox->m_vCenter + (vX * 0.0f) ;
			pCamera->m_vLookAtPt = m_pFRO->m_pBBox->m_vCenter + (vX * 0.5f) ;
		}

	// nice!
		if( (m_fPitch / g_PI < -0.5f && m_fPitch / g_PI > -1.5f) || (m_fPitch / g_PI > +0.5f && m_fPitch / g_PI < +1.5f) )
			pCamera->m_vUpVec = xVec3( 0, -1, 0 );
		else
			pCamera->m_vUpVec = xVec3( 0, +1, 0 );

		pCamera->SetViewMatrix( pDev, FALSE/*TRUE*/ );
	}

/*
 *
 * D3DFrame Event Hooks:
 *
 */
	void CFighter::PreFrameMove( CRace* pRace )
	{
		//m_bShieldLevelChanged = FALSE ;
	}

	int CFighter::FrameMove(LPDIRECT3DDEVICE8 pDev,
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
		int ret = 0 ;

	//
	// Check for Death!
	//
		if( m_fShieldLevel == 0.0f && m_fsState == FS_NORMAL )
			StartExplosionEffect( pRace ) ;

	//
	// Weapons & Projectiles:
	//
		AnimateProjectiles( fFPS, fElapsedTime, pTerrain, pRace );

	//
	// Contrail FX:
	//
		if( m_fsState == FS_NORMAL )
		{
			for( int i=0; i<m_nContrailEmitters ; i++ )
				if( m_pPS_Contrails[ i ] )
				{
					if( m_pPS_Contrails[ i ]->m_vMin == xVec3( 0,0,0 ) &&
						m_pPS_Contrails[ i ]->m_vMax == xVec3( 0,0,0 ) )
						m_bContrailVisible[ i ] = TRUE ;
					else
						m_bContrailVisible[ i ] = GetBBoxVisibility( pDev, *pmatProj, pCamera->m_matView, pVP,
																	 m_pPS_Contrails[ i ]->m_vMin,
																	 m_pPS_Contrails[ i ]->m_vMax );

					// HARD CODED, visiblity check disabled for time-being!
					m_bContrailVisible[ i ] = TRUE ;

					if( m_bContrailVisible[ i ] )
					{
					// derive emmision point, duly rotated as per the ship's orientation;
						xVec3 vEmitterPos, vEmitterRotatedPos ;
						vEmitterPos = m_vPosition + m_vContrailEmitters[ i ] * m_pFRO->m_pRenderData->m_fBSphereRadius ;
						GetFighterAbsPosFromRelative( &vEmitterPos, &vEmitterRotatedPos );

					// advance particle system and bbox;
						m_pPS_Contrails[ i ]->FrameMove( fFPS,
														(xVec3*)&( (m_fSpeed * m_vDirectionOfTravel) ) ,
														1.0f / fFPS,
														(int)(20.0f * (60.0f/fFPS)),
														g_Fighters[ m_Type ].fContrailParticleLifetime,
														FALSE,
														m_clrContrailEmitColour/*D3DXCOLOR( 0.25f, 0.25f, 0.75f, 0.1f )*/,
														m_clrContrailFadeColour/*D3DXCOLOR( 1, 0, 0, 0.05f )*/,
														(xVec3*)&( m_vOrientation * (-0.2f/fFPS) * (fabsf(m_fThrustMag) / m_fMaxThrustMag) ),
														&vEmitterRotatedPos,
														FALSE,
														m_pFRO->m_pRenderData->m_fBSphereRadius / 8.0f * (1.0f/fFPS) * m_fContrailScatter,
														TRUE );
						ret = 1 ;
					}
					else
					{
					// fast & cheap update of particles & bbox, no new emmisions or deaths;
						m_pPS_Contrails[ i ]->CulledFrameMove( (xVec3*)&((m_fSpeed * m_vDirectionOfTravel)) ) ;
						ret = 0 ;
					}
				}
		}

	//
	// Explosion FX:
	//
		if( m_fsState == FS_EXPLODING )
		{
			m_fExpAge += ( 1.0f / fFPS );

		// advance animation to next frame
			m_pExp->FrameMove( pDev, fFPS, (xVec3*)&(m_fSpeed * m_vDirectionOfTravel), &m_vPosition );

		// finish explosion FX
			if( m_fExpAge >= m_fExpLifespan )
			{
				m_fsState = FS_NORMAL ;
				m_fExpAge = 0.0f ;
				m_bJustFinishedExploding = TRUE ;
			}
		}

	//
	// Camera:
	//
		if( bManageCamera )
		{
			ManageCamera( pDev, pCamera, pInput, ctCamMode );
		}

		return ret ;
	}
	HRESULT CFighter::RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM, CTextures* pTex, float fAspect)
	{
		ods( "CFighter::RestoreDeviceObjects\n" );

		m_pFRO->RestoreDeviceObjects( pD3D, pDev );
		m_pExp->RestoreDeviceObjects( pDev );

		FIGHTER_PROJECTILE* pProjectile ;
		pProjectile = m_pProjectiles ;
		while( pProjectile )
		{
			pProjectile->pBBox->RestoreDeviceObjects( pDev );
			pProjectile = pProjectile->pNext ;
		}

		return S_OK ;
	}
	HRESULT	CFighter::InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev )
	{
		m_pFRO->InvalidateDeviceObjects( pDev );
		m_pExp->InvalidateDeviceObjects() ;

		FIGHTER_PROJECTILE* pProjectile ;
		pProjectile = m_pProjectiles ;
		while( pProjectile )
		{
			pProjectile->pBBox->InvalidateDeviceObjects() ;
			pProjectile = pProjectile->pNext ;
		}

		return S_OK ;
	}
	void CFighter::Respawn( CTerrain* pTerrain )
	{
		m_fShieldLevel = 1 ;
		m_bShieldLevelChanged = TRUE ;

	// reset stuff;
		/*int n = (int)((float)(pTerrain->m_nSpawnPos-1) * rnd()) ;
		SetPositionFromValues(pTerrain->m_vSpawnPos[ n ].x,
							  pTerrain->m_vSpawnPos[ n ].y + m_pFRO->m_pRenderData->m_fBSphereRadius * 2.0f,
							  pTerrain->m_vSpawnPos[ n ].z );*/

		m_vExternalForce = xVec3( 0,0,0 );

		for( int i=0; i<m_nContrailEmitters ; i++ )
			if( m_pPS_Contrails[ i ] )
			{
				m_pPS_Contrails[ i ]->m_vMin = xVec3( 0,0,0 ) ;
				m_pPS_Contrails[ i ]->m_vMax = xVec3( 0,0,0 ) ;
			}

	// reset some physics stuff;
		m_fsState = FS_NORMAL ;
		m_fSpeed = 0 ;
		m_fMaxThrustMag = this->m_pFRO->m_pRenderData->m_fBSphereRadius * 8 ;
		m_fThrustDelta = m_fMaxThrustMag / 2 ;
		m_fThrustMag = 0 ;
		m_fStrafeThrustMag = 0 ;
	}

	int CFighter::Render(LPDIRECT3DDEVICE8 pDev,
						 CTextures *pTex,
						 unsigned long *pulPolyCount,
						 CCamera* pCam,
						 xMatUA *pmatProj,
						 D3DVIEWPORT8 *pVP,
						 xMatUA* pmatCurWorld,
						 const xMatUA& matView,
						 const xMatUA& matProj,
						 float fTime,
						 const xVec3& vLightDir )
	{
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

		m_bVisible = GetBBoxVisibility( pDev, *pmatProj, pCam->m_matView, pVP, 
										m_pFRO->m_pBBox->m_vMin, 
										m_pFRO->m_pBBox->m_vMax );
		if( !m_bVisible )
			return 0 ;

		if( m_fsState == FS_EXPLODING )
			return 0 ;

	//
	// BotFighter size HACK, aka BigBots
	// ERR: this causes the bot to be invisibile?!
		/*if( !m_bUserFighter )
		{
			xMat matBigBots ; D3DXMatrixScaling( &matBigBots, 10, 10, 10 );
			D3DXMatrixMultiply( pmatCurWorld, pmatCurWorld, &matBigBots );
		}*/
	//
	// Vertex-shaders:
	//
		if( m_rtFighterMode == FRT_VS1 )
		{
		// set for single texture
			pDev->SetTexture(0, pTex->m_pTex[ MISC_TEX_START + 0 ]) ;
			pDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		// render
			m_pFRO->RenderVS( pDev, fTime, pulPolyCount, TRUE, FALSE, pmatCurWorld, matView, matProj, pCam, vLightDir );
			return 1 ;
		}

	//
	// Fixed Vertex Processing:
	//
		if( m_rtFighterMode == FRT_CHROME || m_rtFighterMode == FRT_GLASS )
		{
		// adds in the spheremap texture
			pDev->SetTexture( 0, pTex->m_pTex[SPHERE_TEX_START + 0] );
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

		// generate spheremap texture coords, and shift them over
			xMat mat;
			mat._11 = 0.5f; mat._12 = 0.0f; mat._13 = 0.0f; mat._14 = 0.0f; 
			mat._21 = 0.0f; mat._22 =-0.5f; mat._23 = 0.0f; mat._24 = 0.0f; 
			mat._31 = 0.0f; mat._32 = 0.0f; mat._33 = 1.0f; mat._34 = 0.0f; 
			mat._41 = 0.5f; mat._42 = 0.5f; mat._43 = 0.0f; mat._44 = 1.0f; 
			pDev->SetTransform( D3DTS_TEXTURE0, &mat );
			pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

		// a glass effect can also be achieved:
			if( m_rtFighterMode == FRT_GLASS )
			{
				pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
				pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
				pDev->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
				pDev->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );
			}

			pDev->SetRenderState(D3DRS_AMBIENT, 0xffffffff) ;
		}
		
		if( m_rtFighterMode == FRT_NORMAL )
		{
		// set for single texture
			pDev->SetTexture(0, pTex->m_pTex[ MISC_TEX_START + 0 ]) ;
			SetTextureColorStage(pDev, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1,	D3DTA_CURRENT)
			SetTextureColorStage(pDev, 1, D3DTA_DIFFUSE, D3DTOP_MODULATE,	D3DTA_CURRENT)
			SetTextureColorStage(pDev, 2, D3DTA_CURRENT, D3DTOP_DISABLE,	D3DTA_CURRENT)
			SetTextureAlphaStage(pDev, 0, D3DTA_DIFFUSE, D3DTOP_SELECTARG1,	D3DTA_CURRENT)
		}

	// render
		m_pFRO->Render( pDev, pTex, pulPolyCount, TRUE, FALSE, pmatCurWorld );

	// restore default renderstates
		if( m_rtFighterMode == FRT_CHROME || m_rtFighterMode == FRT_GLASS )
		{
			pDev->SetTransform( D3DTS_TEXTURE0, &g_matIdent );
			pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

			if( m_rtFighterMode == FRT_GLASS )
			{
				pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE) ;
				pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) ;
				pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) ;
				pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;
			}
		}

		//m_pFRO->m_pBBox->Render( pDev, pulPolyCount, &m_pFRO->m_matWorld );
		return 1 ;
	}

	int CFighter::RenderFX_Weapons( LPDIRECT3DDEVICE8 pDev,
									CTextures *pTex,
									unsigned long *pulPolyCount,
									int ctCamMode,
									xMatUA* pmatCurWorld,
									CCamera* pCam,
									xMatUA *pmatProj,
									D3DVIEWPORT8 *pVP,
									CTerrain* pTerrain,
									LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB,
									float fFPS,
									BOOL* pbMustPopulateVB,
									int wtType )
	{
		int ret = 0 ;

		pDev->SetRenderState( D3DRS_POINTSCALE_A, FtoDW(0.00f) );
		pDev->SetRenderState( D3DRS_POINTSCALE_B, FtoDW(1.00f) );
		pDev->SetRenderState( D3DRS_POINTSCALE_C, FtoDW(0.00f) );



	// projectile particles;
		if( m_nProjectiles > 0 )
		{
			FIGHTER_PROJECTILE* pProjectile = m_pProjectiles ;
			while( pProjectile )
			{
				if( pProjectile->Type == wtType )
				{
					BOOL bVisible ;

					if( !pProjectile->bImpacting )
						bVisible = GetBBoxVisibility( pDev, *pmatProj, pCam->m_matView, pVP, pProjectile->pBBox->m_vMin, pProjectile->pBBox->m_vMax );
					else
					{
						xVec4 v4 ;
						xVec3 vMin, vMax ;
						D3DXVec3Transform( &v4, &pProjectile->pPS->m_vMin, &pProjectile->matWorld );
						vMin.x = v4.x ;
						vMin.y = v4.y ;
						vMin.z = v4.z ;
						D3DXVec3Transform( &v4, &pProjectile->pPS->m_vMax, &pProjectile->matWorld );
						vMax.x = v4.x ;
						vMax.y = v4.y ;
						vMax.z = v4.z ;
						bVisible = GetBBoxVisibility( pDev, *pmatProj, pCam->m_matView, pVP, vMin, vMax );
					}

					if( bVisible )
					{
					// render projectile particle system;
						//if( pProjectile->bImpacting )
						{
							pDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
							pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
						}
						//else
						{
						}
						pProjectile->pPS->Render( pDev,
													!pProjectile->bImpacting ? 
														pProjectile->fRenderSize :
														pProjectile->fRenderSizeWhenImpacting,
												  pulPolyCount,
												  pSharedParticleSystemVB,
												  pProjectile->bStaticPS ? &pProjectile->matWorld : NULL,
													!pProjectile->bImpacting ? 
														(*pbMustPopulateVB) :
														(TRUE),
												  pmatCurWorld );
						ret += *pbMustPopulateVB ;

						if( pProjectile->bImpacting )
							*pbMustPopulateVB = TRUE ;
						else
							*pbMustPopulateVB = FALSE ;
					}

					/*if( !pProjectile->bImpacting )
						pProjectile->pBBox->Render( pDev, pulPolyCount, NULL );
					else
					{
						xVec4 v4 ;
						xVec3 vMin, vMax ;
						D3DXVec3Transform( &v4, &pProjectile->pPS->m_vMin, &pProjectile->matWorld );
						vMin.x = v4.x ;
						vMin.y = v4.y ;
						vMin.z = v4.z ;
						D3DXVec3Transform( &v4, &pProjectile->pPS->m_vMax, &pProjectile->matWorld );
						vMax.x = v4.x ;
						vMax.y = v4.y ;
						vMax.z = v4.z ;
						CBoundingBox bb( &vMin, &vMax );
						bb.RestoreDeviceObjects( pDev );
						bb.Render( pDev, pulPolyCount, NULL );
					}*/
				}
				pProjectile = pProjectile->pNext ;
			}
		}
		return ret ;
	}

	HRESULT CFighter::RenderFX_Contrails( LPDIRECT3DDEVICE8 pDev,
										  CTextures *pTex,
										  unsigned long *pulPolyCount,
										  int ctCamMode,
										  xMatUA* pmatCurWorld,
										  CCamera* pCam,
										  xMatUA *pmatProj,
										  D3DVIEWPORT8 *pVP,
										  CTerrain* pTerrain,
										  LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB,
										  float fFPS,
										  int SB,
										  int DB )
	{
		/* for emssion from origin - 
		xMat matWorld ;
		matWorld = *pmatCurWorld ;
		D3DXMatrixTranslation( &matWorld, m_vPosition.x, m_vPosition.y, m_vPosition.z );*/

		pDev->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );	// 0 works on GF2 better @ large distances. driver issue?
		pDev->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );	// 1
		pDev->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );	// 0

	// contrails
		if( m_fsState == FS_NORMAL )
		{
			for( int i=0 ; i<m_nContrailEmitters ; i++ )
				if( m_pPS_Contrails[ i ] )
				{
					// visibility check is disabled; we have a visibility bug - we probably don't need visibility check anyway
					// as contrails are disabled for bots.
					// TODO: take out the AABB-frustrum check.

					//if( m_bContrailVisible[ i ] )
					{
						float fParticleSize = m_pFRO->m_fScaledLargestSide / 25 * m_fContrailSize ;
						m_pPS_Contrails[ i ]->Render( pDev,
													  fParticleSize,
													  pulPolyCount,
													  pSharedParticleSystemVB,
													  /*&matWorld*/NULL,
													  TRUE,
													  pmatCurWorld );
						
					}
						/*CBoundingBox bb( &m_pPS_Contrails[ i ]->m_vMin, &m_pPS_Contrails[ i ]->m_vMax );
						bb.RestoreDeviceObjects( pDev );
						bb.Render( pDev, pulPolyCount, NULL );*/
				}
		}
		return S_OK ;
	}

	void CFighter::StartExplosionEffect( CRace* pRace )
	{
		m_fsState = FS_EXPLODING ;
		m_fExpAge = 0.0f ;
		m_pExp->Reset() ;

		float s = rnd() ;
		if( s < 0.33f )
			pRace->m_pX7->m_pSound->PlaySFX3D( SFX_EXP2, &m_vPosition, NULL, 0.85f );
		else if( s < 0.66f ) 
			pRace->m_pX7->m_pSound->PlaySFX3D( SFX_EXP1, &m_vPosition, NULL, 1 );
		else
			pRace->m_pX7->m_pSound->PlaySFX3D( SFX_EXP3, &m_vPosition, NULL, 0.95f );

		if( m_bUserFighter )
			pRace->m_pX7->m_pSound->PlaySFX3D( SFX_HUMILIATION, &m_vPosition, NULL, 1 );

	}

	HRESULT CFighter::RenderFX_Explosion( LPDIRECT3DDEVICE8 pDev,
										  CTextures *pTex,
										  unsigned long *pulPolyCount,
										  int ctCamMode,
										  xMatUA* pmatCurWorld,
										  CCamera* pCam,
										  xMatUA *pmatProj,
										  D3DVIEWPORT8 *pVP,
										  CTerrain* pTerrain,
										  LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB,
										  float fFPS,
										  int SB,
										  int DB )
	{
	// billboard explosion
		if( m_fsState == FS_EXPLODING )
		{
			float fExplosionSize = 5 ;
		
			m_pExp->Render( pDev,
							pTex,
							pulPolyCount,
							pCam,
							m_pFRO->m_pRenderData->m_fBSphereRadius * 32 * fExplosionSize,
							&m_vPosition,
							m_pFRO->m_pRenderData->m_fBSphereRadius * 128 * fExplosionSize, 							pSharedParticleSystemVB,
							SB, DB );
		}
		return S_OK ;
	}

	void CFighter::StartShieldEffect( float fDamage, CRace *pRace )
	{
		if( !m_bShowingShieldEffect )
		{
			m_bShowingShieldEffect = TRUE ;
			m_fShieldEffectLifetime = fDamage * 6.0f ;
			m_fShieldEffectAge = 0.0f ;
			
			if( m_bUserFighter )
				pRace->m_pX7->m_pSound->PlaySFX3D( SFX_SHIELDHIT1, &m_vPosition, NULL, 1 );
		}
	}

	HRESULT CFighter::RenderFX_Shield( LPDIRECT3DDEVICE8 pDev,
									   CTextures *pTex,
									   unsigned long *pulPolyCount,
									   int ctCamMode,
									   xMatUA* pmatCurWorld,
									   CCamera* pCam,
									   xMatUA *pmatProj,
									   D3DVIEWPORT8 *pVP,
									   CTerrain* pTerrain,
									   LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB,
									   float fFPS,
									   int SB,
									   int DB )
	{
		if( !m_bShowingShieldEffect )
			return S_OK ;

		HRESULT hr ;
		D3DMATERIAL8 mtr ;
		xMat matTex, m0, m1, m2, m3 ;
		static float X = 0.0f, Y = 0.0f, Z = 0.0f ;
		float fI = m_fShieldEffectAge / m_fShieldEffectLifetime ;
		float fInitialAlpha = 0.5f + ( 0.5f * m_fShieldLevel );

	// ask for auto-gen'd tex coords;
		xMat matScale, matTrans, matFinal ;
		pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

	// set world matrix;
		D3DXMatrixScaling( &matScale, m_pFRO->m_pRenderData->m_fBSphereRadius * fI * 2,
									  m_pFRO->m_pRenderData->m_fBSphereRadius * fI * 2,
									  m_pFRO->m_pRenderData->m_fBSphereRadius * fI * 2 );
		D3DXMatrixTranslation( &matTrans, m_vPosition.x, m_vPosition.y, m_vPosition.z );
		D3DXMatrixMultiply( &matFinal, &matScale, &matTrans );
		D3DXMatrixMultiply( &matFinal, &matFinal, pmatCurWorld );
		pDev->SetTransform( D3DTS_WORLD, &matFinal );

		for( int i=0; i<2 ; i++ )
		{
			switch( i )
			{
				case 0:
					pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
					pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+2 ] );
					SetupSpecularMaterial( &mtr, 0.0f, 0.0f, 1.0f, fInitialAlpha - fInitialAlpha*fI );
					pDev->SetMaterial( &mtr );
					
				// apply x,y,z rotations ot auto-gen'd tex coords;
					D3DXMatrixRotationX( &m0, X / 2.0f );
					D3DXMatrixRotationY( &m1, Y / 2.0f );
					D3DXMatrixRotationZ( &m2, Z / 2.0f );
					D3DXMatrixMultiply( &matTex, &m0, &m1 );
					D3DXMatrixMultiply( &matTex, &matTex, &m2 );
					X+=(0.1f) * (60.0f/fFPS) ;
					Y+=(0.2f) * (60.0f/fFPS) ;
					Z+=(0.3f) * (60.0f/fFPS) ;
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

					hr = pTerrain->m_pItemSphereMesh->DrawSubset( 0 );
					*pulPolyCount += pTerrain->m_pItemSphereMesh->GetNumFaces() ;
					assert( D3D_OK==hr );
					break ;

				case 1:
					pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
					pDev->SetTexture( 0, pTex->m_pTex[ MISC_TEX_START+2 ] );
					SetupSpecularMaterial( &mtr, 0.0f, 0.0f, 1.0f, fInitialAlpha*0.75f - fInitialAlpha*fI*0.75f );
					pDev->SetMaterial( &mtr );
					
				// apply different x,y,z rotations ot auto-gen'd tex coords;
					D3DXMatrixRotationX( &m0, Y / 0.5f );
					D3DXMatrixRotationY( &m1, X / 0.5f );
					D3DXMatrixRotationZ( &m2, Z / 0.5f );
					D3DXMatrixScaling( &m3, 2.0f, 2.0f, 2.0f );
					D3DXMatrixMultiply( &matTex, &m0, &m1 );
					D3DXMatrixMultiply( &matTex, &matTex, &m2 );
					D3DXMatrixMultiply( &matTex, &matTex, &m3 );
					pDev->SetTransform( D3DTS_TEXTURE0, &matTex );

					hr = pTerrain->m_pItemSphereMesh->DrawSubset( 0 );
					*pulPolyCount += pTerrain->m_pItemSphereMesh->GetNumFaces() ;
					assert( D3D_OK==hr );
					break ;
			}
		}

	// cleanup renderstates;
		pDev->SetTransform( D3DTS_TEXTURE0, &g_matIdent );
		pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
		
	// kill shield effect after required lifetime;
		m_fShieldEffectAge += ( 1.0f / fFPS );
		if( m_fShieldEffectAge >= m_fShieldEffectLifetime )
			m_bShowingShieldEffect = FALSE ;

		return S_OK ;
	}

	CFighter::GetFighterAbsPosFromRelative( xVec3* pvRelPos, xVec3* pvRotatedPos )
	{
		xMat mT, mT1, mT2 ;
		xVec4 v4RotatedPos ;
		
		D3DXMatrixTranslation( &mT1, -m_vPosition.x, -m_vPosition.y, -m_vPosition.z ); 
		D3DXMatrixTranslation( &mT2, +m_vPosition.x, +m_vPosition.y, +m_vPosition.z ); 
		D3DXMatrixMultiply( &mT, &mT1, &m_matRot );
		D3DXMatrixMultiply( &mT, &mT, &mT2 );

		D3DXVec3Transform( &v4RotatedPos, pvRelPos, &mT );
		pvRotatedPos->x = v4RotatedPos.x ;
		pvRotatedPos->y = v4RotatedPos.y ;
		pvRotatedPos->z = v4RotatedPos.z ;
	}

	CFighter::GetAvgNormalFrom4TerrainVerts( xVec3* pvnAvg, xVec3* pvTerrainVerts )
	{
		xVec3 vd1, vd2, vcp, vn1, vn2 ;

/*ods( "pvTerrainVerts[0].x=(%0.1f,%0.1f,%0.1f)\n", pvTerrainVerts[0].x, pvTerrainVerts[0].y, pvTerrainVerts[0].z );
ods( "pvTerrainVerts[1].x=(%0.1f,%0.1f,%0.1f)\n", pvTerrainVerts[1].x, pvTerrainVerts[1].y, pvTerrainVerts[1].z );
ods( "pvTerrainVerts[2].x=(%0.1f,%0.1f,%0.1f)\n", pvTerrainVerts[2].x, pvTerrainVerts[2].y, pvTerrainVerts[2].z );
ods( "pvTerrainVerts[3].x=(%0.1f,%0.1f,%0.1f)\n", pvTerrainVerts[3].x, pvTerrainVerts[3].y, pvTerrainVerts[3].z );*/

		D3DXVec3Subtract( &vd1, &pvTerrainVerts[1], &pvTerrainVerts[2] );
		D3DXVec3Subtract( &vd2, &pvTerrainVerts[2], &pvTerrainVerts[0] );
		D3DXVec3Cross( &vcp, &vd1, &vd2 );
		D3DXVec3Normalize( &vn1, &vcp );

		D3DXVec3Subtract( &vd1, &pvTerrainVerts[3], &pvTerrainVerts[2] );
		D3DXVec3Subtract( &vd2, &pvTerrainVerts[3], &pvTerrainVerts[1] );
		D3DXVec3Cross( &vcp, &vd1, &vd2 );
		D3DXVec3Normalize( &vn2, &vcp );

		(*pvnAvg).x = (vn1.z + vn2.z) / 2.0f ;	// must reverse x and z here! -- caused by LERPdY's logic?
		(*pvnAvg).y = (vn1.y + vn2.y) / 2.0f ;
		(*pvnAvg).z = (vn1.x + vn2.x) / 2.0f ;
	}


/*struct WEAPON_TYPE_DETAILS
{
	float fEmmisionMinInterval ;
	float fLifetime ;
	float fDamage ;
	int sfx ;
} ;*/
WEAPON_TYPE_DETAILS g_Weapons[] =
{
	{ 0.2f,  3.0f,  0.6f, SFX_LASER1 },		// WT_GREENLASER
	{ 0.1f,  3.0f,  0.5f, SFX_BULLET1 },	// WT_BLUELASER
	{ 0.05f, 3.0f,  0.3f, SFX_LASER2 }		// WT_BULLET
} ;

/*struct FIGHTER_TYPE_DETAILS
{
	int			nResID ;
	char		szFilename[ 128 ];

	int			nWeaponEmitters ;
	float		fBaseWeaponEmmisionSpeedModifier ;
	D3DVECTOR   vWeaponEmitters[ MAX_WEAPON_EMITTERS ];

	int			nContrailEmitters ;
	float		fContrailParticleLifetime ;
	D3DVECTOR   vContrailEmitters[ MAX_CONTRAIL_EMITTERS ];
} ;*/
FIGHTER_TYPE_DETAILS g_Fighters[] =
{
	{
		0/*IDR_X_BIGSHIP1*/,	"BigShip1.X",
		4, 1.0f,
		{
			{ -0.5f, -0.5f, -0.5f },
			{ +0.5f, -0.5f, -0.5f },
			{ +0.5f, +0.5f, -0.5f },
			{ -0.5f, +0.5f, -0.5f }
		},

		2, 1.5f,
		{
			{ -0.1f, 0.0f, +0.59f },
			{ +0.1f, 0.0f, +0.59f }
		}
	},

	{
		0/*IDR_X_FIGHTER*/, "Fighter.X",
		1, 1.0f,
		{
			{ 0.0f, 0.0f, 0.0f }
		},

		2, 1.5f,
		{
			{ -0.15f, 0.05f, +0.6f },
			{ +0.15f, 0.05f, +0.6f }
		}
	},

	{
		0/*IDR_X_STORM*/, "Storm.X",
		2, 1.0f,
		{
			{ -0.8f, 0.0f, -0.8f }, 
			{ +0.8f, 0.0f, -0.8f }
		},

		4,	0.5f,
		{
			{ -0.07f, 0.0f, +0.65f }, 
			{ +0.07f, 0.0f, +0.65f }, 
			{ 0.0f, -0.07f, +0.65f }, 
			{ 0.0f, +0.07f, +0.65f }
		}
	},

	{
		0/*IDR_X_FIGHTER1*/, "Fighter1.X",
		1, 1.0f,
		{
			{ 0.0f, 0.0f, 0.0f }
		},

		1, 2.0f,
		{
			{ 0.0f, 0.0f, 0.0f }
		}
	},

	{
		0/*IDR_X_FIGHTER2*/, "Fighter2.X",
		1, 1.0f,
		{
			{ 0.0f, 0.0f, 0.0f }
		},

		1, 2.0f,
		{
			{ 0.0f, 0.0f, 0.0f }
		}
	},

	{
		0/*IDR_X_LYRAX*/, "Lyrax.X",
		1, 1.0f,
		{
			{ 0.0f, 0.0f, 0.0f }
		},

		1, 2.0f,
		{
			{ 0.0f, 0.0f, +0.75f }
		}
	},
} ;