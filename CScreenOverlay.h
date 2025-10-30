#ifndef __SCREENOVERLAY__
#define __SCREENOVERLAY__

#include <stdio.h>
#include <D3DX8.h>
#include "x7_utils.h"

/*class CScreenOverlayString ;
class CMainMenu ;
class Cx7 ;*/

//-----------------------------------------------------------------------------
// Custom vertex types
//-----------------------------------------------------------------------------
struct OVERLAYVERTEX
{
    xVec3 v;
    D3DCOLOR    color;
    FLOAT       tu;
    FLOAT       tv;
};
#define D3DFVF_OVERLAYVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class CScreenOverlay
{
//
// Construction / Destruction:
//
public:
	CScreenOverlay() ;
	CScreenOverlay(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fX, float fY, float fZ, float fWidth, float fHeight, int nTag) ;
	CScreenOverlay(LPDIRECT3DTEXTURE8 pTex, float fTU, float fTV, float fUExtent, float fTVExent, float fX, float fY, float fZ, float fWidth, float fHeight, int nTag) ;
	CScreenOverlay(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fX, float fY, float fZ, float fWidth, float fHeight, D3DXCOLOR* colTL, D3DXCOLOR* colTR, D3DXCOLOR* colBL, D3DXCOLOR* colBR, int nTag) ;
	
	~CScreenOverlay() ;

//
// inherited classes have access to protected members
// (friend declaration grants access to unrelated classes)
//
	LPDIRECT3DVERTEXBUFFER8 m_pSharedVB ;
	DWORD					m_dwIndexOffset ;
	xVec3				m_vCenter ;
	LPDIRECT3DTEXTURE8		m_pTex ;
	float					m_fTexRep ;
	float					m_fTU, m_fTV, m_fTUExtent, m_fTVExtent ;
	D3DXCOLOR				m_colTL, m_colTR, m_colBL, m_colBR ;
	float					m_fTopX ;
	float					m_fTopY ;
	float					m_fTopZ ;
	float					m_fWidth ;
	float					m_fHeight ;

	BOOL					m_bActive ;

	xMat				m_matWorld ;

	int						m_nTag ;
	BOOL					m_bDrawingCanBeSkipped ;

	BOOL				    m_bFrameMoveRequired ;

//
// Framework events:
//
public:
	virtual HRESULT Render(LPDIRECT3DDEVICE8 pDev) ;
    virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
	virtual HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices) ;
    HRESULT InvalidateDeviceObjects(LPDIRECT3DDEVICE8 pDev);

} ;

#endif