#ifndef __CCMDLINEPARSER__
#define __CCMDLINEPARSER__

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include "x7_utils.h"

#define MAX_NAMEVALUES 128

class CCmdLineParser
{
public:
	CCmdLineParser( char *szCmdLine );
	~CCmdLineParser() ;

	int		GetInt   ( char* szName, int   Def );
	float	GetFloat ( char* szName, float Def );
	char*	GetString( char* szName, char* Def );
	DWORD	GetDWord ( char* szName, DWORD Def );

private:
	char*  m_szNames[ MAX_NAMEVALUES ];
	char*  m_szValues[ MAX_NAMEVALUES ];
	int	   m_nNameValues ;
} ;

#endif