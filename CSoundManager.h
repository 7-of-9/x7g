#ifndef __CSoundManager__
#define __CSoundManager__

class CSoundManager ;

#include <io.h>
#include "x7.h"
#include "fmod.h"
#include "resource.h"
	
#define USE_SOUND_SYSTEM

#define MAX_FILES	256
#define MAX_SFX		1024
#define CHANNELS	64

enum e_SFXs
{
	SFX_MENU_MOVE = 0,
	SFX_MENU_SELECT,
	SFX_LASER1,
	SFX_EXP1,
	SFX_EXP2,
	SFX_EXP3,
	SFX_BULLET1,
	SFX_SHIELDHIT1,
	SFX_EXCELLENT,
	SFX_IMPRESSIVE,
	SFX_HUMILIATION,
	SFX_LASER2,
	NO_SFXs
} ;

class CSoundManager
{
public:
//
// Construction / Destruction:
//
	CSoundManager( char* szPath, int nMusic, int nSFX );	// both 0-255, both 0 to disable sound system.
	~CSoundManager() ;
	BOOL			m_bUseSoundSystem ;
	BOOL			m_bFMOD_OK ;

//
// Resource Unpacking:
//
	FSOUND_SAMPLE* CSoundManager::LoadFMODSampleFromResource( int nSamplePoolIndex, int nResID, char* strResType );
	FSOUND_STREAM* CSoundManager::LoadFMODStreamFromResource( int nResID, char* strResType );

//
// Music:
//
	bool			m_bMusic ;
	float			m_fMusicMasterVol ;
	float			m_fTrackLengthMS ;
	float			m_fTrackBirthday ;
	FSOUND_STREAM*	m_pfssMusicStream ;
	BOOL			m_bMusic_Playing ;
	int				m_nMusicFiles ;
	int				m_ndxCurMusicFile ;
	char*			m_pszMusicFiles[ MAX_FILES ];
	char*			m_pszMusicFilesShort[ MAX_FILES ];
	int				m_nMusicChannel ;

	int SongSkip( int nSkip );

//
// SFX:
//
	bool			m_bSFX ;
	float			m_fSFXMasterVol ;
	FSOUND_SAMPLE*	m_pfssSFX[ MAX_SFX ];
	int				m_nSFX ;
	
	int  PlaySFX( int SFX );
	int  PlaySFX3D( int SFX, xVec3* pvPos, xVec3* pvVel, float fVol );
	void UpdateSFX3D( int c, xVec3* pvPos, xVec3* pvVel, float fVol );

//
// FMOD 3D per-frame update:
//
	void FrameMove3D( xVec3* pvPos, xVec3* pvVel, xVec3* pvForward, xVec3* pvUp );

//
// AutoSkips to next track:
//
	void FrameMoveSkip(  );

//
// Render:
//
	HRESULT Render( Cx7* pX7, float fFPS );

private:
	char  m_szInfo[ 1024 ];
	float m_fInfoLifetime ;
	float m_fInfoAge ;
	bool  m_bScrollFromLeft ;

	void GetMusicFilesInFolder( char *szPath );
} ;

#endif