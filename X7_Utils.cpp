#pragma warning( disable : 4183 )	// 'missing return type'

/*
 *
 * x7
 * x7_utils.cpp
 *
 * Utility functions.
 *
 */

#include "X7_Utils.h"
#include "X7_Primitives.h"

BOOL g_bDebugMsgs = TRUE ;
xMat g_matIdent ;

// Set up full D3DMATERIAL8 struct
void SetupSpecularMaterial( D3DMATERIAL8* pMat, float r, float g, float b, float a )
{
	D3DUtil_InitMaterial( *pMat, r, g, b, a );
	pMat->Power = 50 ;
	pMat->Specular.r = 
	pMat->Specular.g =
	pMat->Specular.b =
	pMat->Specular.a = 1.0f ;
	pMat->Emissive.r = 0.0f ;
	pMat->Emissive.g = 0.0f ;
	pMat->Emissive.b = 0.0f ;
	pMat->Emissive.a = 1.0f ;
}


// Get power of two, throw rattle out of pram if it's not
int Pow2( int i )
{
	switch( i )
	{
		case 1: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		case 16: return 4;
		case 32: return 5;
		case 64: return 6;
		case 128: return 7;
		case 256: return 8;
		case 512: return 9;
		case 1024: return 10;
		default: Msg("Expected power of two, not received.");
	}
	return -1;
}

void LTrim( char* sz )
{
	if( strlen(sz) == 0 ) return ;
	while( *sz == ' ' )
		strcpy( sz, sz+1 );
}

void RTrim( char* sz )
{
	if( strlen(sz) == 0 ) return ;
	while( sz[strlen(sz)-1] == ' ' )
	{
		sz[strlen(sz)-1] = '\x0' ;
		if( strlen(sz) == 0 ) return ;
	}
}

void Trim( char* sz )
{
	LTrim( sz );
	RTrim( sz );
}

// Helper function to stuff a FLOAT into a DWORD argument
DWORD FtoDW( FLOAT f )
{
	return *((DWORD*)&f);
}

// MessageBox
void Msg(char *sz)
{
	if (g_bDebugMsgs)
	{
		MessageBox(NULL, sz, "Dbg Msg", MB_ICONINFORMATION | MB_OK) ;
		/*abort() ;*/
	}
}

void Msg2( char *fmt, ... ) 
{ 
	va_list va_alist = {0}; 
	char str[256]; 

	va_start( va_alist, fmt ); 
	_vsnprintf( str, 256, fmt, va_alist ); 
	va_end( va_alist ); 

	Msg( str ); 
}

void ods( char *fmt, ... ) 
{
#ifdef _DEBUG
	va_list va_alist = {0}; 
	char str[256]; 

	va_start( va_alist, fmt ); 
	_vsnprintf( str, 256, fmt, va_alist ); 
	va_end( va_alist ); 

	OutputDebugString( str );
#endif
}

// Does what is says on the tin
BOOL GetBBoxVisibility(LPDIRECT3DDEVICE8 pDev, const xMatUA& matProj, const xMatUA& matView, D3DVIEWPORT8* pVP, const xVec3& vMin, const xVec3& vMax)
{
	int i, t, nOut[8] ;
	xVec3 v[8] ;
	xVec4 v4[8] ;
	BOOL vis ;

	v[0].x = vMin.x ; v[0].y = vMin.y ; v[0].z = vMin.z ;
	v[1].x = vMax.x ; v[1].y = vMin.y ; v[1].z = vMin.z ;
	v[2].x = vMin.x ; v[2].y = vMax.y ; v[2].z = vMin.z ;
	v[3].x = vMax.x ; v[3].y = vMax.y ; v[3].z = vMin.z ;
	v[4].x = vMin.x ; v[4].y = vMin.y ; v[4].z = vMax.z ;
	v[5].x = vMax.x ; v[5].y = vMin.y ; v[5].z = vMax.z ;
	v[6].x = vMin.x ; v[6].y = vMax.y ; v[6].z = vMax.z ;
	v[7].x = vMax.x ; v[7].y = vMax.y ; v[7].z = vMax.z ;
	
	for (i = 0 ; i < 8 ; i ++)
	{
		D3DXVec3TransformCoord(&v[i], &v[i], &matView) ;
		D3DXVec3Transform(&v4[i], &v[i], &matProj) ;
		nOut[i] = 0 ;
	}

	vis = TRUE ;
	t = 0;
	for ( i = 0; i < 8; i++ )
		if (v4[i].z < 0)
		{
			nOut[i] += 1 ;
			t++ ;
		}
	if ( t >= 8 ) vis = FALSE ;

	t = 0;
	for ( i = 0; i < 8; i++ )
		if (v4[i].x > v4[i].w)
		{
			nOut[i] += 2 ;
			t++ ;
		}
	if ( t >= 8 ) vis = FALSE ;

	t = 0;
	for ( i = 0; i < 8; i++ )
		if ( v4[i].x < -v4[i].w )
		{
			nOut[i] += 4 ;
			t++ ;
		}
	if ( t >= 8 ) vis = FALSE ;

	t = 0;
	for ( i = 0; i < 8; i++ )
		if ( v4[i].y > v4[i].w )
		{
			nOut[i] += 8 ;
			t++;
		}
	if ( t >= 8 ) vis = FALSE ;

	t = 0;
	for ( i = 0; i < 8; i++ )
		if ( v4[i].y < -v4[i].w )
		{
			nOut[i] += 16 ;
			t++;
		}
	if ( t >= 8 ) vis = FALSE ;

	return vis ;
}



