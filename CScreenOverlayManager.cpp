#pragma warning( disable : 4183 )	// 'missing return type'

#include "CScreenOverlayManager.h"

CScreenOverlayManager::CScreenOverlayManager()
{
	m_pVB = NULL ;
	m_pIB = NULL ;
	m_pVBVertices = NULL ;
	m_bVBLocked = FALSE ;

	m_dwNumOverlays = 0 ;

	ZeroMemory( &m_mtrlDef, sizeof(D3DMATERIAL8) );
	m_mtrlDef.Diffuse.r = m_mtrlDef.Ambient.r = 1.0f;
	m_mtrlDef.Diffuse.g = m_mtrlDef.Ambient.g = 1.0f;
	m_mtrlDef.Diffuse.b = m_mtrlDef.Ambient.b = 1.0f;
	m_mtrlDef.Diffuse.a = m_mtrlDef.Ambient.a = 1.0f;

	m_matWorldIdent = g_matIdent ;

	for( int i = 0 ; i < MAX_OVERLAYS ; i++ )
		m_pOverlays[ i ] = NULL ;
}
CScreenOverlayManager::~CScreenOverlayManager()
{
}

CScreenOverlayManager::RegisterNewOverlay( CScreenOverlay *pOverlay )
{
	pOverlay->m_dwIndexOffset = (m_dwNumOverlays+1) * 4 ;

	m_pOverlays[ m_dwNumOverlays ] = pOverlay ;
	m_dwNumOverlays++ ;

	assert( m_dwNumOverlays < MAX_OVERLAYS );
}

/*CScreenOverlayManager::UnregisterOverlay(CScreenOverlay *pOverlay)
{
	for( DWORD i = 0 ; i < m_dwNumOverlays ; i++ )
		if( m_pOverlays[ i ] == pOverlay )
		{
			for( DWORD j = i + 1 ; j < m_dwNumOverlays ; j++ )
				m_pOverlays[ j-1 ] = m_pOverlays[ j ];
			
			m_dwNumOverlays-- ;
			break ;
		}
}*/

HRESULT CScreenOverlayManager::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect)
{
	ods( "CScreenOverlayManager::RestoreDeviceObjects\n" );

//
// Create common VB:
//
	HRESULT hr ;
    hr = pDev->CreateVertexBuffer( MAX_OMVB_VERTS*sizeof(OVERLAYVERTEX),
                                   D3DUSAGE_WRITEONLY,
								   D3DFVF_OVERLAYVERTEX,
                                   D3DPOOL_MANAGED,
								   &m_pVB );
    if( FAILED(hr) )
        return E_FAIL ;
	
	m_bVBLocked = FALSE ;

//
// Create the IB:
//
	/*hr = pDev->CreateIndexBuffer( sizeof(WORD) * MAX_OVERLAYS * 3,
								  D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
								  D3DFMT_INDEX16,
								  D3DPOOL_DEFAULT,
								  &m_pIB );
	if( FAILED(hr) )
        return E_FAIL ;*/

//
// Restore managed overlays:
//
	for (DWORD i = 0 ; i < m_dwNumOverlays ; i ++)
	{
		if( m_pOverlays[ i ] )
		{
			m_pOverlays[i]->m_pSharedVB = m_pVB ;
			m_bVBLocked = TRUE ;
			m_pOverlays[i]->RestoreDeviceObjects( pDev, fAspect );
			m_bVBLocked = FALSE ;
		}
	}

	return S_OK ;
}

void CScreenOverlayManager::SetProjMat( LPDIRECT3DDEVICE8 pDev )
{
//
// Set View Matrix for Overlays (assumes 45deg. FOV)
//
	xVec3 vEyePt, vLookAtPt, vUp ;
	xMat  matView ;
	float a = 90.0f ;
	float b = 45.0f / 2.0f ;
	float c = 180.0f - a - b ;
	float x = 1.0f / 2.0f ;
	float h = x / sinf(b * g_DEGTORAD) ;
	float r = sqrtf((h * h) - (x * x)) ;
	vEyePt = xVec3(0.0f, 0.0f, -r) ;
	vLookAtPt = xVec3(0.0f, 0.0f, 0.0f) ;
	vUp = xVec3(0.0f, 1.0f, 0.0f) ;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookAtPt, &vUp) ;
	pDev->SetTransform(D3DTS_VIEW, &matView) ;
}

