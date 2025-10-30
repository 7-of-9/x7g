#ifndef __CFighters__
#define __CFighters__

class CFighter ;
class CUserFighter ;
class CBotFighter ;
class CRemoteFighter ;

#include <D3DX8Math.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "tex.h"
#include "CFileRenderObject.h"
#include "CUserInput.h"
#include "CCamera.h"
#include "CTerrain.h"
#include "CParticleSystem.h"
#include "CExplosion.h"
#include "CHUDScaleBar.h"
#include "CHUDText.h"
#include "CScreenOverlayManager.h"
#include "CRace.h"

enum e_CamTypes
{
	CT_FREE = 0,
	CT_BEHIND_FIXED1,
	CT_BEHIND_FIXED2,
	CT_BEHIND_FIXED3,
	CT_BEHIND_VARIABLE1,
	CT_BEHIND_VARIABLE2,
	CT_BEHIND_VARIABLE3,
	CT_COCKPIT_FIXED,
	CT_COCKPIT_ROLL,
	CT_SHOWOFF,
	MAX_CAM_TYPES
} ;

enum e_FighterRenderTypes
{
	FRT_NORMAL = 0,
	FRT_CHROME,
	FRT_GLASS,
	FRT_VS1,
	MAX_FIGHTER_RENDER_TYPES
} ;

enum e_FighterStates
{
	FS_NORMAL = 0,
	FS_EXPLODING,
	MAX_FIGHTER_STATES
} ;

//
// Fighter Types
//

#define MAX_WEAPON_EMITTERS 16
#define MAX_CONTRAIL_EMITTERS 4

struct FIGHTER_TYPE_DETAILS
{
	int			nResID ;
	char		szFilename[ 128 ];

	int			nWeaponEmitters ;
	float		fBaseWeaponEmmisionSpeedModifier ;			// 0 - 1; 1 is base, lower is faster
	D3DVECTOR   vWeaponEmitters[ MAX_WEAPON_EMITTERS ];		// in bounding sphere radius units

	int			nContrailEmitters ;
	float		fContrailParticleLifetime ;					// in seconds, lower values produce larger trails and 
	D3DVECTOR   vContrailEmitters[ MAX_CONTRAIL_EMITTERS ];	// as above
} ;

extern FIGHTER_TYPE_DETAILS g_Fighters[] ;

//
// Weapon Types
//

struct WEAPON_TYPE_DETAILS
{
	float fEmmisionMinInterval ;							// min. amount of time between emissions, seconds.
	float fLifetime ;										// max. lifetime of projectile, seconds.
	float fDamage ;											// 0 - 1, per projectile
	int sfx ;												// sound effect ID
} ;

extern WEAPON_TYPE_DETAILS g_Weapons[] ;

enum e_WeaponTypes
{
	WT_GREENLASER = 0,
	WT_BLUELASER = 1,
	WT_BULLET = 2,
	MAX_WEAPON_TYPES
} ;

enum e_FighterTypes
{
	FT_BIGSHIP1 = 0,
	FT_FIGHTER,
	FT_STORM,
	FT_FIGHTER1,
	FT_FIGHTER2,
	FT_LYRAX,
	MAX_FIGHTER_TYPES
} ;

// smooth camera tracking proportional to thrust mag.
#define THRUSTMAG_HISTORY_BUF		10
#define THRUSTMAG_SMOOTH_WEIGHT_D	0.5f


/*
 *
 * Base Fighter.
 *
 */

#define MAX_PROJECTILES 1024

class CFighter
{
public:

	CFighter( float fSkill,
			  int/*e_FighterTypes*/ ftType,
			  float fMyBSRadius,
			  CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData,
			  BOOL bContrails );
	~CFighter() ;
	
	int/*e_FighterTypes*/		m_Type ;
	int/*e_WeaponTypes*/		m_Weapon ;

	int/*e_FighterRenderTypes*/	m_rtFighterMode ;

	DWORD						m_dwID ;	// internal dbg use only

