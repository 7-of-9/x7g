#pragma warning( disable : 4183 )	// 'missing return type'

#include "CSoundManager.h"

CSoundManager::CSoundManager( char* szPath, int nMusic, int nSFX )
{
	char szFile[ 256 ];

//
// Initialize members:
//
	m_bFMOD_OK = FALSE ;

	m_bMusic = ( nMusic > 0 );
	m_fMusicMasterVol = (float)nMusic / 255 ;
	m_pfssMusicStream = NULL ;
	m_bMusic_Playing = FALSE ;
	m_nMusicFiles = 0 ;
	m_ndxCurMusicFile = 0 ;

	m_bSFX = ( nSFX > 0 );
	m_fSFXMasterVol = (float)nSFX / 255 ;
	m_nSFX = 0 ;
	for (int i = 0 ; i < MAX_SFX ; i ++)
		m_pfssSFX[ i ] = NULL;

	m_bUseSoundSystem = ( m_bMusic || m_bSFX );

#ifndef USE_SOUND_SYSTEM
	return ;
#endif
	if( !m_bUseSoundSystem )
		return ;

//
// Initialize FMOD:
//
	FSOUND_SetDriver(0) ;								// Default soundcard
	FSOUND_SetMinHardwareChannels( CHANNELS );			// This no. of channels will always be available, even on hardware without this many channels! (it will fall back to software for all of them)
	if (FSOUND_Init(44100, CHANNELS, 0) == FALSE)		// ask for some SW channels in addition to the hardware's channels
	// maybe no soundcard?!
		return ;

	m_bFMOD_OK = TRUE ;

	FSOUND_SetSFXMasterVolume( 255 );

//
// Dbg diags:
//
	ods( "FSOUND_GetMaxChannels: %d\n", FSOUND_GetMaxChannels() );

//
// Get list of music files, and start off the first one:
//
	m_nMusicFiles = 0 ;
	m_ndxCurMusicFile = 0 ;
	if( m_bMusic )
	{
		if( szPath )
			GetMusicFilesInFolder( szPath );
		else
			m_bMusic = false ;

		if( m_nMusicFiles > 0 )
			SongSkip( (int)( rnd() * (float)m_nMusicFiles ) );
		else
		{
			m_bScrollFromLeft = false ;
			strcpy( m_szInfo, "*** NO MP3s FOUND IN SPECIFIED FOLDER ***" );
			m_fInfoLifetime = (float)strlen( m_szInfo ) * 0.08f ;
			m_fInfoAge = 0 ;
		}
	}

//
// Load all SFX samples (search order is MP3 then WAV)
//
	m_nSFX = 0 ;
	if( m_bSFX )
	{
		m_nSFX = NO_SFXs ;
		for( i = 0 ; i < m_nSFX ; i++ )
		{
			sprintf( szFile, "..\\resource\\wav1\\%03d.MP3", i+1 );
			m_pfssSFX[ i ] = FSOUND_Sample_Load( i, szFile, FSOUND_HW3D, 0 );
			if( !m_pfssSFX[ i ] )
			{
				sprintf( szFile, "..\\resource\\wav1\\%03d.WAV", i+1 );
				m_pfssSFX[ i ] = FSOUND_Sample_Load( i, szFile, FSOUND_HW3D, 0 );
				if( !m_pfssSFX[ i ] )
				{
					Msg( "Potential User-Error:\n\nCSoundManager::CSoundManager() - FSOUND_Sample_Load(szFile...) == NULL" );
					return ;
				}
			}
		}
	}
}

CSoundManager::~CSoundManager()
{
#ifndef USE_SOUND_SYSTEM
	return ;
#endif 
	if( !m_bUseSoundSystem ) return ;

// stop current song;
	if( m_pfssMusicStream )
	{
		if( m_bMusic_Playing ) FSOUND_Stream_Stop( m_pfssMusicStream );
		FSOUND_Stream_Close( m_pfssMusicStream );
	}

// stop all sound fx;
	for( int i=0 ; i<m_nSFX ; i++ )
		if( m_pfssSFX[i] ) FSOUND_Sample_Free( m_pfssSFX[i] );

// deallocate track list;
	for( i=0 ; i<m_nMusicFiles ; i++ )
	{
		free( m_pszMusicFiles[i] );
		free( m_pszMusicFilesShort[i] );
	}

	FSOUND_Close() ;
}

