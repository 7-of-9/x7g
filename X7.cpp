/*

  (c) AE 2001,2

  Usage:

	x7g

		[/Terrain1:x]		int		Terrain patches sq. Powers of 2 only. Defaults to 8.
		[/Terrain2:x]		int		Patch size sq. Powers of 2 only. Defaults to 32.
		[/TerrainFlat:b]	bool	Create flat test terrain. Defaults to 0.

		[/Fullscreen:b]		bool	Go to fullscreen on startup. Defaults to 0.
		[/FullscreenWidth:n]int		Fullscreen width to go to. Defaults to 640.
		[/FullscreenHeight:n]int	Fullscreen height to go to. Defaults to 480.
		[/VSync:b]			bool	Clamp to vysnc. rate in fullscreen. Defaults to 0.
		[/FSAA:n]			int		Multisampling. Defaults to 0 (none).
		[/DepthBufMinBits:n]int		Depth buffer minimum bits (16/24/32). Defaults to 24.

		[/InputSens:n]		int		0 - 255. Control sensitivity. Defaults to 100.
		[/MouseInvert:b]	bool	Invert mouse y axis. Defaults to 0.
		[/NonStickyMouse:b]	bool	BROKEN. Nonsticky mouse, good for nipple controller. Defaults to 0.	
		[/MouseExclusive:b] bool	DInput exclusive mode mouse capture. Defaults to 1.

		[/WinWidth:x]		int		Window creation width. Defaults to 640.
		[/WinHeight:x]		int		Window creation height. Defaults to 480.
		[/WinX:x]			int		Window creation x position. Defaults to CW_USEDEFAULT.
		[/WinY:x]			int		Window creation y position. Defaults to CW_USEDEFAULT.

		[/SoundMusic:n]		int		0 - 255. Music volume. Defaults to 200. Set both Sound params to 0 to completely disable FMOD.
		[/SoundFX:n]		int		0 - 255. Sound FX volume. Defaults to 200. Set both Sound params to 0 to completely disable FMOD.
		[/MusicPath:s]	    str     Path for MP3 playback.
		
		[/FPS:b]			bool	Show frame stats in Release build. Defaults to 0.
		[/AutoStart:b]		bool	Auto start from main menu. Defaults to 0.
		[/AutoGrab:b]		bool	Auto screen grab during game. Defaults to 0.
		[/AvgStats:b]		bool	Show stats after race finishes. Defaults to 0.
		[/BlackSky:b]		bool	Render skybox black. Defaults to 0.
		[/God:b]			bool	God mode. Defaults to 0.
		[/Bots:x]			int		Number of bots desired. Defaults to 0.
		[/Level:x]			int		Starting level. Defatuls to 1.
		[/PlayerDamage:b]	bool	Does player take damage? Defaults to 1.


		ALSO: mouse invert, sensitivity, MP3 folder autosearch and drag and drop
		NON_STICKY MOUSE POLLING SUCKS BIG TIME, SHOULD FIX IT OR DISABLE IT

		xcopy /sycd $(InputDir)*.* k:\DXSDK81\Dev\x7g_vc6

*** GetTexturesForLevel: fill out to apply variation ***

*** Powerup pickup sound is so quiet because it is loaded as HW3D -- constant loud sounds need to be loaded as HW2D. ***

GAMEPLAN
gets bots to DIE DAMMIT
should be able to lose the bots, either underwater, or line-of-site.
lots of bots, always following, except when underwater, they can't see you!
IMPRESSIVE for good terrain flying
weapons power to decay with use
set # of ground targets to destroy, including additional bot factories, / level.
spacescanner to list BOTS and TARGETS, NOT GAMEITEMS!!! They must be 'SECRET'!

TODO:

BEFORE INITIAL DEMO RELEASE:
************************************************************
************************************************************
************************************************************
***********got lost, flew too high / too far from terrain.
************************************************************
************************************************************
************************************************************
***********NEED to fly close to ground.
************************************************************
************************************************************
************************************************************
***********replay option after each level, w/ extreme cockpit view
************************************************************
************************************************************
************************************************************
***********different fighter / level, w/ target summary during load
************************************************************
************************************************************
************************************************************
***********different colour contrails / level.
************************************************************
************************************************************
************************************************************
***********lev-start scene: show-off for set time, with bots surrounding and tracking!
************************************************************
************************************************************
************************************************************
camera zoom, from far out when going slow, to close in (and to cockpit view?) when
going fast.
take-off from aircraft carrier?!
ground targets
BETTER visibility for enemy indicators
TAKE gameitems off of radar and put ENEMIES into it
VISIBILITY for ground targets
POINT-POINT patrol network?!




explosions should probably be bigger
add new bot mode; destroying ground items
fix underwater blue effect
night levels
ground targets
re-consider shield graphic [also seems to trigger memory faults?]
fix heart render bugs
locking, homing missiles
bot generators
mine layers

TOREL: 
0. hoops to fly through.
0. ground targets to defend, and some to destroy.
0. MORE visual weapon effects.
0. MINE layers.
0. flag for solid floor on some levels, with no alpha and rock texture, i.e. no ocean!
0. bots - they are just too stupid at the moment, i.e. getting stuck on the terrain.
0. bots - they should match the user ship's abilities - user - should scale with powerups.
0. bots - botcam needs to cycle thru each bot to ensure proper behaviour.
1. level differentiation: different terrain textures, different colourations, different skyboxs,
   different water colours and textures and different terrain shapes. preferarably increasing in difficulty.
2. music: auto-play thru an entire folder.
3. scoring: score text should expand and/or show a different render effect as it increases.
4. more sound effects.
5. hiscore table.
6. RELEASE.

docs->surfaces->gamma controls; way cool fast way of
flashing the screen red/blue/green etc. Only available in fullscreen exclusive mode?!

screenshots;
need to convert from BMP to JPG for automatic posting.

cursors for better menu picking; docs->about
devices->using devices->working with mouse cursors -
should allow custom cursor icon.

D3DPOOL_MANAGED; check this is specified everywhere;
seems the only bad place to use it is on VBs or IBs
which are updated every frame; we don't have any of
them.

D3DXMATRIXA16; 16-byte aligned matrix class.

D3DXConvertMeshSubsetToStrips and D3DXConvertMeshSubsetToSingleStrip; could be perf. winner?

	Add light on ship, should look nice as it lights up terrain underneath ship
	Skybox needs attention

*/

#pragma warning( disable : 4183 )	// 'missing return type'

#define STRICT

