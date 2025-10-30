#ifndef __SCREENOVERLAYMANAGER__
#define __SCREENOVERLAYMANAGER__

#include <D3DX8.h>
#include "./D3DFrame8.1 Custom/include/dxutil.h"

#include "CScreenOverlay.h"
#include "x7_primitives.h"
#include "x7_utils.h"

#define MAX_OVERLAYS	128					// max no. of overlays
#define MAX_OMVB_VERTS	(4 * MAX_OVERLAYS)	// max no. of verts. in shared VB

enum e_OverlayRenderTypes
{
	ORT_SLOW,
	ORT_FAST,
	MAX_OVERLAY_RENDER_TYPES
} ;


enum e_OverlayTypes
{
	OT_HUD_SCALEBAR = 0,
	OT_HUD_LEGEND,
	OT_HUD_STRING1,
	OT_HUD_STRING2,
	OT_HUD_STRING3,
	OT_HUD_STRING4,
	OT_HUD_SCANNERBACKG,
	OT_HUD_TARGETICON,
	OT_HUD_CROSSHAIR,
	OT_HUD_PAIN,
	OT_MENU,
	OT_FULLSCREENQUAD,
	MAX_OVERLAY_TYPES
} ;

class CScreenOverlayManager
{
public:
	CScreenOverlayManager() ;
	~CScreenOverlayManager() ;

//
// List of Overlays:
//
	RegisterNewOverlay( CScreenOverlay *pOverlay );
	UnregisterOverlay( CScreenOverlay *pOverlay );
	CScreenOverlay* m_pOverlays[MAX_OVERLAYS] ;	// null-terminated list

//
// Common VB and IB:
//
	LPDIRECT3DVERTEXBUFFER8 m_pVB ;
	LPDIRECT3DINDEXBUFFER8  m_pIB ;
	BOOL					m_bVBLocked ;
	
	DWORD					m_dwNumOverlays ;

	D3DMATERIAL8			m_mtrlDef ;

	xMat				m_matWorldIdent ;

void SetProjMat( LPDIRECT3DDEVICE8 pDev );

//
// Pointer to locked VB data:
//
	HRESULT LockVB() ;
	HRESULT UnlockVB() ;
	OVERLAYVERTEX*			m_pVBVertices ;


//
// Framework events:
//
    HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pDev, FLOAT fAspect) ;
    HRESULT InvalidateDeviceObjects();
	HRESULT FrameMove(LPDIRECT3DDEVICE8 pDev, float fTimeMult, float fAspect) ;
	HRESULT Render(LPDIRECT3DDEVICE8 pDev, int nTag, int nMethod) ;
} ;

#endif