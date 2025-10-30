#pragma warning( disable : 4183 )	// 'missing return type'

#include "CMenuScrollingBackG.h"

CMenuScrollingBackG::CMenuScrollingBackG()
{
}
CMenuScrollingBackG::~CMenuScrollingBackG()
{
}
CMenuScrollingBackG::CMenuScrollingBackG(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fMoveSpeed, float fAlpha, float fX, float fY, float fZ, float fWidth, float fHeight) :
		CScreenOverlay(pTex, fTexRep, fX, fY, fZ, fWidth, fHeight, OT_MENU)
{
	m_fMoveSpeed = fMoveSpeed ;
	m_fAlpha = fAlpha ;
}

HRESULT CMenuScrollingBackG::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect)
{
	ods( "CMenuScrollingBackG::RestoreDeviceObjects\n" );

	HRESULT hr ;

	CScreenOverlay::RestoreDeviceObjects(pDev, fAspect) ;

	m_fLERPDone = 0.0f ;

//
// Change the vertex colors from white to something more fun.
//
	//ods("CMenuScrollingBackG::RestoreDeviceObjects - Lock: ") ;
    OVERLAYVERTEX* pVertices ; 
    hr = m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL) ;
	assert( !FAILED(hr) );
	//ods("OK\n") ;

    pVertices += m_dwIndexOffset ;
	
	pVertices->color = D3DXCOLOR(rnd(), rnd(), rnd(), m_fAlpha) ;
    pVertices++ ;
	pVertices->color = D3DXCOLOR(rnd(), rnd(), rnd(), m_fAlpha) ;
    pVertices++ ;
	pVertices->color = D3DXCOLOR(rnd(), rnd(), rnd(), m_fAlpha) ;
    pVertices++ ;
	pVertices->color = D3DXCOLOR(rnd(), rnd(), rnd(), m_fAlpha) ;
    pVertices++ ;

	//ods("CMenuScrollingBackG::RestoreDeviceObjects - Unlock: ") ;
    hr = m_pSharedVB->Unlock();
	assert( !FAILED(hr) );
	//ods("OK\n") ;

	return S_OK ;
}

HRESULT CMenuScrollingBackG::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices)
{
	D3DXCOLOR c1, c2, c3, c4, cN ;
	float fLERP ;

//
// LERP the vertex color vales, and cycle tex. coords.
//
    /*OVERLAYVERTEX* pVertices ; 
    if (FAILED(m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL)))
        return E_FAIL ;*/
    pVertices += m_dwIndexOffset ;
	
	c1 = (pVertices+0)->color ;
	c2 = (pVertices+1)->color ;
	c3 = (pVertices+2)->color ;
	c4 = (pVertices+3)->color ;

	fLERP = 1.0f / (60.0f * 1.0f) ;	// 1 sec. cycle
	fLERP *= fTimeMult ;
	if (m_fLERPDone >= 1.0f)
		m_fLERPDone = 0.0f ;
	if (m_fLERPDone == 0.0f)
	{
		m_c1 = D3DXCOLOR(0.0f, 0.5f + rnd() * 0.5f, 0.5f + rnd() * 0.5f, m_fAlpha) ;
		m_c2 = D3DXCOLOR(0.5f + rnd() * 0.5f, 1.0f, 0.5f + rnd() * 0.5f, m_fAlpha) ;
		m_c3 = D3DXCOLOR(0.5f + rnd() * 0.5f, 0.0f, 1.0f, m_fAlpha) ;
		m_c4 = D3DXCOLOR(0.5f + rnd() * 0.5f, 0.5f + rnd() * 0.5f, rnd(), m_fAlpha) ;
		m_co1 = c1 ;
		m_co2 = c2 ;
		m_co3 = c3 ;
		m_co4 = c4 ;
	}

	D3DXColorLerp(&cN, &m_co1, &m_c1, m_fLERPDone) ;
	(pVertices+0)->color = cN ;
	(pVertices+0)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	D3DXColorLerp(&cN, &m_co2, &m_c2, m_fLERPDone) ;
	(pVertices+1)->color = cN ;
	(pVertices+1)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	D3DXColorLerp(&cN, &m_co3, &m_c3, m_fLERPDone) ;
	(pVertices+2)->color = cN ;
	(pVertices+2)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;
	D3DXColorLerp(&cN, &m_co4, &m_c4, m_fLERPDone) ;
	(pVertices+3)->color = cN ;
	(pVertices+3)->tu += 0.01f * fTimeMult * m_fMoveSpeed ;

	m_fLERPDone += fLERP ;
    /*m_pSharedVB->Unlock();*/

	return S_OK ;
}
