#ifndef __Primitives__
#define __Primitives__

#include <stdio.h>
#include <windows.h>
#include <D3DX8.h>
#include "./D3DFrame8.1 Custom/include/d3dfont_custom.h"
#include "./D3DFrame8.1 Custom/include/D3DUtil.h"
#include "./D3DFrame8.1 Custom/include/DXUtil.h"

//-----------------------------------------------------------------------------
// Useful Math constants
//-----------------------------------------------------------------------------
const FLOAT g_PI       =  3.14159265358979323846f; // Pi
const FLOAT g_2_PI     =  6.28318530717958623200f; // 2 * Pi
const FLOAT g_PI_DIV_2 =  1.57079632679489655800f; // Pi / 2
const FLOAT g_PI_DIV_4 =  0.78539816339744827900f; // Pi / 4
const FLOAT g_PI_DIV_8 =  0.39269908169872415480f; // Pi / 8
const FLOAT g_PI_DIV_16=  0.19634954084936207740f; // Pi / 16
const FLOAT g_INV_PI   =  0.31830988618379069122f; // 1 / Pi
const FLOAT g_DEGTORAD =  0.01745329251994329547f; // Degrees to Radians
const FLOAT g_RADTODEG = 57.29577951308232286465f; // Radians to Degrees
const FLOAT g_HUGE     =  1.0e+38f;                // Huge number for FLOAT
const FLOAT g_EPSILON  =  1.0e-5f;                 // Tolerance for FLOATs



//
// Texture-stage helpers
//
#define SetTextureColorStage( dev, i, arg1, op, arg2 )     \
   dev->SetTextureStageState( i, D3DTSS_COLOROP, op);      \
   dev->SetTextureStageState( i, D3DTSS_COLORARG1, arg1 ); \
   dev->SetTextureStageState( i, D3DTSS_COLORARG2, arg2 );

#define SetTextureAlphaStage( dev, i, arg1, op, arg2 )     \
   dev->SetTextureStageState( i, D3DTSS_ALPHAOP, op);      \
   dev->SetTextureStageState( i, D3DTSS_ALPHAARG1, arg1 ); \
   dev->SetTextureStageState( i, D3DTSS_ALPHAARG2, arg2 );



//
// 16-byte aligned vectors and matrices
//
// Not used yet, should be throughout for optimal operation on P4s, though.
// Requires C7 or C6+Processor Pack to compile.
//

/*class __declspec(align(16)) xVec2 : public D3DXVECTOR2
{
public: 
	xVec2() : D3DXVECTOR2() {} ;
	xVec2( float x, float y ) : D3DXVECTOR2( x,y ) {} ;
	xVec2( const D3DXVECTOR2& v ) : D3DXVECTOR2( v ) {} ;
} ;*/

/*class __declspec(align(16)) xVec3 : public D3DXVECTOR3
{
public: 
	xVec3() : D3DXVECTOR3() {} ;
	xVec3( float x, float y, float z ) : D3DXVECTOR3( x,y,z ) {} ;
	xVec3( const D3DXVECTOR3& v ) : D3DXVECTOR3( v ) {} ;
} ;*/

/*class __declspec(align(16)) xVec4 : public D3DXVECTOR4
{
public: 
	xVec4() : D3DXVECTOR4() {} ;
	xVec4( float x, float y, float z, float w ) : D3DXVECTOR4( x,y,z,w ) {} ;
	xVec4( const D3DXVECTOR4& v ) : D3DXVECTOR4( v ) {} ;
} ;*/

/*class __declspec(align(16)) xMat  : public D3DXMATRIX  {} ;*/

#define xVec4 D3DXVECTOR4
#define xVec3 D3DXVECTOR3
#define xVec2 D3DXVECTOR2
#define xMatUA D3DXMATRIX
#define xMat D3DXMATRIX


//
// PROCESSEDVERTEX
//
//		Used when we will call ProcessVertices.
//
#define MYFVF_PROCESSEDVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )

typedef struct PROCESSEDVERTEX
{
public:
	float x, y, z, rhw ;
	DWORD c, s ;
	float tu, tv ;
} PROCESSEDVERTEX ;



//
// POINTSPRITEVERTEX
//
//		Used for rendering using pointsprites.
//
#define MYFVF_POINTSPRITEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

typedef struct POINTSPRITEVERTEX
{
    xVec3 v ;
    D3DCOLOR    color ;
} POINTSPRITEVERTEX ;




//
// COLOREDVERTEXWITHNORMAL2TEX
//
//		Used when we will supply color data, and D3D will apply lighting.
//
#define MYFVF_COLOREDVERTEXWITHNORMAL2TEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 | (D3DFVF_TEXCOORDSIZE2(0)) | (D3DFVF_TEXCOORDSIZE2(1)) )