	xVec3	m_TCD_vN, m_TCD_vA ;			// internal dbg use only

//
// Skill Modifier, (scaled up between 0 on first level and 1 on MAX_LEV)
//
	float m_fSkill ;						// 0 - 1

//
// Flight Controls Processing:
//
	void ProcessPhysicsFromFlightControls( FLIGHT_CONTROLS* pFC, float fFPS, CTerrain *pTerrain, BOOL bAcceptInput, CRace* pRace, LPDIRECT3DDEVICE8 pDev, float fElapsedTime, float* fYawDelta );

//
// Render Object:
//
	CFileRenderObject*		m_pFRO ;
	BOOL					m_bVisible ;

//
// Explosion Systems:
//
	CExplosion*				m_pExp ;
	float					m_fExpLifespan ;
	float					m_fExpAge ;
	void StartExplosionEffect( CRace* pRace );

//
// Contrail System:
//
	int						m_nContrailEmitters ;
	xVec3					m_vContrailEmitters[ MAX_CONTRAIL_EMITTERS ];
	CParticleSystem*		m_pPS_Contrails[ MAX_CONTRAIL_EMITTERS ];
	BOOL					m_bContrailVisible[ MAX_CONTRAIL_EMITTERS ];
	D3DXCOLOR				m_clrContrailEmitColour ;
	D3DXCOLOR				m_clrContrailFadeColour ;
	float				    m_fContrailSize ;
	float				    m_fContrailScatter ;

//
// Weapons & Projectiles Systems:
//
	int						m_nWeaponEmitters ;
	float					m_fBaseWeaponEmmisionSpeedModifier ;
	float					m_fLastProjectileEmmitedTime ;
	int						m_ndxCurWeaponEmitter ;
	xVec3				m_vWeaponEmitters[ MAX_WEAPON_EMITTERS ] ;
	struct FIGHTER_PROJECTILE
	{
		e_WeaponTypes		Type ;
		float			    fDamage ;				// 0 - 1
		float				fAge ;					// in seconds
		float				fLifetime ;				// in seconds

		xVec3		    vLeadingPos ;			// instantaneous center pos. of leading particle for CD
		xVec3		    vTrailingPos ;			// instantaneous center pos. of trailing particle for CD 

		float			    fLength ;				// abs. length of projectile for ray-interesection CD purposes
		xVec3			vVel ;					// velocity
		float				fAccel ;				// per-frame: vVel *= fAccel

		CBoundingBox*		pBBox ;					// bbox
		BOOL				bStaticPS ;				// uses quicker world-matrix to animate projectile PS, when set
		xMat			matWorld ;				// only for static projectiles, world-origin relative xfrm

		CParticleSystem*    pPS ;					// particle-system for rendering purposes

		float				fRenderSize;			// pointsprite size for rendering
		float				fRenderSizeWhenImpacting;// pointsprite size for rendering
		BOOL				bImpacting;				// set when collision occurs; produces different animation effects
		float			    fImpactFXLifetime;		// seconds
		float				fImpactFXSpeed;			// abs. constant (>0)
		float				fImpactFXAge;			// seconds

		int					nSoundChannel;			// FMOD channel ID.
		float				fVol;					// FMOD channel volume 0-1.

		FIGHTER_PROJECTILE* pPrev ;
		FIGHTER_PROJECTILE* pNext ;
	} ;
	FIGHTER_PROJECTILE*		m_pProjectiles ;
	int					    m_nProjectiles ;
	void LaunchProjectile( LPDIRECT3DDEVICE8 pDev, float fFPS, float fElapsedTime, /*e_WeaponTypes*/int Type, CRace* pRace );
	void AnimateProjectiles( float fFPS, float fElapsedTime, CTerrain* pTerrain, CRace* pRace );
	FIGHTER_PROJECTILE* KillProjectile( FIGHTER_PROJECTILE* pProjectileToDie );

//
// Sbield:
//
	BOOL					m_bShowingShieldEffect ;
	float					m_fShieldEffectLifetime ;
	float					m_fShieldEffectAge ;
	void StartShieldEffect( float fDamage, CRace* pRace );

//
// Helpers:
//
	GetFighterAbsPosFromRelative ( xVec3* pvRelPos, xVec3* pvRotatedPos );
	GetAvgNormalFrom4TerrainVerts( xVec3* pvnAvg, xVec3* pvTerrainVerts );

//
// Fighter State:
//
	e_FighterStates			m_fsState ;

//
// Positioning, velocity and orientation:
//
	void SetPositionFromValues( float x, float y, float z );
	
	float					m_fSpeed ;			// combined magnitude of thrust & external forces, abs world units.
	float					m_fThrustDelta ;	// amount by which to change thrust, per second.
	float					m_fThrustMag ;		// user-controlled thrust level, abs world units.
	float					m_fStrafeThrustMag ;// user-controlled strafe thrust (left) level, abs world units, (per 60th? check.)
	float					m_fMaxThrustMag ;	// max user-controlled thrust level, abs world units, (per 60th? check.)

