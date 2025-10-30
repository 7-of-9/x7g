#ifndef __CBoundingBox__
#define __CBoundingBox__

class CBoundingBox ;

#include <D3DX8Math.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "tex.h"

#define MAX_OT_NODES 8192

class CBoundingBox
{
public:
//
// Construction / Destruction:
//
	CBoundingBox( xVec3 *pvMin, xVec3 *pvMax );
	CBoundingBox( xVec3* pvVerts );
	InitRenderData() ;
	
	~CBoundingBox() ;

//
// Geometric Data:
//
	xVec3 m_vMin ;
	xVec3 m_vMax ;
	xVec3 m_vVerts[8] ;
	xVec3 m_vCenter ;

	xVec3 m_vInitialMin ;
	xVec3 m_vInitialMax ;
	xVec3 m_vInitialVerts[8] ;
	xVec3 m_vInitialCenter ;

//
// Memory Usage Monitor:
//
	DWORD m_dwMem ;
	DWORD m_dwMem_OT ;

//
// Octree:
//
	struct _OTTriList					// 40 bytes
	{
		void *pNext ;
		xVec3 vVerts[3] ;
	} ;

	struct _OTNode						// 60 bytes
	{
		void *pKids[8] ;
		xVec3 vMin, vMax ;
		_OTTriList *pTris ;
	} ;

	_OTNode *m_pOT ;					// single-linked list's root node
	_OTNode *m_pOTNodes[MAX_OT_NODES] ;	// list of all node addresses
	int m_nOTNodes ;
	BOOL m_bOTBuilt ;

	void BuildOctree( int nSubdivides );
	
	void RecurseOctree( int	     nSubdivides,
					    _OTNode* pNode );

	void InitOctreeNode( _OTNode **ppNode );

//
// World Matrix:
//
	xMat m_matWorld ;
	void ApplyMatrix( xMatUA* m );
	void ApplyMatrixFromInitialValues( xMatUA* m );

//
// Render Data:
//
	COLOREDVERTEX m_fvfVerts[8] ;
	WORD		  m_wIndices[36] ;
	LPD3DXMESH    m_pMesh ;
	D3DMATERIAL8  m_Mtrl ;

	LPDIRECT3DVERTEXBUFFER8 m_pVB ;
	LPDIRECT3DVERTEXBUFFER8 m_pVBProcessed ;
	LPDIRECT3DINDEXBUFFER8 m_pIB ;

	HRESULT MakeRenderData( LPDIRECT3DDEVICE8 pDev );

//
// Collision Detection:
//
	LineIntersects( xVec3* pvLineStart,
					xVec3* pvDir,
					float		 fDistance );

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE8 pDev );
	
	HRESULT InvalidateDeviceObjects() ;

	HRESULT Render( LPDIRECT3DDEVICE8 pDev,
					unsigned long*	  pulPolyCount,
					xMatUA*	  pmatWorld );

} ;

#endif
