#ifndef __PARTICLESYSTEM__
#define __PARTICLESYSTEM__

#include <stdio.h>
#include <D3DX8.h>
#include "x7_utils.h"


//-----------------------------------------------------------------------------
// Particle
//-----------------------------------------------------------------------------
struct PARTICLE
{
	BOOL		m_bSpark;

    xVec3 m_vPos;			// Current position
    xVec3 m_vVel;			// Current velocity

    xVec3 m_vPos0;		// Initial position
    xVec3 m_vVel0;		// Initial velocity
    FLOAT       m_fTime0;		// Time of creation

	FLOAT		m_fAge ;		// in seconds

	FLOAT		m_fAbsLifeTime;	// in seconds, 0 for indefinite, will only dies when m_vPos.y < 0.

	BOOL		m_bFeelGravity; // Whether gravity affects particle, or not

    D3DXCOLOR   m_clrDiffuse;	// Initial diffuse color
    D3DXCOLOR   m_clrFade;		// Faded diffuse color
    FLOAT       m_fFade;		// Fade progression

	BOOL		m_bMarkedForDeath ;	// we can set this to allow FrameMove to kill the particle

    PARTICLE*   m_pNext;		// Next particle in list
};

//
// Color fades:
//
enum PARTICLE_COLORS { COLOR_WHITE, COLOR_RED, COLOR_BLUE, NUM_PARTICLE_COLORS };
extern D3DXCOLOR g_ParticleColor[ NUM_PARTICLE_COLORS ];
extern D3DXCOLOR g_ParticleColorFade[ NUM_PARTICLE_COLORS ];

class CParticleSystem
{
//
// Construction / Destruction:
//
public:
	CParticleSystem( DWORD dwFlush, DWORD dwDiscard, float fRadius );
	~CParticleSystem() ;
	Clear() ;

    float	  m_fRadius;
	DWORD     m_dwBase;
	DWORD     m_dwFlush;
    DWORD     m_dwDiscard;

    DWORD     m_dwParticles;
    DWORD     m_dwParticlesLim;
    PARTICLE* m_pParticles;
    PARTICLE* m_pParticlesFree;

	xVec3	m_vMin, m_vMax ;

	float	  m_fTime ;

//
// Geometry:
//
    //LPDIRECT3DVERTEXBUFFER8 m_pVB;

//
// Framework events:
//
	HRESULT Render( LPDIRECT3DDEVICE8 pDev,
					float fPointSize,
					unsigned long *pulPolycount,
					LPDIRECT3DVERTEXBUFFER8 pVB,
					xMatUA* pmatWorld,
					BOOL bMustFillVB,
					xMatUA* pmatCurWorld );

	HRESULT FrameMove( float fFPS,
					   xVec3* pvMove,
					   FLOAT fSecsPerFrame,
					   DWORD dwNumParticlesToEmit,
					   FLOAT fAbsLifeTime,
					   BOOL  bFeelGravity,
					   const D3DXCOLOR &clrEmitColor,
					   const D3DXCOLOR &clrFadeColor,
					   xVec3* pvEmitVel,
					   xVec3* pvEmitPosition,
					   BOOL bSphericalEmit,
					   float fScatter,
					   BOOL bTrackBoundingVolume );

	ImpactTerrain( xVec3* pvDir, float fFPS, float fLenSQ, float fImpactFXLifetime, float fSpeed );
	ImpactFighter( xVec3* pvDir, float fFPS, float fLenSQ, float fImpactFXLifetime, float fSpeed, xVec3* pvVel );
	CulledFrameMove( xVec3* pvMove );
	
} ;

#endif