#include "CCmdLineParser.h"

CCmdLineParser::CCmdLineParser( char* szCmdLine )
{
	unsigned int i ;
	char		 c ;
	bool		 bBuildingName = false ;
	bool		 bBuildingValue = false ;
	char		 szTmpName[ 1024 ], szTmpValue[ 1024 ];
	int			 nLenTmpName = 0, nLenTmpValue = 0 ;
	
// init. members
	m_nNameValues = 0 ;

// scan supplied command-line
	for( i=0 ; i < strlen( szCmdLine ) ; i++ )
	{
		bool bFoundSep = false ;
		bool bFoundName = false ;
		bool bFoundValue = false ;

		c = szCmdLine[ i ];

	// look for a separator char
		if( c == '/' /*|| c == '-'*/ )
		{
		// have we got a name/value pair?
			if( nLenTmpName > 0 && nLenTmpValue > 0 )
			{
				szTmpName[ nLenTmpName ] = '\x0' ;
				szTmpValue[ nLenTmpValue ] = '\x0' ;
				m_szNames[ m_nNameValues ] = (char*)malloc( nLenTmpName+1 );
				m_szValues[ m_nNameValues ] = (char*)malloc( nLenTmpValue+1 );
				Trim( szTmpName );
				Trim( szTmpValue );
				strupr( szTmpName );
				strcpy( m_szNames[ m_nNameValues ], szTmpName );
				strcpy( m_szValues[ m_nNameValues ], szTmpValue );
				m_nNameValues++ ;
				assert( m_nNameValues<MAX_NAMEVALUES );
			}

			bBuildingName = true ;
			bBuildingValue = false ;
			nLenTmpName = 0 ;
			nLenTmpValue = 0 ;
			bFoundSep = true ;
		}
		if( bFoundSep )
			continue ;

	// build name
		if( bBuildingName )
			if( c == ':' )
			{
				bBuildingName = false ;
				bBuildingValue = true ;
				bFoundName = true ;
			}
			else
				szTmpName[ nLenTmpName++ ] = c ;
		if( bFoundName )
			continue ;

	// build value
		if( bBuildingValue )
			szTmpValue[ nLenTmpValue++ ] = c ;
	}

// have we got a name/value pair?
	if( nLenTmpName > 0 && nLenTmpValue > 0 )
	{
		szTmpName[ nLenTmpName ] = '\x0' ;
		szTmpValue[ nLenTmpValue ] = '\x0' ;
		m_szNames[ m_nNameValues ] = (char*)malloc( nLenTmpName+1 );
		m_szValues[ m_nNameValues ] = (char*)malloc( nLenTmpValue+1 );
		Trim( szTmpName );
		Trim( szTmpValue );
		strupr( szTmpName );
		strcpy( m_szNames[ m_nNameValues ], szTmpName );
		strcpy( m_szValues[ m_nNameValues ], szTmpValue );
		m_nNameValues++ ;
		assert( m_nNameValues<MAX_NAMEVALUES );
	}
}

CCmdLineParser::~CCmdLineParser()
{
	int i ;
	for( i=0 ; i < m_nNameValues ; i++ )
	{
		free( m_szNames[ i ] );
		free( m_szValues[ i ] );
	}
}

int CCmdLineParser::GetInt( char* szName, int Def )
{
	char szSearchFor[ 1024 ];
	strcpy( szSearchFor, szName );
	strupr( szSearchFor );
	for( int i = 0 ; i < m_nNameValues ; i++ )
		if( strcmp(m_szNames[ i ], szSearchFor) == 0 )
		{
			return atoi(m_szValues[ i ]) ;
		}
	return Def ;
}

DWORD CCmdLineParser::GetDWord( char* szName, DWORD Def )
{
	char szSearchFor[ 1024 ];
	strcpy( szSearchFor, szName );
	strupr( szSearchFor );
	for( int i = 0 ; i < m_nNameValues ; i++ )
		if( strcmp(m_szNames[ i ], szSearchFor) == 0 )
		{
			return (DWORD)atol(m_szValues[ i ]) ;
		}
	return Def ;
}

float CCmdLineParser::GetFloat( char* szName, float Def )
{
	char szSearchFor[ 1024 ];
	strcpy( szSearchFor, szName );
	strupr( szSearchFor );
	for( int i = 0 ; i < m_nNameValues ; i++ )
		if( strcmp(m_szNames[ i ], szSearchFor) == 0 )
		{
			return (float)atof(m_szValues[ i ]);
		}
	return Def ;
}

char* CCmdLineParser::GetString( char* szName, char* Def )
{
	char szSearchFor[ 1024 ];
	strcpy( szSearchFor, szName );
	strupr( szSearchFor );
	for( int i = 0 ; i < m_nNameValues ; i++ )
		if( strcmp(m_szNames[ i ], szSearchFor) == 0 )
		{
			return m_szValues[ i ];
		}
	return Def ;
}