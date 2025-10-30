
#define GAME_EXE "x7_vc7.exe"
#define CONFIG_FILE "config.dat"

class CConfig
{
public:

	enum xRES	{ LOW_RES, MED_RES, HI_RES };
	enum xDET	{ LOW_DET, MED_DET, HI_DET };

	CConfig() ;
	~CConfig() ;
	void Load() ;
	void Save() ;

// video
	void setFullscreen( bool b );
	void setFSAA( int n );
	void setClampVSync( bool b );
	void setRes( xRES res );
	void setDet( xDET res );
	void setDepthBuf( int n );

	bool getFullscreen( );
	int  getFSAA( );
	bool getClampVSync( );
	xRES getRes( );
	xDET getDet( );
	int  getDepthBuf( );

// sound
	void setFXVol( float v );
	void setMusicVol( float v );
	void setMusicPath( char* szPath );

	float getFXVol( );
	float getMusicVol( );
	char* getMusicPath( );

// input
	float getInputSensitivity() ;
	bool getMouseInvert() ;
	
	void setInputSensitivity( float f );
	void setMouseInvert( bool b );

private:
	typedef struct
	{
		bool	bFullscreen ;
		xRES	xRes ;
		xDET	xDet ;
		bool	bClampVSync ;
		int     nFSAA ;
		int     nDepthBuf ;
		float	fFXVol, fMusicVol ;
		char	szMusicPath[ 1024 ];
		float	fInputSensitivity ;
		bool	bMouseInvert ;
	} CONFIG ;
	CONFIG m_ConfigData ;
} ;
