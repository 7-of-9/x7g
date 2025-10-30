#ifndef __CRaceTrackSegment__
#define __CRaceTrackSegment__

	#include <D3DX8Math.h>
	#include "x7_primitives.h"
	#include "x7_utils.h"
	#include "tex.h"
	#include "CProceduralRenderObject.h"

#define MAX_STEPS			128
#define TILE_WIDTH			4
#define KMS_2_ABS			100
#define TEX_TO_ABS			20.0f

#define BANK_GRADUATIONS	5
#define BANK_WIDTH			30.0f


class CRaceTrackSegment : public CProceduralRenderObject
{
public:

//
// Construction / Destruction:
//
	CRaceTrackSegment(float fSide,
					  int	nSteps,
					  float aKMs,
					  float bKMs,
					  float fSineStart,
					  float fSinePerc,
					  float fSineMag,
					  float fLBHeightDelta,
					  float fRBHeightDelta,
					  float fWidthDelta,
					  BOOL  bHeHe,
					  int	nHeHeStart,
					  int	nHeHeLen,
					  float fLBStartHeight,
					  float fRBStartHeight,
					  float fStartWidth,
					  float fBL_x,
					  float fBL_y,
					  float fBL_z,
					  float fStartOrientation,

					  LPD3DXVECTOR3 lpvEnd,
					  float *pfEndWidth,
					  float *pfEndOrientation) ;
	~CRaceTrackSegment() ;

//
// Geometry Construction:
//
	HRESULT AddBend2(float fSide,
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
					 float *pfEndOrientation) ;

	HRESULT AddBank2(xVec3 *vRef1,
					 xVec3 *vRef2,
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
					 int  ndxRightDecalTex) ;

	X7LT *AddSingleRect(xVec3 vTL, float tx_tl, float ty_tl,
	 					xVec3 vTR, float tx_tr, float ty_tr,
						xVec3 vBL, float tx_bl, float ty_bl,
						xVec3 vBR, float tx_br, float ty_br,
						unsigned ndxTex, xVec3 n) ;

//
// Flight Path Points:
//
	int m_nFPPs ;
	xVec3 m_vFPPs[MAX_STEPS] ;
} ;

#endif