void sphere_line_intersection (
    float x1, float y1 , float z1,
    float x2, float y2 , float z2,
    float x3, float y3 , float z3, float r, float* p )
{

 // x1,y1,z1  P1 coordinates (point of line)
 // x2,y2,z2  P2 coordinates (point of line)
 // x3,y3,z3, r  P3 coordinates and radius (sphere)
 // x,y,z   intersection coordinates
 //
 // This function returns a pointer array which first index indicates
 // the number of intersection point, followed by coordinate pairs.

 float a, b, c, mu, i ;

 a =  square(x2 - x1) + square(y2 - y1) + square(z2 - z1);
 b =  2* ( (x2 - x1)*(x1 - x3)
      + (y2 - y1)*(y1 - y3)
      + (z2 - z1)*(z1 - z3) ) ;
 c =  square(x3) + square(y3) +
      square(z3) + square(x1) +
      square(y1) + square(z1) -
      2* ( x3*x1 + y3*y1 + z3*z1 ) - square(r) ;
 i =   b * b - 4 * a * c ;

 if ( i < 0.0 )
 {
  // no intersection
  p[0] = 0.0;
  return ;
 }
 if ( i == 0.0 )
 {
  // one intersection
  p[0] = 1.0;

  mu = -b/(2*a) ;
  p[1] = x1 + mu*(x2-x1);
  p[2] = y1 + mu*(y2-y1);
  p[3] = z1 + mu*(z2-z1);
  return ;
 }
 if ( i > 0.0 )
 {
  // two intersections
  p[0] = 2.0;

  // first intersection
  mu = (-b + sqrtf( square(b) - 4*a*c )) / (2*a);
  p[1] = x1 + mu*(x2-x1);
  p[2] = y1 + mu*(y2-y1);
  p[3] = z1 + mu*(z2-z1);
  // second intersection
  mu = (-b - sqrtf(square(b) - 4*a*c )) / (2*a);
  p[4] = x1 + mu*(x2-x1);
  p[5] = y1 + mu*(y2-y1);
  p[6] = z1 + mu*(z2-z1);

  return ;
 }
}
// ********************************************************************

float square( float f ) { return (f*f) ;}

