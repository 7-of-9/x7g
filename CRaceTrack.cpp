#include "CRaceTrack.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CRaceTrack::CRaceTrack()
	{
		m_nSegs = 0 ;
		
		CreateDefinition(FALSE, FALSE) ;

		D3DXVECTOR3 vEnd ;
		float fEndWidth ;
		float fEndOrientation ;

		D3DXVECTOR3 vStart ;
		float fStartOrientation ;
		float fStartWidth ;

		vStart = tdefTest.vStart ;
		fStartOrientation = 0.0f ;
		fStartWidth = tdefTest.fStartWidth ;
		for (int i = 0 ; i < tdefTest.nSegs ; i ++)
		{
			m_pSegs[i] = new CRaceTrackSegment((float)tdefTest.nSide[i],
											   tdefTest.nSteps[i],
											   tdefTest.fA[i],
											   tdefTest.fB[i],
											   tdefTest.fSineStart[i],
											   tdefTest.fSinePerc[i],
											   tdefTest.fSineMag[i],
											   tdefTest.fLBHeightDelta[i],
											   tdefTest.fRBHeightDelta[i],
											   tdefTest.fWidthDelta[i],
											   FALSE,
											   0,
											   0,
											   tdefTest.fLBStartHeight[i],
											   tdefTest.fRBStartHeight[i],
											   fStartWidth,
											   vStart.x,
											   vStart.y,
											   vStart.z,
											   fStartOrientation,
											   &vEnd, &fEndWidth, &fEndOrientation) ;
			vStart = vEnd ;
			fStartOrientation = fEndOrientation ;
			fStartWidth = fEndWidth ;
			m_nSegs++ ;
		}
	}
	CRaceTrack::~CRaceTrack()
	{
		for (int i = 0 ; i < m_nSegs ; i ++)
			delete m_pSegs[i] ;
	}

