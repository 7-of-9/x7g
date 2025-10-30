/*
 *
 * x7
 * CRaceTrackSegment.cpp
 *
 * One segment of racetrack.
 *
 */

#include "CRaceTrackSegment.h"

/*
 *
 * Constructor / Destructors.
 *
 */
	CRaceTrackSegment::CRaceTrackSegment(float fSide,
										 int   nSteps,
										 float aKMs,
										 float bKMs,
										 float fSineStart,
										 float fSinePerc,
										 float fSineMag,
										 float fLBHeightDelta,
										 float fRBHeightDelta,
										 float fWidthDelta,
										 BOOL  bHeHe,
										 int   nHeHeStart,
										 int   nHeHeLen,
										 float fLBStartHeight,
										 float fRBStartHeight,
										 float fStartWidth,
										 float fBL_x,
										 float fBL_y,
										 float fBL_z,
										 float fStartOrientation,

										 LPD3DXVECTOR3 lpvEnd,
										 float *pfEndWidth,
										 float *pfEndOrientation)
	{
		HRESULT hr ;

		m_dwFVF =	/*MYFVF_COLOREDVERTEX*/
					/*MYFVF_UNCOLOREDVERTEX*/
					MYFVF_COLOREDVERTEXWITHNORMAL ;
		m_nFPPs = 0 ;
		
		hr = AddBend2(fSide, nSteps, aKMs, bKMs, fSineStart, fSinePerc, fSineMag, fLBHeightDelta, fRBHeightDelta, fWidthDelta, bHeHe, nHeHeStart, nHeHeLen, fLBStartHeight, fRBStartHeight, fStartWidth, fBL_x, fBL_y, fBL_z, fStartOrientation, lpvEnd, pfEndWidth, pfEndOrientation) ; 
		if (hr != S_OK)
			Msg("CRaceTrackSegment::CRaceTrackSegment(): AddBend2(...) != S_OK") ;
		
		if (IndexToList() != S_OK)
			Msg("CRaceTrackSegment::CRaceTrackSegment(): IndexToList() != S_OK") ;
	}
	CRaceTrackSegment::~CRaceTrackSegment()
	{
	}