#pragma message(" ")
#pragma message("GAMEPLAY CONCEPT:")
#pragma message(" ")
#pragma message("  POWERUPS:")
#pragma message("    Linear-powerup system; use alpha-blended and textured spheres of various radii.")
#pragma message("    Radii proportional to powerup strength?")
#pragma message(" ")
#pragma message("    GREEN: Movement; discreet steps, like rolling & max. speed, brakes etc.")
#pragma message("    BLUE:  Shield;   discreet steps, abs. shield power, orbiting death ball?")
#pragma message("    RED:   Weapons;  linearly increasing particle emitters with color & type change steps for variety?")
#pragma message(" ")
#pragma message("  SCORING:")
#pragma message("    Bonus points for flying close to terrain...!")
#pragma message("    Constant bonus points for flying close = danger!")
#pragma message(" ")
#pragma message("  KILLING:")
#pragma message("    What to use weapons on??! Other flying opponents, or LAND-BASED TARGETS?")
#pragma message(" ")
#pragma message("  GAMEPLAY CONCEPT:")
#pragma message("    Timed collection of set no. of items, with opposition computer ships;")
#pragma message("    Some attack you, some attack your items, SOME COULD LAY MINES!.")
#pragma message(" ")
#pragma message("OPTIMIZATIONS:")
#pragma message("    Terrain-vertex's 2nd tex. coord - *not currently used* - could be auto. gen'd by DX anyway.")
#pragma message("    Use 'const &' instead of pointers when passing. optimizr hint.")
#pragma message("    P4 only: _ALIGN_16 on xMat & D3DXVECTOR-types passed to D3DX; not allowed on parameters?")
#pragma message("    URGENT: Particle system projectiles are grossly inefficient; they need batching in a _large_ VB thru a manager class")
#pragma message("    URGENT: Projectiles frustrum-culling")
#pragma message("    Item Lights; are currently v. costly SPOTLIGHT type - this probably isn't necessary.")
#pragma message("    Explosions; can contain up two two billboards+particle system = 3 VBs, each fighter has")
#pragma message("    multiple explosions; when rendering lots of these, we'd be switching VBs needlessly;")
#pragma message("    the billboard VBs only have 4 verts each in though! This isn't optimal.")
#pragma message("    URGENT: do a ray-bbox intersection test between projectile rays and terrain qtree bboxs @ time of launch.")
#pragma message("            those that fail need not be LERPdY each frame.")
#pragma message("    URGENT: LOD algo. for contrails.")
#pragma message("    URGENT: FrameMove'ing contrails is MASSIVELY CPU intensive, for large #s of fighters. We need 2 ")
#pragma message("            figure perhaps a distance-based way of LODing this expense right down.")
#pragma message("            Contrails currently DISABLED for BotFighters.")
#pragma message(" ")
#pragma message("DO-ABLE TODOS:")
#pragma message("    Lights associated with projectiles, would look SO nice, but could be SO costly.")
#pragma message("    Set cam. to ShowOff while exploding")
#pragma message("    Better physics model for collisions; a 'spin' component would be cool.")
#pragma message(" ")
#pragma message("GAME INFRASTRUCTURE TODOS.")
#pragma message("    Level intro: flyby with cool cam mode as fighter approaches island")
#pragma message("    Controls selection/binding screen.")
#pragma message("    MP3-folder browsing screen.")
#pragma message("    Fighter selection screen.")
#pragma message(" ")
#pragma message("ALPHA RELEASE TODOS.")
#pragma message("    BMP->PNG conversions w/ auto-submit to website.")
#pragma message("    Installer.")
#pragma message("    GFX card checks?")
#pragma message("    Autosync. with global hiscore board (https would be good!)")
#pragma message(" ")
#pragma message("DIFFICULT THINGS - WISHLIST:")
#pragma message("    Vidtex-playback boards; just cool.")
#pragma message("    Billboard trees; would need v. good LOD algo.")
#pragma message("    Reflections+shadows, now quite high priority - good eye candy, v. hard, maybe v. expensive")
#pragma message(" ")
#pragma message("BUGS:")
#pragma message("    FIXED? Fixed inversion bug once and for all?! =)++ ")
#pragma message("    Contrail visibility bug (probably requires emision from world origin + translation)?")
#pragma message("    Weapon Firing Bug; fly to great height to witness this.")
#pragma message("    'DSOUND leaked one secondary buffer' in debug output.")
#pragma message("    FIXED? Terrain collision bouncing and sticking?.")
#pragma message(" ")
#pragma message("PLAN:")
#pragma message("    Scoring, Shield, Shield+Movement Powerups, Opposition, Weapons, Collectables, Timer, Level Progression")
#pragma message("    Weapon terrain CD (+OBBs, Lights, explosions), Shield Sphere, Bounce 'n' Spin (TM), keyb. roll + increase turn rate")
#pragma message("    MORE weapons; static missiles & homing missiles (moving paticle emitters?)")
#pragma message("    BETTER impact effects; SHIELD effect")
#pragma message("    SCALING weapons;")
#pragma message("    animated HUD with targeting, locking, and fancy shit;")
#pragma message("    Vertex Shader fighter render modes, (with trails?);")
#pragma message("    Harder handling w/ Bounce 'n' Spin.")
#pragma message("    Input Recording & Playback.")
#pragma message("    Scoring; bonus for item collections, with additional bonus for getting multiple items quickly.")
#pragma message("    Scoring; bonus for spinning.")
#pragma message("    Level progression; timer countdown + incremental level progressions...")
#pragma message("    Activate powerups and scale weapons.")
#pragma message("    Motion-blur as per flipcode IOTD.")
#pragma message("    Change line-sphere test to box-sphere test; CRUCIAL for allowing large lasers to have a bigger hit area!")
#pragma message("    Homing missiles.")
#pragma message("    Powerup combos?!")
#pragma message("    HUD target hilighting & locking Algos.")
#pragma message("    Bot Fighter Movement Algos... inc. dynamic flocking")
#pragma message("    Reinstate underwater fog.")
#pragma message("    Blast radius on projectiles..")
#pragma message("    VC++ Macros: keyboard shortcuts for 'Go to Definition' and for switching between .H and .CPP")
#pragma message("    PLAN: reinstate special weapon: FLAMETHROWER! ")
#pragma message("    PLAN: Work on bot terrain avoidance")
#pragma message("    PLAN: Work on more diverse explosions")
#pragma message("    PLAN: Work applying external 'shake' force when hit. Should shake the camera.")
#pragma message(" ")


#include "x7.h"
#include "resource.h"

#define SHOW_FRAMEWORK_EVENTS 0


int g_nAppVerMaj = 0 ;
int g_nAppVerMin = 0 ;
char g_szAppRelease[] = "Alpha" ;
#ifdef _DEBUG				
	char g_szAppBuild[] = "Debug" ;
#else
	char g_szAppBuild[] = "Release" ;
#endif



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR szCmdLine, INT)
	{
		CCmdLineParser CmdLine( szCmdLine );
		Cx7 d3dApp( &CmdLine );

		/*short s1 = 1 ;
		D3DXMATRIX mat1 ;
		BYTE b = 2 ;
		D3DXMATRIXA16 mat2;

		 if (reinterpret_cast<int>(&mat2) & 15)
			 Msg2("******test D3DXMATRIXA16 NOT aligned!!");
		 else
			 Msg2("test D3DXMATRIXA16 IS aligned!!");

		 if (reinterpret_cast<int>(&mat1) & 15)
			 Msg2("test D3DXMATRIX NOT aligned!!");
		 else
			 Msg2("test D3DXMATRIX IS aligned!!");*/

		//Msg2( "sizeof(COLOREDVERTEXWITHNORMAL2TEX)=%d\n", (int)sizeof(COLOREDVERTEXWITHNORMAL2TEX) );
		//Msg2( "sizeof(COLOREDVERTEXWITHNORMAL)=%d\n", (int)sizeof(COLOREDVERTEXWITHNORMAL) );
		//Msg2( "sizeof(POINTSPRITEVERTEX)=%d\n", (int)sizeof(POINTSPRITEVERTEX) );
		//Msg2(" %d ", D3DXSphereBoundProbe( &xVec3(0,10,0), 1, &xVec3(0,0,0), &xVec3(0,100.01f,0) ) );

		/*xVec3 vI1, vI2 ;
		int n ;
		BOOL b ;
		b = SphereLineTest( &xVec3(0,12,0), &xVec3(0,13,0), &xVec3(0,10,0), 1, &n, &vI1, &vI2 );
		Msg2("%d [%d (%0.3f,%0.3f,%0.3f) (%0.3f,%0.3f,%0.3f)]", b, n, vI1.x, vI1.y, vI1.z, vI2.x, vI2.y, vI2.z );*/

		//Msg2( "sizeof(xVec3)=%d", (int)sizeof(xVec3) );

		D3DXMatrixIdentity( &g_matIdent );

		if (FAILED(d3dApp.Create(hInst)))
			return 0 ;
		INT r = d3dApp.Run() ;
		
		return r ;
	}




//-----------------------------------------------------------------------------
// Name: Cx7()
// Desc: Constructor
//-----------------------------------------------------------------------------
	Cx7::Cx7( CCmdLineParser* pCmdLine )
	{
	    srand( (unsigned)time( NULL ) << 16 );


	//
	// Misc. Initializations:
	//
//#ifndef _DEBUG
//		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS) ;
//#endif

	//
	// Cmd-line:
	//
		m_pCmdLine = pCmdLine ;

	//
	// D3DFrame Member Overrides:
	// 
		m_strWindowTitle    = _T("·———• ×¬ •———·") ;  //ë??? .ø.ÐÆ ™²æ •—·øØ©øø1——Æ™——Ø¹·®®°·©ØØ1—æ™--×¬--
		m_bUseDepthBuffer   = TRUE ;
		m_dwMinDepthBits	= m_pCmdLine->GetInt( "DepthBufMinBits", 24 );
		m_dwCreationWidth   = m_pCmdLine->GetInt( "WinWidth", 640  );
		m_dwCreationHeight  = m_pCmdLine->GetInt( "WinHeight", 480 );
		m_dwCreationX		= m_pCmdLine->GetDWord( "WinX", CW_USEDEFAULT );
		m_dwCreationY		= m_pCmdLine->GetDWord( "WinY", CW_USEDEFAULT );
		m_bVSync			= m_pCmdLine->GetInt( "VSync", 0 );
		m_nFSAA				= m_pCmdLine->GetInt( "FSAA", 0 );
		m_nFullScreenWidth  = m_pCmdLine->GetInt( "FullscreenWidth", 640 );
		m_nFullScreenHeight = m_pCmdLine->GetInt( "FullscreenHeight", 480 );

	//
	// Application State:
	//
		m_bLoadingGame = false ;
		m_bWaitingToStartGame = false ;
		m_bInGame = false ;
		m_bGamePaused = false ;
		m_bInMainMenu = false ;
		m_bAppMustExit = false ;
		m_bInLevelTransition = false ;
		m_bPlayerJustLost = false ;
		m_bRaceCreated = false ;


		m_szLoadingGameStatus[0] = '\x0' ;
		m_szLoadingGameInfo1[0] = '\x0' ;
		m_szLoadingGameInfo2[0] = '\x0' ;
		
	//
	// Object Instances:
	//
		m_pMenuTex = NULL ;
		m_pOverlayMan = NULL ;
		m_pTestRace = NULL ;
		m_pTextures = NULL ;
		m_pStatsFont = NULL ;
		m_pTitleFont = NULL ;
		m_pSubtitleFont = NULL ;
		m_pUserInput = NULL ;
		m_pSound = NULL ;
		m_pMainMenu = NULL ;
		m_pInGameMenu = NULL ;

		m_nLevel = m_pCmdLine->GetInt( "Level", 1 );
		m_fScore = 0 ;
		m_nLives = 3 ;

	//
	// Timing and Performance Counters:
	//
		m_fMinFPS = 0.0f ;
		m_fMaxFPS = 0.0f ;
		m_fAvgFPS = 0.0f ;
		m_ulPPF = 0 ;
		m_ulMinPPF = 0 ;
		m_ulMaxPPF = 0 ;
		m_ulAvgPPF = 0 ;
		m_fMTS = 0.0f ;
		m_fMinMTS = 0.0f ;
		m_fMaxMTS = 0.0f ;
		m_fAvgMTS = 0.0f ;

	//
	// Top-level Display Settings:
	//
		m_bWireframe = FALSE ;
		m_bStats = FALSE ;
		m_bFSAA_Supported = FALSE ;
		m_bFSAA_Enabled = FALSE ;

	//
	// Debug:
	//
		SB = DB = D3DBLEND_ZERO ;
		m_bGodMode = ( 1 == pCmdLine->GetInt( "God", 0 ) );
		m_bPlayerDamage = ( 1 == pCmdLine->GetInt( "PlayerDamage", 1 ) );
	}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Application entry point.
