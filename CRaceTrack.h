#ifndef __CRaceTrack__
#define __CRaceTrack__

#include <D3DX8Math.h>

	#include "x7_primitives.h"
	#include "x7_utils.h"
	#include "CRaceTrackSegment.h"
	#include "CTextures.h"
	#include "CUserInput.h"

#define MAX_SEGS			128
#define BANK_MAX_MAG		+60.0f
#define BANK_MIN_HEIGHT		-60.0f

class CRaceTrack
{
public:

//
// Construction / Destruction:
//
	CRaceTrack() ;
	~CRaceTrack() ;

//
// Track Segments:
//
	CRaceTrackSegment *m_pSegs[1024] ;
	int m_nSegs ;

//
// Track Definition:
//
	void CreateDefinition(BOOL bTestTrack, BOOL bRandomTrack) ;

	typedef struct
	{
		D3DVECTOR	vStart ;
		int			nSegs ;
		float		fStartWidth ;
		int			nSide[MAX_SEGS] ;
		int			nSteps[MAX_SEGS] ;
		float		fA[MAX_SEGS] ;
		float		fB[MAX_SEGS] ;
		float		fSineStart[MAX_SEGS] ;
		float		fSinePerc[MAX_SEGS] ;
		float		fSineMag[MAX_SEGS] ;
		float		fLBHeightDelta[MAX_SEGS] ;
		float		fRBHeightDelta[MAX_SEGS] ;
		float		fLBStartHeight[MAX_SEGS] ;
		float		fRBStartHeight[MAX_SEGS] ;
		float		fWidthDelta[MAX_SEGS] ;		// 1.0f = ~+50
		int			nTotalFPPs ;
	} _TRACKDEF ;
	_TRACKDEF tdefTest ;	  

//
// D3DFrame Event Hooks:
//
	HRESULT RestoreDeviceObjects(LPDIRECT3D8 pD3D, LPDIRECT3DDEVICE8 pDev) ;
	HRESULT	InvalidateDeviceObjects() ;
    HRESULT Render(LPDIRECT3DDEVICE8 pDev, CTextures	*pTex, unsigned long *pulPolyCount) ;
    HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, CCamera *pCamera, xMat *pmatProj, D3DVIEWPORT8 *pVP) ;

//
// Visibility Determination:
//
	DetermineVisibility(LPDIRECT3DDEVICE8 pDev, CCamera *pCam, xMat *pmatProj, D3DVIEWPORT8 *pVP) ;
	int m_nVisibleSegs ;
} ;

#endif