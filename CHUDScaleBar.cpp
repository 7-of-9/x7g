#pragma warning( disable : 4183 )

#include "CHUDScaleBar.h"

CHUDScaleBar::CHUDScaleBar()
{
}
CHUDScaleBar::~CHUDScaleBar()
{
}
CHUDScaleBar::CHUDScaleBar(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fMoveSpeed, float fRed, float fGreen, float fBlue, float fAlpha, BOOL bFade, float fX, float fY, float fZ, float fWidth, float fHeight) :
		CScreenOverlay(pTex, fTexRep, fX, fY, fZ, fWidth, fHeight, OT_HUD_SCALEBAR)
{
	m_fMoveSpeed = fMoveSpeed ;
	m_fRed = fRed ;
	m_fGreen= fGreen ;
	m_fBlue = fBlue ;
	m_fAlpha = fAlpha ;
	m_fLevel = 1.0f ;
	m_bFade = bFade ;
	m_bFrameMoveRequired = FALSE ;
}

HRESULT CHUDScaleBar::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect)
{
	HRESULT hr ;

#ifdef FULL_DBG_TRACE
	ods( "CHUDScaleBar::RestoreDeviceObjects\n" );
#endif

	CScreenOverlay::RestoreDeviceObjects(pDev, fAspect) ;

//
// Change the vertex colors from white to blue.
//
    //ods("CHUDScaleBar::RestoreDeviceObjects - Lock: ");
	OVERLAYVERTEX* pVertices ; 
	hr = m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL) ;
	assert( !FAILED(hr) );
    //ods("OK\n");

    pVertices += m_dwIndexOffset ;
	
	if( m_bFade )
		pVertices->color = D3DXCOLOR(m_fRed * 0.25f + 0.75f, m_fGreen * 0.25f + 0.75f, m_fBlue * 0.25f + 0.75f, m_fAlpha) ;
	else
		pVertices->color = D3DXCOLOR(m_fRed, m_fGreen, m_fBlue, m_fAlpha) ;
    pVertices++ ;

	pVertices->color = D3DXCOLOR(m_fRed, m_fGreen, m_fBlue, m_fAlpha) ;
    pVertices++ ;

	if( m_bFade )
		pVertices->color = D3DXCOLOR(m_fRed * 0.25f + 0.75f, m_fGreen * 0.25f + 0.75f, m_fBlue * 0.25f + 0.75f, m_fAlpha) ;
	else
		pVertices->color = D3DXCOLOR(m_fRed, m_fGreen, m_fBlue, m_fAlpha) ;
    pVertices++ ;

	pVertices->color = D3DXCOLOR(m_fRed, m_fGreen, m_fBlue, m_fAlpha) ;
    pVertices++ ;

    //ods("CHUDScaleBar::RestoreDeviceObjects - Unlock: ");
    hr = m_pSharedVB->Unlock();
	assert( !FAILED(hr) );
    //ods("OK\n");

	return S_OK ;
}

HRESULT CHUDScaleBar::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices)
{
    if( m_bFrameMoveRequired )
	{
		m_bFrameMoveRequired = FALSE ;

		/*OVERLAYVERTEX* pVertices ; 
		if (FAILED(m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, 0)))
			return E_FAIL ;*/
		pVertices += m_dwIndexOffset ;
		
		float fVertSize = 1.0f * m_fHeight ;
		float fHorizSize = 1.0f * m_fWidth * m_fLevel ;

		pVertices->v.x   = (-0.5f + (m_fTopX))* fAspect ;				// bottom-left
		pVertices->v.y   = +0.5f -(m_fTopY);
		pVertices->v.z   = m_fTopZ ;
		pVertices->tu    = 0.0f ;
		pVertices->tv    = 0.0f ;
		pVertices++ ;

		pVertices->v.x   = (-0.5f + (m_fTopX) + fHorizSize)* fAspect ;	// bottom-right
		pVertices->v.y   = +0.5f - (m_fTopY);
		pVertices->v.z   = m_fTopZ ;
		pVertices->tu    = m_fTexRep ;
		pVertices->tv    = 0.0f ;
		pVertices++ ;

		pVertices->v.x   = (-0.5f + (m_fTopX))* fAspect ;				// top-left
		pVertices->v.y   = +0.5f - (m_fTopY) - fVertSize ;
		pVertices->v.z   = m_fTopZ ;
		pVertices->tu    = 0.0f ;
		pVertices->tv    = m_fTexRep ;
		pVertices++ ;

		pVertices->v.x   = (-0.5f + (m_fTopX) + fHorizSize)* fAspect ;	// top-right
		pVertices->v.y   = +0.5f - (m_fTopY) - fVertSize;
		pVertices->v.z   = m_fTopZ ;
		pVertices->tu    = m_fTexRep ;
		pVertices->tv    = m_fTexRep ;
		pVertices++ ;

		/*m_pSharedVB->Unlock();*/
	}
	
	/*(pVertices+0)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	(pVertices+0)->color = D3DXCOLOR(0.0f, 0.0f, 1.0f, m_fLevel + 0.2f) ;

	(pVertices+1)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	(pVertices+1)->color = D3DXCOLOR(0.0f, 0.0f, 1.0f, m_fLevel/2.0f) ;

	(pVertices+2)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	(pVertices+2)->color = D3DXCOLOR(0.0f, 0.0f, 1.0f, m_fLevel + 0.2f) ;

	(pVertices+3)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	(pVertices+3)->color = D3DXCOLOR(0.0f, 0.0f, 1.0f, m_fLevel/2.0f) ;*/

	return S_OK ;
}