BOOL SphereLineTest ( const xVec3& vA,	// IN:  line start
					  const xVec3& vB,	// IN:  line end
					  const xVec3& vS,	// IN:  sphere pos.
					  const float r,			// IN:  sphere radius
					  int* pN,					// OUT: no. of intersections (0, 1 or 2), set if not NULL
					  xVec3* pvI1,		// OUT: intersection coords 1, set if not NULL
					  xVec3* pvI2			// OUT: intersection coords 2. set if not NULL
				    )
{
	int nIntersects ;
	xVec3 vMiddleOfLine = vA + (vB - vA) / 2 ;
	xVec3 vIntersect ;
	float fHalfLineLength = D3DXVec3Length( &(vB - vA) ) / 2.0f ;
	float fDistanceFromIntersectionToMiddleOfLine ;
	BOOL bIntersect = FALSE ;
	BOOL bLineStartStopIn = FALSE ;

// is start/stop inside circle?
	if( D3DXVec3Length( &(vA - vS) ) <= r )
		bLineStartStopIn = TRUE ;
	if( D3DXVec3Length( &(vB - vS) ) <= r )
		bLineStartStopIn = TRUE ;

// get ray (not line!) intersection points;
	float f[7] ;
	sphere_line_intersection( vA.x, vA.y, vA.z,
							  vB.x, vB.y, vB.z,
							  vS.x, vS.y, vS.z, r, f );

// save no. of ray intersects;
	nIntersects = (int)f[0] ;
	if( pN ) *pN = nIntersects ;
	if( nIntersects==0 )
		return FALSE ;

// determine if the line extends to the ray-intersect points;
	for( int i=0; i<nIntersects; i++ )
	{
		vIntersect = xVec3( f[1 + i*3], f[2 + i*3], f[3 + i*3] );
		if( i==0 ) if( pvI1 ) *pvI1 = vIntersect ;
		if( i==1 ) if( pvI2 ) *pvI2 = vIntersect ;
		
		if( bLineStartStopIn )
			bIntersect = TRUE ;
		else
		{
			fDistanceFromIntersectionToMiddleOfLine = D3DXVec3Length( &(vIntersect - vMiddleOfLine) );
			if( fDistanceFromIntersectionToMiddleOfLine <= fHalfLineLength )
				bIntersect = TRUE ;
		}
	}
	return bIntersect ;
}

/*

BOOL SphereBoxTest( ... )

Fast & easy, but not accurate at corners:

1. grow box bu sphere radius
2. check sphere center within new larger box

*/
 


// 0 - 1
float rnd(void)
{
	return (float)rand() / (float)RAND_MAX ;
}

// D3DFORMAT -> String
void D3DFormat2Str(DWORD format, char *sz)
{
	switch(format)

	{
		case D3DFMT_UNKNOWN:	strcpy(sz, "UNKNOWN") ;			break ;
		case D3DFMT_R8G8B8:		strcpy(sz, "D3DFMT_R8G8B8") ;	break ;
		case D3DFMT_A8R8G8B8:	strcpy(sz, "D3DFMT_A8R8G8B8") ; break ;
		case D3DFMT_X8R8G8B8:	strcpy(sz, "D3DFMT_X8R8G8B8") ; break ;
		case D3DFMT_R5G6B5:		strcpy(sz, "D3DFMT_R5G6B5") ;	break ;
		case D3DFMT_X1R5G5B5:	strcpy(sz, "D3DFMT_X1R5G5B5") ; break ;
		case D3DFMT_A1R5G5B5:	strcpy(sz, "D3DFMT_A1R5G5B5") ; break ;
		case D3DFMT_A4R4G4B4:	strcpy(sz, "D3DFMT_A4R4G4B4") ; break ;
		case D3DFMT_R3G3B2:		strcpy(sz, "D3DFMT_R3G3B2") ;	break ;
		case D3DFMT_A8:			strcpy(sz, "D3DFMT_A8") ;		break ;
		case D3DFMT_A8R3G3B2:	strcpy(sz, "D3DFMT_A8R3G3B2") ; break ;
		case D3DFMT_X4R4G4B4:	strcpy(sz, "D3DFMT_X4R4G4B4") ; break ;

		case D3DFMT_A8P8:		strcpy(sz, "D3DFMT_A8P8") ;		break ;
		case D3DFMT_P8:         strcpy(sz, "D3DFMT_P8") ;		break ;

		case D3DFMT_L8:         strcpy(sz, "D3DFMT_L8") ;		break ;
		case D3DFMT_A8L8:       strcpy(sz, "D3DFMT_A8L8") ;		break ;
		case D3DFMT_A4L4:       strcpy(sz, "D3DFMT_A4L4") ;		break ;

    	case D3DFMT_V8U8:       strcpy(sz, "D3DFMT_V8U8") ;		break ;
    	case D3DFMT_L6V5U5:     strcpy(sz, "D3DFMT_L6V5U5") ;	break ;
    	case D3DFMT_X8L8V8U8:   strcpy(sz, "D3DFMT_X8L8V8U8") ; break ;
    	case D3DFMT_Q8W8V8U8:   strcpy(sz, "D3DFMT_Q8W8V8U8") ; break ;
    	case D3DFMT_V16U16:     strcpy(sz, "D3DFMT_V16U16") ;	break ;
    	case D3DFMT_W11V11U10:  strcpy(sz, "D3DFMT_W11V11U10") ;break ;

    	case D3DFMT_UYVY:       strcpy(sz, "D3DFMT_UYVY") ;		break ;
    	case D3DFMT_YUY2:       strcpy(sz, "D3DFMT_YUY2") ;		break ;
    	case D3DFMT_DXT1:       strcpy(sz, "D3DFMT_DXT1") ;		break ;
    	case D3DFMT_DXT2:       strcpy(sz, "D3DFMT_DXT2") ;		break ;
    	case D3DFMT_DXT3:       strcpy(sz, "D3DFMT_DXT3") ;		break ;
    	case D3DFMT_DXT4:       strcpy(sz, "D3DFMT_DXT4") ;		break ;
    	case D3DFMT_DXT5:       strcpy(sz, "D3DFMT_DXT5") ;		break ;

    	case D3DFMT_D16_LOCKABLE:strcpy(sz, "D3DFMT_D16_LOCKABLE") ; break ;
    	case D3DFMT_D32:        strcpy(sz, "D3DFMT_D32") ;		break ;
    	case D3DFMT_D15S1:      strcpy(sz, "D3DFMT_D15S1") ;	break ;
    	case D3DFMT_D24S8:      strcpy(sz, "D3DFMT_D24S8") ;	break ;
    	case D3DFMT_D16:        strcpy(sz, "D3DFMT_D16") ;		break ;
    	case D3DFMT_D24X8:      strcpy(sz, "D3DFMT_D24X8") ;	break ;
    	case D3DFMT_D24X4S4:    strcpy(sz, "D3DFMT_D24X4S4") ;	break ;


		case D3DFMT_VERTEXDATA: strcpy(sz, "D3DFMT_VERTEXDATA") ; break ;
		case D3DFMT_INDEX16:    strcpy(sz, "D3DFMT_INDEX16") ;	break ;
		case D3DFMT_INDEX32:    strcpy(sz, "D3DFMT_INDEX32") ;	break ;

		default:				strcpy(sz, "OTHER") ;
	}
}