//-----------------------------------------------------------------------------
	HRESULT Cx7::OneTimeSceneInit()
	{
		m_hMenu = GetMenu(m_hWnd) ;

		EnableMenuItem(m_hMenu, ID_VIEW_WIREFRAME, MF_ENABLED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_WIREFRAME, m_bWireframe ? MF_CHECKED : MF_UNCHECKED) ;

		EnableMenuItem(m_hMenu, ID_VIEW_STATS, MF_ENABLED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_STATS, m_bStats ? MF_CHECKED : MF_UNCHECKED) ;

		m_pOverlayMan = new CScreenOverlayManager() ;
		assert( m_pOverlayMan );
		
		m_pMenuTex = new CMenuTextures() ;
		assert( m_pMenuTex );

		m_pMainMenu = new CMainMenu(this) ;
		assert( m_pMainMenu );
		
		m_pInGameMenu = new CInGameMenu(this) ;
		assert( m_pInGameMenu );

		m_pSound = new CSoundManager( m_pCmdLine->GetString( "MusicPath", "..\\resource\\wav2" ),
									  m_pCmdLine->GetInt( "SoundMusic", 200 ),
									  m_pCmdLine->GetInt( "SoundFX", 200 ) );
		assert( m_pSound );

		m_pStatsFont = new CD3DFont(_T("Courier New"), 9) ;
		assert( m_pStatsFont );

		m_pTitleFont = new CD3DFont(_T("Arial Black"), 25, 0/*D3DFONT_BOLD*/) ;
		assert( m_pTitleFont );

		m_pSubtitleFont = new CD3DFont(_T("Arial"), 8, 0/*D3DFONT_BOLD*/) ;
		assert( m_pSubtitleFont );

		m_pUserInput = new CUserInput( m_hWnd,
									   m_pCmdLine->GetInt( "NonStickyMouse", 0 ) == 1 ? true : false,
									   (float)m_pCmdLine->GetInt( "InputSens", 100 ) / 255,
									   m_pCmdLine->GetInt( "MouseInvert", 0 ) == 1 ? true : false,
									   m_pCmdLine->GetInt( "MouseExclusive", 1 ) == 1 ? true : false );
		assert( m_pUserInput );

		m_bInMainMenu = TRUE ;
		
		return S_OK ;
	}



	
//-----------------------------------------------------------------------------
// Name: StartGame()
// Desc: Called by CGameSelector when play must commence.
//-----------------------------------------------------------------------------
	void Cx7::StartGame()
	{
		m_bLoadingGame = TRUE ;
		BeginThreadProc() ;
	}

	void Cx7::StartNextLevel()
	{
		InvalidateDeviceObjects() ;
		m_bRaceCreated = false ;
		SAFE_DELETE( m_pTestRace )
		SAFE_DELETE( m_pTextures )
		RestoreDeviceObjects() ;
		m_nLevel++ ;
		StartGame() ;
	}

//-----------------------------------------------------------------------------
// Name: ThreadProc()
// Desc: Called during game startup, this function performs all the
//       permanent initialization of device-independent scene objects, i.e.
//		 X7LTs and indexed data (NOT VBs & IBs), and peripheral objects.
//-----------------------------------------------------------------------------
	DWORD Cx7::ThreadProc()
	{
		HRESULT hr ;
		FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height ;

	//
	// Get some of the level attributes:
	//
		D3DXCOLOR OceanColour ;								
		int nSkyboxFolder ;
		D3DXCOLOR Ambient ;
		e_LightingType light_type ;
		int nTerrainTex1, nTerrainTex2 ;

		GetOceanColourForLevel( m_nLevel, &OceanColour );
		light_type = GetLightingAndSkyboxTexturesForLevel( m_nLevel, &Ambient, &nSkyboxFolder );
		GetTerrainTexturesForLevel( m_nLevel, &nTerrainTex1, &nTerrainTex2 );

	//
	// Load texture pool, informing tex. manager of our some of our level's attributes.
	// This is to stop it loading all resources for these types, and just the ones
	// that we will be drawing with.
	//
		strcpy(m_szLoadingGameStatus, "m_pTextures") ;
		m_pTextures = new CTextures( m_szLoadingGameInfo1, m_szLoadingGameInfo2, light_type, nSkyboxFolder, nTerrainTex1, nTerrainTex2 );
		assert( m_pTextures );

	//
	// Load race manager:
	//
		strcpy(m_szLoadingGameStatus, "m_pTestRace") ;

		m_pTestRace = new CRace( this,
								 m_pd3dDevice,
								 m_nLevel,
								 GetTimeForLevel( m_nLevel ),
								 GetOceanTextureForLevel( m_nLevel ),
								 &OceanColour,
					 		     false,												/* bSolidOcean */
								 ( light_type == LT_NIGHT ),						/* bNight */
								 Ambient,
								 ((float)(m_nLevel % 10) / 9),						/* fMountainFactor */
								 nTerrainTex1,
								 nTerrainTex2,
								 (unsigned)(((float)(m_nLevel % 5) / 5) * 800),		/* nNoisePatches1 */
								 (unsigned)(((float)(m_nLevel % 10) / 10) * 3000)	/* nNoisePatches2 */
								 );
		assert( m_pTestRace );
		m_bRaceCreated = true ;

		/*EnableMenuItem(m_hMenu, ID_VIEW_OBJECTS_SKYDOME, m_pTestRace->m_pSky ? MF_ENABLED : MF_GRAYED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_OBJECTS_SKYDOME, m_pTestRace->m_pSky && m_pTestRace->m_bShowSky ? MF_CHECKED : MF_UNCHECKED) ;
		EnableMenuItem(m_hMenu, ID_VIEW_OBJECTS_TERRAIN2,  m_pTestRace->m_pTerrain ? MF_ENABLED : MF_GRAYED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_OBJECTS_TERRAIN2,  m_pTestRace->m_pTerrain && m_pTestRace->m_bShowTerrain ? MF_CHECKED : MF_UNCHECKED) ;
		EnableMenuItem(m_hMenu, ID_VIEW_OBJECTS_FIGHTER, m_pTestRace->m_pFighter ? MF_ENABLED : MF_GRAYED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_OBJECTS_FIGHTER, m_pTestRace->m_pFighter && m_pTestRace->m_bShowFighter ? MF_CHECKED : MF_UNCHECKED) ;
		EnableMenuItem(m_hMenu, ID_VIEW_OBJECTS_FLATGROUND, m_pTestRace->m_pOcean ? MF_ENABLED : MF_GRAYED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_OBJECTS_FLATGROUND, m_pTestRace->m_pOcean && m_pTestRace->m_bShowOcean ? MF_CHECKED : MF_UNCHECKED) ;
		
		CheckMenuItem(m_hMenu,  ID_VIEW_HUD, m_pTestRace->m_bShowHUD ? MF_CHECKED : MF_UNCHECKED) ;

		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_FREE, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_FREE ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_BEHINDFIXED1, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_BEHIND_FIXED1 ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_BEHINDFIXED2, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_BEHIND_FIXED2 ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_BEHINDFIXED2, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_BEHIND_FIXED3 ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_BEHINDVARIABLE1, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_BEHIND_VARIABLE1 ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_BEHINDVARIABLE2, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_BEHIND_VARIABLE2 ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_BEHINDVARIABLE2, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_BEHIND_VARIABLE3 ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_COCKPITFIXED, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_COCKPIT_FIXED ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_COCKPITROLL, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_COCKPIT_ROLL ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_CAMERA_SHOWOFF, m_pTestRace->m_pFighter && m_pTestRace->m_ctCamMode == CT_SHOWOFF ? MF_CHECKED : MF_UNCHECKED) ;

		CheckMenuItem(m_hMenu,  ID_VIEW_FIGHTER_NORMAL, m_pTestRace->m_pFighter && m_pTestRace->m_rtFighterMode == FRT_NORMAL ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_FIGHTER_CHROME, m_pTestRace->m_pFighter && m_pTestRace->m_rtFighterMode  == FRT_CHROME ? MF_CHECKED : MF_UNCHECKED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_FIGHTER_GLASS, m_pTestRace->m_pFighter && m_pTestRace->m_rtFighterMode  == FRT_GLASS ? MF_CHECKED : MF_UNCHECKED) ;*/

	//
	// Create D3D device-dependent render objects:
	//
		strcpy(m_szLoadingGameStatus, "m_pTextures->RestoreDeviceObjects") ;
		if (m_pTextures)
		{
			hr = m_pTextures->RestoreDeviceObjects(m_pD3D, m_pd3dDevice) ;
			assert( hr==S_OK );
		}

		strcpy(m_szLoadingGameStatus, "m_pTestRace->RestoreDeviceObjects") ;
		if (m_pTestRace)
		{
			hr = m_pTestRace->RestoreDeviceObjects(m_pD3D, m_pd3dDevice, fAspect, m_pOverlayMan, m_pTextures);
			assert( hr==S_OK );
		}


		m_bWaitingToStartGame = TRUE ;

		//m_bAppMustExit = TRUE ;

		return 1 ;
	}

