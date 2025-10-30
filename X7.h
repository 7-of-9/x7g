#ifndef __X7__
#define __X7__

#include <stdio.h>
#include <windows.h>
#include <commdlg.h>

#define MIN_ERROR_FPS	  0.1f
#define MIN_PHYSICS_FPS   0.1f

class Cx7 ;

#include <assert.h>
#include <time.h>
#include <direct.h>

#include <D3DX8.h>
#include <DDraw.h>

#include "./D3DFrame8.1 Custom/include/d3dapp_custom.h"
#include "./D3DFrame8.1 Custom/include/d3dfont_custom.h"
#include "./D3DFrame8.1 Custom/include/D3DUtil.h"
#include "./D3DFrame8.1 Custom/include/DXUtil.h"

#include "CCmdLineParser.h"	
#include "CRace.h"
#include "CTextures.h"
#include "CUserInput.h"
#include "CThread.h"
#include "CSoundManager.h"
#include "CScreenOverlayManager.h"
#include "CMenuTextures.h"
#include "CMainMenu.h"
#include "CInGameMenu.h"



#define MAX_LEV 500



class Cx7 : public CD3DApplication, CThread
{

public:
    HRESULT OneTimeSceneInit() ;
    HRESULT InitDeviceObjects() ;
    HRESULT RestoreDeviceObjects() ;
    HRESULT InvalidateDeviceObjects() ;
    HRESULT DeleteDeviceObjects() ;
    HRESULT Render() ;
    HRESULT FrameMove() ;
    HRESULT ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format );
    HRESULT FinalCleanup() ;
    LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;

	DWORD	ThreadProc() ;
	HRESULT Screengrab() ;

//
// Construction / Destruction:
//
    Cx7( CCmdLineParser *pCmdLine ) ;

//
// Application State:
//
	bool m_bInMainMenu ;
	bool m_bLoadingGame ;
	bool m_bWaitingToStartGame ;
	bool m_bInGame ;
	bool m_bGamePaused ;
	bool m_bAppMustExit ;
	bool m_bInLevelTransition ;
	bool m_bPlayerJustLost ;
	bool m_bRaceCreated ;

	char m_szLoadingGameStatus[80] ;
	char m_szLoadingGameInfo1[80] ;
	char m_szLoadingGameInfo2[80] ;
	void StartGame() ;
	void StartNextLevel() ;

//
// Level Attributes:
//
	float GetTimeForLevel( int lev );
	void GetOceanColourForLevel( int lev, D3DXCOLOR* pColour );
	int GetOceanTextureForLevel( int lev );
	e_LightingType GetLightingAndSkyboxTexturesForLevel( int lev, D3DXCOLOR* pAmbient, int* nSkyboxFolder );
	void GetTerrainTexturesForLevel( int lev, int* tex1, int* tex2 );


//
// Objects:
//
	CCmdLineParser*			m_pCmdLine ;
	CScreenOverlayManager*	m_pOverlayMan ;
	CMenuTextures*			m_pMenuTex ;
	CMainMenu*				m_pMainMenu ;
	CInGameMenu*			m_pInGameMenu ;

	CTextures*				m_pTextures ;
	CRace*					m_pTestRace ;

	CD3DFont*				m_pStatsFont ;
	CD3DFont*				m_pTitleFont ;
	CD3DFont*				m_pSubtitleFont ;

	CUserInput*				m_pUserInput ;
	CSoundManager*			m_pSound ;

	int						m_nLevel ;
	float					m_fScore ;
	int						m_nLives ;
	
//
// Performance Counters:
//
	float m_fMinFPS ;
	float m_fMaxFPS ;
	float m_fAvgFPS ;
	unsigned long m_ulPPF ;
	unsigned long m_ulMinPPF ;
	unsigned long m_ulMaxPPF ;
	unsigned long m_ulAvgPPF ;
	float m_fMTS ;
	float m_fMinMTS ;
	float m_fMaxMTS ;
	float m_fAvgMTS ;

//
// Top-level Display Settings:
//
	D3DVIEWPORT8 m_vpViewport ;
	xMat		 m_matProj ;
	BOOL	     m_bWireframe ;
	BOOL		 m_bStats ;
	BOOL		 m_bFSAA_Supported ;
	BOOL		 m_bFSAA_Enabled ;
	DWORD		 m_dwNumPassesForAlphaBlend2Tex ;

//
// Windows Information:
//
	HMENU m_hMenu ;

//
// Debug:
//
	DWORD		SB, DB ;
	bool		m_bGodMode ;
	bool		m_bPlayerDamage ;
} ;


#endif