/*
 *
 *
 *
 */

	HRESULT CRaceTrackSegment::AddBend2(float fSide,
									    int	  nSteps,
										float aKMs,
										float bKMs,
										float fSineStart,
										float fSinePerc,
										float fSineMag,
										float fLBHeightDelta,
										float fRBHeightDelta,
										float fWidthDelta,
										BOOL  bHeHe,
										int	  nHeHeStart,
										int	  nHeHeLen,
										float fLBStartHeight,
										float fRBStartHeight,
										float fStartWidth,
										float fBL_x,
										float fBL_y,
										float fBL_z,
										float fStartOrientation,
										
										LPD3DXVECTOR3 lpvEnd,
										float *pfEndWidth,
										float *pfEndOrientation
										)
	{
	//
	// The static dimensions below are !!!DODGY!!! (they cause 'unknown exceptions')
	//
		float	  fHeights [MAX_STEPS] ;
		D3DXVECTOR3 vRef [MAX_STEPS] [TILE_WIDTH + 1] ;
		D3DXVECTOR3 vRefBankLeft [MAX_STEPS] [10 + 1] ;
		D3DXVECTOR3 vRefBankRight [MAX_STEPS] [10 + 1] ;

		float SINGLE_TILE_WIDTH = fStartWidth / (float)TILE_WIDTH ;

		float fWidth = SINGLE_TILE_WIDTH ;
		/*float fBL_x = Segments[nSegments].vStart.x,
			  fBL_y = Segments[nSegments].vStart.y,
			  fBL_z = Segments[nSegments].vStart.z ;*/
		int   nQuads = 0, ndxEndQuadA, ndxEndQuadB, ndxStartQuadA, ndxStartQuadB ;
		int	  nSteps2Use/*, ndxFirstFlightPathPoint = nFlightPathPoints*/ ;

		D3DXVECTOR3 vTiles_BL[MAX_STEPS*(TILE_WIDTH+1)],
				  vTiles_TL[MAX_STEPS*(TILE_WIDTH+1)],
				  vTiles_BR[MAX_STEPS*(TILE_WIDTH+1)],
				  vTiles_TR[MAX_STEPS*(TILE_WIDTH+1)] ;

		float	  fTUs_BL[MAX_STEPS] [TILE_WIDTH + 1],
				  fTUs_TL[MAX_STEPS] [TILE_WIDTH + 1],
				  fTUs_BR[MAX_STEPS] [TILE_WIDTH + 1],
				  fTUs_TR[MAX_STEPS] [TILE_WIDTH + 1] ;
		float	  fTVs_BL[MAX_STEPS] [TILE_WIDTH + 1],
				  fTVs_TL[MAX_STEPS] [TILE_WIDTH + 1],
				  fTVs_BR[MAX_STEPS] [TILE_WIDTH + 1],
				  fTVs_TR[MAX_STEPS] [TILE_WIDTH + 1] ;

		float	  fLBCurHeight, fRBCurHeight, fLBHeightDeltaI, fRBHeightDeltaI ;

		nSteps2Use = (int)(nSteps * 1.0) ;
	
		if (nSteps > MAX_STEPS)
			Msg("CRaceTrackSegment::AddBend2() - nSteps Warning") ;
		
		/*
		Segments[nSegments].fLBEndHeight = Segments[nSegments].fLBStartHeight + fLBHeightDelta ;
		Segments[nSegments].fRBEndHeight = Segments[nSegments].fRBStartHeight + fRBHeightDelta ;
		*/

		/*
		fLBCurHeight = Segments[nSegments].fLBStartHeight ;
		fRBCurHeight = Segments[nSegments].fRBStartHeight ;
		*/
		fLBCurHeight = fLBStartHeight ;
		fRBCurHeight = fRBStartHeight ;
		
		fLBHeightDeltaI = fLBHeightDelta / (float)nSteps ;
		fRBHeightDeltaI = fRBHeightDelta / (float)nSteps ;

	//
	// Sample sin(x) to produce height reference points:
	//
		for (int i = 0 ; i <= nSteps2Use ; i ++)
			fHeights [i] = fBL_y - (float)( sin((((float)i / (float)nSteps2Use) + fSineStart) * g_2_PI / fSinePerc) * fSineMag) ;

	//
	// Sample equation to produce the CURVE REFERENCE POINTS:
	//
		for (int x = 0 ; x <= TILE_WIDTH ; x ++)
			for (int i = 0 ; i <= nSteps2Use ; i ++)
			{
				float _a, _b, z ;
				float __b = bKMs + (((float)x / 8.0f) * fWidthDelta) ;

				/*
				if (i == nSteps2Use)
					Segments[nSegments].fEndBKMs = __b ;
				*/

				_a = aKMs * KMS_2_ABS + (SINGLE_TILE_WIDTH * x) ;
				_b = __b * KMS_2_ABS + (SINGLE_TILE_WIDTH * x) ;
				
				//linear sample: z = _b * ((float)i / (float)nSteps) ;
				//non-linear sample: z = _b * (1.0f - ((float)pow((double)i, 1.2f) / (float)pow((double)nSteps, 1.2f))) ;
				
				//non-linear, iterative sampling: 0.x is a a bit a magic no. here...
				float t = (float)(
					pow(i,			1.0f - ((float)(pow(i,0.6))/(float)(pow(nSteps2Use,0.6))))
						/ 
					pow(nSteps2Use,	1.0f - ((float)(pow(i,0.6))/(float)(pow(nSteps2Use,0.6))))) ;
				z = _b * (t) ;
				//now, the bunching of the z-values is proportional to x ^ 0.x,
				//giving us many *more* samples as x approaches b.

				float x2b2 = (z * z) / (_b * _b) ;

				vRef [i][x].x = fBL_x + ((float)sqrt(_a * _a * (1.0f - (x2b2))) - aKMs * KMS_2_ABS) ;
				vRef [i][x].y = fHeights [i] ;
				vRef [i][x].z = fBL_z + z ;

			//
			// Compute flight-path points:
			//
				if (x == TILE_WIDTH / 2 && i < nSteps2Use)
				{
					/*
					BOOL bAdd ;
					if (!bHeHe)
						bAdd = TRUE ;
					else
						bAdd = i < nHeHeStart || i > nHeHeStart + nHeHeLen ;

					if (bAdd)
					*/

					{
						/*
						ndxFlightPathPointSegment [nFlightPathPoints] = nSegments ;
						*/

						m_vFPPs[m_nFPPs++] = vRef[i][x] ;
						if (m_nFPPs == MAX_STEPS)
							Msg("CRaceTrackSegment::AddBend2() - FPP Warning.") ;
					}
				}
			}
	//
	// Compute texture addresses:
	//
		for (x = 0 ; x < TILE_WIDTH ; x ++)
			for (int i = 0 ; i < nSteps2Use ; i ++)
			{
			//
			// Figure out how DEEP the quad is so we can texture it uniformly:
			//
				if (i == 0)
				{
					//if (nSegments == 0)
					{
						fTVs_BL[i][x] = 0.00f ;
						fTVs_BR[i][x] = 0.00f ;
					}
					/*else
					{
						fTVs_BL[i][x] = Segments[nSegments - 1].fTVs_TL_End[x] ;
						fTVs_BR[i][x] = Segments[nSegments - 1].fTVs_TR_End[x] ;
					}*/
				}
				else
				{
					// TL <--> BL thing.
					fTVs_BL[i][x] = fTVs_TL[i - 1][x] ;
					fTVs_BR[i][x] = fTVs_TR[i - 1][x] ;
				}
				float dx, dz, Z ;

				// left points.
				dx = (float)fabs(vRef[i][x].x - vRef[i + 1][x].x) ;
				dz = (float)fabs(vRef[i][x].z - vRef[i + 1][x].z) ;
				Z = (float)sqrt(dx*dx + dz*dz) ;
				fTVs_TL[i][x] = fTVs_BL[i][x] + 1.00f * (Z / TEX_TO_ABS) ;

				// right points.
				dx = (float)fabs(vRef[i][x+1].x - vRef[i+1][x+1].x) ;
				dz = (float)fabs(vRef[i][x+1].z - vRef[i+1][x+1].z) ;
				Z = (float)sqrt(dx*dx + dz*dz) ;
				fTVs_TR[i][x] = fTVs_BR[i][x] + 1.00f * (Z / TEX_TO_ABS) ;

			//
			// Find out how WIDE the quad is so we can texture it uniformly:
			//
				/*if (i == 0)
					if (nSegments == 0)
					{
						fTUs_BL[i][x] = 0.00f ;
						fTUs_TL[i][x] = 0.00f ;
					}
					else
					{
						fTUs_BL[i][x] = Segments[nSegments - 1].fTUs_TL_End[x] ;
						fTUs_TL[i][x] = Segments[nSegments - 1].fTUs_TL_End[x] ;
					}
				else*/
					if (x == 0)
					{
						fTUs_BL[i][x] = 0.00f ;
						fTUs_TL[i][x] = 0.00f ;
					}
					else
					{
						fTUs_BL[i][x] = fTUs_BR[i][x - 1] ;
						fTUs_TL[i][x] = fTUs_TR[i][x - 1] ;
					}

				float X ;

				dx = (float)fabs(vRef[i][x + 1].x - vRef[i][x].x) ;
				dz = (float)fabs(vRef[i][x + 1].z - vRef[i][x].z) ;
				X = (float)sqrt(dx*dx + dz*dz) ;
				fTUs_BR[i][x] = fTUs_BL[i][x] + 1.00f * (X / TEX_TO_ABS) ;

				dx = (float)fabs(vRef[i + 1][x + 1].x - vRef[i + 1][x].x) ;
				dz = (float)fabs(vRef[i + 1][x + 1].z - vRef[i + 1][x].z) ;
				X = (float)sqrt(dx*dx + dz*dz) ;
				fTUs_TR[i][x] = fTUs_TL[i][x] + 1.00f * (X / TEX_TO_ABS) ;

			//
			// Save end step tex. addresses for next seg.
			//
				/*
				if (i == nSteps2Use - 1)
				{
					Segments[nSegments].fTVs_TL_End[x] = fTVs_TL[i][x] ;
					Segments[nSegments].fTVs_TR_End[x] = fTVs_TR[i][x] ;
					Segments[nSegments].fTUs_TL_End[x] = fTUs_TL[i][x] ;
					Segments[nSegments].fTUs_TR_End[x] = fTUs_TR[i][x] ;
				}
				*/
			}
			
	//
	// Sample equation to produce the bank reference points:
	//
		int nBankTileWidth = BANK_GRADUATIONS ;
		float fBankWidth = BANK_WIDTH ;
		float fBankSingleTileWidth = fBankWidth / (float)nBankTileWidth ;

		for (x = 0 ; x <= nBankTileWidth ; x ++)
			for (int i = 0 ; i <= nSteps2Use ; i ++)
			{
				float _a, _b, z, __b ;

			// left bank:
				__b = bKMs ;
				_a = aKMs * KMS_2_ABS - fBankWidth + (fBankSingleTileWidth * x) ;
				_b = __b * KMS_2_ABS - fBankWidth + (fBankSingleTileWidth * x) ;
				//non-linear, iterative sampling: 0.x is a a bit a magic no. here...
				float t = (float)(
					pow(i,			1.0f - ((float)(pow(i,0.6))/(float)(pow(nSteps2Use,0.6))))
						/ 
					pow(nSteps2Use,	1.0f - ((float)(pow(i,0.6))/(float)(pow(nSteps2Use,0.6))))) ;
				z = _b * (t) ;
				float x2b2 = (z * z) / (_b * _b) ;
				vRefBankLeft [i][x].x = fBL_x + ((float)sqrt(_a * _a * (1.0f - (x2b2))) - aKMs * KMS_2_ABS) ;
				vRefBankLeft [i][x].y = fHeights [i] ;
				vRefBankLeft [i][x].z = fBL_z + z ;

			// right-bank:				
				__b = bKMs + (((float)TILE_WIDTH / 8.0f) * fWidthDelta) ;
				_a = aKMs * KMS_2_ABS + (SINGLE_TILE_WIDTH * TILE_WIDTH) + (fBankSingleTileWidth * x) ;
				_b = __b * KMS_2_ABS + (SINGLE_TILE_WIDTH * TILE_WIDTH) + (fBankSingleTileWidth * x) ;

				//non-linear, iterative sampling: 0.x is a a bit a magic no. here...
				t = (float)(
					pow(i,			1.0f - ((float)(pow(i,0.6))/(float)(pow(nSteps2Use,0.6))))
						/ 
					pow(nSteps2Use,	1.0f - ((float)(pow(i,0.6))/(float)(pow(nSteps2Use,0.6))))) ;
				z = _b * (t) ;
				x2b2 = (z * z) / (_b * _b) ;
				vRefBankRight [i][x].x = fBL_x + ((float)sqrt(_a * _a * (1.0f - (x2b2))) - aKMs * KMS_2_ABS) ;
				vRefBankRight [i][x].y = fHeights [i] ;//fBL_y ;fBL_y ;
				vRefBankRight [i][x].z = fBL_z + z ;
			}

	//
	// Rotate all reference points as required by the left- or right- handedness of the bend:
	//
		D3DXVECTOR3 vSpinAround = vRef [0][TILE_WIDTH / 2] ;
		
		if (fSide == +1)
		{
			for (int x = 0 ; x <= TILE_WIDTH ; x ++)
				for (int i = 0 ; i <= nSteps2Use ; i ++)
				{
					RotateVectorZ(&vRef[i][x], 180, &vSpinAround) ;
					
					if (i < nSteps2Use)
						RotateVectorZ(&m_vFPPs[i/*+ndxFirstFlightPathPoint*/], 180, &vSpinAround) ;
				}

			for (x = 0 ; x <= nBankTileWidth ; x ++)
				for (int i = 0 ; i <= nSteps2Use ; i ++)
				{
					RotateVectorZ(&vRefBankLeft[i][x],  180, &vSpinAround) ;
					RotateVectorZ(&vRefBankRight[i][x], 180, &vSpinAround) ;
				}
		}
		

	//*************************************************************
	// Build track's flat asymetric quads, on the reference points:
	//
		for (i = 0 ; i < nSteps2Use ; i ++)
		{
			for (int x = 0 ; x < TILE_WIDTH ; x ++)
			{
				vTiles_BL [nQuads] = D3DXVECTOR3 (vRef[i][x].x,	  vRef[i][x].y,		vRef[i][x].z) ;
				vTiles_BR [nQuads] = D3DXVECTOR3 (vRef[i][x+1].x,	  vRef[i][x+1].y,	vRef[i][x+1].z) ;
				vTiles_TL [nQuads] = D3DXVECTOR3 (vRef[i+1][x].x,	  vRef[i+1][x].y,   vRef[i+1][x].z) ;
				vTiles_TR [nQuads] = D3DXVECTOR3 (vRef[i+1][x+1].x, vRef[i+1][x+1].y, vRef[i+1][x+1].z) ;
			
			//
			// Store segment's outmost quads, to calculate the orientation:
			//
				if (i == nSteps2Use - 1)
					if (x == 0)
						ndxEndQuadA = nQuads ;
					else
						if (x == TILE_WIDTH - 1)
							ndxEndQuadB = nQuads ;
				if (i == 0)
					if (x == 0)
						ndxStartQuadA = nQuads ;
					else
						if (x == TILE_WIDTH - 1)
							ndxStartQuadB = nQuads ;

			//
			// Compute start of segment summary data:
			//
				if (i == 0 && x == TILE_WIDTH - 1)
				{
					// hypotenuse is the track's width
					D3DXVECTOR3 d ;
					float h, o, a ;
					d = vTiles_TR [ndxStartQuadB] - vTiles_TL [ndxStartQuadA] ;
					o = d.z ;
					a = d.x ;
					h = (float)sqrt(a * a + o * o) ;
					/*
					Segments[nSegments].fStartWidth = h ;
					*/
				}
				
				unsigned tex ;

			//
			// Add the quad:
			//
				tex = TRACK_TEX_START + 0 ;
				
				/*
				if (nSegments == 0 && i < 2) // start/finish grid tex.
				{
					fTUs_TR[i][x] /= 8 ;
					fTUs_TL[i][x] /= 8 ;
					fTUs_BR[i][x] /= 8 ;
					fTUs_BL[i][x] /= 8 ;

					fTVs_TR[i][x] /= 8 ;
					fTVs_TL[i][x] /= 8 ;
					fTVs_BR[i][x] /= 8 ;
					fTVs_BL[i][x] /= 8 ;

					tex = TRACK_TEX_START + 10 ;
				}
				*/

				/*BOOL bAdd ;
				if (!bHeHe)
					bAdd = TRUE ;
				else
					bAdd = i < nHeHeStart || i > nHeHeStart + nHeHeLen ;*/

				X7LT *p ;
				if (fSide == +1)
				{
					//if (bAdd)
						if ((p = AddSingleRect (vTiles_TR[nQuads], fTUs_TR[i][x], fTVs_TR[i][x],
												vTiles_TL[nQuads], fTUs_TL[i][x], fTVs_TL[i][x],
												vTiles_BR[nQuads], fTUs_BR[i][x], fTVs_BR[i][x],
												vTiles_BL[nQuads], fTUs_BL[i][x], fTVs_BL[i][x],
												tex , D3DXVECTOR3(0,1,0))) == NULL)
							return E_FAIL ;
				}
				else
				{
					//if (bAdd)
						if ((p = AddSingleRect (vTiles_TL[nQuads], fTUs_TL[i][x], fTVs_TL[i][x],
												vTiles_TR[nQuads], fTUs_TR[i][x], fTVs_TR[i][x],
												vTiles_BL[nQuads], fTUs_BL[i][x], fTVs_BL[i][x],
												vTiles_BR[nQuads], fTUs_BR[i][x], fTVs_BR[i][x],
												tex , D3DXVECTOR3(0,1,0))) == NULL)
							return E_FAIL ;
				}

				/*
				if (nSegments == 0 && i < 2) // undo start/finish grid tex b4 doing banks
				{
					fTUs_TR[i][x] *= 8 ;
					fTUs_TL[i][x] *= 8 ;
					fTUs_BR[i][x] *= 8 ;
					fTUs_BL[i][x] *= 8 ;

					fTVs_TR[i][x] *= 8 ;
					fTVs_TL[i][x] *= 8 ;
					fTVs_BR[i][x] *= 8 ;
					fTVs_BL[i][x] *= 8 ;

					tex = TRACK_TEX_START + 0 ;
					//tex = TRACK_TEX_START + 9 ;
				}
				*/

			//
			// Get flight-path points & tile pointers: OBSOLETE?
 			//
				/*
				if (x == TILE_WIDTH / 2)
					px4ltFlightPathPoints[i + ndxFirstFlightPathPoint] = p ;
				*/

			//
			// Build the curved sides of the track:
			//
				
				static int DT = 1 ; // decal type
				static int bBuildingLeftDecal = FALSE, bBuildingRightDecal = FALSE ;
				static int nDecalBuildSteps = 0 ;

				if (i == (int)((float)nSteps2Use * 0.50f) && !bBuildingLeftDecal && !bBuildingRightDecal)
				{
					DT = rnd() < 0.5f ? 1 : 2 ;
					bBuildingLeftDecal = TRUE ;
					bBuildingRightDecal = TRUE ;
					nDecalBuildSteps = 0 ;
				}

				if (bBuildingLeftDecal || bBuildingRightDecal)
					nDecalBuildSteps ++ ;

				if (x == TILE_WIDTH - 1)
				{
					/*if (bAdd)*/
						if (AddBank2 (

									  &vRefBankRight[i][0], &vRefBankRight[i+1][0],
									  nBankTileWidth,
									  fRBCurHeight, fRBCurHeight + fRBHeightDeltaI,
									  fSide, +1.0f,
									  tex ,
									  fTVs_BR[i][x],
									  fTVs_TR[i][x],
									  fTUs_BR[i][x],
									  fTUs_TR[i][x],

									  fSide == + 1 && i == 0 && rnd() < 0.2f /*&& nSegments > 0*/,
									  0,

									  fSide == + 1 && i == (int)((float)nSteps2Use * 0.50f),
									  DT == 1 ? 16 : 4,
									  DT == 1 ? 4  : 1,
									  DT == 1 ? 0  : 2,
									  DT == 1 ? TRACK_TEX_START + 8 : TRACK_TEX_START + 9,

									  fSide == + 1 && i == (int)((float)nSteps2Use * 0.50f),
									  DT == 1 ? 16 : 4,
									  DT == 1 ? 4  : 1,
									  DT == 1 ? 0  : 2,
									  DT == 1 ? TRACK_TEX_START + 8 : TRACK_TEX_START + 9

									 ) != S_OK)

								return E_FAIL ;
				}
 
				if (x == 0)
				{
					/*if (bAdd)*/
						if (AddBank2(

									 &vRefBankLeft[i][0], &vRefBankLeft[i+1][0],
									  nBankTileWidth,
									  fLBCurHeight, fLBCurHeight + fLBHeightDeltaI,
									  fSide, -1.0f,
									  tex ,
									  fTVs_BL[i][x],
									  fTVs_TL[i][x],
									  fTUs_BL[i][x],
									  fTUs_TL[i][x],
									  
									  fSide == + 1 && i == 0 && rnd() < 0.2f /*&& nSegments > 0*/,
									  0,

									  fSide == + 1 && i == (int)((float)nSteps2Use * 0.50f),
									  DT == 1 ? 16 : 4,
									  DT == 1 ? 4  : 1,
									  DT == 1 ? 0 :  2,
									  DT == 1 ? TRACK_TEX_START + 11 : TRACK_TEX_START + 9,

									  fSide == + 1 && i == (int)((float)nSteps2Use * 0.50f),
									  DT == 1 ? 16 : 4,
									  DT == 1 ? 4  : 1,
									  DT == 1 ? 0  : 2,
									  DT == 1 ? TRACK_TEX_START + 11 : TRACK_TEX_START + 9
									 
									 ) != S_OK)

							return E_FAIL ;
				}

				if (nDecalBuildSteps == 4)
				{
					bBuildingLeftDecal = FALSE ;
					bBuildingRightDecal = FALSE ;
				}
				

				nQuads ++ ;
			}

			fLBCurHeight += fLBHeightDeltaI ;
			fRBCurHeight += fRBHeightDeltaI ;
		}

		/*
		Segments[nSegments].ro.GetBoundaries(FALSE) ;
		Segments[nSegments].ro.MakeBoundingSphere() ;
		*/

	//
	// Compute end of segment summary data:
	//
				//LPD3DXVECTOR3 lpvEnd,
				//float *pfEndWidth,
				//float *pfEndOrientation

		if (fSide == +1)
			*lpvEnd = vTiles_TR[ndxEndQuadB] ;
		else
			*lpvEnd = vTiles_TL[ndxEndQuadA] ;

		D3DXVECTOR3 d ;
		float h, o, a, t ;
		
		// hypotenuse is the track's width
		d = vTiles_TR [ndxEndQuadB] - vTiles_TL [ndxEndQuadA] ;
		o = d.z ;
		a = d.x ;
		h = (float)sqrt (a * a + o * o) ;	
		*pfEndWidth = h ;

		// sin t = o/h
		t = (float)asin (o / h) * g_RADTODEG ;
		*pfEndOrientation = -t ;

		if (fSide == +1)
			*pfEndOrientation = +t ;

	//
	// Rotate segment according to its initially specified orientation:
	//
		RotateX7LTsY(fStartOrientation, &(D3DXVECTOR3(fBL_x, fBL_y, fBL_z))) ;

		/*Segments[nSegments].ro.RotateY (Segments[nSegments].vStart,
										Segments[nSegments].fStartOrientation, FALSE) ;

		for (int j = 0 ; j < Segments[nSegments].nAVIs ; j ++)
		{
			Segments[nSegments].proAVIs[j]->RotateY (Segments[nSegments].vStart,
													   Segments[nSegments].fStartOrientation, FALSE) ;
		}		
		for (j = 0 ; j < Segments[nSegments].nDecals ; j ++)
		{
			Segments[nSegments].proDecals[j]->RotateY (Segments[nSegments].vStart,
													   Segments[nSegments].fStartOrientation, FALSE) ;
		}*/

		*pfEndOrientation += fStartOrientation ;
		
		RotateVectorY(lpvEnd, fStartOrientation,
					   /*&Segments[nSegments].vStart*/
					   &(D3DXVECTOR3(fBL_x, fBL_y, fBL_z))) ;

		for (i = 0 ; i < m_nFPPs ; i ++)
			RotateVectorY(&m_vFPPs[i], fStartOrientation,
						  /*&Segments[nSegments].vStart*/
						  &(D3DXVECTOR3(fBL_x, fBL_y, fBL_z))) ;
		

		return S_OK ;
	}

