#pragma warning( disable : 4183 )

#include "CMenuItem.h"

CMenuItem::CMenuItem()
{
}
CMenuItem::~CMenuItem()
{
}
CMenuItem::CMenuItem(LPDIRECT3DTEXTURE8 pTex, float fTexRep, float fX, float fY, float fZ, float fWidth, float fHeight) :
		CScreenOverlay(pTex, fTexRep, fX, fY, fZ, fWidth, fHeight, OT_MENU)
{
	m_fUnselectedAlpha = 0.5f ;
	m_fSelectedAlpha = 0.95f ;
	m_bSelected = FALSE ;
}

CMenuItem::SetSelected(BOOL bSelected, OVERLAYVERTEX* _pVertices)
{
	HRESULT hr ;

	if (m_bSelected && bSelected)
		return S_OK ;

	m_bSelected = bSelected ;
	
	if (m_bSelected)
		m_fTimeSelected = 0.0f ;

//
// Change the vertex colors either to white or to something more fun,
// depending on the Selected status:
//
	//ods("CMenuItem::SetSelected - Lock: ") ;
    OVERLAYVERTEX* pVertices ; 
    hr = m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL) ;
	assert( !FAILED(hr) );
	//ods("OK\n") ;

    pVertices += m_dwIndexOffset ;
	
	if (m_bSelected)
		;//pVertices->color = D3DXCOLOR(0.5, 0.0f, 0.0f, m_fSelectedAlpha) ;
	else
		pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fUnselectedAlpha) ;
    pVertices++ ;

	if (m_bSelected)
		;//pVertices->color = D3DXCOLOR(0.0f, 0.5, 0.0f, m_fSelectedAlpha) ;
	else
		pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fUnselectedAlpha) ;
    pVertices++ ;

	if (m_bSelected)
		;//pVertices->color = D3DXCOLOR(0.5, 0.0f, 0.0f, m_fSelectedAlpha) ;
	else
		pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fUnselectedAlpha) ;
    pVertices++ ;

	if (m_bSelected)
		;//pVertices->color = D3DXCOLOR(0.0f, 0.5, 0.0f, m_fSelectedAlpha) ;
	else
		pVertices->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fUnselectedAlpha) ;
    pVertices++ ;

	//ods("CMenuItem::SetSelected - Unlock: ") ;
    hr = m_pSharedVB->Unlock();
	assert( !FAILED(hr) );
	//ods("OK\n") ;
	return S_OK ;
}

HRESULT CMenuItem::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect)
{
	ods( "CMenuItem::RestoreDeviceObjects\n" );

	CScreenOverlay::RestoreDeviceObjects(pDev, fAspect) ;

	return S_OK ;
}

HRESULT CMenuItem::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect, OVERLAYVERTEX* pVertices)
{
//
// Flash if selected
//
    if (m_bSelected)
	{
		m_fTimeSelected += 1.0f / (60.0f / fTimeMult) ;	// seconds

		/*OVERLAYVERTEX* pVertices ;
		if (FAILED(m_pSharedVB->Lock(0, 0, (BYTE**)&pVertices, NULL)))
			return E_FAIL ;*/
		pVertices += m_dwIndexOffset ;
		
		float fS = 0.25f * (float)((sin((double)m_fTimeSelected * 1000.0f * g_DEGTORAD) + 1.0f) / 2.0f) ;

		pVertices->color = D3DXCOLOR(0.75f + fS,0.0f,		0.0f,	m_fSelectedAlpha) ;
		pVertices++ ;

		pVertices->color = D3DXCOLOR(0.75f,		0.75f + fS,	0.0f,	m_fSelectedAlpha) ;
		pVertices++ ;

		pVertices->color = D3DXCOLOR(0.75f + fS,0.0f,		0.0f,	m_fSelectedAlpha) ;
		pVertices++ ;

		pVertices->color = D3DXCOLOR(0.75f,		0.75f + fS,	0.0f,	m_fSelectedAlpha) ;
		pVertices++ ;

		/*m_pSharedVB->Unlock();*/
	}

	return S_OK ;
}
