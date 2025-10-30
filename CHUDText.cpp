#pragma warning( disable : 4183 )

#include "CHUDText.h"

	CHUDText::CHUDText( char* szText, int nTex, float fX, float fY, float fWidth, float fHeight, float fOverlap, int OverlayType )
	{
		strcpy( m_szText, szText );
		m_nChars = strlen( szText );
		m_fX = fX ;
		m_fY = fY ;
		m_fWidth = fWidth ;
		m_fHeight = fHeight ;
		m_fOverlap = fOverlap ;
		m_nTex = nTex ;
		m_OverlayType = OverlayType ;
		
		m_ppChars = new CScreenOverlay*[ m_nChars ];
		for (int i = 0 ; i < m_nChars ; i ++)
			m_ppChars[i] = NULL ;

		m_bFrameMoveRequired = FALSE ;
		m_bActive = FALSE ;
	}

	CHUDText::~CHUDText()
	{
		SAFE_DELETE_ARRAY( m_ppChars )
	}

	CHUDText::Enable( BOOL bActive )
	{
		m_bActive = bActive ;
		for (int i = 0 ; i < m_nChars ; i ++)
			if (m_ppChars[i])
				m_ppChars[i]->m_bActive = bActive ;
	}

	CHUDText::SetFrameMoveRequired( BOOL bRequired )
	{
		m_bFrameMoveRequired = bRequired ;
	//
	// no need to set FM required on these base Overlays,
	// FM is handled by this class.
	//
	// this still leads to the VB being locked multiple times per frame,
	// once per instance of this class, and once by the OM. Bad.
	//
		/*for (int i = 0 ; i < m_nChars ; i ++)
			if (m_ppChars[i])
				m_ppChars[i]->m_bFrameMoveRequired = bRequired ;*/
	}

	HRESULT CHUDText::RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect, CScreenOverlayManager* pOM, CTextures* pTex)
	{
		ods( "CHUDText::RestoreDeviceObjects\n" );
		float fX, fY ;
		
		pOM->m_bVBLocked = TRUE ;

		fX = m_fX ;
		fY = m_fY ;
		//ods("CHUDText::RestoreDeviceObjects: BEGIN\n") ;
		for (int i = 0 ; i < m_nChars ; i ++)
		{
			char  cChar = m_szText[ i ];
			float fSize = 0.0625f ;
			float col = (float)((cChar) % 16) ;
			float row = (float)floor((double)(cChar) / 16) ;

			m_ppChars[ i ] = new CScreenOverlay( pTex->m_pTex[ m_nTex ], col * fSize, row * fSize, fSize, fSize, fX, fY, 0.0f, m_fWidth, m_fHeight, m_OverlayType );
			pOM->RegisterNewOverlay( m_ppChars[ i ] );
			m_ppChars[ i ]->m_pSharedVB = pOM->m_pVB ;
			m_ppChars[ i ]->RestoreDeviceObjects( pDev, fAspect );

			fX += m_fWidth - m_fOverlap ;
		}
		//ods("CHUDText::RestoreDeviceObjects: END\n") ;

		pOM->m_bVBLocked = FALSE ;
		return S_OK ;
	}

	HRESULT CHUDText::FrameMove(LPDIRECT3DDEVICE8 pDev, /*float fTimeMult,*/ CUserInput* pIn, OVERLAYVERTEX* pVBVertices)
	{
		if( m_bFrameMoveRequired )
		{
			m_bFrameMoveRequired = FALSE ;

			assert( (signed)strlen(m_szText) == m_nChars );

			/*OVERLAYVERTEX* pVBVertices ;
			if (FAILED(m_ppChars[ 0 ]->m_pSharedVB->Lock(0, 0, (BYTE**)&pVBVertices, 0)))
				return E_FAIL ;*/

			for(int i = 0 ; i < m_nChars ; i ++)
			{
				char  cChar = m_szText[ i ];
				if( cChar == ' ' )
				{
					m_ppChars[ i ]->m_bDrawingCanBeSkipped = TRUE ;
				}
				else
				{
					m_ppChars[ i ]->m_bDrawingCanBeSkipped = FALSE ;

					float fSize = 0.0625f ;
					float col = (float)((cChar) % 16) ;
					float row = (float)floor((double)(cChar) / 16) ;
					float fTU = col * fSize ;
					float fTV = row * fSize ;
					float fTUExtent = fSize ;
					float fTVExtent = fSize ;

					OVERLAYVERTEX *pVertices ;
					pVertices = pVBVertices ;
					pVertices += m_ppChars[ i ]->m_dwIndexOffset ;
					
					pVertices->tu = fTU ;
					pVertices->tv = fTV ;
					pVertices++ ;

					pVertices->tu = fTU + fTUExtent ;
					pVertices->tv = fTV ;
					pVertices++ ;

					pVertices->tu = fTU ;
					pVertices->tv = fTV + fTVExtent ;
					pVertices++ ;

					pVertices->tu = fTU + fTUExtent ;
					pVertices->tv = fTV + fTVExtent ;
					pVertices++ ;
				}
			}

		// unlock VB
			/*m_ppChars[ 0 ]->m_pSharedVB->Unlock();*/
		}

		return S_OK ;
	}

	HRESULT CHUDText::InvalidateDeviceObjects()
	{
		for (int i = 0 ; i < m_nChars ; i ++)
			SAFE_DELETE( m_ppChars[ i ] ) 
		
		return S_OK ;
	}