// D3DBLEND -> String
void D3DBlend2Str(DWORD blend, char *sz)
{
	switch(blend)
	{
		case D3DBLEND_ZERO				:	strcpy(sz, "D3DBLEND_ZERO") ;	break ;
		case D3DBLEND_ONE				:	strcpy(sz, "D3DBLEND_ONE") ;	break ;
		case D3DBLEND_SRCCOLOR			:	strcpy(sz, "D3DBLEND_SRCCOLOR") ; break ;
		case D3DBLEND_INVSRCCOLOR       :	strcpy(sz, "D3DBLEND_INVSRCCOLOR") ; break ;
		case D3DBLEND_SRCALPHA          :	strcpy(sz, "D3DBLEND_SRCALPHA") ; break ;
		case D3DBLEND_INVSRCALPHA       :	strcpy(sz, "D3DBLEND_INVSRCALPHA") ; break ;
		case D3DBLEND_DESTALPHA         :	strcpy(sz, "D3DBLEND_DESTALPHA") ; break ;
		case D3DBLEND_INVDESTALPHA      :	strcpy(sz, "D3DBLEND_INVDESTALPHA") ; break ;
		case D3DBLEND_DESTCOLOR         :	strcpy(sz, "D3DBLEND_DESTCOLOR") ; break ;
		case D3DBLEND_INVDESTCOLOR      :	strcpy(sz, "D3DBLEND_INVDESTCOLOR") ; break ;
		case D3DBLEND_SRCALPHASAT       :	strcpy(sz, "D3DBLEND_SRCALPHASAT") ; break ;
		case D3DBLEND_BOTHSRCALPHA      :	strcpy(sz, "D3DBLEND_BOTHSRCALPHA") ; break ;
		case D3DBLEND_BOTHINVSRCALPHA   :	strcpy(sz, "D3DBLEND_BOTHINVSRCALPHA") ; break ;
		default							:	strcpy(sz, "N/A") ;
	}
}


// do you really like it, is it, is it wicked?
HRESULT LoadDDSTextureFromResource( LPDIRECT3DDEVICE8 pd3dDevice, 
									TCHAR* strRes, 
									LPDIRECT3DTEXTURE8* ppTex,
									TCHAR* strResType )
{
    HRESULT hr;
    HMODULE hModule = NULL;
    HRSRC rsrc;
    HGLOBAL hgData;
    LPVOID pvData;
    DWORD cbData;

    rsrc = FindResource( hModule, strRes, strResType );
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
                    if( FAILED( hr = D3DXCreateTextureFromFileInMemoryEx( 
                        pd3dDevice, pvData, cbData, D3DX_DEFAULT, D3DX_DEFAULT, 
                        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                        D3DX_DEFAULT, 0, NULL, NULL, ppTex ) ) )
                    {
                        return hr;
                    }
                }
            }
        }
    }
    
    if( *ppTex == NULL)
        return E_FAIL;

    return S_OK;
}