/*
 *
 * Track Definitions:
 *
 */
	void CRaceTrack::CreateDefinition(BOOL bTestTrack, BOOL bRandomTrack)
	{
		int		    nStepsPerSeg = 12 ;
		int		    nFirstQEndSeg ;
		D3DXVECTOR3 vStart = D3DXVECTOR3(0, 50, 0) ;
		int		    nSegs = 0 ;
		int		    nSegsPerQuarter = 3 ;		// odd no.s only!

		if (bTestTrack)
			nSegsPerQuarter = 1 ;

		tdefTest.vStart = vStart ;
		tdefTest.fStartWidth = 50.0f ;
		tdefTest.nSegs = 0 ;
		tdefTest.nTotalFPPs = 0 ;

	// makes one quarter circle:
		float fFirstQWidthDelta = 0.0f ;
		for (int nSeg = 0 ; nSeg < nSegsPerQuarter ; nSeg ++)
		{
			tdefTest.fWidthDelta[nSegs] = 0 ;

			if (nSeg == 0)
			{
				tdefTest.fA[nSegs] = /*rnd() */ 0.5f + 0.5f + (bRandomTrack ? rnd() * 5.0f : 0) ;
				tdefTest.fB[nSegs] = /*rnd() */ 2.5f + 0.5f + (bRandomTrack ? rnd() * 5.0f : 0) ;

				tdefTest.fWidthDelta[nSegs] = (bRandomTrack ? rnd() * 2.0f : 2) ;

				tdefTest.fSineStart[nSegs] = 0.0f ;
				tdefTest.fSinePerc[nSegs] = 1.0f ;
				tdefTest.fSineMag[nSegs] = /*rnd() */ +5.0f - (bRandomTrack ? rnd() * 4.0f : 0) ;
			}
			else if (nSeg == 1)
			{
				tdefTest.fA[nSegs] = /*rnd() */ 2.5f + 0.5f + (bRandomTrack ? rnd() * 5.0f : 0) ;
				tdefTest.fB[nSegs] = /*rnd() */ 7.5f + 0.5f + (bRandomTrack ? rnd() * 5.0f : 0) ;

				tdefTest.fWidthDelta[nSegs] = (bRandomTrack ? rnd() * 5.0f : 2) ;
				
				tdefTest.fSineStart[nSegs] = 0.0f ;
				tdefTest.fSinePerc[nSegs] = 1.0f ;
				tdefTest.fSineMag[nSegs] = /*rnd() */ +30.0f - (bRandomTrack ? rnd() * 25.0f : 0) ;
			}
			else
			{
				tdefTest.fA[nSegs] = /*rnd() */ 0.5f + 0.5f + (bRandomTrack ? rnd() * 5.0f : 0) ;
				tdefTest.fB[nSegs] = /*rnd() */ 4.0f + 0.5f + (bRandomTrack ? rnd() * 5.0f : 0) ;
				
				tdefTest.fWidthDelta[nSegs] = 0 ;

				tdefTest.fSineStart[nSegs] = 1.0f ;
				tdefTest.fSinePerc[nSegs] = 1.0f ;
				tdefTest.fSineMag[nSegs] = /*rnd() */ 10.0f - (bRandomTrack ? rnd() * 8.0f : 0) ;
			}
			
			fFirstQWidthDelta += tdefTest.fWidthDelta[nSegs] ;

			tdefTest.nSide[nSegs] = nSeg % 2 == 0 ? +1 : -1 ;

			tdefTest.nSteps[nSegs] = nStepsPerSeg ;

			if (nSegs == 0)
			{
				tdefTest.fLBHeightDelta[nSegs] = 0 ;
				tdefTest.fRBHeightDelta[nSegs] = 0 ;
				tdefTest.fLBStartHeight[nSegs] = BANK_MAX_MAG / 2 ;
				tdefTest.fRBStartHeight[nSegs] = BANK_MAX_MAG / 2 ;
			}
			else
			{
				tdefTest.fLBHeightDelta[nSegs] = /*rnd()*/ 0.5f * (BANK_MAX_MAG / 2) * (/*rnd()*/ 0.2f > 0.5f ? +1 : -1) ;
				tdefTest.fRBHeightDelta[nSegs] = /*rnd()*/ 0.5f * (BANK_MAX_MAG / 2) * (/*rnd()*/ 0.8f > 0.5f ? +1 : -1) ;
				tdefTest.fRBStartHeight[nSegs] = tdefTest.fLBStartHeight[nSegs-1] + tdefTest.fLBHeightDelta[nSegs-1] ;
				tdefTest.fLBStartHeight[nSegs] = tdefTest.fRBStartHeight[nSegs-1] + tdefTest.fRBHeightDelta[nSegs-1] ;
				
				if (fabs(tdefTest.fLBStartHeight[nSegs] + tdefTest.fLBHeightDelta[nSegs]) > BANK_MAX_MAG ||
					    (tdefTest.fLBStartHeight[nSegs] + tdefTest.fLBHeightDelta[nSegs]) < BANK_MIN_HEIGHT)
					tdefTest.fLBHeightDelta[nSegs] *= -1 ;

				if (fabs(tdefTest.fRBStartHeight[nSegs] + tdefTest.fRBHeightDelta[nSegs]) > BANK_MAX_MAG ||
					    (tdefTest.fRBStartHeight[nSegs] + tdefTest.fRBHeightDelta[nSegs]) < BANK_MIN_HEIGHT)
					tdefTest.fRBHeightDelta[nSegs] *= -1 ;
			}
			
			nSegs ++ ;
			tdefTest.nSegs ++ ;
		}
		nFirstQEndSeg = nSegs - 1 ;

	// make second, third and fourth quarters the same!
		//if (!bTestTrack)
			for (int i = 0 ; i < 3 ; i ++)
				for (nSeg = 0 ; nSeg < nSegsPerQuarter ; nSeg ++)
				{
					if (i % 2 == 0)
						tdefTest.fWidthDelta[nSegs] = tdefTest.fWidthDelta[nSegsPerQuarter - 1 - nSeg] * -1.0f ;
					else
						tdefTest.fWidthDelta[nSegs] = tdefTest.fWidthDelta[nSeg] * +1.0f ;

					if (i % 2 == 0)
					{
						tdefTest.fA[nSegs] = tdefTest.fB[nFirstQEndSeg - nSeg] ;
						tdefTest.fB[nSegs] = tdefTest.fA[nFirstQEndSeg - nSeg] ;
					}
					else
					{
						tdefTest.fA[nSegs] = tdefTest.fA[nFirstQEndSeg - nSeg] ;
						tdefTest.fB[nSegs] = tdefTest.fB[nFirstQEndSeg - nSeg] ;
					}
					
					if (nSeg == nSegsPerQuarter - 1 && i == 2)
					{
						tdefTest.fLBStartHeight[nSegs] = tdefTest.fLBStartHeight[nSegs - 1] + tdefTest.fLBHeightDelta[nSegs - 1] ;
						tdefTest.fRBStartHeight[nSegs] = tdefTest.fRBStartHeight[nSegs - 1] + tdefTest.fRBHeightDelta[nSegs - 1] ;

						tdefTest.fLBHeightDelta[nSegs] = -(tdefTest.fLBStartHeight[nSegs] - tdefTest.fLBStartHeight[0]) ;
						tdefTest.fRBHeightDelta[nSegs] = -(tdefTest.fRBStartHeight[nSegs] - tdefTest.fRBStartHeight[0]) ;
					}
					else if (nSeg == 0)
					{
						tdefTest.fLBHeightDelta[nSegs] = 0;//tdefTest.fLBHeightDelta[nFirstQEndSeg - nSeg] ;
						tdefTest.fRBHeightDelta[nSegs] = 0;//tdefTest.fRBHeightDelta[nFirstQEndSeg - nSeg] ;
						tdefTest.fLBStartHeight[nSegs] = tdefTest.fLBStartHeight[nSegs - 1] + tdefTest.fLBHeightDelta[nSegs - 1] ;
						tdefTest.fRBStartHeight[nSegs] = tdefTest.fRBStartHeight[nSegs - 1] + tdefTest.fRBHeightDelta[nSegs - 1] ;
					}
					else
					{
						tdefTest.fLBHeightDelta[nSegs] = (bRandomTrack ? tdefTest.fLBHeightDelta[nFirstQEndSeg - nSeg] : -2) ;
						tdefTest.fRBHeightDelta[nSegs] = (bRandomTrack ? tdefTest.fRBHeightDelta[nFirstQEndSeg - nSeg] : -2) ;
						tdefTest.fRBStartHeight[nSegs] = tdefTest.fLBStartHeight[nSegs - 1] + tdefTest.fLBHeightDelta[nSegs - 1] ;
						tdefTest.fLBStartHeight[nSegs] = tdefTest.fRBStartHeight[nSegs - 1] + tdefTest.fRBHeightDelta[nSegs - 1] ;

						if (fabs(tdefTest.fLBStartHeight[nSegs] + tdefTest.fLBHeightDelta[nSegs]) > BANK_MAX_MAG ||
								(tdefTest.fLBStartHeight[nSegs] + tdefTest.fLBHeightDelta[nSegs]) < BANK_MIN_HEIGHT)
							tdefTest.fLBHeightDelta[nSegs] *= -1 ;

						if (fabs(tdefTest.fRBStartHeight[nSegs] + tdefTest.fRBHeightDelta[nSegs]) > BANK_MAX_MAG ||
								(tdefTest.fRBStartHeight[nSegs] + tdefTest.fRBHeightDelta[nSegs]) < BANK_MIN_HEIGHT)
							tdefTest.fRBHeightDelta[nSegs] *= -1 ;
					}

					tdefTest.nSide[nSegs] = tdefTest.nSide[nFirstQEndSeg - nSeg] ;

					tdefTest.nSteps[nSegs] = nStepsPerSeg ;
					tdefTest.fSineStart[nSegs] = 1.0f ;
					tdefTest.fSinePerc[nSegs] = 1.0f ;
					tdefTest.fSineMag[nSegs] = tdefTest.fSineMag[nFirstQEndSeg - nSeg] ;

					nSegs ++ ;
					tdefTest.nSegs ++ ;
				}
	}