HRESULT CScreenOverlayManager::Render(LPDIRECT3DDEVICE8 pDev, int nTag, int nMethod)
{
	HRESULT hr ;
	
	if(	m_bVBLocked )
		return S_OK ;

	//pDev->SetMaterial(&m_mtrlDef) ;
	//pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

	pDev->SetVertexShader( D3DFVF_OVERLAYVERTEX );
	pDev->SetStreamSource( 0, m_pVB, sizeof(OVERLAYVERTEX) );

//
// Render managed overlays:
//

// method A; multiple DrawPrimitive. Sloooowwww.
// always render with this method;
	if( 1 /*nMethod == eDrawSlow*/ )
	{
		for( DWORD i = 0 ; i < m_dwNumOverlays ; i ++ )
		{
			if( m_pOverlays[ i ] )
				if (
					m_pOverlays[i]->m_bActive &&
					nTag == m_pOverlays[i]->m_nTag && 
					m_pOverlays[i]->m_bDrawingCanBeSkipped == FALSE
				   )
				{
					pDev->SetTexture( 0, m_pOverlays[i]->m_pTex );
					if( m_bVBLocked )
						goto done ;
					hr = m_pOverlays[i]->Render( pDev ) ;
					assert( hr==D3D_OK );
				}
		}
	}

//
// method B; single DrawIndexedPrimitive. Faster?
// doesn't work properly yet; may well not be worth the effort?
//
	/*if( 0 //nMethod == eDrawFast )
	{
		BYTE *pbIndices ;
		WORD *pIndices ;
		m_pIB->Lock( 0, 0, &pbIndices, D3DLOCK_DISCARD );
		assert( pbIndices != NULL );
		
		pIndices = (WORD*)pbIndices ;

		DWORD MinIndex = 99999, MaxIndex = 0 ;
		DWORD nPolyCount = 0 ;
		
		for( DWORD i = 0 ; i < m_dwNumOverlays ; i ++ )
		{
			if( m_pOverlays[ i ] )
				if (
					m_pOverlays[i]->m_bActive &&
					nTag == m_pOverlays[i]->m_nTag && 
					m_pOverlays[i]->m_bDrawingCanBeSkipped == FALSE
				   )
				{
					*pIndices = (WORD)m_pOverlays[i]->m_dwIndexOffset ;
					if( *pIndices < MinIndex ) MinIndex = *pIndices ;
					if( *pIndices > MaxIndex ) MaxIndex = *pIndices ;
					pIndices++ ;

					*pIndices = (WORD)m_pOverlays[i]->m_dwIndexOffset + 1 ;
					if( *pIndices < MinIndex ) MinIndex = *pIndices ;
					if( *pIndices > MaxIndex ) MaxIndex = *pIndices ;
					pIndices++ ;

					*pIndices = (WORD)m_pOverlays[i]->m_dwIndexOffset + 2 ;
					if( *pIndices < MinIndex ) MinIndex = *pIndices ;
					if( *pIndices > MaxIndex ) MaxIndex = *pIndices ;
					pIndices++ ;

					*pIndices = (WORD)m_pOverlays[i]->m_dwIndexOffset + 3 ;
					if( *pIndices < MinIndex ) MinIndex = *pIndices ;
					if( *pIndices > MaxIndex ) MaxIndex = *pIndices ;
					pIndices++ ;

					nPolyCount += 2 ;
				}
		}
		m_pIB->Unlock() ;
		if( nPolyCount > 0 )
		{
			pDev->SetTransform(D3DTS_WORLD, &m_matWorldIdent) ;
			pDev->SetIndices( m_pIB, 0 );
			pDev->SetStreamSource( 0, m_pVB, sizeof(OVERLAYVERTEX) );
			pDev->SetVertexShader( D3DFVF_OVERLAYVERTEX );
			if( pDev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP,
											MinIndex,
											MaxIndex - MinIndex,
											0,
											nPolyCount ) != D3D_OK )
				Msg("DrawIndexedPrimitive FAILURE 2!!!") ;
		}
	}*/

done:
	//pDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	return S_OK ;
}

HRESULT CScreenOverlayManager::LockVB()
{
	//ods("CScreenOverlayManager::LockVB()\n") ;
	m_bVBLocked = TRUE ;
	HRESULT hr ;
	hr = m_pVB->Lock( 0, 0, (BYTE**)&m_pVBVertices, 0 );
	assert( !FAILED(hr) );
	return S_OK ;
}

HRESULT CScreenOverlayManager::UnlockVB()
{
	if( m_bVBLocked )
	{
		//ods("CScreenOverlayManager::UnlockVB()\n") ;
		HRESULT hr ;
		hr = m_pVB->Unlock() ;
		assert( !FAILED(hr) );
		m_bVBLocked = FALSE ;
	}
	return S_OK ;
}

HRESULT CScreenOverlayManager::FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect)
{
	DWORD i ;

//
// Managed overlays may require a FrameMove:
//
	for( i = 0 ; i < m_dwNumOverlays ; i++ )
		if( m_pOverlays[i]->m_bActive )
			if( m_pOverlays[i]->m_bActive &&
				m_pOverlays[i]->m_bFrameMoveRequired )
			{
				m_pOverlays[i]->FrameMove( pDev, fTimeMult, fAspect, m_pVBVertices );
			}

	return S_OK ;
}

HRESULT CScreenOverlayManager::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB )
	SAFE_RELEASE( m_pIB )
	m_dwNumOverlays = 0 ;
	return S_OK ;
}