	float					m_fSteeringSensitivity ;
	float					m_fRollSensitivity ;

	xVec3				m_vOrientation, m_vInitialOrientation ;
	xVec3				m_vDirectionOfTravel ;
	xVec3				m_vUp, m_vLeft ;

	xVec3				m_vPosition ;

	xVec3				m_vExternalForce ;

	BOOL					m_bJustBouncedOffTerrain ;

	float					m_fPitch ;
	float					m_fYaw ;
	float					m_fRoll ;
	xMat				m_matTrans, m_matRot, m_matLastTransDelta ;

	float					m_fDistanceToGround ;

	BOOL					m_bTerrainCollision ;

	xVec3				m_vVelocityMS ;
	float					m_fSpeedMS ;

	float					m_fTerrainForce ;				 // current terrain force +/- y in world units / sec.
	float					m_fMaxTerrainForce ;			 // max grav force that the terrain exerts.
	float					m_fTerrainForceBalanceAltitude ; // the target terrain force altitude.

//
// Core Systems:
//
	float					m_fShieldLevel ;		// 0 - 1
	float					m_fWeaponsLevel ;		// 0 - 1
	float					m_fEngineLevel ;		// 0 - 1

//
// Flags:
//
	BOOL					m_bShieldLevelChanged ;
	BOOL					m_bJustFinishedExploding ;
	void Respawn( CTerrain* pTerrain );

	BOOL					m_bUserFighter ;

//
// CD:
//
	BOOL DetectAndHandleTerrainCollision( xVec4* pvTestBBVerts, CTerrain* pTerrain, CRace* pRace );

//
// Dynamic Camera Management:
//
	void ManageCamera( LPDIRECT3DDEVICE8 pDev, CCamera *pCamera, CUserInput *pIn, int ctCamMode );
	float m_fThrustMagHistory[ THRUSTMAG_HISTORY_BUF ];

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM, CTextures* pTex, float fAspect );

	HRESULT	InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev );

	int Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, CCamera* pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, xMatUA* pmatCurWorld, const xMatUA& matView, const xMatUA& matProj, float fTime, const xVec3& vLightDir );

	int RenderFX_Weapons( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int ctCamMode, xMatUA* pmatCurWorld, CCamera* pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, CTerrain* pTerrain, LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB, float fFPS, BOOL* pbMustPopulateVB, int wtType );

	HRESULT RenderFX_Contrails( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int ctCamMode, xMatUA* pmatCurWorld, CCamera* pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, CTerrain* pTerrain, LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB, float fFPS, int SB, int DB );

	HRESULT RenderFX_Explosion( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int ctCamMode, xMatUA* pmatCurWorld, CCamera* pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, CTerrain* pTerrain, LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB, float fFPS, int SB, int DB );

	HRESULT RenderFX_Shield( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int ctCamMode, xMatUA* pmatCurWorld, CCamera* pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, CTerrain* pTerrain, LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB, float fFPS, int SB, int DB );
	
	int FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, BOOL bManageCamera, CTerrain *pTerrain, int ctCamMode, float fElapsedTime, CRace* pRace );

	void PreFrameMove( CRace* pRace );

} ;



/*
 *
 * User-controlled derived fighter class;
 *
 */

#define MAX_SCANNER_POINTS 1024

class CUserFighter : public CFighter
{
public:
	CUserFighter( float fSkill,
				  int nLevel,
				  int nLives,
				  int/*e_FighterTypes*/ ftType,
				  float fMyBSRadius,
				  CTerrain* pTerrain,
				  CScreenOverlayManager* pOM,
				  CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData );
	~CUserFighter() ;
	CScreenOverlayManager* m_pOM ;

	BOOL DetectAndHandleItemCollisions( xVec3* pvCurPos, xVec3* pvTestPos, CTerrain* pTerrain, CRace* pRace );

	int FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, BOOL bAcceptInput, BOOL bManageCamera, CTerrain *pTerrain, int ctCamMode, float fElapsedTime, BOOL nShowHUD, CRace* pRace );

	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM, CTextures* pTex, float fAspect );

	HRESULT	InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev );

//
// HUD:
//
	float	  m_fScoreAddition ;
	bool	  m_bUpdatedScore ;
	float	  m_fTimeInScoreZone ;
	int		  m_nCherries ;
	BOOL	  m_bGameOver ;

	CHUDScaleBar*	 m_pHUDShieldLevel ;
	CHUDScaleBar*	 m_pHUDShieldBackG ;
	CHUDScaleBar*	 m_pHUDWeaponsLevel ;
	CHUDScaleBar*	 m_pHUDWeaponsBackG ;
	CHUDScaleBar*	 m_pHUDEngineLevel ;
	CHUDScaleBar*	 m_pHUDEngineBackG ;
	
	CHUDText*		 m_pHUDSpeed ;
	CHUDText*		 m_pHUDScore ;
	CHUDText*		 m_pHUDItems ;
	CHUDText*		 m_pHUDLives ;

	CScreenOverlay*	 m_pRedQuad ;

	EnableHUD( BOOL bActive );
	HRESULT RenderHUD( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int ctCamMode, xMatUA* pmatCurWorld, CCamera *pCamera, CScreenOverlayManager* pOM, CRace* pRace, float fFPS );

//
// HUD SpaceScanner:
//
	CScreenOverlay*			m_pScannerBackG ;
	int						m_nScannerPoints ;
	LPDIRECT3DVERTEXBUFFER8	m_pVB_Scanner ;
	float				 	m_fScannerRot ;
	float					m_fScannerSize ;	// length of scanner square
	xVec2					m_vScannerPos ;		// scanner center in origin centered coords
	void FrameMoveSpaceScanner( CTerrain* pTerrain, CRace* pRace );

//
// HUD Target Icon:
//
	CScreenOverlay*			m_pTargetIcon ;
	float					m_fTargetIconMinSize ;
	float					m_fTargetIconMaxSize ;

//
// HUD Crosshair:
//
	CScreenOverlay*			m_pCrosshair ;

//
// HUD misc. text draws:
//
	char					m_szHUDText[ 80 ];
	bool					m_bShowingHUDText ;
	float					m_fHUDTextLifespan ; // seconds
	float					m_fHUDTextAge ;		// counts up to lifetime, seconds
	float					m_fHUDTextRed ;		// 0 - 1
	float					m_fHUDTextGreen ;	// 0 - 1
	float					m_fHUDTextBlue ;	// 0 - 1
	float					m_fHUDTextAlpha ;	// 0 - 1: initial alpha, always scales down to zero through lifetime
	float					m_fHUDTextX, m_fHUDTextY ;				// params to DrawTextScaled
	float					m_fHUDTextXScale, m_fHUDTextYScale ;	// params to DrawTextScaled
} ;



/*
 *
 * Computer-controlled derived fighter class;
 *
 */
enum e_BotFighterMovementModes
{
	BFMM_STATIONARY,
	BFMM_HUNTING,
	BFMM_WANDERING,
	BFMM_DEFENDING,
	BFMM_MINELAYING,
	BFMM_TERRAINCOLLISIONRESPONSE
} ;

class CBotFighter : public CFighter
{
public:
//
// Construction / Destruction:
//
	CBotFighter( float fSkill,
			     int nLevel,
				 int/*e_FighterTypes*/ ftType,
				 float fMyBSRadius,
				 CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData );
	~CBotFighter() ;
	e_BotFighterMovementModes m_MovementMode ;
	e_BotFighterMovementModes m_PrevMovementMode ;

//
// Dead or Alive?
//
	bool m_bBotIsDead ;
	bool m_bBotWasKilledByProjectile ;
	int  m_nBotLives ;

//
// Skill Thrust Mag Modifier:
//
	float m_fThrustMagSkillModifier ;

//
// Flight Controls:
//
	FLIGHT_CONTROLS	m_FlightControls ;
	void FC_Zero() ;
	void FC_Set( LPDIRECT3DDEVICE8 pDev, CRace* pRace, float fFPS );
	void FC_AlignWithTarget( const xVec3& vTarget, float* fAngleX, float* fAngleY, float fTurnSpeed );

//
// Navigation:
//
	float m_fDistanceToTargetLastFrame ;
	float m_fTerrainCollisionResponseTime ;

//
// D3DFrame Event Hooks:
//
	int FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, BOOL bManageCamera, CTerrain *pTerrain, int ctCamMode, float fElapsedTime, CRace* pRace );
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, CScreenOverlayManager* pOM, CTextures* pTex, float fAspect );
	HRESULT	InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev );
} ;



/*
 *
 * Remote-controlled derived fighter class;
 *
 */
class CRemoteFighter : public CFighter
{
public:
	CRemoteFighter() ;
	~CRemoteFighter() ;

	void Process(CUserInput *pInput, float fFPS, CTerrain *pTerrain) ;
} ;

#endif