/*
 *
 * D3DFrame Event Hooks.
 *
 */
	HRESULT CRaceTrack::RestoreDeviceObjects(
												LPDIRECT3D8		  pD3D,
												LPDIRECT3DDEVICE8 pDev
										    )
	{
		HRESULT hr = S_OK ;
		
		for (int i = 0 ; i < m_nSegs ; i ++)
			if ((hr = m_pSegs[i]->RestoreDeviceObjects(pD3D, pDev)) != S_OK)
				return hr ;

		return hr ;
	}
	HRESULT	CRaceTrack::InvalidateDeviceObjects()
	{
		for (int i = 0 ; i < m_nSegs ; i ++)
			m_pSegs[i]->InvalidateDeviceObjects() ;

		return S_OK ;
	}

	HRESULT CRaceTrack::Render(
								LPDIRECT3DDEVICE8 pDev,
								CTextures		 *pTex,
								unsigned long	 *pulPolyCount
							  )
	{
		for (int i = 0 ; i < m_nSegs ; i ++)
			if (m_pSegs[i]->m_bVisible)
				m_pSegs[i]->Render(pDev, pTex, pulPolyCount, 0, NULL) ;

		return S_OK ;
	}

	HRESULT CRaceTrack::FrameMove(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, D3DXMATRIX *pmatProj, D3DVIEWPORT8 *pVP)
	{
		DetermineVisibility(pDev, pCamera, pmatProj, pVP) ;

		return S_OK ;
	}

	CRaceTrack::DetermineVisibility(LPDIRECT3DDEVICE8 pDev, CCamera *pCam, D3DXMATRIX *pmatProj, D3DVIEWPORT8 *pVP)
	{
		m_nVisibleSegs = 0 ;

		for (int i = 0 ; i < m_nSegs ; i ++)
		{
			m_pSegs[i]->GetBBoxVisibilityFlags(pDev, pmatProj, pCam, pVP) ;
			if (m_pSegs[i]->m_bVisible)
				m_nVisibleSegs++ ;
		}
	}