/*
 * Dynamic level attributes.
 * Assumes an upper limit of MAX_LEV levels, undefined after that.
 */

	// not a very linear progression here at the moment!
	float Cx7::GetTimeForLevel( int lev )
	{
		float fMaxTime = 60 * 3 ;
		float fDec = (fMaxTime / (float)MAX_LEV) * 1000 * (1-(float)lev/(float)MAX_LEV) ;
		float fTime = ( fMaxTime - ( (float)((lev-1) * fDec) * (float)lev/(float)MAX_LEV ) );
		if( fTime < 0 ) fTime = 0 ;
		return fTime + 30 ;
	}

	void Cx7::GetOceanColourForLevel( int lev, D3DXCOLOR* pColour )
	{
		float r = (float)lev / ( (float)MAX_LEV * 0.3f );
		if( r > 1 ) r = 1 ;

		float g = (float)lev / ( (float)MAX_LEV * 0.6f );
		if( g > 1 ) g = 1 ;

		float b = (float)lev / ( (float)MAX_LEV * 0.9f );
		if( b > 1 ) b = 1 ;
		b = 1 - b ;

		pColour->r = r ;
		pColour->g = g ;
		pColour->b = b ;
		pColour->a = 0.25f + ( rnd() * 0.5f );
	}
	int Cx7::GetOceanTextureForLevel( int lev )
	{
		if( lev % 2 == 0 )
			return CAUSTIC_TEX_START ;
		else
			return CAUSTIC2_TEX_START ;
	}

// 1 in 3 is nightime
// 1 in 3 is dusk
// 1 in 3 is full bright
	e_LightingType Cx7::GetLightingAndSkyboxTexturesForLevel( int lev, D3DXCOLOR* pAmbient, int* nSkyboxFolder )
	{
		if( lev % 3 == 1 )
		{	// dusk
			int nSkyboxes = 1 ;
			*pAmbient = D3DXCOLOR( 0.25f,0.25f,0.25f,0 );
			*nSkyboxFolder = 1 ;
			return LT_DUSK ;
		}
		if( lev % 3 == 2 )
		{	// full bright
			int nSkyboxes = 1 ;
			*pAmbient = D3DXCOLOR( 0.5f,0.5f,0.5f,0 );
			*nSkyboxFolder = 1 ;
			return LT_DAY ;
		}
		else//( lev % 3 == 2 )
		{
			int nSkyboxes = 1 ;
			*pAmbient = D3DXCOLOR( 0.15f,0.15f,0.15f,0 );
			*nSkyboxFolder = 1 ;
			return LT_NIGHT ;
		}
	}

//
//
//
	void Cx7::GetTerrainTexturesForLevel( int lev, int* tex1, int* tex2 )
	{
		int nTerrainTextures = 18 ;

		{
			*tex1 = (int)( TERRAIN_TEX_START + rnd() * (float)(nTerrainTextures-1) );
			
			*tex2 = *tex1 ;
			while( *tex2 == *tex1 )
				*tex2 = (int)( TERRAIN_TEX_START + rnd() * (float)(nTerrainTextures-1) );
		}
	}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
	HRESULT Cx7::FrameMove()
	{
		FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height ;

#ifdef _DEBUG
	// DBG; maintain sourceblend and destblend debug cycles;
		if( m_pUserInput->m_bKB_Pressed[ DIK_Z ] )
			if( ++SB == 14 ) SB = D3DBLEND_ZERO ;
		if( m_pUserInput->m_bKB_Pressed[ DIK_X ] )
			if( ++DB == 14 ) DB = D3DBLEND_ZERO ;
#endif

	//
	// Wait for a keypress when we're ready to start level:
	//
		if( m_bWaitingToStartGame )
		{
			if( m_pUserInput->m_bAnyKey ||
				m_pUserInput->m_bMouseLB || m_pUserInput->m_bMouseCB || m_pUserInput->m_bMouseRB ||
				m_pCmdLine->GetInt( "AutoStart", 0 ) )
			{
				m_bWaitingToStartGame = FALSE ;
				m_bLoadingGame = FALSE ;
				m_bInGame = TRUE ;
				m_pUserInput->Init() ;
			}
		}

	//
	// Process track skipping:
	//
		if( m_pSound && m_pUserInput )
		{
			if( m_pUserInput->m_bKB_Pressed[ DIK_LEFT ] || m_pUserInput->m_bKB_Pressed[ DIK_NUMPAD4 ] /*&& 
			   (m_pUserInput->m_bKB_Cur[ DIK_LCONTROL ] || m_pUserInput->m_bKB_Cur[ DIK_RCONTROL ])*/ )
			   m_pSound->SongSkip( -1 );
			
			if( m_pUserInput->m_bKB_Pressed[ DIK_RIGHT ] || m_pUserInput->m_bKB_Pressed[ DIK_NUMPAD6 ] /*&& 
			   (m_pUserInput->m_bKB_Cur[ DIK_LCONTROL ] || m_pUserInput->m_bKB_Cur[ DIK_RCONTROL ])*/ )
			   m_pSound->SongSkip( +1 );

			m_pSound->FrameMoveSkip() ;
		}

	//
	// Process Level Transition:
	//
		if( m_bInLevelTransition )
		{
			m_bInLevelTransition = FALSE ;
			StartNextLevel() ;
			return S_OK ;
		}

	//
	// Process GameOver:
	//
		if( m_bPlayerJustLost )
		{
			m_bPlayerJustLost = FALSE ;

			m_nLevel = m_pCmdLine->GetInt( "Level", 1 );
			m_fScore = 0 ;
			m_nLives = 3 ;
			m_bInMainMenu = TRUE ;
			InvalidateDeviceObjects() ;
			m_bRaceCreated = false ;
			SAFE_DELETE( m_pTestRace )
			SAFE_DELETE( m_pTextures )
			RestoreDeviceObjects() ;
			return S_OK ;
		}

	//
	// Process kill flag:
	//
		if (m_bAppMustExit)
	        PostMessage( m_hWnd, WM_CLOSE, 0, 0 );

	//
	// Go fullscreen:
	//
		static BOOL bFirstFrameMove = TRUE ;
		if ( bFirstFrameMove && m_bActive && m_bReady )
		{
			bFirstFrameMove = FALSE ;
			if( m_pCmdLine->GetInt( "Fullscreen", 0 ) )
				ToggleFullscreen() ;
		}

	//
	// Poll (unbuffered) user-input devices:
	//
		if (m_pUserInput)
		{
			m_pUserInput->Poll( m_bInGame );
		}

	//
	// Overlays Lock + FrameMove
	//
		if (m_fFPS > MIN_ERROR_FPS)
			if (m_pOverlayMan)
			{
				m_pOverlayMan->LockVB() ;
				m_pOverlayMan->FrameMove( m_pd3dDevice, (60.0f / m_fFPS), fAspect );
			}
			
	//
	// Main Menu & InGame Menu
	//
		if (m_fFPS > MIN_ERROR_FPS)
		{
			if (m_pMainMenu && m_bInMainMenu)
				m_pMainMenu->FrameMove( m_pd3dDevice, m_fFPS, m_pOverlayMan, m_pUserInput, m_pSound, m_fElapsedTime, m_pCmdLine->GetInt( "AutoStart", 0 ) );
			
			if (m_pInGameMenu && m_bGamePaused)
				m_pInGameMenu->FrameMove( m_pd3dDevice, m_fFPS, m_pOverlayMan, m_pUserInput, m_pSound );
		}

	//
	// Check for InGame Menu enter/exit status
	//
		if (m_pUserInput->m_bKB_Pressed[DIK_ESCAPE] && m_bInGame && m_pInGameMenu)
		{
			m_bGamePaused = !m_bGamePaused ;
			if (m_bGamePaused)
			{
				m_pInGameMenu->EnterMenu() ;
				m_pTestRace->EnableHUDs( FALSE );
			}
			if (!m_bGamePaused)
			{
				m_pInGameMenu->ExitMenu() ;
				m_pTestRace->EnableHUDs( TRUE );
			}
		}

	//
	// Game
	//
		if (m_fFPS > MIN_PHYSICS_FPS)
			if (m_bReady && m_bInGame && !m_bGamePaused)
				if (m_pTestRace)
					m_pTestRace->FrameMove( m_pd3dDevice, m_pUserInput, m_fFPS );

	//
	// Overlays unlock
	//
		if (m_fFPS > MIN_ERROR_FPS)
			if (m_pOverlayMan)
				m_pOverlayMan->UnlockVB() ;

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------

	HRESULT Cx7::Render()
	{
		//static DWORD dwFramesInGame = 0 ;
		//static float fTimeInGameS = 0 ;

		m_ulPPF = 0 ;

	//
	// Wait for framerate to be available:
	//
		if (m_fFPS == 0.0f)
			return S_OK ;

	//
	// RenderStates we know and love:
	//
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID) ;
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) ;
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) ;
		m_pd3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1) ;
		m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, m_bFSAA_Enabled) ;

	//
	// Render:
	//
		if (SUCCEEDED(m_pd3dDevice->BeginScene()))
		{
			float y = 110, d = 10 ;
			char sz[256] ;

		// validate framerate
			if (m_fFPS < MIN_ERROR_FPS)
			{
				m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00ff0000, 1.0f, 0) ;
				sprintf(sz, "FRAMERATE ERROR") ;
				m_pSubtitleFont->DrawText(2, 0, D3DCOLOR_ARGB(255,255,255,255), sz) ;
				sprintf(sz, "Min = %0.1f, Actual = %0.9f", MIN_ERROR_FPS, m_fFPS) ;
				m_pSubtitleFont->DrawText(2, 15, D3DCOLOR_ARGB(255,180,180,180), sz) ;
				sprintf(sz, "If this doesn't go away, then please exit.") ;
				m_pSubtitleFont->DrawText(2, 30, D3DCOLOR_ARGB(255,180,180,180), sz) ;
				m_pd3dDevice->EndScene() ;
				return S_OK ;
			}

		// main menu
			if (m_bInMainMenu)
			{
				FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height ;
				D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI/4, fAspect, 0.01f, 1000.0f) ;
				m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj) ;
				m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0) ;
				m_pMainMenu->Render(m_pd3dDevice, m_pOverlayMan) ;
				
				static float fTime = 0.0f ;
				static int nKey = 0 ;
				if( fTime == 0.0f )
					SetKey( nKey==0?VK_NUMLOCK:
							nKey==1?VK_CAPITAL:VK_SCROLL, 1 );
				fTime += (1.0f/m_fFPS);
				if( fTime >= 0.1f )
				{
					SetKey( nKey==0?VK_NUMLOCK:
							nKey==1?VK_CAPITAL:VK_SCROLL, 0 );
					nKey++ ;
					fTime=0.0f;
					if(nKey==3)
						nKey=0;
				}
			}

		// loading
			if (m_bLoadingGame)
			{
				FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height ;
				D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI/4, fAspect, 0.01f, 1000.0f) ;
				m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj) ;
				m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0) ;

				sprintf( sz, "LEVEL %d", m_nLevel );
				m_pTitleFont->DrawText(20, 20, D3DCOLOR_ARGB(254,254,254,254), sz) ;

				if( m_bRaceCreated )
				{
					char szMillis[ 256 ], szTime[ 80 ];
					sprintf(szMillis, "%02.0f", (m_pTestRace->m_fGameTimeRemaining - floorf( m_pTestRace->m_fGameTimeRemaining )) * 100.0f );
					sprintf( szTime, "%01d:%02d.%c%c",
							(int)( floorf( m_pTestRace->m_fGameTimeRemaining / 60.0f ) ),
							(int)( fmodf( m_pTestRace->m_fGameTimeRemaining, 60.0f ) ), szMillis[ 0 ], szMillis[ 1 ] );
					sprintf( sz, "%ss / %d / %d / %d bots",
								szTime,
								m_pTestRace->m_pTerrain->m_nCherries,
								m_pTestRace->m_pTerrain->m_nEnginePwrUps,
								m_pTestRace->m_nBots );
					m_pTitleFont->DrawText(20, 100, D3DCOLOR_ARGB(254,180,180,180), sz) ;
				}

				if( m_bWaitingToStartGame )
				{
					m_pTitleFont->DrawText( 20, 150, D3DCOLOR_ARGB(255,255,255,255), "Click to start..." );
				}
				else
				{
					m_pSubtitleFont->DrawText( 20, 240, D3DCOLOR_ARGB(255,220,220,220), m_szLoadingGameStatus) ;
					m_pSubtitleFont->DrawText( 20, 260, D3DCOLOR_ARGB(255,200,200,200), m_szLoadingGameInfo1) ;
					m_pSubtitleFont->DrawText( 20, 280, D3DCOLOR_ARGB(255,180,180,180), m_szLoadingGameInfo2) ;
				}
			}

		// version & compile info
			if( m_bLoadingGame /*|| m_bInMainMenu*/ )
			{
				sprintf( sz, "Ver. %d.%02d %s -- %s Build -- VC%d %s %s", g_nAppVerMaj, g_nAppVerMin, g_szAppRelease, g_szAppBuild, _MSC_VER, __DATE__, __TIME__ );
				m_pSubtitleFont->DrawText(20, m_vpViewport.Height * 0.9f, D3DCOLOR_ARGB(255,160,160,160), sz) ;
			}

		// in game
			if (m_bInGame /*&& !m_bGamePaused*/)
			{
				//dwFramesInGame++ ;
				//fTimeInGameS += ( 1 / m_fFPS );

			// non T&L capable device workaround; (reports MaxActiveLights as > 0xffffff)
				int nLightsToUse = m_d3dCaps.MaxActiveLights > 0xff ? 8 : m_d3dCaps.MaxActiveLights ;

			// note; projection matrix is set per object in CRace
				float fAspect = m_d3dsdBackBuffer.Width / (float)m_d3dsdBackBuffer.Height ;
#ifdef _DEBUG
				m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00ffffff, 1.0f, 0x00 );
