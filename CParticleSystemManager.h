#ifndef __PARTICLESYSTEMMANAGER__
#define __PARTICLESYSTEMMANAGER__

#include <D3DX8.h>
#include "dxutil.h"

#include "x7_primitives.h"

#define MAX_PARTICLES	1024
#define MAX_PSMVB_VERTS	(4 * MAX_PARTICLES)			// max no. of verts. in shared VB

class CParticleSystemManager
{
public:
	CParticleSystemManager() ;
	~CParticleSystemManager() ;

//
// List of Particle Systems:
//
	RegisterNewParticleSystem() ;
	CParticleSystem *m_pPSs[MAX_PARTICLE_SYSTEMS] ;	// null-terminated list

//
// Common VB and IB:
//
	LPDIRECT3DVERTEXBUFFER8 m_pVB ;
	DWORD					m_dwNumPSs ;

	D3DMATERIAL8			m_mtrlDef ;

//
// Framework events:
//
    HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
    HRESULT InvalidateDeviceObjects();
	HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult) ;
	HRESULT Render(LPDIRECT3DDEVICE8 pDev) ;
} ;

#endif