// we're lovin it, lovin it, lovin it
HRESULT LoadXMeshFromResource( LPDIRECT3DDEVICE8 pd3dDevice, 
							   TCHAR* strRes, 
							   LPD3DXMESH* ppMesh,
							   LPD3DXBUFFER* ppAdj,
							   LPD3DXBUFFER* ppMat,
							   DWORD* pNumMat,
							   DWORD dwOptions
							   )
{
    HRESULT hr;
    HMODULE hModule = NULL;
    HRSRC rsrc;
    HGLOBAL hgData;
    LPVOID pvData;
    DWORD cbData;

    *ppMesh = NULL ;
	rsrc = FindResource( hModule, strRes, "X" );
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
					hr = D3DXLoadMeshFromXInMemory(
										   (BYTE*)pvData, cbData,
										   dwOptions,
										   pd3dDevice,
										   ppAdj,
										   ppMat,
										   pNumMat,
										   ppMesh );
					if( FAILED(hr) )
						return hr ;
                }
            }
        }
    }
    
    if( *ppMesh == NULL)
        return E_FAIL;

    return S_OK;
}



	void TranslateVector(xVec3* v, float x, float y, float z)
	{
		xVec4		  v0 ;
		static xMat m0 ;
		static float	  px, py, pz ;

		if (px != x || py != y || pz != z)
		{
			D3DXMatrixTranslation(&m0, x, y, z) ;
			px = x ;
			py = y ;
			pz = z ;
		}

		D3DXVec3Transform(&v0, v, &m0) ;
		v->x = v0.x ;
		v->y = v0.y ;
		v->z = v0.z ;
	}

	void RotateVectorX(xVec3* v, float n, xVec3* o)
	{
		xVec4 v0 ;
		static xMat m, m0, m1, m2 ;
		static float pn = 0.0f ;
		static xVec3 po ;

		if (n != pn || po.x != o->x || po.y != o->y || po.z != o->z)
		{
			D3DXMatrixTranslation(&m0, -o->x, -o->y, -o->z) ;
			D3DXMatrixRotationX(&m1, n * g_DEGTORAD) ;
			D3DXMatrixTranslation(&m2, +o->x, +o->y, +o->z) ;
			m = g_matIdent ;
			D3DXMatrixMultiply(&m, &m, &m0) ;
			D3DXMatrixMultiply(&m, &m, &m1) ;
			D3DXMatrixMultiply(&m, &m, &m2) ;

			pn = n ;
			po = xVec3(o->x, o->y, o->z) ;
		}

		D3DXVec3Transform(&v0, v, &m) ;
		v->x = v0.x ;
		v->y = v0.y ;
		v->z = v0.z ;
	}
	
	void RotateVectorY(xVec3* v, float n, xVec3* o)
	{
		xVec4 v0 ;
		static xMat m, m0, m1, m2 ;
		static float pn = 0.0f ;
		static xVec3 po ;

		if (n != pn || po.x != o->x || po.y != o->y || po.z != o->z)
		{
			D3DXMatrixTranslation(&m0, -o->x, -o->y, -o->z) ;
			D3DXMatrixRotationY(&m1, n * g_DEGTORAD) ;
			D3DXMatrixTranslation(&m2, +o->x, +o->y, +o->z) ;
			m = g_matIdent ;
			D3DXMatrixMultiply(&m, &m, &m0) ;
			D3DXMatrixMultiply(&m, &m, &m1) ;
			D3DXMatrixMultiply(&m, &m, &m2) ;

			pn = n ;
			po = xVec3(o->x, o->y, o->z) ;
		}

		D3DXVec3Transform(&v0, v, &m) ;
		v->x = v0.x ;
		v->y = v0.y ;
		v->z = v0.z ;
	}

	void RotateVectorZ(xVec3* v, float n, xVec3* o)
	{
		xVec4 v0 ;
		static xMat m, m0, m1, m2 ;
		static float pn = 0.0f ;
		static xVec3 po ;

		if (n != pn || po.x != o->x || po.y != o->y || po.z != o->z)
		{
			D3DXMatrixTranslation(&m0, -o->x, -o->y, -o->z) ;
			D3DXMatrixRotationZ(&m1, n * g_DEGTORAD) ;
			D3DXMatrixTranslation(&m2, +o->x, +o->y, +o->z) ;
			m = g_matIdent ;
			D3DXMatrixMultiply(&m, &m, &m0) ;
			D3DXMatrixMultiply(&m, &m, &m1) ;
			D3DXMatrixMultiply(&m, &m, &m2) ;

			pn = n ;
			po = xVec3(o->x, o->y, o->z) ;
		}

		D3DXVec3Transform(&v0, v, &m) ;
		v->x = v0.x ;
		v->y = v0.y ;
		v->z = v0.z ;
	}
	/*void RotateVectorArb (LPD3DXVECTOR3 v, float n, LPD3DXVECTOR3 o, LPD3DXVECTOR3 plane)
	{
		D3DVECTOR v0 ;
		static D3DMATRIX m, m0, m1, m2 ;
		static float pn = 0.0f ;
		static D3DVECTOR po ;

		if (n != pn || po.x != o->x || po.y != o->y || po.z != o->z)
		{
			D3DUtil_SetTranslateMatrix  (m0, -o->x, -o->y, -o->z) ;
			D3DUtil_SetRotationMatrix (m1, *plane, n * g_DEGTORAD) ;
			D3DUtil_SetTranslateMatrix  (m2, +o->x, +o->y, +o->z) ;
			D3DUtil_SetIdentityMatrix (m) ;
			D3DMath_MatrixMultiply (m, m, m0) ;
			D3DMath_MatrixMultiply (m, m, m1) ;
			D3DMath_MatrixMultiply (m, m, m2) ;
			pn = n ;
			po = D3DVECTOR(o->x, o->y, o->z) ;
		}

		D3DMath_VectorMatrixMultiply (v0, *v, m) ;
		v->x = v0.x ;
		v->y = v0.y ;
		v->z = v0.z ;
	}

/*
 *
 * Computes the X and Z aspects (-1 to 1) of a Y angle (degrees).
 *
 */
