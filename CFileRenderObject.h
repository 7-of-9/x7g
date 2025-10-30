#ifndef __CFileRenderObject__
#define __CFileRenderObject__

#include <D3DX8Math.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "CTextures.h"
#include "CBoundingBox.h"

class CFileRenderObject
{
public:

//
// RENDER_DATA_INFO:
//
	struct RENDER_DATA_INFO
	{
		char				 m_szFile[256] ;
		int					 m_nResID ;
		LPD3DXMESH			 m_pMesh ;
		D3DVERTEXBUFFER_DESC m_vbdVBDesc ;
		DWORD				 m_dwNumVertices ;
		D3DMATERIAL8*		 m_mtrlMeshMaterials ;
		DWORD				 m_dwNumMaterials ;
		DWORD				 m_dwNumFaces ;
		LPD3DXBUFFER		 m_pAdjacencyBuffer ;

		float				 m_fInitialBSphereRadius ;
		float				 m_fBSphereRadius ;
		xVec3			 m_vInitialMin, m_vInitialMax ;

		D3DXATTRIBUTERANGE*  m_pAttrTable ;
		DWORD				 m_dwNumAttr ;

	    DWORD				 m_dwShaderRipple ;
	} ;
	RENDER_DATA_INFO*		 m_pRenderData ;
	BOOL					 m_bRenderDataIsMine ;

//
// Construction / Destruction:
//
	 CFileRenderObject( char *pszFile, int nResID, RENDER_DATA_INFO* pSharedRenderData );
	~CFileRenderObject() ;


//
// D3DX Mesh, BBox & BSphere Creation Layer:
//
	HRESULT MakeRenderDataAndBoundingData(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;

//
// Bounding Box & Sphere:
//
	CBoundingBox *m_pBBox ;
	HRESULT MakeBoundingBoxAndSphere(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev, LPD3DXMESH pReadableMesh) ;

//
// World Matrix:
//
	/*void WorldRotateX(float r, xVec3 *pv) ;
	void WorldRotateY(float r, xVec3 *pv) ;
	void WorldRotateZ(float r, xVec3 *pv) ;
	void WorldRotateAxis(float r, xVec3 *pv, xVec3 *pvAxis) ;
	void WorldTranslate(xVec3 *pv) ;*/
	
	void SetLargestSide( float fLargestSide );
	void WorldSetMatrix( xMatUA *m, float fScale );
	void WorldSetMatrixIncremental( xMatUA *pmatFull, xMatUA *pmatTransDeltaInc, xMatUA *pmatRotAboutCenter, float fScale );

	xMat m_matRot ;
	xMat m_matWorld ;
	xMat m_matScale ;

	float	   m_fLargestSide ;
	float	   m_fScaledLargestSide ;
	float	   m_fScale ;

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT	InvalidateDeviceObjects( LPDIRECT3DDEVICE8 pDev );

//
// Render Methods:
//
	HRESULT Render(LPDIRECT3DDEVICE8 pDev, CTextures *pTex, unsigned long *pulPolyCount, BOOL bUseMeshMaterials, BOOL bUseDefMaterial, xMatUA* pmatCurWorld) ;
	HRESULT RenderVS( LPDIRECT3DDEVICE8 pDev, float fTime, unsigned long *pulPolyCount, BOOL bUseMeshMaterials, BOOL bUseDefMaterial, xMatUA* pmatCurWorld, const xMatUA& matView, const xMatUA& matProj, CCamera* pCam, const xVec3& vLightDir );
    D3DMATERIAL8 m_matDef ;
} ;

#endif