/*
 * Changes the current song, -1 for previous, +1 for next.
 */
int CSoundManager::SongSkip( int nSkip )
{
#ifndef USE_SOUND_SYSTEM
	return -1 ;
#endif 
	if( !m_bUseSoundSystem ) return -1 ;
	if( !m_bMusic ) 		 return -1 ;

// sanity check;
	if( 0==m_nMusicFiles )
		return -1 ;

// skip track index;
	m_ndxCurMusicFile += nSkip ;
	if( m_ndxCurMusicFile > m_nMusicFiles-1 ) m_ndxCurMusicFile = 0 ;
	if( m_ndxCurMusicFile < 0 )				  m_ndxCurMusicFile = m_nMusicFiles-1 ;

// stop current track;
	if( m_pfssMusicStream )
	{
		if( m_bMusic_Playing ) FSOUND_Stream_Stop( m_pfssMusicStream );
		FSOUND_Stream_Close( m_pfssMusicStream );
	}

	m_fTrackBirthday = DXUtil_Timer( TIMER_GETABSOLUTETIME );

// start new track on loop;
	m_pfssMusicStream = FSOUND_Stream_OpenFile(m_pszMusicFiles[m_ndxCurMusicFile], FSOUND_2D | FSOUND_LOOP_NORMAL, 0) ;
	if( m_pfssMusicStream )
	{
		m_nMusicChannel = FSOUND_Stream_Play( FSOUND_FREE, m_pfssMusicStream );
		if( m_nMusicChannel != -1 )
		{
			FSOUND_SetVolume( m_nMusicChannel, (int)( m_fMusicMasterVol * 255 ) );
			m_bMusic_Playing = TRUE ;
		}
	}
	if( m_bMusic_Playing )
		m_fTrackLengthMS = (float)FSOUND_Stream_GetLengthMs( m_pfssMusicStream ) ;
	else
		m_fTrackLengthMS = 1000 * 5 ;	// track may have failed to load, give it 5 seconds of silence before autoskipping.

// setup info string;
	if( nSkip >= 0 )
		m_bScrollFromLeft = false ;
	else
		m_bScrollFromLeft = true ;

	if( m_bScrollFromLeft )
	{
		sprintf( m_szInfo, "%s (%d/%d) >>>",
				 m_pszMusicFilesShort[ m_ndxCurMusicFile ],
				 m_ndxCurMusicFile + 1,
				 m_nMusicFiles );
	}

	else
	{
		sprintf( m_szInfo, "<<< (%d/%d) %s", 
				 m_ndxCurMusicFile + 1,
				 m_nMusicFiles,
				 m_pszMusicFilesShort[ m_ndxCurMusicFile ] );
	}
	m_fInfoLifetime = (float)strlen( m_szInfo ) * 0.07f ;
	m_fInfoAge = 0 ;

	PlaySFX( SFX_MENU_MOVE );
	return m_nMusicChannel ;
}

/*
 * Renders track filenames
 */
HRESULT CSoundManager::Render( Cx7* pX7, float fFPS )
{
	if( 0==m_fInfoLifetime )
		return S_OK ;
	
// advance age;
	m_fInfoAge += ( 1 / fFPS );
	if( m_fInfoAge >= m_fInfoLifetime )
	{
		m_fInfoLifetime = 0 ;
		return S_OK ;
	}

// font hack!
	float x = 0.75f ;

// render;
	float	  i = m_fInfoAge / m_fInfoLifetime ;
	D3DXCOLOR c( 1,1,1, 1 - ( i * 1 ) );
	float	  fXScale = 0.1f ;
	float	  fYScale = 0.1f ;
	float	  fLenScreenX = strlen( m_szInfo ) * ( fXScale / 2 );
	float	  fAmountToScroll = fLenScreenX + 1 ;
	float	  fY = 0 ;
	float	  fX ;
	if( m_bScrollFromLeft )
		fX = (-fLenScreenX * x) + (fAmountToScroll * i) ;
	else
		fX = 1 - ( fAmountToScroll * i );

	pX7->m_pTitleFont->SetRenderStateDM() ;
	pX7->m_pTitleFont->DrawTextScaledDM( (fX*2)-1, (fY*2)-1, 0, fXScale, fYScale, c, m_szInfo, D3DFONT_FILTERED, &pX7->m_vpViewport );
	pX7->m_pTitleFont->ResetRenderStateDM() ;

	return S_OK ;
}

