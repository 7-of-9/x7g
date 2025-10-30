#include "stdafx.h"

#include "CConfig.h"

CConfig::CConfig() { 
	Load() ;
}
CConfig::~CConfig() {}


void CConfig::Load() {
// defaults
	m_ConfigData.bClampVSync = true ;
	m_ConfigData.bFullscreen = false ;
	m_ConfigData.xRes = MED_RES ;
	m_ConfigData.xDet = LOW_DET ;
	m_ConfigData.nDepthBuf = 16 ;
	m_ConfigData.nFSAA = 0 ;

	m_ConfigData.fFXVol = 1 ;
	m_ConfigData.fMusicVol = 0.5f ;
	strcpy( m_ConfigData.szMusicPath, "..\\resource\\wav2" );

	m_ConfigData.fInputSensitivity = 0.5f ;
	m_ConfigData.bMouseInvert = false ;

// read from file, if present
	FILE* fp = fopen( CONFIG_FILE, "rb" );
	if( !fp )
		return ;
	fread( (void*)&m_ConfigData, sizeof( m_ConfigData ), 1, fp );
	fclose( fp );
}
void CConfig::Save() {
	FILE* fp = fopen( CONFIG_FILE, "wb" );
	if( !fp )
		return ;
	fwrite( (void*)&m_ConfigData, sizeof( m_ConfigData ), 1, fp );
	fclose( fp );
}


//
// Video
//
bool CConfig::getFullscreen( ) { return m_ConfigData.bFullscreen ; }
int CConfig::getFSAA( ) { return m_ConfigData.nFSAA ; }
int CConfig::getDepthBuf( ) { return m_ConfigData.nDepthBuf ; }
bool CConfig::getClampVSync( ) { return m_ConfigData.bClampVSync ; }
CConfig::xRES CConfig::getRes( ) { return m_ConfigData.xRes ; }
CConfig::xDET CConfig::getDet( ) { return m_ConfigData.xDet ; }

void CConfig::setFullscreen( bool b ) {
	m_ConfigData.bFullscreen = b ; Save() ;
}
void CConfig::setFSAA( int n ) {
	m_ConfigData.nFSAA = n ; Save() ;
}
void CConfig::setDepthBuf( int n ) {
	m_ConfigData.nDepthBuf = n ; Save() ;
}
void CConfig::setClampVSync( bool b ) {
	m_ConfigData.bClampVSync = b ; Save() ;
}
void CConfig::setRes( xRES res ) {
	m_ConfigData.xRes = res ; Save() ;
}
void CConfig::setDet( xDET det ) {
	m_ConfigData.xDet = det ; Save() ;
}

//
// Sound
//
float CConfig::getFXVol() { return m_ConfigData.fFXVol ; }
float CConfig::getMusicVol() { return m_ConfigData.fMusicVol ; }
char* CConfig::getMusicPath(){ return m_ConfigData.szMusicPath ; }

void CConfig::setFXVol( float v ) {
	m_ConfigData.fFXVol = v ; Save () ;
}
void CConfig::setMusicVol( float v ) {
	m_ConfigData.fMusicVol = v ; Save () ;
}
void CConfig::setMusicPath( char* p ) {
	strcpy( m_ConfigData.szMusicPath, p ); Save() ;
}

//
// Input
//
float CConfig::getInputSensitivity() { return m_ConfigData.fInputSensitivity ; } 
bool CConfig::getMouseInvert() { return m_ConfigData.bMouseInvert ; }

void CConfig::setInputSensitivity( float f ) {
	m_ConfigData.fInputSensitivity = f ; Save () ;
}
void CConfig::setMouseInvert( bool b ) {
	m_ConfigData.bMouseInvert = b ; Save () ;
}