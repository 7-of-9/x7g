/*
 *
 * x7
 * x7_utils.h
 *
 * Utility functions.
 *
 */

#ifdef UTILS
#else

#include <windows.h>
#include <assert.h>
#include <D3DX8.h>
#include "./D3DFrame8.1 Custom/include/d3dfont.h"
#include "./D3DFrame8.1 Custom/include/D3DUtil.h"
#include "./D3DFrame8.1 Custom/include/DXUtil.h"

#include "x7_primitives.h"

#include "CCamera.h"

#define LOAD_RES_FROM_FILE

//#define FULL_DBG_TRACE


	extern BOOL		g_bDebugMsgs ;

	extern xMat		g_matIdent ;

	void			SetKey( int VK, int b );

	void			SetupSpecularMaterial( D3DMATERIAL8* pMat, float r, float g, float b, float a  );
	
	int				Pow2( int i );

	void			LTrim( char* sz );
	void			RTrim( char* sz );
	void			 Trim( char* sz );
	
	DWORD			FtoDW( FLOAT f );

	void			Msg( char *sz );
	void			Msg2( char *fmt, ... );
	void			ods( char *fmt, ... );
	
	void			D3DFormat2Str( DWORD format, char *sz );
	void			D3DBlend2Str(DWORD blend, char *sz)	;

	float			rnd( void );

	BOOL			GetBBoxVisibility(LPDIRECT3DDEVICE8 pDev, const xMatUA& matProj, const xMatUA& matView, D3DVIEWPORT8* pVP, const xVec3& vMin, const xVec3& vMax);

	float			square( float f );
	void sphere_line_intersection (
    float x1, float y1 , float z1,
    float x2, float y2 , float z2,
    float x3, float y3 , float z3, float r, float* p );

BOOL SphereLineTest ( const xVec3& vA,	// IN:  line start
					  const xVec3& vB,	// IN:  line end
					  const xVec3& vS,	// IN:  sphere pos.
					  const float r,			// IN:  sphere radius
					  int* pN,					// OUT: no. of intersections (0, 1 or 2), set if not NULL
					  xVec3* pvI1,		// OUT: intersection coords 1, set if not NULL
					  xVec3* pvI2			// OUT: intersection coords 2. set if not NULL
				    );

	float SqrDistance (const xVec3& vPoint,
					   const xVec3& vLineA,
					   const xVec3& vLineB );

	HRESULT			LoadDDSTextureFromResource( LPDIRECT3DDEVICE8 pd3dDevice, 
												TCHAR* strRes, 
												LPDIRECT3DTEXTURE8* ppTex,
												TCHAR* strResType );

	HRESULT			LoadXMeshFromResource( LPDIRECT3DDEVICE8 pd3dDevice, 
										   TCHAR* strRes, 
										   LPD3DXMESH* ppMesh,
										   LPD3DXBUFFER* ppAdj,
										   LPD3DXBUFFER* ppMat,
										   DWORD* pNumMat,
										   DWORD dwOptions );
	
	void  MultVectorArb   (xVec3* v, xMatUA* m) ;
	void  RotateVectorX   (xVec3*  v, float n, xVec3* o) ;
	void  RotateVectorY   (xVec3*  v, float n, xVec3* o) ;
	void  RotateVectorZ   (xVec3*  v, float n, xVec3* o) ;
	void  RotateVectorArb (xVec3*  v, float n, xVec3* o, xVec3* plane) ;
	void  TranslateVector (xVec3*  v, float x, float y, float z) ;


#endif