#else
				m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00ffffff, 1.0f, 0x00 );
#endif
				m_pTestRace->Render( m_pd3dDevice, m_pTextures, &m_ulPPF, fAspect, m_fFPS, nLightsToUse );
			// update performance counters
				if (m_fMinFPS == 0.0f) m_fMinFPS = m_fFPS ;
				if (m_fMaxFPS == 0.0f) m_fMaxFPS = m_fFPS ;
				if (m_fFPS < m_fMinFPS) m_fMinFPS = m_fFPS ;
				if (m_fFPS > m_fMaxFPS) m_fMaxFPS = m_fFPS ;
				m_fAvgFPS += m_fFPS ;
				m_fAvgFPS /= 2.0f ;

				if (m_ulMinPPF == 0.0f) m_ulMinPPF = m_ulPPF ;
				if (m_ulMaxPPF == 0.0f) m_ulMaxPPF = m_ulPPF ;
				if (m_ulPPF < m_ulMinPPF) m_ulMinPPF = m_ulPPF ;
				if (m_ulPPF > m_ulMaxPPF) m_ulMaxPPF = m_ulPPF ;
				m_ulAvgPPF += m_ulPPF ;
				m_ulAvgPPF /= 2UL ;

				m_fMTS = m_fFPS * (float)m_ulPPF / 1000000.0f ;
				if (m_fMinMTS == 0.0f) m_fMinMTS = m_fMTS ;
				if (m_fMaxMTS == 0.0f) m_fMaxMTS = m_fMTS ;
				if (m_fMTS < m_fMinMTS) m_fMinMTS = m_fMTS ;
				if (m_fMTS > m_fMaxMTS) m_fMaxMTS = m_fMTS ;
				m_fAvgMTS += m_fMTS ;
				m_fAvgMTS /= 2.0f ;

				//CProceduralRenderObject *pRO = m_pTestRace->m_pTerrain->m_pPatches[0][0]->m_pPatchLOD[0] ;

				//sprintf(sz, "%d %d", (int)pRO->m_csVis.ClipUnion & D3DCS_BOTTOM, (int)pRO->m_csVis.ClipIntersection & D3DCS_BOTTOM) ;
				//m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
				//for (int i = 0 ; i < 8 ; i ++)
				//{
				//	sprintf(sz, "i:%d x=%0.2f y=%0.2f z=%0.2f", i, (float)pRO->m_vP[i].x, (float)pRO->m_vP[i].y, (float)pRO->m_vP[i].z) ;
				//	m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
				//}

				if (m_bStats)
				{
					//sprintf(sz, "            PPF: %d (%0.2f MT/s)", (int)ulPolyCount, ((float)m_fHiResFPS * (float)ulPolyCount) / 1000000.0f) ;
					//m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					//y += d ;
					sprintf(sz, "Joystick: %d [%s] [%s]", m_pUserInput->m_bJoystickPresent, m_pUserInput->m_szJoystickProductName, m_pUserInput->m_szJoystickInstanceName) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, "         vEyePt: %0.1f, %0.1f, %0.1f", m_pTestRace->m_pCamera->m_vEyePt.x, m_pTestRace->m_pCamera->m_vEyePt.y, m_pTestRace->m_pCamera->m_vEyePt.z) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, "      vLookAtPt: %0.1f, %0.1f, %0.1f", m_pTestRace->m_pCamera->m_vLookAtPt.x, m_pTestRace->m_pCamera->m_vLookAtPt.y, m_pTestRace->m_pCamera->m_vLookAtPt.z) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, " m_vOrientation: %0.6f, %0.6f, %0.6f", m_pTestRace->m_pCamera->m_vOrientation.x, m_pTestRace->m_pCamera->m_vOrientation.y, m_pTestRace->m_pCamera->m_vOrientation.z) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					y += d ;

					sprintf(sz, "     Terrain BV: %0.1f, %0.1f, %0.1f -- %0.1f, %0.1f, %0.1f", (float)m_pTestRace->m_pTerrain->m_vMin.x, (float)m_pTestRace->m_pTerrain->m_vMin.y, (float)m_pTestRace->m_pTerrain->m_vMin.z, (float)m_pTestRace->m_pTerrain->m_vMax.x, (float)m_pTestRace->m_pTerrain->m_vMax.y, (float)m_pTestRace->m_pTerrain->m_vMax.z) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;

					sprintf(sz, "         Camera: [%d] MinY=%0.2f", m_pTestRace->m_bCameraAboveGround, m_pTestRace->m_fY) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, "        Terrain: %0.0f KB (%d[%d] patches, each @ %dx%d max. LOD)", (float)m_pTestRace->m_pTerrain->QueryMemUsage() / 1024.0f, m_pTestRace->m_pTerrain->m_nPatchesSQ * m_pTestRace->m_pTerrain->m_nPatchesSQ, m_pTestRace->m_pTerrain->m_nVisiblePatches, m_pTestRace->m_pTerrain->m_pPatches[0]->m_nPatchMaxLODSQ, m_pTestRace->m_pTerrain->m_pPatches[0]->m_nPatchMaxLODSQ) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, "      Heightmap: %d x %d [%0.2f unit size]", (int)m_pTestRace->m_pTerrain->m_nHeightmapSQ, (int)m_pTestRace->m_pTerrain->m_nHeightmapSQ, m_pTestRace->m_pTerrain->m_fUnitSize) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, " Vertices/Patch: %lu", (unsigned long)m_pTestRace->m_pTerrain->m_pPatches[0]->m_dwTotalNumVertices) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					sprintf(sz, "  Indices/Patch: %lu", (unsigned long)m_pTestRace->m_pTerrain->m_pPatches[0]->m_dwTotalNumIndices) ;
					m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					//sprintf(sz, "        CProcRO: %d bytes", (int)sizeof(CProceduralRenderObject)) ;
				    //m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					//sprintf(sz, "          CBBox: %d bytes", (int)sizeof(CBoundingBox)) ;
					//m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
					//y += d ;
					//sprintf(sz, "  OTNodes/Patch: %d [%d bytes/node] = %d KB", m_pTestRace->m_pTerrain->m_pPatches[0][0]->m_pPatchLOD[0]->m_pBBox->m_nOTNodes, (int)sizeof(CBoundingBox::_OTNode), sizeof(CBoundingBox::_OTNode) * m_pTestRace->m_pTerrain->m_pPatches[0][0]->m_pPatchLOD[0]->m_pBBox->m_nOTNodes / 1024) ;
					//m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
				}
			}

		// InGame menu
			if (m_bGamePaused && m_bInGame)
			{
				FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height ;
				D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI/4, fAspect, 0.01f, 1000.0f) ;
				m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj) ;
				m_pInGameMenu->Render(m_pd3dDevice, m_pOverlayMan) ;
			}

