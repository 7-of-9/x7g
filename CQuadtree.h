#ifndef __CQuadtree__
#define __CQuadtree__

class CQuadtree ;

	#include <D3DX8Math.h>
	#include "./D3DFrame8.1 Custom/include/D3DUtil.h"
	#include "x7_primitives.h"
	#include "x7_utils.h"
	#include "tex.h"

#define MAX_QT_NODES 8192

class CQuadtree
{
public:
//
// Construction / Destruction:
//
	CQuadtree(xVec3 *pvMin, xVec3 *pvMax, int nRequiredDepth) ;
	~CQuadtree() ;

//
// Geometric Data:
//
	/*xVec3 m_vMin ;
	xVec3 m_vMax ;
	xVec3 m_vVerts[8] ;
	xVec3 m_vCenter ;*/

//
// Memory Usage Monitor:
//
	DWORD m_dwMem ;

//
// Quadree:
//
	struct _QTNode
	{
		int nDepth ;					// currently only used for rendering.
		int nX, nY ;					// used by CTerrain to store indices of associated path in terminal nodes
		void *pKids[4] ;
		xVec3 vMin, vMax ;
	} ;

	_QTNode *m_pQT ;					// single-linked list's root node
	_QTNode *m_pQTNodes[MAX_QT_NODES] ;	// list of all node addresses
	int m_nQTNodes ;
	int m_nQTDepth ;
	BOOL m_bQTBuilt ;

	void BuildQuadtree(int nSubdivides, xVec3 *pvMin, xVec3 *pvMax) ;
	void RecurseQuadtree(int nSubdivides, _QTNode *pNode) ;
	void InitQuadtreeNode(_QTNode **ppNode, int nDepth) ;

//
// Render Data:
//
	COLOREDVERTEX m_fvfVerts[8] ;
	WORD		  m_wIndices[36] ;
	D3DMATERIAL8  m_Mtrl ;

	LPDIRECT3DVERTEXBUFFER8 m_pVB ;
	LPDIRECT3DINDEXBUFFER8 m_pIB ;

	HRESULT MakeRenderData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT InvalidateDeviceObjects() ;
	HRESULT Render(LPDIRECT3DDEVICE8 pDev, unsigned long *pulPolyCount, xMatUA* pmatWorld, int nNodeDepth) ;
} ;

#endif
