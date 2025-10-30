#pragma warning( disable : 4183 )

#ifndef __CCamera__
#define __CCamera__

class CCamera ;

#include <D3DX8Math.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "CUserInput.h"

#define DIST_FROM_EYE_TO_LOOKAT 1.0f

class CCamera
{
	
public:
//
// Construction / Destruction:
//
	CCamera() ;
	~CCamera() ;

//
// Camera Data:
//
	SetViewMatrix(LPDIRECT3DDEVICE8 pDev, BOOL bFlipUpVec) ;

	xVec3 m_vEyePt, m_vLookAtPt, m_vUpVec ;
	xVec3 m_vOrientation ;
	xMat  m_matView ;

	xVec3 m_vMouseRelativeXAxis ;
	xVec3 m_vMouseRelativeYAxis ;
	float		m_fMouseLockCurRot ;

//
// Manual Changes:
//
	SetManualFromInput(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, float fMinY) ;
	SetManualFromValues(LPDIRECT3DDEVICE8 pDev, float x, float y, float z) ;

//
// Presets:
//
	SetPresetFromInput(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput) ;

//
// Rotation around a point over time:
//
	SetShowoff(LPDIRECT3DDEVICE8 pDev, xVec3 *vCenter, float fRadius, float fFPS, float fYaw, float fPitch, float fRoll) ;
} ;

#endif