/*
 * Use with caution, not very compatible with 3D sounds.
 *
 * Behaviour is for samples loaded with HW3D (i.e. all of em), that the current listener pos. and vel. is assigned
 * to the channel. As the listener moves, the sound diminishes. Not a problem on the opening menu screens, but 
 * is a problem in-game.
 *
 * TODO: split samples into two folders, one to be loaded as HW3D (in-game sound FX)
 * another for non-3D FX (i.e. menu stuff).
 *
 */
int CSoundManager::PlaySFX( int SFX )
{
#ifndef USE_SOUND_SYSTEM
	return -1 ;
#endif 
	if( !m_bUseSoundSystem ) return -1 ;
	if( !m_bSFX ) 			 return -1 ;

	int c = FSOUND_PlaySound( FSOUND_FREE, m_pfssSFX[ SFX ] );
	if( c != -1 )
	{
		FSOUND_SetVolume( c, (int)( m_fSFXMasterVol * 255 ) );
		return c ;
	}
	else
	{
		ods( "ERROR: PlaySFX() - Couldn't play a sample.\n" );
		return -1 ;
	}
}

/*
 * pvPos is the listener's position.
 * pvVel is the listener's veloctiy in m/s. Set to NULL for no Doppler effects.
 * fVol should range 0-1.
 *
 * Returns the channel that it's being played on, or -1 on failure.
 */
int CSoundManager::PlaySFX3D( int SFX, xVec3* pvPos, xVec3* pvVel, float fVol )
{
#ifndef USE_SOUND_SYSTEM
	return -1 ;
#endif 
	if( !m_bUseSoundSystem ) return -1 ;
	if( !m_bSFX )			 return -1 ;

	int c = FSOUND_PlaySound( FSOUND_FREE, m_pfssSFX[ SFX ] );
	if( c != -1 )
	{
		float fEffectiveVol = m_fSFXMasterVol * fVol ;
		FSOUND_SetVolume( c, (int)(255 * fEffectiveVol) );
		FSOUND_3D_SetAttributes( c, (float*)pvPos, (float*)pvVel );
		return c ;
	}
	else
	{
		ods( "ERROR: PlaySFX3D() - Couldn't play a sample.\n" );
		return -1 ;
	}
}

/*
 * Updats a HW channel's attributes.
 * Same params as above.
 */
void CSoundManager::UpdateSFX3D( int c, xVec3* pvPos, xVec3* pvVel, float fVol )
{
	FSOUND_3D_SetAttributes( c, (float*)pvPos, (float*)pvVel );
}

/*
 * pvPos is the listener's position.
 * pvVel is the listener's veloctiy in m/s. Set to NULL for no Doppler effects.
 * pvForward is the listener's unit forward vector.
 * pvUp is the listener's unit up vector.
 */
void CSoundManager::FrameMove3D( xVec3* pvPos, xVec3* pvVel, xVec3* pvForward, xVec3* pvUp )
{
// update 3D stuff
	FSOUND_3D_Listener_SetAttributes( (float*)pvPos,
									  (float*)pvVel,
									  pvForward->x, pvForward->y, pvForward->z,
									  pvUp->x, pvUp->y, pvUp->z
									  );
	FSOUND_3D_Listener_SetDistanceFactor( 0.1f );	// world-space units per meter
	FSOUND_3D_Listener_SetRolloffFactor( 0.01f );	// 0 - 10
	FSOUND_3D_SetAttributes( m_nMusicChannel, (float*)pvPos, (float*)pvVel );
	FSOUND_3D_Update() ;

}

