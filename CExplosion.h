#ifndef __CExplosion__
#define __CExplosion__

class CExplosion ;

#include <D3DX8Math.h>
#include "CBillboard.h"
#include "CParticleSystem.h"
#include "CTextures.h"
#include "x7_primitives.h"
#include "x7_utils.h"
#include "tex.h"

#define MAX_EXPLOSION_SUBEXPS 16

struct EXPLOSION_TYPE_DETAILS
{
	int ndxTex ;
	int ndxMaxTex ;
} ;

enum e_ExpTypes
{
	ET_ALPHA = 0,								// 35 frames, resource\tex\, exp2, exp2_tex_start, 256x256
	ET_BETA,									// 16 frames, resource\tex\exp, exp_tex_start, 32x32
	MAX_EXP_TYPES
} ;

extern EXPLOSION_TYPE_DETAILS g_Explosions[] ;

class CExplosion
{
public:
//
// Construction / Destruction:
//
	CExplosion( float fLifespan, BOOL bShockwave, BOOL bParticles, int nSubexplosions );
	~CExplosion() ;

	float			m_fLifespan ;				// total lifespan of the effect in seconds
	float			m_fAge ;					// current age of the effect in seconds

	struct SUBEXPLOSION
	{
		BOOL		bActive ;
		xVec2 vRelCenter ;				// center of billboard, for rendering; relative to value passed to Render()
		float		fRelSize ;					// length of the billboard sides; relative to value passed to Render()
		int			ndxTex ;					// texture address
		int			ndxMaxTexFrame ;			// maximum frames in texture animation
		int			ndxCurTexFrame ;			// curent frame in texture animation
		float		fLifespan ;					// total lifetime of subexplosion
		float		fCurFrameAge ;				// age of current frame
		float		fDelay ;					// time to wait before commencing animation in seconds
	} ;
	SUBEXPLOSION	m_SubExp[ MAX_EXPLOSION_SUBEXPS ];
	int				m_nSubExps ;				// no. of sub-explosions
	CBillboard*		m_pBB ;						// explosion render object

	CBillboard*		m_pBB_Shockwave ;			// shockwave render object

	CParticleSystem*m_pPS_Explosion ;			// particle system

	InitSubExplosion( e_ExpTypes Type, float fLifespan, float fRelSize, float fDelay, xVec2* pvRelCenter );

	Reset() ;

//
// D3DFrame Event Hooks:
//
	HRESULT FrameMove( LPDIRECT3DDEVICE8 pDev, float fFPS, xVec3* pvSourceVelocity, xVec3* pvSource );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE8 pDev );
	HRESULT InvalidateDeviceObjects() ;
	HRESULT Render( LPDIRECT3DDEVICE8 pDev, CTextures* pTex, unsigned long *pulPolyCount, CCamera *pCam, float fSize, xVec3* pvCenter, float fShockwaveRadius, LPDIRECT3DVERTEXBUFFER8 pSharedParticleSystemVB, int SB, int DB );
} ;

#endif
