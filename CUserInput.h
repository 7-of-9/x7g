#ifndef __CUserInput__
#define __CUserInput__

class CUserInput ;

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include "x7_primitives.h"
#include "x7_utils.h"
#include "tex.h"
	
// large lag
//#define MOUSE_HISTORY_BUF		50
//#define MOUSE_SMOOTH_WEIGHT_D	1.0f

// mouse smoothing
#define MOUSE_HISTORY_BUF		1
#define MOUSE_SMOOTH_WEIGHT_D	1

// flight mapping
#define MAX_FLIGHTMAP			20.0f		// maximum magnitude assigned to steering public-access flags

struct FLIGHT_CONTROLS
{
	float fAccelerate, fBrake ;
	
	float fSteerLeft, fSteerRight ;
	float fDive, fClimb ;
	float fRollLeft, fRollRight ;
	float fStrafeLeft, fStrafeRight ;
	BOOL  bSteerAndRoll ;
	
	BOOL  bNextWeapon, bPrevWeapon ;
	BOOL  bFireA, bFireB ;
} ;

class CUserInput
{
public:
//
// Keyboard:
//
	LPDIRECTINPUT8       m_pDI ;
	LPDIRECTINPUTDEVICE8 m_pKeyboard ;
    BYTE				 m_bKB_Prev[256] ;
    BYTE				 m_bKB_Cur[256] ;
    BOOL				 m_bKB_Pressed[256] ;
	BOOL				 m_bGotPrevKBBuf ;
	BOOL				 m_bF1, m_bF2, m_bF3, m_bF4, m_bF5, m_bF6, m_bF7, m_bF8, m_bF9, m_bF10, m_bF11, m_bF12 ;
	BOOL				 m_bAnyKey ;

//
// Mouse:
//
	LPDIRECTINPUTDEVICE8 m_pMouse ;
	xVec3				 m_vMouseHistory[MOUSE_HISTORY_BUF] ;
	float				 m_fMousePrevZ ;
	float				 m_fMouseX, m_fMouseY, m_fMouseZ ;
	BOOL				 m_bMouseLB, m_bMouseCB, m_bMouseRB ;
	bool				 m_bMouseWheelUp, m_bMouseWheelDown ;
	BOOL				 m_bScanForChangeInMouseWheel ;

	float				 m_fMouseStickyX, m_fMouseStickyY, m_fMouseStickyZ ;
	bool				 m_bNonStickyMouse ;
	float				 m_fMouseSensitivity ;
	bool				 m_bMouseInvert ;


//
// Joystick:
//
	LPDIRECTINPUTDEVICE8 m_pJoystick ;
	BOOL				 m_bJoystickPresent ;
	GUID				 m_guidJoystick ;
	TCHAR				 m_szJoystickInstanceName[MAX_PATH] ;
	TCHAR				 m_szJoystickProductName[MAX_PATH] ;

//
// Construction / Destruction:
//
	CUserInput( HWND hWnd,
				bool bNonStickyMouse,
				float fMouseSensitivity,
				bool bMouseInvert,
				bool bMouseExclusive );
	~CUserInput() ;
	Init() ;

//
// Status Polling:
//
	HRESULT Poll( bool bInGame );
	HRESULT PollKeyboard( bool bInGame );
	HRESULT PollMouse( bool bInGame );
	HRESULT PollMouseSticky( bool bInGame );
	HRESULT PollJoystick( bool bInGame );

//
// Mapped, Public-Access Flags:
//
	FLIGHT_CONTROLS m_FlightControls ;
	
} ;


BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) ;

BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext ) ;


#endif