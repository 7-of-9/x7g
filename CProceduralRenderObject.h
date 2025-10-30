#ifndef __CProceduralRenderObject__
#define __CProceduralRenderObject__

class CProceduralRenderObject ;

	#include <D3DX8Math.h>
	#include "./D3DFrame8.1 Custom/include/D3DUtil.h"
	#include "x7_primitives.h"
	#include "x7_utils.h"
	#include "CCamera.h"
	#include "CTextures.h"
	#include "CBoundingBox.h"

class CProceduralRenderObject
{
public:
	DWORD m_dwFVF ;

//
// Construction / Destruction:
//
	~CProceduralRenderObject() ;
	 CProceduralRenderObject() ;

	 bool m_bUse32bitIndexesIfAvailable ;

//
// Base X7LT Layer:
//
	unsigned long m_nBaseX7LTs ;	
	X7LT *m_pX7LTs ;
	X7LT *m_pFastLastX7LT ;

	X7LT *AddX7LT(UNCOLOREDVERTEX *pVerts, D3DCOLOR *pColors, D3DCOLOR *pSpecularColors, unsigned long ndxTex, unsigned long ndxTex2, float *pfTU2, float *pfTV2) ;
	X7LT *LastX7LT() ;
	X7LT *FirstX7LT() ;

	void ScaleX7LTs(float s) ;
	void RotateX7LTsY(float n, xVec3* pvO) ;
	void TranslateX7LTs(float x, float y, float z) ;

//
// Memory Usage Monitors:
//
	DWORD m_dwMem ;
	DWORD m_dwMem_X7LTs ;
	DWORD m_dwMem_IndexedVerts ;
	DWORD m_dwMem_RenderData ;
	DWORD m_dwMem_BBox ;

//
// Indexed Triangle List Layer, w/ pointers to D3DX Mesh Layer:
//
// One m_ntvRS entry per texture.
//
#define MAX_DS 10
	typedef struct
	{
		unsigned		 ndxTex ;
		unsigned		 ndxTex2 ;
		
		UNCOLOREDVERTEX				*pUnlitVertices ;
		COLOREDVERTEX				*pLitVertices ;
		COLOREDVERTEX2TEX			*pLitVertices2Tex ;
		COLOREDVERTEXWITHNORMAL		*pLitVerticesWithNormals ;
		COLOREDVERTEXWITHNORMAL2TEX *pLitVerticesWithNormals2Tex ;

		WORD			*pIndices ;
		DWORD			 dwNumVertices ;
		DWORD			 dwNumIndices ;

	    D3DMATERIAL8	 mtrlMaterial ;

		LPD3DXMESH		 pMesh ;

		LPDIRECT3DVERTEXBUFFER8 pVB ;
		LPDIRECT3DINDEXBUFFER8 pIB ;

		int nDSs ;						// no. of drawing subsets in the VB.
		DWORD dwSS_MinIndex[MAX_DS] ;
		DWORD dwSS_MaxIndex[MAX_DS] ;
		DWORD dwSS_NumVerts[MAX_DS] ;
		DWORD dwSS_StartIndex[MAX_DS] ;
		DWORD dwSS_PrimCount[MAX_DS] ;

	} _INDEXED_VERTS ;
	_INDEXED_VERTS m_ntvRS[8] ;
	int m_nRSs ;

	/*UNCOLOREDVERTEX			*m_pUnlitVertices ;
	COLOREDVERTEX				*m_pLitVertices ;
	COLOREDVERTEXWITHNORMAL		*m_pLitVerticesWithNormals ;
	COLOREDVERTEXWITHNORMAL2TEX *m_pLitVerticesWithNormals2Tex ;
	WORD  *m_pIndices ;
	DWORD  m_dwNumVertices ;
	DWORD  m_dwNumIndices ;
	struct _DrawingSubset ;
	{
		unsigned ndxTex ;
		DWORD dwSS_MinIndex[5] ;
		DWORD dwSS_NumVerts[5] ;
		DWORD dwSS_StartIndex[5] ;
		DWORD dwSS_PrimCount[5] ;
	} m_DSs[8] ;
	int m_nDSs ;	// no. of drawing subsets in the mesh.
	LPD3DXMESH m_pMesh ;*/

	HRESULT IndexToList() ;
	void GetDistinctRenderStates() ;
	BOOL CompareIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS) ;
	void AddIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS) ;

//
// Render Data (D3DXMesh or VB+IB) Layer:
//
	HRESULT MakeRenderData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT MakeBoundingBox(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	DWORD m_dwMeshOptions ;
	DWORD m_dwUsageOptions ;
	BOOL m_bUseD3DXMesh ;
	xVec3 m_vMin, m_vMax ;		// must be supplied when m_bUseD3DXMesh==FALSE

//
// Bounding Box:
//
	CBoundingBox *m_pBBox ;

//
// World Matrix:
//
	xMat m_matWorld ;
	void WorldRotateX(float r, xVec3 *pv) ;
	void WorldRotateY(float r, xVec3 *pv) ;
	void WorldRotateZ(float r, xVec3 *pv) ;
	void WorldTranslate( xVec3 *pv) ;

//
// Meta data:
//
	GetBBoxVisibilityFlags(LPDIRECT3DDEVICE8 pDev, xMatUA *pmatProj, CCamera *pCam, D3DVIEWPORT8 *pVP) ;
	BOOL m_bVisible ;
	/*D3DCLIPSTATUS8 m_csVis ;*/
	/*PROCESSEDVERTEX m_vP[8] ;*/

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT	InvalidateDeviceObjects() ;

//
// Render Methods:
//
	HRESULT Render( LPDIRECT3DDEVICE8 pDev,
					CTextures*		  pTex,
					unsigned long*	  pulPolyCount,
					int				  nDS,
					xMatUA*			  pmatCurWorld,
					bool			  bCallerWantsAutoGendTexCoords );

	HRESULT RenderDetailPass( LPDIRECT3DDEVICE8		pDev,
						      LPDIRECT3DTEXTURE8	pTexA,
							  unsigned long*		pulPolyCount,
							  int					nDS,
							  xMatUA*				pmatCurWorld );
} ;

#endif