typedef struct COLOREDVERTEXWITHNORMAL2TEX
{
public:
	float x, y, z ;
    float nx, ny, nz;
	DWORD color, specular ;
	float tu1, tv1 ;
	float tu2, tv2 ;

    COLOREDVERTEXWITHNORMAL2TEX() {} ;
    COLOREDVERTEXWITHNORMAL2TEX(FLOAT __x, FLOAT __y, FLOAT __z, FLOAT __tu1, FLOAT __tv1, FLOAT __tu2, FLOAT __tv2)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
		tu2 = __tu2 ; tv2 = __tv2 ;
	}

} COLOREDVERTEXWITHNORMAL2TEX ;




//
// COLOREDVERTEXWITHNORMAL
//
//		Used when we will supply color data, and D3D will apply lighting.
//
#define MYFVF_COLOREDVERTEXWITHNORMAL (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

typedef struct COLOREDVERTEXWITHNORMAL
{
public:
	float x, y, z ;
    float nx, ny, nz;
	DWORD color, specular ;
	float tu1, tv1 ;

    COLOREDVERTEXWITHNORMAL() {} ;
    COLOREDVERTEXWITHNORMAL(FLOAT __x, FLOAT __y, FLOAT __z, FLOAT __tu1, FLOAT __tv1)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
	}

} COLOREDVERTEXWITHNORMAL ;



//
// UNCOLOREDVERTEX
//
//		Used when we've got no color data, and D3D will light the vertices.
//
#define MYFVF_UNCOLOREDVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

typedef struct UNCOLOREDVERTEX
{
public:
	float x, y, z ;
    float nx, ny, nz;
    float tu1, tv1;

    UNCOLOREDVERTEX() {} ;
    UNCOLOREDVERTEX(FLOAT __x, FLOAT __y, FLOAT __z,
				FLOAT _nx, FLOAT _ny, FLOAT _nz,
				FLOAT __tu1, FLOAT __tv1)
	{
		x = __x; y = __y; z = __z;
		nx = _nx; ny = _ny; nz = _nz;
		tu1 = __tu1 ; tv1 = __tv1 ;
	}
    UNCOLOREDVERTEX(FLOAT __x, FLOAT __y, FLOAT __z,
				FLOAT __tu1, FLOAT __tv1)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
	}

} UNCOLOREDVERTEX ;

extern DWORD dwUNCOLOREDVERTEX_vsdec[] ;




//
// COLOREDVERTEX
//
//		Used when we will supply all color data, and D3D will apply no lighting.
//
#define MYFVF_COLOREDVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

typedef struct COLOREDVERTEX
{
public:
	float x, y, z ;
	DWORD color, specular ;
	float tu1, tv1 ;

    COLOREDVERTEX() {} ;
    COLOREDVERTEX(FLOAT __x, FLOAT __y, FLOAT __z, FLOAT __tu1, FLOAT __tv1)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
	}

    COLOREDVERTEX(FLOAT __x, FLOAT __y, FLOAT __z, FLOAT __tu1, FLOAT __tv1, DWORD c, DWORD s)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
		color = c ;
		specular = s ;
	}
} COLOREDVERTEX ;


//
// COLOREDVERTEX2TEX *************** UNTESTED **********************************************************************
//
//		Used when we will supply all color data, and D3D will apply no lighting.
//
#define MYFVF_COLOREDVERTEX2TEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 | (D3DFVF_TEXCOORDSIZE2(0)) | (D3DFVF_TEXCOORDSIZE2(1)))

typedef struct COLOREDVERTEX2TEX
{
public:
	float x, y, z ;
	DWORD color, specular ;
	float tu1, tv1 ;
	float tu2, tv2 ;

    COLOREDVERTEX2TEX() {} ;
    COLOREDVERTEX2TEX(FLOAT __x, FLOAT __y, FLOAT __z, FLOAT __tu1, FLOAT __tv1, FLOAT __tu2, FLOAT __tv2)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
		tu2 = __tu2 ; tv2 = __tv2 ;
	}

    COLOREDVERTEX2TEX(FLOAT __x, FLOAT __y, FLOAT __z, FLOAT __tu1, FLOAT __tv1, FLOAT __tu2, FLOAT __tv2, DWORD c, DWORD s)
	{
		x = __x; y = __y; z = __z;
		tu1 = __tu1 ; tv1 = __tv1 ;
		tu2 = __tu2 ; tv2 = __tv2 ;
		color = c ;
		specular = s ;
	}
} COLOREDVERTEX2TEX ;


//
// Base render class data: double linked triangle.
//
struct X7LT
{
	UNCOLOREDVERTEX	 Points[3] ;

	BOOL			 bColors ;
	D3DCOLOR		 Colors[3] ;
	BOOL			 bSpecularColors ;
	D3DCOLOR		 SpecularColors[3] ;

	BOOL			 b2Tex ;
	float			 fTU2[3] ;
	float			 fTV2[3] ;
	
	void			*pNext ;
	void			*pPrevious ;
	
	unsigned		 ndxTex ;
	unsigned		 ndxTex2 ;
} ;

#endif