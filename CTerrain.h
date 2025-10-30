#ifndef __CTerrain__
#define __CTerrain__

class CTerrain ;
	
#include <D3DX8Math.h>
#include "x7_primitives.h"
#include "x7_utils.h"

#include "tex.h"
#include "CCamera.h"
#include "CUserInput.h"
#include "CTerrainPatch.h"
#include "CQuadtree.h"
#include "CFileRenderObject.h"
#include "x7.h"

#define MAX_SPAWN_POS 9


struct PATCH_SORT 
{
	int x, y ;	// indices
	float d ;	// distance to camera
} ;
int SortPatchDistancesCallback( const VOID* arg1, const VOID* arg2 );

struct ITEM_SORT
{
	GAMEITEM* pItem;	// pointer to gameitem struct
	float d ;			// distance to camera
	int nType ;			// type of gameitem
	BOOL bVisible ;		// inherits per frame from parent terrain patch
} ;
int SortItemDistancesCallback( const VOID* arg1, const VOID* arg2 );



class CTerrain
{
public:

	int			  m_nCurLOD ;

//
// Construction / Destruction:
//
	CTerrain( float fSizeSQ,
			  Cx7 *pX7,
			  int nPatchesSQ,
			  int nPatchMaxLODSQ,
			  int nCherriesReq,
			  int nShieldPwrUpsReq,
			  int nEnginePwrUpsReq,
			  int nWeaponPwrUpsReq,
			  int nHealthPwrUpsReq,
			  float fMountainFactor,
			  int nTex1,
			  int nTex2,
			  unsigned nNoisePatches1,
			  unsigned nNoisePatches2, int ColorFactor1, int ColorFactor2, int ColorFactor3 );

	~CTerrain() ;
	Cx7*		  m_pX7 ;

//
// Basic Parameters:
//
	int			  m_nPatchesSQ ;
	int			  m_nPatchesReqQuadTDepth ;
	
	int			  m_nPatchMaxLODSQ ;
	int			  m_nPatchMaxLODLevels ;

	float		  m_fLenSQ, m_fMinY, m_fMaxY ;	// abs. world coords.

//
// Items stuff:
//
	LPD3DXMESH	  m_pItemSphereMesh ;
	D3DMATERIAL8  m_mtrlItemSphere ;
	int			  m_nCherriesReq,	  m_nCherries ;
	int			  m_nShieldPwrUpsReq, m_nShieldPwrUps ;
	int			  m_nEnginePwrUpsReq, m_nEnginePwrUps ;
	int			  m_nWeaponPwrUpsReq, m_nWeaponPwrUps ;
	int			  m_nHealthPwrUpsReq, m_nHealthPwrUps ;
	int RenderItem( LPDIRECT3DDEVICE8 pDev, CTextures* pTex, unsigned long *pulPolyCount, xMatUA* pmatCurWorld, GAMEITEM* pItem, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, CCamera* pCam, float fFPS, float d ) ;

	ITEM_SORT*	  m_pisAllItemsDistanceSorted ;
	int			  m_nItemsDistanceSorted ;

//
// Cherry stuff:
//
	CFileRenderObject* m_pCherry ;
	D3DMATERIAL8	   m_mtrlCherry ;
	D3DXCOLOR		   m_colCherry ;
	D3DLIGHT8	       m_litCherry ;

//
// Shield stuff:
//
	CFileRenderObject* m_pShield ;
	D3DMATERIAL8	   m_mtrlShield ;
	D3DXCOLOR		   m_colShield ;
	D3DLIGHT8	       m_litShield ;

//
// Engine stuff:
//
	D3DMATERIAL8	   m_mtrlEngine ;
	D3DXCOLOR		   m_colEngine ;
	D3DLIGHT8	       m_litEngine ;

//
// Weapon stuff:
//
	CFileRenderObject* m_pWeapon ;
	D3DMATERIAL8	   m_mtrlWeapon ;
	D3DXCOLOR		   m_colWeapon ;
	D3DLIGHT8	       m_litWeapon ;

//
// Health stuff:
//
	CFileRenderObject* m_pHealth ;
	D3DMATERIAL8	   m_mtrlHealth ;
	D3DXCOLOR		   m_colHealth ;
	D3DLIGHT8	       m_litHealth ;

//
// Used for cycling tex. coords on game items, and for spinning game items:
//
	/*float X0, Y0, Z0 ;
	float X1, Y1, Z1 ;
	float X2, Y2, Z2 ;
	float X3, Y3, Z3 ;*/

//
// Memory Usage Monitor:
//
	DWORD m_dwMem ;
	DWORD QueryMemUsage() ;

//
// Heightmap:
//
	float*		  m_pfHeightmap ;
	unsigned long m_nHeightmapSQ ;		// no. of rows and cols in the heightmap
	float		  m_fUnitSize ;			// length/height of each lowest res. row/col

//
// Colormap:
//
	D3DXCOLOR*	  m_pColormap ;			// works just like heightmap, except contains RGBA, not height, values
	BlendColorSpot( unsigned Row, unsigned Col, int Radius, D3DXCOLOR *pColor );

//
// Hermite-spline:
// 
	BlendSplineTrench() ;

//
// Bounding Volume & Quadtree:
//
	xVec3	  m_vMin, m_vMax ;
	CQuadtree*	  m_pQT ;

//
// LERP:
//
	float*		  m_pfLERPHelper ;
	void LERPdY( float x, float z, float *pfY, BOOL *pbOverTerrain, xVec3 *pvOut );

//
// Spawn Positions:
//
	xVec3		 m_vSpawnPos[ MAX_SPAWN_POS ];
	int			  m_nSpawnPos ;

//
// Ground Patches:
//
	CTerrainPatch** m_pPatches ;
	
	int*			m_nPatchesRequiredLOD ;
	int*			m_dsPatchesRequiredTopEdge ;
	int*			m_dsPatchesRequiredLeftEdge ;
	int*			m_dsPatchesRequiredBottomEdge ;
	int*			m_dsPatchesRequiredRightEdge ;

//
// Patch Distance Sorting Memory:
//
	PATCH_SORT*		m_psPatches ;

//
// Per-frame LOD & visibility & light determinations:
//
	int			  m_nVisiblePatches ;
	BOOL*		  m_bPatchesPicked ;
	
	DisableItemLights( LPDIRECT3DDEVICE8 pDev, int nLightsToSpendOnItems, int nItemLightsStartIndex );
	DetermineRequiredLOD( CCamera* pCam );
	DistanceSortItemsAndSetLights( LPDIRECT3DDEVICE8 pDev, int nLights, int nStartLightIndex, CCamera* pCam );
	DetermineVisibility( LPDIRECT3DDEVICE8 pDev, CCamera *pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP );
	BOOL DetermineVisibilityForQuadtreeNode( CQuadtree::_QTNode *pQTNode, LPDIRECT3DDEVICE8 pDev, CCamera *pCam, xMatUA *pmatProj, D3DVIEWPORT8 *pVP );

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects( LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev );
	HRESULT	InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev );
    HRESULT Render( LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, CCamera *pCamera, xMatUA *pmatCurWorld, xMatUA *pmatProj, D3DVIEWPORT8 *pVP, float fFPS );
    HRESULT FrameMove( LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMatUA *pmatProj, D3DVIEWPORT8 *pVP );
} ;


#endif