// frame stats;
			y = 0 ;

#ifdef _DEBUG
			/*sprintf( sz, "%03.0f %06lu %0.2f", m_fFPS, m_ulPPF, m_fMTS );
			m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/
			if( m_pTestRace )
			{
				/*sprintf(sz, "PSa: %d", m_pTestRace->m_pFighter->m_pPS_Contrails[0]->m_dwParticles );
				m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/

				/*sprintf(sz, "%lu/%lu", m_pTestRace->m_dwNumFightersDrawn, m_pTestRace->m_dwNumFightersPVS );
				m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/

				/*sprintf(sz, "%d %d %d %d", m_pTestRace->m_pFighter->m_bContrailVisible[0], m_pTestRace->m_pFighter->m_bContrailVisible[1], m_pTestRace->m_pFighter->m_bContrailVisible[2], m_pTestRace->m_pFighter->m_bContrailVisible[3] );
				m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/

				char szSB[256], szDB[256] ;
				D3DBlend2Str( m_pTestRace->SB, szSB );
				D3DBlend2Str( m_pTestRace->DB, szDB );
				sprintf( sz, "[SB=%s DB=%s]", szSB, szDB );
				m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				sprintf( sz, "weapon: %d", m_pTestRace->m_pFighter->m_Weapon );
				m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;
			}
#endif
			
			if( m_pCmdLine->GetInt("FPS", 0) )
			{
				sprintf(sz, "%03.0f %lu %0.1f", m_fFPS, m_ulPPF, m_fMTS) ;
				m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;
			}

			if (m_bInGame)
			{
				//sprintf( sz, "speed: %0.1f world units/s", m_pTestRace->m_pFighter->m_fSpeed * m_fFPS );
				//m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				/*sprintf( sz, "N=(%0.1f,%0.1f,%0.1f) ",
							m_pTestRace->m_pFighter->m_TCD_vN.x,
							m_pTestRace->m_pFighter->m_TCD_vN.y,
							m_pTestRace->m_pFighter->m_TCD_vN.z );
				m_pStatsFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				sprintf( sz, "A=(%0.1f,%0.1f,%0.1f) ",
							m_pTestRace->m_pFighter->m_TCD_vA.x,
							m_pTestRace->m_pFighter->m_TCD_vA.y,
							m_pTestRace->m_pFighter->m_TCD_vA.z );
				m_pStatsFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				xVec3 vnEF ;
				D3DXVec3Normalize( &vnEF, &m_pTestRace->m_pFighter->m_vExternalForce );
				sprintf( sz, "x=(%0.1f,%0.1f,%0.1f) ",
							vnEF.x,
							vnEF.y,
							vnEF.z );
				m_pStatsFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				sprintf( sz, "v=(%0.1f,%0.1f,%0.1f) ",
							m_pTestRace->m_pFighter->m_vDirectionOfTravel.x,
							m_pTestRace->m_pFighter->m_vDirectionOfTravel.y,
							m_pTestRace->m_pFighter->m_vDirectionOfTravel.z );
				m_pStatsFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/

				if( m_pTestRace->m_pFighter->m_bTerrainCollision )
					m_pStatsFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), "TC!"); y += d ;

				//sprintf( sz, "pitch=%f", m_pTestRace->m_pFighter->m_fPitch / g_PI );
				//m_pSubtitleFont->DrawText( 0, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				/*sprintf(sz, "%0.2f %0.2f, %0.2f", m_pTestRace->m_pFighter->m_vPosition.x, m_pTestRace->m_pFighter->m_vPosition.y, m_pTestRace->m_pFighter->m_vPosition.z ) ;
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/

				/*sprintf( sz, "%d %d", m_pTestRace->m_pFighter->SB, m_pTestRace->m_pFighter->DB );
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/

				/*sprintf(sz, "%0.2f, %0.1f %0.1f", m_pTestRace->m_pFighter->m_fShieldLevel, m_pTestRace->m_pFighter->m_fSpeed, m_pTestRace->m_pFighter->m_fThrustMag) ;
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/
				
				/*sprintf(sz, " %06lu %06lu m_fThrustMag: %0.2f vO: %0.7f, %0.7f, %0.7f",
					m_pTestRace->m_pFighter->m_pPS_Explosion != NULL
					? (unsigned long)m_pTestRace->m_pFighter->m_pPS_Explosion->m_dwParticles
					: 0,
					m_pTestRace->m_pFighter->m_pPS_Contrail != NULL
					? (unsigned long)m_pTestRace->m_pFighter->m_pPS_Contrail->m_dwParticles
					: 0,
					m_pTestRace->m_pFighter->m_fThrustMag,
					m_pTestRace->m_pFighter->m_vOrientation.x, m_pTestRace->m_pFighter->m_vOrientation.y, m_pTestRace->m_pFighter->m_vOrientation.z) ;
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;
				sprintf(sz, "m_vExternalForce: %0.7f vO: %0.7f, %0.7f, %0.7f", m_pTestRace->m_pFighter->m_vExternalForce.x, m_pTestRace->m_pFighter->m_vExternalForce.y, m_pTestRace->m_pFighter->m_vExternalForce.z) ;
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;*/
			}

			if (m_bStats)
			{
				char szBBFormat[256], szDepthBFormat[256] ;
				D3DFormat2Str(m_d3dsdBackBuffer.Format, szBBFormat) ;
				D3DFormat2Str(m_d3dpp.AutoDepthStencilFormat, szDepthBFormat) ;
			
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), m_strDeviceStats ); y += d ;

				sprintf( sz, "Multisample: %dx", m_d3dpp.MultiSampleType );
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				sprintf(sz, "%s - PPF: %lu [%0.2f MT/s]", m_strFrameStats, m_ulPPF, m_fMTS) ;
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				sprintf(sz, "FPS: %0.0f - %0.0f [Avg: %0.0f]", m_fMinFPS, m_fMaxFPS, m_fAvgFPS) ;
				m_pStatsFont->DrawText( 2, y, D3DCOLOR_ARGB(255,255,255,255), sz); y += d ;

				sprintf(sz, "[MaxActiveLights: 0x%x] [DirectionLights: %s] [MaxTexWidth: %d] [MaxTexHeight: %d]", (unsigned long)m_d3dCaps.MaxActiveLights, m_d3dCaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ? "yes" : "no", (int)m_d3dCaps.MaxTextureWidth, (int)m_d3dCaps.MaxTextureHeight );
				m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
				
				sprintf(sz, "[MaxPointSize: %0.1f] [BackBuffer: %s] [DepthBuffer: %s] [WBufOK: %s]", m_d3dCaps.MaxPointSize, szBBFormat, szDepthBFormat, m_d3dCaps.RasterCaps & D3DPRASTERCAPS_WBUFFER ? "yes" : "no") ;
				m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
				
				sprintf(sz, "[MaxSimulTex: %d] [MaxTexBlendStages: %d] [NoPasses4AlphaBlend2Tex: %d]", (int)m_d3dCaps.MaxSimultaneousTextures, (int)m_d3dCaps.MaxTextureBlendStages , m_dwNumPassesForAlphaBlend2Tex) ;
				m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
				
				sprintf(sz, "[MultiSampType: %d] [MinFLin: %s] [MaxFLin: %s] [MipFLin: %s]", (int)m_d3dpp.MultiSampleType, m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ? "yes" : "no", m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR ? "yes" : "no", m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR ? "yes" : "no") ;
				m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
				
				sprintf(sz, "[MaxPrimCount: 0x%X] [MaxVertexIndex: 0x%X] [MaxAnisotropy: %d]", m_d3dCaps.MaxPrimitiveCount, m_d3dCaps.MaxVertexIndex, (int)m_d3dCaps.MaxAnisotropy) ;
				m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;

				sprintf(sz, "[FSAASupport: %d] [FSAAEnabled: %d]", m_bFSAA_Supported, m_bFSAA_Enabled) ;
				m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,255,255,255), sz) ; y += d ;
			}
		
			//sprintf(sz, "%d/%d segs visible", m_pTestRace->m_pTrack->m_nVisibleSegs, m_pTestRace->m_pTrack->m_nSegs) ;
			//m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
			//if (m_pTestRace->m_pTerrain)
			//{
			//	sprintf(sz, "%d^2 patches @ max res. %d^2",
			//		PATCHES_SQ,
			//		PATCH_MAX_LOD_SQ) ;
			//	m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
			//	sprintf(sz, "%d patches visible [heightmap res.: %lu ^2]",
			//		m_pTestRace->m_pTerrain->m_nVisiblePatches,
			//		m_pTestRace->m_pTerrain->m_ulHeightmapSQ) ;
			//	m_pStatsFont->DrawText(2, y, D3DCOLOR_ARGB(255,0,255,0), sz) ; y += d ;
			//}

			//if (m_fFPS <= MIN_PHYSICS_FPS)
			//	m_pStatsFont->DrawText(0, 0, D3DCOLOR_ARGB(255,255,0,0), "m_fFPS <= MIN_PHYSICS_FPS") ;

			
			// DBG
			/*m_pTitleFont->SetRenderStateDM() ;
			m_pTitleFont->DrawTextScaledDM( -1, 0 - (0.25f), 0, 0.2f, 0.25f, D3DCOLOR_ARGB(128,255,0,0), "GAME OVER!", D3DFONT_FILTERED, &m_vpViewport );
			m_pTitleFont->ResetRenderStateDM() ;*/
			
		// sound manager info
			if( m_pSound )
			{
				m_pSound->Render( this, m_fFPS );
			}

			m_pd3dDevice->EndScene() ;
		}

	//
	// Get a debug screenshot:
	//
		static float fTimeSinceLastScreengrab = 0 ;
		fTimeSinceLastScreengrab += ( 1 / m_fFPS );
		if( ( fTimeSinceLastScreengrab > 30 &&
			  m_pCmdLine->GetInt( "AutoGrab", 0 ) &&
			  m_bInGame && !m_bGamePaused ) ||
			 m_pUserInput->m_bKB_Pressed[ DIK_F11 ] )
		{
			fTimeSinceLastScreengrab = 0 ;
			Screengrab() ;
		}

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
	HRESULT Cx7::InitDeviceObjects()
	{
		if (SHOW_FRAMEWORK_EVENTS)
			Msg("InitDeviceObjects()") ;

		m_pStatsFont->InitDeviceObjects(m_pd3dDevice) ;
		m_pTitleFont->InitDeviceObjects(m_pd3dDevice) ;
		m_pSubtitleFont->InitDeviceObjects(m_pd3dDevice) ;

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initializes device-dependent scene objects (VBs & IBs).
//-----------------------------------------------------------------------------
	HRESULT Cx7::RestoreDeviceObjects()
	{
		ods( "Cx7::RestoreDeviceObjects\n" );

		HRESULT hr ;
		m_fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height ;

		if (SHOW_FRAMEWORK_EVENTS)
			Msg("RestoreDeviceObjects()") ;

	//
	// Define the viewport:
	//
		m_vpViewport.Width  = (DWORD)(m_d3dsdBackBuffer.Width  * 1.0f) ;
		m_vpViewport.Height = (DWORD)(m_d3dsdBackBuffer.Height * 1.0f) ;
		m_vpViewport.X		= (DWORD)(m_d3dsdBackBuffer.Width  * 0.0f) ;
		m_vpViewport.Y		= (DWORD)(m_d3dsdBackBuffer.Height * 0.0f) ;
		m_vpViewport.MinZ	= 0 ;
		m_vpViewport.MaxZ	= 1 ;
		m_pd3dDevice->SetViewport(&m_vpViewport) ;

	//
	// (Re)create Device Objects (VBs & IBs for CRenderObject-based objects):
	//
		if (m_pStatsFont)
		{
			hr = m_pStatsFont->RestoreDeviceObjects() ;
			assert( hr==S_OK );
		}

		if (m_pTitleFont)
		{
			hr = m_pTitleFont->RestoreDeviceObjects() ;
			assert( hr==S_OK );
		}

		if (m_pSubtitleFont)
		{
			hr = m_pSubtitleFont->RestoreDeviceObjects() ;
			assert( hr==S_OK );
		}

	//
	// This seems to work better than a z-buffer:
	//
		if (m_d3dCaps.RasterCaps & D3DPRASTERCAPS_WBUFFER)
			m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW) ;

	//
	// Determine device multisample support:
	//
		m_bFSAA_Supported = ( m_d3dpp.MultiSampleType != D3DMULTISAMPLE_NONE );
		m_bFSAA_Enabled = m_bFSAA_Supported && m_nFSAA != 0 ; 
		EnableMenuItem(m_hMenu, ID_VIEW_FSAA, m_bFSAA_Supported ? MF_ENABLED : MF_GRAYED) ;
		CheckMenuItem(m_hMenu,  ID_VIEW_FSAA, m_bFSAA_Enabled ? MF_CHECKED : MF_UNCHECKED) ;

	//
	// Enable linear texture filtering:
	//
		if (m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
		{
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR) ;
			m_pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR) ;
		}
		if (m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
		{
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR) ;
			m_pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR) ;
		}
		if (m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		{
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR) ;
			m_pd3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR) ;
		}

	//
	// Menu Textures
	//
		hr = m_pMenuTex->RestoreDeviceObjects(m_pD3D, m_pd3dDevice) ;
		assert( hr==S_OK );

	//
	// Main Menu
	//
		if (m_pMainMenu)
			m_pMainMenu->RestoreDeviceObjects( m_pd3dDevice, m_fAspect, m_pOverlayMan, m_pMenuTex );

		if (m_bInMainMenu)
			m_pMainMenu->EnterMenu() ;

	//
	// InGame Menu
	//
		if (m_pInGameMenu)
			m_pInGameMenu->RestoreDeviceObjects( m_pd3dDevice, m_fAspect, m_pOverlayMan, m_pMenuTex );

	//
	// Overlay Manager
	//
		if (m_pOverlayMan)
			m_pOverlayMan->RestoreDeviceObjects( m_pd3dDevice, m_fAspect );
		//m_pOverlayMan->LockVB() ;

		if( !m_bLoadingGame || m_bWaitingToStartGame )
		{
			if (m_pTextures)
			{
				hr = m_pTextures->RestoreDeviceObjects(m_pD3D, m_pd3dDevice) ;
				assert( hr==S_OK );
			}

			if (m_pTestRace)
			{
				hr = m_pTestRace->RestoreDeviceObjects(m_pD3D, m_pd3dDevice, m_fAspect, m_pOverlayMan, m_pTextures) ;
				assert( hr==S_OK );
			}
		}

		//m_pOverlayMan->UnlockVB() ;

	//
	// Setup the texture stages 1 and 2 for single-pass alpha-vertex blend multitexturing, and validate card support
	//
		/*m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1) ;
		SetTextureColorStage(m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1,			D3DTA_CURRENT)
		SetTextureColorStage(m_pd3dDevice, 1, D3DTA_TEXTURE, D3DTOP_BLENDDIFFUSEALPHA,	D3DTA_CURRENT)
		SetTextureColorStage(m_pd3dDevice, 2, D3DTA_DIFFUSE, D3DTOP_MODULATE,			D3DTA_CURRENT)
		SetTextureAlphaStage(m_pd3dDevice, 0, D3DTA_DIFFUSE, D3DTOP_SELECTARG1,			D3DTA_CURRENT)
		HRESULT hr = m_pd3dDevice->ValidateDevice(&m_dwNumPassesForAlphaBlend2Tex) ;*/
		//if (hr != D3D_OK)	// release notes say DD_OK returned on success for Win9x! :o
		{					// undefined HR returned on GeForce2??
			/*if (hr == D3DERR_CONFLICTINGTEXTUREFILTER)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_CONFLICTINGTEXTUREFILTER]") ;
			else if (hr == D3DERR_CONFLICTINGTEXTUREPALETTE)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_CONFLICTINGTEXTUREPALETTE]") ;
			else if (hr == D3DERR_TOOMANYOPERATIONS)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_TOOMANYOPERATIONS]") ;
			else if (hr == D3DERR_UNSUPPORTEDALPHAARG)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_UNSUPPORTEDALPHAARG]") ;
			else if (hr == D3DERR_UNSUPPORTEDALPHAOPERATION)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_UNSUPPORTEDALPHAOPERATION]") ;
			else if (hr == D3DERR_UNSUPPORTEDCOLORARG)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_UNSUPPORTEDCOLORARG]") ;
			else if (hr == D3DERR_UNSUPPORTEDCOLOROPERATION)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_UNSUPPORTEDCOLOROPERATION]") ;
			else if (hr == D3DERR_UNSUPPORTEDFACTORVALUE)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_UNSUPPORTEDFACTORVALUE]") ;
			else if (hr == D3DERR_UNSUPPORTEDTEXTUREFILTER)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_UNSUPPORTEDTEXTUREFILTER]") ;
			else if (hr == D3DERR_WRONGTEXTUREFORMAT)
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [D3DERR_WRONGTEXTUREFORMAT]") ;
			else
				Msg("WARN: CX7::RestoreDeviceObjects(): ValidateDevice != D3D_OK [UNDEFINED FAILURE]") ;*/
		}

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys device-dependent scene objects (VBs & IBs).
//-----------------------------------------------------------------------------
	HRESULT Cx7::InvalidateDeviceObjects()
	{
		if (SHOW_FRAMEWORK_EVENTS)
			Msg("InvalidateDeviceObjects()") ;

		/*while (ThreadIsRunning())
			;*/

		if (m_pTestRace)
			m_pTestRace->InvalidateDeviceObjects( m_pd3dDevice );

		if (m_pSubtitleFont)
			m_pSubtitleFont->InvalidateDeviceObjects() ;

		if (m_pStatsFont)
			m_pStatsFont->InvalidateDeviceObjects() ;

		if (m_pTitleFont)
			m_pTitleFont->InvalidateDeviceObjects() ;

		if (m_pTextures)
			m_pTextures->InvalidateDeviceObjects() ;

		if (m_pMenuTex)
			m_pMenuTex->InvalidateDeviceObjects() ;

		if (m_pOverlayMan)
			m_pOverlayMan->InvalidateDeviceObjects() ;

		if (m_pMainMenu)
			m_pMainMenu->InvalidateDeviceObjects();

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
	HRESULT Cx7::DeleteDeviceObjects()
	{
		/*while (ThreadIsRunning())
			;*/

		m_pStatsFont->DeleteDeviceObjects() ;
		m_pTitleFont->DeleteDeviceObjects() ;
		m_pSubtitleFont->DeleteDeviceObjects() ;

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
//-----------------------------------------------------------------------------
	HRESULT Cx7::FinalCleanup()
	{
		if( m_fAvgFPS > 0.0f && m_pCmdLine->GetInt( "AvgStats", 0 ) )
		{
			char sz[256] ;
			sprintf(sz, "Min FPS: %0.2f\nMax FPS: %0.2f\nAvg FPS: %0.2f\n\nMin MTS: %0.2f\nMax MTS: %0.2f\nAvg MTS: %0.2f\n\nMin PPF: %lu\nMax PPF: %lu\nAvg PPF: %lu\n",
					m_fMinFPS,  m_fMaxFPS,  m_fAvgFPS,
					m_fMinMTS,  m_fMaxMTS,  m_fAvgMTS,
					m_ulMinPPF, m_ulMaxPPF, m_ulAvgPPF) ;
			MessageBox(NULL, sz, "", MB_OK) ;	
		}

		if (SHOW_FRAMEWORK_EVENTS)
			Msg( "FinalCleanup()" );

		SAFE_DELETE( m_pOverlayMan )
		SAFE_DELETE( m_pMainMenu ) 
		SAFE_DELETE( m_pInGameMenu ) 
		SAFE_DELETE( m_pMenuTex )
		SAFE_DELETE( m_pStatsFont )
		SAFE_DELETE( m_pTitleFont )
		SAFE_DELETE( m_pSubtitleFont )
		m_bRaceCreated = false ;
		SAFE_DELETE( m_pTestRace )
		SAFE_DELETE( m_pTextures )
		SAFE_DELETE( m_pUserInput )
		SAFE_DELETE( m_pSound )

		return S_OK ;
	}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
	LRESULT Cx7::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		HDROP hDrop ;
		char szDropFile0[ 1024 ];

		switch (uMsg) 
		{ 
			case WM_DROPFILES:
				hDrop = (HDROP)wParam ;
				DragQueryFile( hDrop, 0, szDropFile0, 1024 );
				Msg2(" got hdrop;\nfile='%s'", szDropFile0);
				DragFinish( hDrop );
				break ;

			case WM_COMMAND: 
				switch(LOWORD(wParam)) 
				{
					case ID_VIEW_OBJECTS_SKYDOME:
						if (m_pTestRace)
						{
							m_pTestRace->m_bShowSky = !m_pTestRace->m_bShowSky ;
							CheckMenuItem(m_hMenu, ID_VIEW_OBJECTS_SKYDOME, m_pTestRace->m_bShowSky ? MF_CHECKED : MF_UNCHECKED) ;
						}
						break ;
					case ID_VIEW_OBJECTS_TERRAIN2:
						if (m_pTestRace)
						{
							m_pTestRace->m_bShowTerrain = !m_pTestRace->m_bShowTerrain ;
							CheckMenuItem(m_hMenu, ID_VIEW_OBJECTS_TERRAIN2, m_pTestRace->m_bShowTerrain ? MF_CHECKED : MF_UNCHECKED) ;
						}
						break ;
					case ID_VIEW_OBJECTS_FLATGROUND: 
						if (m_pTestRace)
						{
							m_pTestRace->m_bShowOcean = !m_pTestRace->m_bShowOcean ;
							CheckMenuItem(m_hMenu, ID_VIEW_OBJECTS_FLATGROUND, m_pTestRace->m_bShowOcean ? MF_CHECKED : MF_UNCHECKED) ;
						}
						break ;
					case ID_VIEW_OBJECTS_FIGHTER:
						if (m_pTestRace)
						{
							m_pTestRace->m_bShowFighter = !m_pTestRace->m_bShowFighter ;
							CheckMenuItem(m_hMenu, ID_VIEW_OBJECTS_FIGHTER, m_pTestRace->m_bShowFighter ? MF_CHECKED : MF_UNCHECKED) ;
						}
						break ;
					case ID_VIEW_WIREFRAME:
						m_bWireframe = !m_bWireframe ;
						CheckMenuItem(m_hMenu, ID_VIEW_WIREFRAME, m_bWireframe ? MF_CHECKED : MF_UNCHECKED) ;
						break ;

					case ID_VIEW_HUD:
						m_pTestRace->m_bShowHUD = !m_pTestRace->m_bShowHUD ;
						CheckMenuItem(m_hMenu, ID_VIEW_HUD, m_pTestRace->m_bShowHUD ? MF_CHECKED : MF_UNCHECKED) ;
						break ;

					case ID_VIEW_CAMERA_FREE:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_FREE ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
					// need to reset camera's internal mouse axes!
						if (m_pTestRace)
							m_pTestRace->m_pCamera->SetManualFromValues(m_pd3dDevice, m_pTestRace->m_pCamera->m_vEyePt.x, m_pTestRace->m_pCamera->m_vEyePt.y, m_pTestRace->m_pCamera->m_vEyePt.z) ;
						break ;
					case ID_VIEW_CAMERA_BEHINDFIXED1:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_BEHIND_FIXED1 ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_BEHINDFIXED2:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_BEHIND_FIXED2 ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_BEHINDFIXED3:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_BEHIND_FIXED3 ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_BEHINDVARIABLE1:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_BEHIND_VARIABLE1 ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_BEHINDVARIABLE2:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_BEHIND_VARIABLE2 ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_BEHINDVARIABLE3:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_BEHIND_VARIABLE3 ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_COCKPITFIXED:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_COCKPIT_FIXED ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_COCKPITROLL:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_COCKPIT_ROLL ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_CAMERA_SHOWOFF:
						if (m_pTestRace)
							m_pTestRace->m_ctCamMode = CT_SHOWOFF ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_FREE, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED1, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDFIXED3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE1, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE2, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_BEHINDVARIABLE3, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITFIXED, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_COCKPITROLL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_CAMERA_SHOWOFF, MF_CHECKED) ;
						break ;

					case ID_VIEW_FIGHTER_NORMAL:
						if (m_pTestRace)
							m_pTestRace->m_pFighter->m_rtFighterMode = FRT_NORMAL ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_NORMAL, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_CHROME, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_GLASS, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_VERTEXSHADER1, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_FIGHTER_CHROME:
						if (m_pTestRace)
							m_pTestRace->m_pFighter->m_rtFighterMode = FRT_CHROME ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_CHROME, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_NORMAL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_GLASS, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_VERTEXSHADER1, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_FIGHTER_GLASS:
						if (m_pTestRace)
							m_pTestRace->m_pFighter->m_rtFighterMode = FRT_GLASS ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_GLASS, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_NORMAL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_CHROME, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_VERTEXSHADER1, MF_UNCHECKED) ;
						break ;
					case ID_VIEW_FIGHTER_VERTEXSHADER1:
						if( m_pTestRace )
							m_pTestRace->m_pFighter->m_rtFighterMode = FRT_VS1 ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_VERTEXSHADER1, MF_CHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_GLASS, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_NORMAL, MF_UNCHECKED) ;
						CheckMenuItem(m_hMenu, ID_VIEW_FIGHTER_CHROME, MF_UNCHECKED) ;
						break ;

					case ID_VIEW_STATS:
						m_bStats = !m_bStats ;
						CheckMenuItem(m_hMenu, ID_VIEW_STATS, m_bStats ? MF_CHECKED : MF_UNCHECKED) ;
						break ;
					case ID_VIEW_FSAA:
						m_bFSAA_Enabled = !m_bFSAA_Enabled ;
						CheckMenuItem(m_hMenu, ID_VIEW_FSAA, m_bFSAA_Enabled ? MF_CHECKED : MF_UNCHECKED) ;
						break ;
				}
		}
		return CD3DApplication::MsgProc(hWnd, uMsg, wParam, lParam) ;
	}

	HRESULT Cx7::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
	{
	//
	// Demand a mixed-VP device:
	//
		/*if(!(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING))
			return E_FAIL ;*/

	//
	// Demand a vertex-shader capable device:
	//
		if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) || (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
		{
			if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,0) )
				return E_FAIL;
		}

		return S_OK;
	}

	HRESULT Cx7::Screengrab()
	{
		HRESULT hr ;
		LPDIRECT3DSURFACE8 pFrontBuf ;
		D3DDISPLAYMODE DispMode ;
		char szPath[ 1024 ], szFullFilename [ 1024 + 512 ];
		time_t t ;

		hr = m_pd3dDevice->GetDisplayMode( &DispMode );
		assert( D3D_OK==hr );

		hr = m_pd3dDevice->CreateImageSurface( DispMode.Width,
									 		   DispMode.Height,
											   D3DFMT_A8R8G8B8,
											   &pFrontBuf );
		assert( D3D_OK==hr );

		hr = m_pd3dDevice->GetFrontBuffer( pFrontBuf );
		assert( D3D_OK==hr );

		time( &t );
		//_getcwd( szPath, 1024 );
		strcpy( szPath, "..\\images" );
		sprintf( szFullFilename, "%s\\Screenshot_%lu.BMP", szPath, (unsigned long)t );
		hr = D3DXSaveSurfaceToFile( szFullFilename, D3DXIFF_BMP, pFrontBuf, NULL, NULL );
		assert( D3D_OK==hr );

		pFrontBuf->Release() ;
		return S_OK ;
	}

	//void Cx7::test(__declspec(align(16)) xMat* pTest) {} ;
