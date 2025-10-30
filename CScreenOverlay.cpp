#pragma warning( disable : 4183 )	// 'missing return type'

#include "CScreenOverlay.h"

CScreenOverlay::CScreenOverlay()
{
	m_bDrawingCanBeSkipped = FALSE ;
	m_pSharedVB = NULL ;
	m_dwIndexOffset = 0 ;

	m_vCenter = xVec3(0.0f, 0.0f, 0.0f) ;
	m_fHeight = 1.0f ;
	m_fWidth = 1.0f ;

	m_pTex = NULL ;
	m_fTexRep = 1.0f ;
	m_fTopX = 0.0f ;
	m_fTopY = 0.0f ;
	m_fWidth = 1.0f ;
	m_fHeight = 1.0f ;
	m_colTL = m_colTR = m_colBL = m_colBR = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );

	m_bActive = FALSE ;
	m_bFrameMoveRequired = FALSE ;

	m_matWorld = g_matIdent ;
	m_nTag = 0 ;
}

// basic, white, full alpha
CScreenOverlay::CScreenOverlay(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fX, float fY, float fZ, float fWidth, float fHeight, int nTag)
{
	m_bDrawingCanBeSkipped = FALSE ;
	m_pSharedVB = NULL ;
	m_dwIndexOffset = 0 ;

	m_vCenter = xVec3(0.0f, 0.0f, 0.0f) ;
	m_fHeight = 1.0f ;
	m_fWidth = 1.0f ;

	m_pTex = pTex ;
	m_fTexRep = fTexRep ;
	m_fTU = 0.0f ;
	m_fTV = 0.0f ;
	m_fTUExtent = 0.0 ;
	m_fTVExtent = 0.0 ;
	m_fTopX = fX ;
	m_fTopY = fY ;
	m_fTopZ = fZ ;
	m_fWidth = fWidth ;
	m_fHeight = fHeight ;
	m_colTL = m_colTR = m_colBL = m_colBR = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );

	m_bActive = FALSE ;
	m_bFrameMoveRequired = FALSE ;

	m_matWorld = g_matIdent ;
	m_nTag = nTag ;
}

// advanced, any colour + alpha
CScreenOverlay::CScreenOverlay(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fX, float fY, float fZ, float fWidth, float fHeight, D3DXCOLOR* colTL, D3DXCOLOR* colTR, D3DXCOLOR* colBL, D3DXCOLOR* colBR, int nTag )
{
	m_bDrawingCanBeSkipped = FALSE ;
	m_pSharedVB = NULL ;
	m_dwIndexOffset = 0 ;

	m_vCenter = xVec3(0.0f, 0.0f, 0.0f) ;
	m_fHeight = 1.0f ;
	m_fWidth = 1.0f ;

	m_pTex = pTex ;
	m_fTexRep = fTexRep ;
	m_fTU = 0.0f ;
	m_fTV = 0.0f ;
	m_fTUExtent = 0.0 ;
	m_fTVExtent = 0.0 ;
	m_fTopX = fX ;
	m_fTopY = fY ;
	m_fTopZ = fZ ;
	m_fWidth = fWidth ;
	m_fHeight = fHeight ;
	m_colTL = *colTL ;
	m_colTR = *colTR ;
	m_colBL = *colBL ;
	m_colBR = *colBR ;

	m_bActive = FALSE ;
	m_bFrameMoveRequired = FALSE ;

	m_matWorld = g_matIdent ;
	m_nTag = nTag ;
}

// for portions of the full texture
CScreenOverlay::CScreenOverlay(LPDIRECT3DTEXTURE8 pTex, float fTU, float fTV, float fTUExtent, float fTVExtent, float fX, float fY, float fZ, float fWidth, float fHeight, int nTag)
{
	m_bDrawingCanBeSkipped = FALSE ;
	m_pSharedVB = NULL ;
	m_dwIndexOffset = 0 ;

	m_vCenter = xVec3(0.0f, 0.0f, 0.0f) ;
	m_fHeight = 1.0f ;
	m_fWidth = 1.0f ;

	m_pTex = pTex ;
	m_fTexRep = 0.0f ;
	m_fTU = fTU ;
	m_fTV = fTV ;
	m_fTUExtent = fTUExtent ;
	m_fTVExtent = fTVExtent ;
	m_fTopX = fX ;
	m_fTopY = fY ;
	m_fTopZ = fZ ;
	m_fWidth = fWidth ;
	m_fHeight = fHeight ;
	m_colTL = m_colTR = m_colBL = m_colBR = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );

	m_bActive = FALSE ;
	m_bFrameMoveRequired = FALSE ;

	m_matWorld = g_matIdent ;
	m_nTag = nTag ;
}
CScreenOverlay::~CScreenOverlay()
{
}