/*
 *
 *
 *
 */
	HRESULT CRaceTrackSegment::AddBank2(D3DXVECTOR3 *vRef1,
									    D3DXVECTOR3 *vRef2,
										int   nSteps,
										float fHeightStart,
										float fHeightEnd,
										float fSide,
										float fBankSide,
										int   ndxTex,
										float fTVStart,
										float fTVEnd,
										float fTUStart,
										float fTUEnd,

										BOOL bStartBuildLeftAVI,
										BOOL bStartBuildRightAVI,

										BOOL bStartBuildLeftDecal,
										int  nSpecifiedLeftDecalLen,
										int  nSpecifiedLeftDecalHeight,
										int  nSpecifiedLeftDecalStart,
										int  ndxLeftDecalTex,

										BOOL bStartBuildRightDecal,
										int  nSpecifiedRightDecalLen,
										int  nSpecifiedRightDecalHeight,
										int  nSpecifiedRightDecalStart,
										int  ndxRightDecalTex)
	{
		D3DXVECTOR3 vTR, vTL, vBR, vBL ;
		float fStepHeightsStart[16], fStepHeightsEnd[16], fStepCs[16] ;
		float fStepHeight = 0.1f ;
		float fAngleOfIncline [16] ;
		float fTotalBankLength = 0.0f ;
		float fBankLength [16] ;
		static i = 0 ;
		int ndxTexRumble = TRACK_TEX_START + 3 ;

		static int nLeftAVILen = 4 ;
		static BOOL bBuildingLeftAVI = FALSE ;
		static int nLeftAVIStep = 0 ;
		static D3DXVECTOR3 vLeftAVI_TR, vLeftAVI_TL, vLeftAVI_BR, vLeftAVI_BL ;
		static int nRightAVILen = 4 ;
		static BOOL bBuildingRightAVI = FALSE ;
		static int nRightAVIStep = 0 ;
		static D3DXVECTOR3 vRightAVI_TR, vRightAVI_TL, vRightAVI_BR, vRightAVI_BL ;

		static int nLeftDecalLen, nLeftDecalHeight, nLeftDecalStart ;
		static BOOL bBuildingLeftDecal = FALSE ;
		static int nLeftDecalStep = 0 ;
		static D3DXVECTOR3 vLeftDecal_TR, vLeftDecal_TL, vLeftDecal_BR, vLeftDecal_BL ;
		
		static int nRightDecalLen, nRightDecalHeight, nRightDecalStart ;
		static BOOL bBuildingRightDecal = FALSE ;
		static int nRightDecalStep = 0 ;
		static D3DXVECTOR3 vRightDecal_TR, vRightDecal_TL, vRightDecal_BR, vRightDecal_BL ;

		if (bStartBuildRightDecal)
		{
			nRightDecalLen = nSpecifiedRightDecalLen ;
			nRightDecalStart = nSpecifiedRightDecalStart ;
			nRightDecalHeight = nSpecifiedRightDecalHeight ;
			bBuildingRightDecal = TRUE ;
		}
		if (bStartBuildLeftDecal)
		{
			nLeftDecalLen = nSpecifiedLeftDecalLen ;
			nLeftDecalStart = nSpecifiedLeftDecalStart ;
			nLeftDecalHeight = nSpecifiedLeftDecalHeight ;
			bBuildingLeftDecal = TRUE ;
		}
		
		if (bStartBuildLeftAVI)
			bBuildingLeftAVI = TRUE ;
		if (bStartBuildRightAVI)
			bBuildingRightAVI = TRUE ;

	//
	// Sample y = x * (x / 50) for the curve of the bank:
	//
		fStepHeightsStart[0] = 0.0f ;
		fStepHeightsEnd[0] = 0.0f ;
		for (int nStep = 1 ; nStep <= nSteps ; nStep ++)
		{
			float mx2 = ((nSteps) * ((nSteps) / 500.0f)) ;
			float cx2 = ((nStep * (nStep / 500.0f))) ;
			float c, p ;
			int ndx = nStep ;

			c = cx2 / mx2 ;
			p = c * fHeightStart ;

			fStepCs[ndx] = c ;
			fStepHeightsStart[ndx] = c * fHeightStart ; 
			fStepHeightsEnd[ndx]   = c * fHeightEnd ;   
	
			float o = fStepHeightsEnd[nStep] - fStepHeightsEnd[nStep-1] ;
			float a = (BANK_WIDTH / (float)BANK_GRADUATIONS) ;
			float h = (float)sqrt(o*o + a*a) ;

			fTotalBankLength += h ;
			fBankLength [ndx] = h ;

			fAngleOfIncline [ndx] = (float)acos (o / a) * g_RADTODEG ;
		}

	//
	// Build quads on sampled points:
	//
		float tx1, tx2, txmax, tx1e, tx2e ;
		txmax = fTUStart + fTotalBankLength / TEX_TO_ABS ;

		if (fBankSide == +1)
		{
			if (bBuildingLeftAVI)
				nLeftAVIStep ++ ;
			
			if (bBuildingLeftDecal)
				nLeftDecalStep ++ ;

			for (int x = 0 ; x < nSteps ; x ++)
			{
				D3DXVECTOR3 vTiles_BL, vTiles_BR, vTiles_TL, vTiles_TR ;
				
				if (x == 0)
				{
					tx1  = fTUStart ;
					tx1e = fTUEnd ;
				}
				else
				{
					tx1  = tx2 ;
					tx1e = tx2e ;
				}
				
				tx2  = tx1  + (fBankLength[x+1] / TEX_TO_ABS) ;
				tx2e = tx1e + (fBankLength[x+1] / TEX_TO_ABS) ;
				
				vTiles_BL = D3DXVECTOR3 (vRef1[x].x,   vRef1[x].y,   vRef1[x].z) ;
				vTiles_BR = D3DXVECTOR3 (vRef1[x+1].x, vRef1[x+1].y, vRef1[x+1].z) ;
				vTiles_TL = D3DXVECTOR3 (vRef2[x].x,   vRef2[x].y,   vRef2[x].z) ;
				vTiles_TR = D3DXVECTOR3 (vRef2[x+1].x, vRef2[x+1].y, vRef2[x+1].z) ;

				vTiles_BL.y += fStepHeightsStart [x] ;
				vTiles_TL.y += fStepHeightsEnd [x] ;
				vTiles_BR.y += fStepHeightsStart [x+1] ;
				vTiles_TR.y += fStepHeightsEnd [x+1] ;
 
				if (fSide == +1)
				{
					if (AddSingleRect (
									   vTiles_TR, tx2e, fTVEnd,
									   vTiles_TL, tx1e, fTVEnd,
									   vTiles_BR, tx2, fTVStart,
									   vTiles_BL, tx1, fTVStart,
									   ndxTex, D3DXVECTOR3(0,1,0)) == NULL)
						return E_FAIL ;

					// AVIs
					if (bBuildingLeftAVI && (x == 3 || x == 2))
					{
						if (nLeftAVIStep == 1)//left
						{
							if (x == 3)//top
								vLeftAVI_TL = vTiles_BR ;
							if (x == 2)//bot
								vLeftAVI_BL = vTiles_BL ;
						}
						if (nLeftAVIStep == nLeftAVILen)//right
						{
							if (x == 3)//top
								vLeftAVI_TR = vTiles_TR ;
							if (x == 2)//bot
								vLeftAVI_BR = vTiles_TL ;
						}
					}
					// Decals
 					if (bBuildingLeftDecal && (x == nSpecifiedLeftDecalStart+nSpecifiedLeftDecalHeight || x == nSpecifiedLeftDecalStart))
					{
						if (nLeftDecalStep == 1)//left
						{
							if (x == nSpecifiedLeftDecalStart+nSpecifiedLeftDecalHeight)//top
								vLeftDecal_TL = vTiles_BR ;
							if (x == nSpecifiedLeftDecalStart)//bot
								vLeftDecal_BL = vTiles_BL ;
						}
						if (nLeftDecalStep == nLeftDecalLen)//right
						{
							if (x == nSpecifiedLeftDecalStart+nSpecifiedLeftDecalHeight)//top
								vLeftDecal_TR = vTiles_TR ;
							if (x == nSpecifiedLeftDecalStart)//bot
								vLeftDecal_BR = vTiles_TL ;
						}
					}
					// rumble strip
					if (x == 0)
						if (AddSingleRect (
										   vTiles_TR, tx2e, fTVEnd,
										   vTiles_TL, tx1e, fTVEnd,
										   vTiles_BR, tx2, fTVStart,
										   vTiles_BL, tx1, fTVStart,
										   ndxTexRumble, D3DXVECTOR3(0,1,0)) == NULL)
							return E_FAIL ;
				}
				else
				{
					if (AddSingleRect (
									   vTiles_TL, tx1e, fTVEnd,
									   vTiles_TR, tx2e, fTVEnd,
									   vTiles_BL, tx1, fTVStart,
									   vTiles_BR, tx2, fTVStart,
									   ndxTex, D3DXVECTOR3(0,1,0)) == NULL)
						return E_FAIL ;
					
					// rumble strip
					if (x == 0)
						if (AddSingleRect (
										   vTiles_TL, tx1e, fTVEnd,
										   vTiles_TR, tx2e, fTVEnd,
										   vTiles_BL, tx1, fTVStart,
										   vTiles_BR, tx2, fTVStart,
										   ndxTexRumble, D3DXVECTOR3(0,1,0)) == NULL)
							return E_FAIL ;
				}
			}
		}
		else
		{
			if (bBuildingRightAVI)
				nRightAVIStep ++ ;
			
			if (bBuildingRightDecal)
				nRightDecalStep ++ ;

			for (int x = nSteps - 1 ; x >= 0 ; x --)
			{
				D3DXVECTOR3 vTiles_BL, vTiles_BR, vTiles_TL, vTiles_TR ;
				
				// DODGY .05!
				if (x == nSteps - 1)
					tx1 = fTUStart ;//.05
				else
					tx1 = tx2 ;
				tx2 = tx1 + (fBankLength[nSteps-x] / TEX_TO_ABS) ;

				vTiles_BL = D3DXVECTOR3 (vRef1[x].x,	 vRef1[x].y,   vRef1[x].z) ;
				vTiles_BR = D3DXVECTOR3 (vRef1[x+1].x, vRef1[x+1].y, vRef1[x+1].z) ;
				vTiles_TL = D3DXVECTOR3 (vRef2[x].x,   vRef2[x].y,   vRef2[x].z) ;
				vTiles_TR = D3DXVECTOR3 (vRef2[x+1].x, vRef2[x+1].y, vRef2[x+1].z) ;

				vTiles_BL.y += fStepHeightsStart [nSteps-x] ;
				vTiles_TL.y += fStepHeightsEnd [nSteps-x] ;
				vTiles_BR.y += fStepHeightsStart [nSteps-x-1] ;
				vTiles_TR.y += fStepHeightsEnd [nSteps-x-1] ;
 
				if (fSide == +1)
				{
					if (AddSingleRect (
									   vTiles_TR, tx1, fTVEnd,
									   vTiles_TL, tx2, fTVEnd,
									   vTiles_BR, tx1, fTVStart,
									   vTiles_BL, tx2, fTVStart,
 									   ndxTex, D3DXVECTOR3(0,1,0)) == NULL)
						return E_FAIL ;

					// AVIs
					if (bBuildingRightAVI && (x == nSteps-4 || x == nSteps-3))
					{
						if (nRightAVIStep == 1)//Right
						{
							if (x == nSteps-3)//top
								vRightAVI_TL = vTiles_BR ;
							if (x == nSteps-4)//bot
								vRightAVI_BL = vTiles_BL ;
						}
						if (nRightAVIStep == nRightAVILen)//right
						{
							if (x == nSteps-3)//top
								vRightAVI_TR = vTiles_TR ;
							if (x == nSteps-4 )//bot
								vRightAVI_BR = vTiles_TL ;
						}
					}
					// Decals
 					if (bBuildingRightDecal && (x == nSteps-1-nSpecifiedLeftDecalStart ||
												x == nSteps-1-nSpecifiedLeftDecalStart-nSpecifiedLeftDecalHeight))
					{
						if (nRightDecalStep == 1)//Right
						{
							if (x == nSteps-1-nSpecifiedLeftDecalStart)//top
								vRightDecal_TL = vTiles_BR ;
							if (x == nSteps-1-nSpecifiedLeftDecalStart-nSpecifiedLeftDecalHeight)//bot
								vRightDecal_BL = vTiles_BL ;
						}
						if (nRightDecalStep == nRightDecalLen)//right
						{
							if (x == nSteps-1-nSpecifiedLeftDecalStart)//top
								vRightDecal_TR = vTiles_TR ;
							if (x == nSteps-1-nSpecifiedLeftDecalStart-nSpecifiedLeftDecalHeight)//bot
								vRightDecal_BR = vTiles_TL ;
						}
					}

					// rumble strip
					if (x == nSteps - 1)
						if (AddSingleRect (
										   vTiles_TR, tx1, fTVEnd,
										   vTiles_TL, tx2, fTVEnd,
										   vTiles_BR, tx1, fTVStart,
										   vTiles_BL, tx2, fTVStart,
										   ndxTexRumble, D3DXVECTOR3(0,1,0)) == NULL)
							return E_FAIL ;
				}
				else
				{
					if (AddSingleRect (
									   vTiles_TL, tx2, fTVEnd,
									   vTiles_TR, tx1, fTVEnd,
									   vTiles_BL, tx2, fTVStart,
									   vTiles_BR, tx1, fTVStart,
									   ndxTex, D3DXVECTOR3(0,1,0)) == NULL)
						return E_FAIL ;
					
					// rumble strip
					if (x == nSteps - 1)
						if (AddSingleRect (
										   vTiles_TL, tx2, fTVEnd,
										   vTiles_TR, tx1, fTVEnd,
										   vTiles_BL, tx2, fTVStart,
										   vTiles_BR, tx1, fTVStart,
										   ndxTexRumble, D3DXVECTOR3(0,1,0)) == NULL)
							return E_FAIL ;
				}
			}
		}
				
		//if (fSide == +1)
		{
			// Build AVIs
			if (nLeftAVIStep == nLeftAVILen)
			{
				/*bBuildingLeftAVI = FALSE ;
				nLeftAVIStep = 0 ;

				CRenderObject *paro ;
				paro = new CRenderObject() ;
				paro->OneTimeInit() ;
				paro->m_dwFVF = D3DFVF_LVERTEX ;
				Segments[nSegments].proAVIs[Segments[nSegments].nAVIs++] = paro ;

				if (AddSingleRectAVI (paro,
								   vLeftAVI_TR, 1, 1,
								   vLeftAVI_TL, 0, 1,
								   vLeftAVI_BR, 1, 0,
								   vLeftAVI_BL, 0, 0,
								   VID_TEX_START + 0, D3DXVECTOR3(0,1,0), FALSE) == NULL)
					return E_FAIL ;*/
			}
			if (nRightAVIStep == nRightAVILen)
			{
				/*bBuildingRightAVI = FALSE ;
				nRightAVIStep = 0 ;

				CRenderObject *paro ;
				paro = new CRenderObject() ;
				paro->OneTimeInit() ;
				paro->m_dwFVF = D3DFVF_LVERTEX ;
				Segments[nSegments].proAVIs[Segments[nSegments].nAVIs++] = paro ;

				if (AddSingleRectAVI (paro,
								   vRightAVI_TR, 1, 1,
								   vRightAVI_TL, 0, 1,
								   vRightAVI_BR, 1, 0,
								   vRightAVI_BL, 0, 0,
								   VID_TEX_START + 0, D3DXVECTOR3(0,1,0), FALSE) == NULL)
					return E_FAIL ;*/
			}

			// Build Decals
			if (bBuildingLeftDecal && nLeftDecalStep == nLeftDecalLen)
			{
				/*bBuildingLeftDecal = FALSE ;
				nLeftDecalStep = 0 ;
				
				CRenderObject *pdro ;
				pdro = new CRenderObject() ;
				pdro->OneTimeInit() ;
				pdro->m_dwFVF = D3DFVF_LVERTEX ;
				Segments[nSegments].proDecals[Segments[nSegments].nDecals++] = pdro ;

				if (AddSingleRectDecal (pdro,
								   vLeftDecal_TR, 1, 0,
								   vLeftDecal_TL, 0, 0,
								   vLeftDecal_BR, 1, 1,
								   vLeftDecal_BL, 0, 1,
								   ndxLeftDecalTex, D3DXVECTOR3(0,1,0), FALSE) == NULL)
					return E_FAIL ;*/
			}
			if (bBuildingRightDecal && nRightDecalStep == nRightDecalLen)
			{
				/*bBuildingRightDecal = FALSE ;
				nRightDecalStep = 0 ;
				
				CRenderObject *pdro ;
				pdro = new CRenderObject() ;
				pdro->OneTimeInit() ;
				pdro->m_dwFVF = D3DFVF_LVERTEX ;
				Segments[nSegments].proDecals[Segments[nSegments].nDecals++] = pdro ;

				if (AddSingleRectDecal (pdro,
								   vRightDecal_TR, 0, 1,
								   vRightDecal_TL, 1, 1,
								   vRightDecal_BR, 0, 0,
								   vRightDecal_BL, 1, 0,
								   ndxRightDecalTex, D3DXVECTOR3(0,1,0), FALSE) == NULL)
					return E_FAIL ;*/
			}
		}

		return S_OK ;
	}


	X7LT *CRaceTrackSegment::AddSingleRect(D3DXVECTOR3 vTL, float tx_tl, float ty_tl,
	 									   D3DXVECTOR3 vTR, float tx_tr, float ty_tr,
										   D3DXVECTOR3 vBL, float tx_bl, float ty_bl,
										   D3DXVECTOR3 vBR, float tx_br, float ty_br,
										   unsigned ndxTex, D3DXVECTOR3 n)
	{
		UNCOLOREDVERTEX pVerts[3] ;
		D3DCOLOR    c[3], s[3] ;
		X7LT	    *p ;
		static float a = 0.5f * 255.0f ;
		static float r = 0.9f * 255.0f ;
		static float g = 0.9f * 255.0f ;
		static float b = 0.9f * 255.0f ;

	//
	// Set color and specular.
	//
		s[0] = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		s[1] = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		s[2] = D3DCOLOR_ARGB(0, 0, 0, 0) ;
		c[0] = D3DCOLOR_ARGB((int)a, (int)r, (int)g, (int)b) ;
		c[1] = D3DCOLOR_ARGB((int)a, (int)r, (int)g, (int)b) ;
		c[2] = D3DCOLOR_ARGB((int)a, (int)r, (int)g, (int)b) ;

		pVerts[0] = UNCOLOREDVERTEX(vTL.x, vTL.y, vTL.z, n.x, n.y, n.z, tx_tl, ty_tl) ;
		pVerts[1] = UNCOLOREDVERTEX(vBL.x, vBL.y, vBL.z, n.x, n.y, n.z, tx_bl, ty_bl) ;
		pVerts[2] = UNCOLOREDVERTEX(vTR.x, vTR.y, vTR.z, n.x, n.y, n.z, tx_tr, ty_tr) ;
		if ((p = AddX7LT(pVerts, &c[0], &s[0], ndxTex, 0, NULL, NULL)) == NULL)
			return NULL ;

		pVerts[0] = UNCOLOREDVERTEX(vTR.x, vTR.y, vTR.z, n.x, n.y, n.z, tx_tr, ty_tr) ;
		pVerts[1] = UNCOLOREDVERTEX(vBL.x, vBL.y, vBL.z, n.x, n.y, n.z, tx_bl, ty_bl) ;
		pVerts[2] = UNCOLOREDVERTEX(vBR.x, vBR.y, vBR.z, n.x, n.y, n.z, tx_br, ty_br) ;
		if ((p = AddX7LT(pVerts, &c[0], &s[0], ndxTex, 0, NULL, NULL)) == NULL)
			return NULL ;

		return p ;
	}