void CSoundManager::FrameMoveSkip(  )
{
// autoskip to the next track, when the current one is finished.
	float fToday = DXUtil_Timer( TIMER_GETABSOLUTETIME );
	if( (fToday - m_fTrackBirthday) * 1000 > m_fTrackLengthMS )
		SongSkip( +1 );
}

void CSoundManager::GetMusicFilesInFolder( char *szPath )
{
	char *szEXTs[] = { "*.MP3", '\x0' } ;
	char sz[1024], szFullPath[1024], szSafePath[1024] ;
	int i = 0, nSearchResult ;
	long lSearchHandle ;
	_finddata_t fd ;
	
	m_nMusicFiles = 0 ;
	strcpy( szSafePath, szPath );
	if( szSafePath[ strlen(szSafePath)-1 ] == '\\' )
		szSafePath[ strlen(szSafePath)-1 ] = '\x0' ;

	while (szEXTs[i])
	{
		sprintf(szFullPath, "%s\\%s", szSafePath, szEXTs[i]) ;
		lSearchHandle = _findfirst(szFullPath, &fd) ;
		nSearchResult = lSearchHandle; 
		while (nSearchResult != -1)
		{
			sprintf(sz, "%s\\%s", szPath, fd.name) ;
			
			m_pszMusicFiles[ m_nMusicFiles ] = (char*)malloc( strlen(sz) + 1 );
			assert( m_pszMusicFiles[ m_nMusicFiles ] );
			strcpy( m_pszMusicFiles[ m_nMusicFiles ], sz );

			m_pszMusicFilesShort[ m_nMusicFiles ] = (char*)malloc( strlen(fd.name) + 1 );
			assert( m_pszMusicFilesShort[ m_nMusicFiles ] );
			strcpy( m_pszMusicFilesShort[ m_nMusicFiles ], fd.name );

			m_nMusicFiles++ ;
			nSearchResult = _findnext(lSearchHandle, &fd) ;
		}
		i++ ;
	}
}

/*FSOUND_STREAM* CSoundManager::LoadFMODStreamFromResource( int nResID, char* strResType )
{
	FSOUND_STREAM* ps ;
    HMODULE hModule = NULL;
    HRSRC rsrc;
    HGLOBAL hgData;
    LPVOID pvData;
    DWORD cbData;

    rsrc = FindResource( hModule, MAKEINTRESOURCE(nResID), strResType );
    if( rsrc != NULL )
    {
        cbData = SizeofResource( hModule, rsrc );
        if( cbData > 0 )
        {
            hgData = LoadResource( hModule, rsrc );
            if( hgData != NULL )
            {
                pvData = LockResource( hgData );
                if( pvData != NULL )
                {
					ps = FSOUND_Stream_OpenFile( (const char*)pvData,
												 FSOUND_LOOP_NORMAL | FSOUND_LOADMEMORY,
												 cbData );

					return ps ;
                }
            }
        }
    }
    return NULL ;
}

FSOUND_SAMPLE* CSoundManager::LoadFMODSampleFromResource( int nSamplePoolIndex, int nResID, char* strResType )
{
	FSOUND_SAMPLE* ps ;
    HMODULE hModule = NULL;
    HRSRC rsrc;
    HGLOBAL hgData;
    LPVOID pvData;
    DWORD cbData;

    rsrc = FindResource( hModule, MAKEINTRESOURCE(nResID), strResType );
    if( rsrc != NULL )
    {
        cbData = SizeofResource( hModule, rsrc );
        if( cbData > 0 )
        {
            hgData = LoadResource( hModule, rsrc );
            if( hgData != NULL )
            {
                pvData = LockResource( hgData );
                if( pvData != NULL )
                {
					ps = FSOUND_Sample_Load( nSamplePoolIndex,
											 (const char*)pvData,
											 FSOUND_LOADMEMORY,
											 cbData );
					return ps ;
                }
            }
        }
    }
    return NULL ;
}*/