/*	void GetXAndZComponentsFromYRotation (float cam_y_rot, float *XComponent, float *ZComponent)
	{
		static float p_cam_y_rot ;
		static float p_XComponent, p_ZComponent ;

		p_cam_y_rot = cam_y_rot ;
		if (fabs(cam_y_rot) <= 90.0f)
		{
			if (cam_y_rot > 0.0f)
			{
				*XComponent = +((float)fabs(cam_y_rot) / 90.0f) ;
				*ZComponent = +(1.0f - ((float)fabs(cam_y_rot) / 90.0f)) ;
			}
			else
			{
				*XComponent = -((float)fabs(cam_y_rot) / 90.0f) ;
				*ZComponent = +(1.0f - ((float)fabs(cam_y_rot) / 90.0f)) ;
			}
		}
		if ((float)fabs(cam_y_rot) > 90.0f && (float)fabs(cam_y_rot) <= 180.0f)
		{
			if (cam_y_rot > 0.0f)
			{
				*XComponent = +(1.0f - (((float)fabs(cam_y_rot) - 90) / 90.0f)) ;
				*ZComponent = -(1.0f * (((float)fabs(cam_y_rot) - 90) / 90.0f)) ;
			}
			else
			{
				*XComponent = -(1.0f - (((float)fabs(cam_y_rot) - 90) / 90.0f)) ;
				*ZComponent = -(1.0f * (((float)fabs(cam_y_rot) - 90) / 90.0f)) ;
			}
		}
		p_XComponent = *XComponent ;
		p_ZComponent = *ZComponent ;
	}*/

