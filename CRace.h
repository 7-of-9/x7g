#ifndef __CRace__
#define __CRace__

class CRace ;

#include <D3DX8Math.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "CTerrain.h"
#include "COcean.h"
#include "CFighters.h"
#include "CSky.h"
/*#include "CSkyDome.h"*/
#include "CTextures.h"
#include "CUserInput.h"
#include "CCamera.h"
#include "CHUDText.h"
#include "CFileRenderObject.h"
#include "x7.h"

// not used, yet.
enum e_GameControllerStates
{
	GCS_LEVEL_PREINTRO = 0,
	GCS_LEVEL_INTRO,
	GCS_LEVEL_INGAME,
	GCS_LEVEL_COMPLETE
} ;

struct BOT_FIGHTER
{
	DWORD		 dwID ;
	CBotFighter* pFighter ;
	BOT_FIGHTER* pPrev ;
	BOT_FIGHTER* pNext ;
} ;

class CRace
{

public:

//
// Construction / Destruction:
//
	CRace( Cx7 *pX7, LPDIRECT3DDEVICE8 pDev,
		   int nLevel,
		   float fMaxTime,
		   int nOceanTex,
		   D3DXCOLOR* pOceanColour,
		   bool bSolidOcean,
		   bool bNight,
		   DWORD dwAmbient,
		   float fMountainFactor,
		   int nTex1,
		   int nTex2,
		   unsigned nNoisePatches1,
		   unsigned nNoisePatches2 );
	~CRace() ;

//
// Game State:
//
	e_GameControllerStates	m_State ;
	float			m_fIntroTimeS ;

	float			m_fSkill ;

	BOOL			m_bDoneOneTimeSetup ;
	BOOL			m_bTimeRanOut ;

//
// BotFighters Linked List:
//
	BOT_FIGHTER*	m_pBots ;
	int				m_nBots ;
	void AddBot( int ftType, CFileRenderObject::RENDER_DATA_INFO* pSharedRenderData );

//
// Counters:
//
	DWORD			m_dwNumFightersPVS ;
	DWORD			m_dwNumFightersDrawn ;
	
//
// Single Objects:
//
	CTerrain*		m_pTerrain ;
	CSky*			m_pSky ;
	/*CSkyDome*		m_pSkyDome ;*/
	CCamera*		m_pCamera ;
	CUserFighter*	m_pFighter ;
	COcean*			m_pOcean ;
	Cx7*			m_pX7 ;

//
// 
//
	D3DXCOLOR		m_OceanColour ;
	CScreenOverlay*	m_pUnderWaterQuad ;

//
// Projectile Collision & Response:
//
	BOOL DetectAndHandleProjectileAgainstAllFighters( void* pvProjectile, void* pvOwningFighter, void** ppvHitFighter );
	BOOL DetectAndHandleFighterProjectileCollision( void* pvFighter, void* pvProjectile, bool bOwningFighterIsUserFighter );

//
// Debug Options:
//
	BOOL m_bShowTerrain ;
	BOOL m_bShowOcean ;
	BOOL m_bShowSky ;
	BOOL m_bShowFighter ;
	BOOL m_bShowHUD ;

	int SB, DB ;

//
// Camera Status:
//
	int m_ctCamMode ;
	BOOL m_bCameraAboveGround ;
	float m_fY ;

//
// Global Lighting:
//
	DWORD m_dwAmbientLight ;
	bool m_bNight ;

//
// Directional Light:
//
	D3DLIGHT8 m_litMain ;

//
// Global Size:
//
	float m_fTerrainLenSq ;
	float m_fFighterSize ;
	float m_fTerrainLenSqM ;
	float m_fWorldSpaceToM ;

//
// Game State:
//
	int				 m_nLevel ;
	CHUDText*		 m_pHUDLevel ;

	float			 m_fGameElapsedTime ;
	float			 m_fGameTimeRemaining ;
	float			 m_fGameMaxTime ;
	CHUDText*		 m_pHUDElapsedTime ;

	EnableHUDs( BOOL bActive );

//
// Shared particle system VB:
//
	LPDIRECT3DVERTEXBUFFER8 m_pSharedParticleSystemVB ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, float fAspect, CScreenOverlayManager* pOM, CTextures* pTex );
	HRESULT	InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev );
    HRESULT Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, float fAspect, float fFPS, int nMaxActiveLights );
    HRESULT FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS );
} ;

#endif