HRESULT CScreenOverlay::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect)
{
#ifdef FULL_DBG_TRACE
	ods( "CScreenOverlay::RestoreDeviceObjects\n" );
#endif

	HRESULT hr ;

	/*char sz[256] ;
	sprintf(sz, "pVB=%p; dwIndexOffset=%d", m_pSharedVB, (int)m_dwIndexOffset) ;
	MessageBox(NULL, "Baseclass RDO", sz, MB_OK) ;*/

//
// Fill the shared VB with our data, at the index specified
// by the overlay manager.
//
	//ods("CScreenOverlay::RestoreDeviceObjects - Lock: ") ;
    OVERLAYVERTEX* pVertices ; 
	hr = m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL) ;
	assert( !FAILED(hr) );
	//ods("OK\n") ;

    pVertices += m_dwIndexOffset ;
	
	float fVertSize = 1.0f * m_fHeight ;
	float fHorizSize = 1.0f * m_fWidth ;

// bottom-left
	pVertices->v.x   = (-0.5f + (m_fTopX))* fAspect ; 				
	pVertices->v.y   = +0.5f -(m_fTopY);
	pVertices->v.z   = m_fTopZ ;
	pVertices->color = m_colBL ;
	if( m_fTexRep != 0.0f )
	{
		pVertices->tu = 0.0f ;
		pVertices->tv = 0.0f ;
	}
	else
	{
		pVertices->tu = m_fTU ;
		pVertices->tv = m_fTV ;
	}
    pVertices++ ;

// bottom-right
	pVertices->v.x   = (-0.5f + (m_fTopX) + fHorizSize)* fAspect ;	
	pVertices->v.y   = +0.5f - (m_fTopY);
	pVertices->v.z   = m_fTopZ ;
	pVertices->color = m_colBR ;
	if( m_fTexRep != 0.0f )
	{
		pVertices->tu = m_fTexRep ;
		pVertices->tv = 0.0f ;
	}
	else
	{
		pVertices->tu = m_fTU + m_fTUExtent ;
		pVertices->tv = m_fTV ;
	}
    pVertices++ ;

// top-left	
	pVertices->v.x   = (-0.5f + (m_fTopX))* fAspect ;				
	pVertices->v.y   = +0.5f - (m_fTopY) - fVertSize ;
	pVertices->v.z   = m_fTopZ ;
	pVertices->color = m_colTL ;
	if( m_fTexRep != 0.0f )
	{
		pVertices->tu = 0.0f ;
		pVertices->tv = m_fTexRep ;
	}
	else
	{
		pVertices->tu = m_fTU ;
		pVertices->tv = m_fTV + m_fTVExtent ;
	}
    pVertices++ ;

// top-right
	pVertices->v.x   = (-0.5f + (m_fTopX) + fHorizSize)* fAspect ;	
	pVertices->v.y   = +0.5f - (m_fTopY) - fVertSize;
	pVertices->v.z   = m_fTopZ ;
	pVertices->color = m_colTR ;
	if( m_fTexRep != 0.0f )
	{
		pVertices->tu = m_fTexRep ;
		pVertices->tv = m_fTexRep ;
	}
	else
	{
		pVertices->tu = m_fTU + m_fTUExtent ;
		pVertices->tv = m_fTV + m_fTVExtent ;
	}
    pVertices++ ;

	//ods("CScreenOverlay::RestoreDeviceObjects - Unlock: ") ;
	hr = m_pSharedVB->Unlock();
	assert( !FAILED(hr) );
	//ods("OK\n") ;
	
	return S_OK ;
}
HRESULT CScreenOverlay::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices)
{
	return S_OK ;
}
HRESULT CScreenOverlay::Render(LPDIRECT3DDEVICE8 pDev)
{
	pDev->SetTransform(D3DTS_WORLD, &m_matWorld) ;
	pDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_dwIndexOffset, 2) ;
	return S_OK ;
}