/*
 *
 * Sets all pixels in the given surface with the specified RGB values to the specified Alpha value.
 *
 * NOTE: requires full alpha (ARGB?) format?
 *
 */
	/*void MGM_SetAlphaColor(LPDIRECTDRAWSURFACE7 TheSurface,BYTE bRed , BYTE bGreen, BYTE bBlue,BYTE bAlpha)
	{
		DDSURFACEDESC2 Desc;
		ZeroMemory( &Desc, sizeof( Desc ));
		Desc.dwSize = sizeof( Desc ); 
		TheSurface->Lock( NULL, &Desc, DDLOCK_WAIT, NULL );

		DWORD AlphaMask = ( DWORD )Desc.ddpfPixelFormat.dwRGBAlphaBitMask; 
		DWORD RGBMask = ( DWORD )( Desc.ddpfPixelFormat.dwRBitMask | Desc.ddpfPixelFormat.dwGBitMask | Desc.ddpfPixelFormat.dwBBitMask );

		DWORD* SurfPtr;
		WORD* Sixteenbit_SurfPtr;

		float red=bRed / 255.0f;
		float green=bGreen / 255.0f;
		float blue=bBlue / 255.0f;
		float alpha=bAlpha / 255.0f; 

		DWORD mode=Desc.ddpfPixelFormat.dwRGBBitCount;

		DWORD color =( ((DWORD)(Desc.ddpfPixelFormat.dwRBitMask*red)) & Desc.ddpfPixelFormat.dwRBitMask) + ( ((DWORD)(Desc.ddpfPixelFormat.dwGBitMask*green)) & Desc.ddpfPixelFormat.dwGBitMask) + ( ((DWORD)(Desc.ddpfPixelFormat.dwBBitMask*blue)) & Desc.ddpfPixelFormat.dwBBitMask); 

		DWORD totalcolor=(((DWORD)(AlphaMask*alpha)) & AlphaMask)+ ( ((DWORD)(Desc.ddpfPixelFormat.dwRBitMask*red)) & Desc.ddpfPixelFormat.dwRBitMask) + ( ((DWORD)(Desc.ddpfPixelFormat.dwGBitMask*green)) & Desc.ddpfPixelFormat.dwGBitMask) + ( ((DWORD)(Desc.ddpfPixelFormat.dwBBitMask*blue)) & Desc.ddpfPixelFormat.dwBBitMask); 

		if (mode==32)
		{
		for( DWORD y = 0; y != Desc.dwHeight; y++ )
		{ 
		SurfPtr = ( DWORD* )(( BYTE* )Desc.lpSurface + y * Desc.lPitch );
		for( DWORD x = 0; x != Desc.dwWidth; x++ ) 
		{ 
		if(( *SurfPtr & RGBMask ) == color )
		*SurfPtr=totalcolor; 
		SurfPtr++; 
		} 
		} 
		}

		else

		if (mode==16)
		{

		for( DWORD y = 0; y != Desc.dwHeight; y++ )
		{ 
		Sixteenbit_SurfPtr = ( WORD* )(( BYTE* )Desc.lpSurface + y * Desc.lPitch );
		for( DWORD x = 0; x != Desc.dwWidth; x++ ) 
		{ 
		if(( *Sixteenbit_SurfPtr & RGBMask ) == color )
		*Sixteenbit_SurfPtr=(unsigned short)totalcolor; 
		Sixteenbit_SurfPtr++; 
		} 
		} 
		}

		TheSurface->Unlock( NULL ); 
	}*/

/*
 *
 * Returns the DDCOLORKEY value corresponding to the given RGB values for the 
 * given surface.
 *
 */
	/*DWORD MGM_RGBConvert (BYTE red,BYTE green,BYTE blue,LPDIRECTDRAWSURFACE7 surface)
	{
		float FractionRed=red/255.0f;
		float FractionGreen=green/255.0f;
		float FractionBlue=blue/255.0f;
		float FractionAlpha=1.0f;

		DDPIXELFORMAT ddpf;
		ddpf.dwSize=sizeof(ddpf); 
		surface->GetPixelFormat(&ddpf);

		DWORD color =( ((DWORD) (ddpf.dwRGBAlphaBitMask*FractionAlpha)) & ddpf.dwRGBAlphaBitMask) +
		( ((DWORD) (ddpf.dwRBitMask*FractionRed)) & ddpf.dwRBitMask) +
		( ((DWORD) (ddpf.dwGBitMask*FractionGreen)) & ddpf.dwGBitMask) +
		( ((DWORD) (ddpf.dwBBitMask*FractionBlue)) & ddpf.dwBBitMask);

		return color;
	}*/

void SetKey( int VK, int b )
{
	if( b )
	{
		if(  GetKeyState( VK ) ) return ;
	}
	else
	{
		if( !GetKeyState( VK ) ) return ;
	}
	keybd_event( VK, 0, 0, 0 );
	keybd_event( VK, 0, KEYEVENTF_KEYUP, 0 );
}