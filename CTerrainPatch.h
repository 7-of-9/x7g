#ifndef __CTerrainPatch__
#define __CTerrainPatch__

	#include <assert.h>	
	#include <D3DX8Math.h>
	#include "x7_primitives.h"
	#include "x7_utils.h"
	#include "tex.h"
	#include "CUserInput.h"
	#include "CProceduralRenderObject.h"

// NOTE: minimal LOD = 4x4

//#define PATCH_MAX_LOD_SQ		4
//#define PATCH_MAX_LOD_LEVELS	1

#undef DS_CENTER

enum e_TerrainPatch_DrawingSubsets
{
	DS_CENTER = 0,

	DS_TOP_EDGE,			// = 1
	DS_LEFT_EDGE,			// = 2
	DS_BOTTOM_EDGE,			// = 3
	DS_RIGHT_EDGE,			// = 4

	DS_ALL,					// = 5

	DS_TOP_EDGE_LODPLUS1,	// = 6
	DS_LEFT_EDGE_LODPLUS1,	// = 7
	DS_BOTTOM_EDGE_LODPLUS1,// = 8
	DS_RIGHT_EDGE_LODPLUS1  // = 9
} ;

/*
 *
 * WARNING! When adding new types to this enum. look at the code in:
 * 
 *   CTerrain::DistanceSortItemsAndSetLights 
 *
 * RE mem. allocation. Very, very dangerous.
 *
 */
enum e_ItemTypes
{
	IT_CHERRY = 0,
	IT_SHIELD,
	IT_ENGINE,
	IT_WEAPON,
	IT_HEALTH,
	MAX_ITEM_TYPES
} ;
	
struct GAMEITEM
{
	int			nType ;			 // one of e_ItemTypes
	xVec3		vPos ;			 // abs. center pos.
	float		fRadius ;		 // abs. radius
	float		fRot ;			 // y rotation, radians
	float		fShine ;		 // light brightness 0 - 1
	float		fShineModSpeed ; // light modulation speed 0 - 1
	float		fX1, fY1, fZ1 ;	 // general-purpose values,
								 // used by CTerrain::RenderItem for texgen effects
} ;

class CTerrainPatch
{
public:

//
// Construction / Destruction:
//
	CTerrainPatch( int nPatchMaxLODSQ,
				   xVec3* pvTL,
				   float fWidth,
				   float fDepth,
				   float *pfHeightmap,
				   D3DXCOLOR* pColormap,
				   unsigned long ulNextRowHeightmapDelta,
				   unsigned long ulMaxExtent,
				   float fMinY,
				   float fMaxY,
				   xVec3* pvSpawnPos,
				   float fTerrainLenSQ,
				   float nCherriesReq,
				   float nShieldPwrUpsReq,
				   float nEnginePwrUpsReq,
				   float nWeaponPwrUpsReq,
				   float fHealthPwrUpsReq,
				   int nTex1,
				   int nTex2 );
	~CTerrainPatch() ;

//
// Basic Parameters:
//
	int		m_nPatchMaxLODSQ ;
	int		m_nPatchMaxLODLevels ;
	float	m_fTerrainLenSQ ;

//
// Items:
//
	GAMEITEM* m_pItems ;
	int		  m_nItems ;
	float	  m_fItemsReq ;
	int		  m_nItemsActuallyAllocated ;
	
	int		  m_nCherries ;
	float	  m_fCherriesReq ;
	float	  m_fCherryProb ;// prob. of dropping item at max. LOD / terrain vertex

	int		  m_nShieldPwrUps ;
	float	  m_fShieldPwrUpsReq ;
	float	  m_fShieldPwrUpProb ;

	int		  m_nEnginePwrUps ;
	float	  m_fEnginePwrUpsReq ;
	float	  m_fEnginePwrUpProb ;

	int		  m_nWeaponPwrUps ;
	float	  m_fWeaponPwrUpsReq ;
	float	  m_fWeaponPwrUpProb ;
	
	int		  m_nHealthPwrUps ;
	float	  m_fHealthPwrUpsReq ;
	float	  m_fHealthPwrUpProb ;

//
// Terrain Generator:
//
	HRESULT CreateFastPatch(int nPatchSQ, xVec3* pvTL, float fWidth, float fDepth, CProceduralRenderObject *pRO, float* pfHeightmap, D3DXCOLOR* pColormap, unsigned long ulNextRowHeightmapDelta, unsigned long ulMaxExtent, float fMinY, float fMaxY, BOOL bBuildEdgeConnectors, xVec3* pvSpawnPos, int nTex1, int nTex2 ) ;
	/*void inline TerrainVertexColours(float *r, float *g, float *b, float *a, float fY, float fMaxY, float fMinY) ;*/
	void inline AddVertex(CProceduralRenderObject *pRO, int nVert, float fVertexX, float fVertexY, float fVertexZ, float fX, float fY, float a, float r, float g, float b) ;
	void inline AddFaceNormalData(CProceduralRenderObject *pRO, int ndxA, int ndxB, int ndxC) ;

	CProceduralRenderObject** m_pPatchLOD ;

//
// Memory Usage Monitor:
//
	DWORD m_dwMem ;
	DWORD QueryMemUsage() ;
	DWORD m_dwTotalNumVertices, m_dwTotalNumIndices ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT	InvalidateDeviceObjects() ;
    HRESULT Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, int nLOD, int dsTopEdge, int dsLeftEdge, int dsBottomEdge, int dsRightEdge, xMatUA *pmatCurWorld) ;
    HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS) ;
} ;

#endif