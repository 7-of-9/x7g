#ifndef __CRenderObject__
#define __CRenderObject__

	#include <D3DX8Math.h>

	#include "x7_primitives.h"
	#include "x7_utils.h"

	#include "CTextures.h"

class CRenderObject
{
public:

//
// Construction / Destruction:
//
	~CRenderObject() ;
	 CRenderObject() ;

//
// Base X7LT Layer:
//
	unsigned long m_nBaseX7LTs ;	
	X7LT *m_pX7LTs ;
	X7LT *m_pFastLastX7LT ;

	X7LT *AddX7LT(UNLITVERTEX *pVerts, D3DCOLOR *pColors, D3DCOLOR *pSpecularColors, unsigned long ndxTex, BOOL bIncludeInCDNet) ;
	X7LT *LastX7LT() ;
	X7LT *FirstX7LT() ;

	void ScaleX7LTs(float s) ;
	void RotateX7LTsY(float n, LPD3DXVECTOR3 pvO) ;

//
// Indexed Triangle List Layer, w/ pointers to D3DX and D3D VB Layers:
//
	typedef struct
	{
		BOOL					 bStrip ;
		BOOL					 bList ;
		unsigned				 ndxTex ;
		unsigned				 nPolys ;
		
		UNLITVERTEX				*pUnlitVertices ;
		LITVERTEX				*pLitVertices ;
		WORD					*pIndices ;
		DWORD					 dwNumVertices ;
		DWORD					 dwNumIndices ;

		LPD3DXMESH				 pMesh ;
		DWORD					*pMeshAdjacency ;

		LPD3DXPMESH				 pPMesh ;
		
		LPD3DXSPMESH			 pSPMesh ;
		LPD3DXMESH				 pMesh1 ;

	} _INDEXED_VERTS ;
	_INDEXED_VERTS m_ntvRS[8] ;
	int m_nRSs ;
	DWORD m_dwFVF ;

	HRESULT IndexToList() ;
	void GetDistinctRenderStates() ;
	BOOL CompareIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS) ;
	void AddIndexedVertex(unsigned ndx, X7LT *tri, int v, int nRS) ;

//
// D3D Device-Dependent Vertex Buffer & Index Buffer Creation Layer:
//
	/*
	HRESULT MakeVBs_IBs(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8	pDev) ;
	*/

//
// D3DX Mesh and PMesh Creation Layer:
//
	HRESULT MakeMesh_PMesh(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT	InvalidateDeviceObjects() ;

//
// Render Methods:
//
	HRESULT Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *ulPolyCount) ;
} ;

/*
BOOL CRenderObject__MoveFileObjectDataCB(
										 CD3DFileObject* pObject,
										 D3DMATRIX* pmat,
										 VOID* pContext
										) ;
*/

#endif