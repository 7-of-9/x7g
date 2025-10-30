#pragma warning( disable : 4183 )	// 'missing return type'

#include "CUserInput.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CUserInput::CUserInput( HWND hWnd,
						    bool bNonStickyMouse,
							float fMouseSensitivity,
							bool bMouseInvert,
							bool bMouseExclusive )
	{
		HRESULT hr ;
		int i ;

		m_pDI = NULL ;
		m_pKeyboard = NULL ;
		m_pMouse = NULL ;
		m_pJoystick = NULL ;

		hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL);
		assert( !FAILED(hr) );
    
	//
	// Keyboard:
	//
		hr = m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
		assert( !FAILED(hr) );
		hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
		assert( !FAILED(hr) );
		hr = m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert( !FAILED(hr) );
		m_pKeyboard->Acquire() ;
		for (i = 0 ; i < 256 ; i ++) // single-key detection
			m_bKB_Pressed[i] = FALSE ;
		m_bGotPrevKBBuf = FALSE ;

	//
	// Mouse:
	//
		hr = m_pDI->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
		assert( !FAILED(hr) );
		hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
		assert( !FAILED(hr) );
		hr = m_pMouse->SetCooperativeLevel( hWnd,
											DISCL_FOREGROUND |
											  ( bMouseExclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE ) );
		assert( !FAILED(hr) );
				/*DIPROPDWORD dipdw ;
				// BUFFERED
				dipdw.diph.dwSize       = sizeof(DIPROPDWORD) ;
				dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER) ;
				dipdw.diph.dwObj        = 0 ;
				dipdw.diph.dwHow        = DIPH_DEVICE ;
				dipdw.dwData            = MOUSE_BUFFER_SIZE ;
				if (FAILED(hr = m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
					Msg("CUserInput::CUserInput(): FAILED(m_pMouse->SetProperty(...)) == TRUE") ;*/
		m_pMouse->Acquire() ;
		for (i = 0 ; i < MOUSE_HISTORY_BUF ; i ++) // mouse-smoothing buffer
			m_vMouseHistory[i] = xVec3( 0,0,0 );
		m_fMousePrevZ = 0 ;
		m_bScanForChangeInMouseWheel = TRUE ;

		m_fMouseStickyX = m_fMouseStickyY = m_fMouseStickyZ = 0 ;
		m_bNonStickyMouse = bNonStickyMouse ;
		m_fMouseSensitivity = fMouseSensitivity ;
		m_bMouseInvert = bMouseInvert ;

	//
	// Joystick:
	// (just uses first attached stick)
	//
		m_bJoystickPresent = FALSE ;
		m_szJoystickProductName[0] = '\x0';
		m_szJoystickInstanceName[0] = '\x0';
		m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, 
						   EnumJoysticksCallback, 
						   this, 
						   DIEDFL_ATTACHEDONLY) ;
		if (m_bJoystickPresent)
		{
			hr = m_pDI->CreateDevice(m_guidJoystick, &m_pJoystick, NULL);
			assert( !FAILED(hr) );
			hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick);
			assert( !FAILED(hr) );
			hr = m_pJoystick->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
			assert( !FAILED(hr) );

		// set relative axis mode
			/*DIPROPDWORD dipdw ;
			dipdw.diph.dwSize       = sizeof(DIPROPDWORD) ;
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER) ;
			dipdw.diph.dwObj        = 0 ;
			dipdw.diph.dwHow        = DIPH_DEVICE ;
			dipdw.dwData            = DIPROPAXISMODE_REL ;
			if (FAILED(hr = m_pJoystick->SetProperty(DIPROP_AXISMODE, &dipdw.diph)))
				Msg("CUserInput::CUserInput(): FAILED(m_pJoystick->SetProperty(DIPROP_AXISMODE, ...)) == TRUE") ;*/
			
			m_pJoystick->Acquire() ;

			hr = m_pJoystick->EnumObjects(EnumAxesCallback, this, DIDFT_AXIS);
			assert( !FAILED(hr) );
		}
	}
	CUserInput::~CUserInput()
	{
		if (m_pKeyboard)
		{
			m_pKeyboard->Unacquire() ;
			m_pKeyboard->Release() ;
			m_pKeyboard = NULL ;
		}

		if (m_pJoystick) 
		{
			m_pJoystick->Unacquire() ;
			m_pJoystick->Release() ;
			m_pJoystick = NULL ;
		}
    
		if (m_pMouse) 
		{
			m_pMouse->Unacquire() ;
			m_pMouse->Release() ;
			m_pMouse = NULL ;
		}

		if (m_pDI)
		{
			m_pDI->Release() ;
			m_pDI = NULL ;
		}
	}

	CUserInput::Init()
	{
		m_fMouseStickyX = m_fMouseStickyY = m_fMouseStickyZ = 0 ;
	}

/*
 *
 * Device Enumeration Callback:
 *
 * Dumb; just grabs first enumerated DI8DEVTYPE_JOYSTICK.
 *
 */
	BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
	{ 
		if (GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_JOYSTICK)
		{
			((CUserInput*)pvRef)->m_bJoystickPresent = TRUE ;
			((CUserInput*)pvRef)->m_guidJoystick = lpddi->guidInstance ;
			strcpy(((CUserInput*)pvRef)->m_szJoystickInstanceName, lpddi->tszInstanceName) ;
			strcpy(((CUserInput*)pvRef)->m_szJoystickProductName, lpddi->tszProductName) ;
			return DIENUM_STOP ;
		}
		return DIENUM_CONTINUE ;
	}

/*
 *
 * Joystick Axes Enumeration Callback:
 *
 * Sets all axes -1000 to +1000.
 *
 */
	BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
	{
		CUserInput* pUI = (CUserInput*)pContext ;

		DIPROPRANGE diprg; 
		diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprg.diph.dwHow        = DIPH_BYID; 
		diprg.diph.dwObj        = pdidoi->dwType; // specify the enumerated axis
		diprg.lMin              = -1000; 
		diprg.lMax              = +1000; 
    
	// figure out which axes the joystick supports
		if (pdidoi->guidType == GUID_XAxis)
		{
		// present on SideWinder Precision 2
		}
		if (pdidoi->guidType == GUID_YAxis)
		{
		// present on SideWinder Precision 2
		}
		if (pdidoi->guidType == GUID_ZAxis)
		{
		}

		if (pdidoi->guidType == GUID_RxAxis)
		{
		}
		if (pdidoi->guidType == GUID_RyAxis)
		{
		}
		if (pdidoi->guidType == GUID_RzAxis)
		{
		// present on SideWinder Precision 2
		}

		if (pdidoi->guidType == GUID_Slider)
		{
		// present on SideWinder Precision 2
			diprg.lMin = +0; 
			diprg.lMax = +1000; 
		}

	// set the range for the axis
		if( FAILED( pUI->m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
			return DIENUM_STOP;

		return DIENUM_CONTINUE;
	}

/*
 *
 * Status Polling:
 *
 */
	HRESULT CUserInput::Poll( bool bInGame )
	{
	// default mapped values
		m_FlightControls.fSteerLeft = 0 ;
		m_FlightControls.fSteerRight = 0 ;
		m_FlightControls.fStrafeLeft = 0 ;
		m_FlightControls.fStrafeRight = 0 ;
		m_FlightControls.fAccelerate = 0 ;
		m_FlightControls.fBrake = 0 ;
		m_FlightControls.fDive = 0 ;
		m_FlightControls.fClimb = 0 ;
		m_FlightControls.fRollLeft = 0 ;
		m_FlightControls.fRollRight = 0 ;
		m_FlightControls.bFireA = FALSE ;
		m_FlightControls.bFireB = FALSE ;
		m_FlightControls.bNextWeapon = FALSE ;
		m_FlightControls.bPrevWeapon = FALSE ;
		m_FlightControls.bSteerAndRoll = FALSE ;

	// poll
		PollKeyboard( bInGame );

		/*if( m_bNonStickyMouse )		NON_STICKY MOUSE POLLING SUCKS BIG TIME, SHOULD FIX IT OR DISABLE IT
			PollMouse( bInGame );
		else*/
			PollMouseSticky( bInGame );

		PollJoystick( bInGame );

		return S_OK ;
	}

/*
 *
 * Joystick Status Polling:
 *
 */
	HRESULT CUserInput::PollJoystick( bool bInGame )
	{
		HRESULT	   hr ;
		DIJOYSTATE dijs ;

		if (!m_pJoystick)
			return E_FAIL ;

		ZeroMemory(&dijs, sizeof(dijs)) ;
		hr = m_pJoystick->GetDeviceState(sizeof(dijs), &dijs) ;
		if (FAILED(hr)) 
		{
			hr = m_pJoystick->Acquire() ;
			while (hr == DIERR_INPUTLOST) 
				hr = m_pJoystick->Acquire() ;

			return E_FAIL ;
		}

	//
	// Map flight controls:
	//
	// Notice that we asign a *fraction* of MAX_FLIGHTMAP, here. This is
	// because the joystick can report this fraction of MAX_FLIGHTMAP *per frame*,
	// with *no movement* from the user. Contrast with the mouse, which the user has to 
	// physically move, per frame, in order to report MAX_FLIGHTMAP... this is in order
	// to balance the two input devices.
	//
		if( dijs.lX < -2 )
			m_FlightControls.fSteerLeft += fabsf((float)dijs.lX) / 1000.0f * MAX_FLIGHTMAP * 0.6f ;
		if( dijs.lX > +2 )
			m_FlightControls.fSteerRight += fabsf((float)dijs.lX) / 1000.0f * MAX_FLIGHTMAP * 0.6f ;

		if( dijs.lY < -2 )
			m_FlightControls.fDive += fabsf((float)dijs.lY) / 1000.0f * MAX_FLIGHTMAP * 0.6f ;
		if( dijs.lY > +2 )
			m_FlightControls.fClimb += fabsf((float)dijs.lY) / 1000.0f * MAX_FLIGHTMAP * 0.6f ;

		if( dijs.lRz < -2 )
			m_FlightControls.fRollLeft += fabsf((float)dijs.lRz) / 1000.0f * MAX_FLIGHTMAP * 0.6f ;
		if( dijs.lRz > +2 )
			m_FlightControls.fRollRight += fabsf((float)dijs.lRz) / 1000.0f * MAX_FLIGHTMAP * 0.6f ;

		if (dijs.rgbButtons[0] & 0x80)
			m_FlightControls.bFireA = TRUE ;
		if (dijs.rgbButtons[1] & 0x80)
			//m_FlightControls.bFireB = TRUE ;
			m_FlightControls.bNextWeapon = TRUE ;

		if (dijs.rgbButtons[2] & 0x80)
			m_FlightControls.fAccelerate += 1.0f ;
		if (dijs.rgbButtons[3] & 0x80)
			m_FlightControls.fBrake += 1.0f ;

		//if( dijs.lRz != 0 )
		//	ods("[%d %d %d %d]\n", (int)dijs.lX, (int)dijs.lY, (int)dijs.lRz, (int)dijs.rglSlider[0]) ;

		return S_OK ;
	}

/*
 *
 * Keyboard Status Polling:
 *
 */
	HRESULT CUserInput::PollKeyboard( bool bInGame )
	{
		HRESULT hr ;

	    m_bF1 = m_bF2 = m_bF3 = m_bF4 = m_bF5 = m_bF6 = m_bF7 = m_bF8 = m_bF9 = m_bF10 = m_bF11 = m_bF12 = FALSE ;
		m_bAnyKey = FALSE ;

	//
	// Get raw data:
	// 
		if (!m_pKeyboard) 
			return E_FAIL ;

		ZeroMemory(&m_bKB_Cur, sizeof(m_bKB_Cur)) ;
		hr = m_pKeyboard->GetDeviceState(sizeof(m_bKB_Cur), &m_bKB_Cur) ;
		if (FAILED(hr)) 
		{
			hr = m_pKeyboard->Acquire();
			while (hr == DIERR_INPUTLOST) 
				hr = m_pKeyboard->Acquire() ;

			return E_FAIL ;
		}

	//
	// Map flight controls:
	//
		// this is no good, steering shouldn't be done by KB:
		/*if (m_bKB_Cur[DIK_LEFT] & 0x80 || m_bKB_Cur[DIK_NUMPAD4] & 0x80)
			m_FlightControls.fSteerLeft += 1.0f ;
		if (m_bKB_Cur[DIK_RIGHT] & 0x80 || m_bKB_Cur[DIK_NUMPAD6] & 0x80)
			m_FlightControls.fSteerRight += 1.0f ;*/

		if (m_bKB_Cur[DIK_NUMPAD1] & 0x80 || m_bKB_Cur[DIK_CAPITAL] & 0x80)
			m_FlightControls.fStrafeLeft += 1.0f ;
		if (m_bKB_Cur[DIK_NUMPAD3] & 0x80 || m_bKB_Cur[DIK_S] & 0x80)
			m_FlightControls.fStrafeRight += 1.0f ;

		if( m_bKB_Cur[ DIK_UP ] & 0x80 || m_bKB_Cur[ DIK_NUMPAD8 ] & 0x80 || m_bKB_Cur[ DIK_Q ] & 0x80 )
			m_FlightControls.fAccelerate += 1.0f ;
		if( m_bKB_Cur[ DIK_DOWN ] & 0x80 || m_bKB_Cur[ DIK_NUMPAD2 ] & 0x80 || m_bKB_Cur[ DIK_A ] & 0x80 )
			m_FlightControls.fBrake += 1.0f ;
		
		if( m_bKB_Cur[ DIK_LSHIFT ] & 0x80 || m_bKB_Cur[ DIK_UP ] & 0x80 )
			m_FlightControls.bSteerAndRoll = TRUE ;

		if( m_bKB_Pressed[ DIK_W ] & 0x80 )
			m_FlightControls.bNextWeapon = TRUE ;

	//
	// Map function keys:
	//
		if (m_bKB_Cur[DIK_F1] & 0x80)
			m_bF1 = TRUE ;
		if (m_bKB_Cur[DIK_F2] & 0x80)
			m_bF2 = TRUE ;
		if (m_bKB_Cur[DIK_F3] & 0x80)
			m_bF3 = TRUE ;
		if (m_bKB_Cur[DIK_F4] & 0x80)
			m_bF4 = TRUE ;
		if (m_bKB_Cur[DIK_F5] & 0x80)
			m_bF5 = TRUE ;
		if (m_bKB_Cur[DIK_F6] & 0x80)
			m_bF6 = TRUE ;
		if (m_bKB_Cur[DIK_F7] & 0x80)
			m_bF7 = TRUE ;
		if (m_bKB_Cur[DIK_F8] & 0x80)
			m_bF8 = TRUE ;
		if (m_bKB_Cur[DIK_F9] & 0x80)
			m_bF9 = TRUE ;
		if (m_bKB_Cur[DIK_F10] & 0x80)
			m_bF10 = TRUE ;
		if (m_bKB_Cur[DIK_F11] & 0x80)
			m_bF11 = TRUE ;
		if (m_bKB_Cur[DIK_F12] & 0x80)
			m_bF12 = TRUE ;

	//
	// Map any key:
	//
		for (int i = 0 ; i < 256 ; i ++)
			if (m_bKB_Cur[i] & 0x80)
			{
				m_bAnyKey = TRUE ;
				break ;
			}

	//
	// Code for detecting *single* key presses:
	//
		if (m_bGotPrevKBBuf)
		{
			for (int i = 0 ; i < 256 ; i ++)
				// if pressed now, but not pressed previously;
				if ((m_bKB_Cur[i] & 0x80) && !(m_bKB_Prev[i] & 0x80))
					m_bKB_Pressed[i] = TRUE ;
				else
					m_bKB_Pressed[i] = FALSE ;
		}
		memcpy(m_bKB_Prev, m_bKB_Cur, sizeof(m_bKB_Cur)) ;
		m_bGotPrevKBBuf = TRUE ;

		return S_OK ;
	}

/*
 *
 * Mouse Status Polling:
 *
 * DEPRECATED, DO NOT USE. Modifiy PollMouseSticky to add non-sticky opertation.
 *
 */
	HRESULT CUserInput::PollMouse( bool bInGame )
	{
		DIMOUSESTATE msMouse ;
		float fMouseSensitivity = 200	;		// TODO: rip out onto cmd-line, and standardize
												// with other inputs.

		m_fMouseX = 0.0f ;
		m_fMouseY = 0.0f ;
		m_fMouseZ = 0.0f ;
		m_bMouseLB = FALSE ;
		m_bMouseCB = FALSE ;
		m_bMouseRB = FALSE ;

	//
	// Get raw data:
	// 
		HRESULT hr ;
		if (!m_pMouse) 
			return E_FAIL ;

		ZeroMemory(&msMouse, sizeof(msMouse)) ;
		hr = m_pMouse->GetDeviceState(sizeof(msMouse), &msMouse) ;
		if (FAILED(hr)) 
		{
			hr = m_pMouse->Acquire() ;
			while (hr == DIERR_INPUTLOST) 
				hr = m_pMouse->Acquire() ;

			return E_FAIL ;
		}

	//
	// Mouse-smoothing:
	//
		int i ;
		xVec3 vA = xVec3( 0,0,0 );
		// move historical data through the buffer
		for (i = MOUSE_HISTORY_BUF - 2 ; i >= 0 ; i --)
			m_vMouseHistory[i+1] = m_vMouseHistory[i] ;

		// drop the new data into the front of the history buffer
		m_vMouseHistory[0].x = +(float)msMouse.lX ;
		m_vMouseHistory[0].y = -(float)msMouse.lY ;
		m_vMouseHistory[0].z = +(float)msMouse.lZ ;

		// get weighted average from the buffer
		float fWeight = 1.0f, fTotalWeight = 0.0f ;
		for (i = 0 ; i < MOUSE_HISTORY_BUF ; i ++)
		{
			vA += m_vMouseHistory[i] * fWeight ;
			fTotalWeight += fWeight ;
			fWeight *= MOUSE_SMOOTH_WEIGHT_D ;
		}
		vA /= fTotalWeight ; //(float)MOUSE_HISTORY_BUF ;

		vA.x = +(float)msMouse.lX ;
		vA.y = -(float)msMouse.lY ;
		vA.z = +(float)msMouse.lZ ;

	//
	// Map Movement:
	//
		m_fMouseX = vA.x ;
		m_fMouseY = vA.y ;
		m_fMouseZ = vA.z ;

		if (vA.x < 0.0f)
		{
			m_FlightControls.fSteerLeft += (float)fabs(vA.x) * fMouseSensitivity ;
			if (m_FlightControls.fSteerLeft > MAX_FLIGHTMAP)
				m_FlightControls.fSteerLeft = MAX_FLIGHTMAP ;
		}
		if (vA.x > 0.0f)
		{
			m_FlightControls.fSteerRight += (float)fabs(vA.x) * fMouseSensitivity ;
			if (m_FlightControls.fSteerRight > MAX_FLIGHTMAP)
				m_FlightControls.fSteerRight = MAX_FLIGHTMAP ;
		}
		if (vA.y < 0.0f)
		{
			m_FlightControls.fClimb += (float)fabs(vA.y) * fMouseSensitivity ;
			if (m_FlightControls.fClimb > MAX_FLIGHTMAP)
				m_FlightControls.fClimb = MAX_FLIGHTMAP ;
		}
		if (vA.y > 0.0f)
		{
			m_FlightControls.fDive += (float)fabs(vA.y) * fMouseSensitivity ;
			if (m_FlightControls.fDive > MAX_FLIGHTMAP)
				m_FlightControls.fDive = MAX_FLIGHTMAP ;
		}

	//
	// Map Buttons:
	//
		m_bMouseLB = msMouse.rgbButtons[0] & 0x80 ;
		m_bMouseRB = msMouse.rgbButtons[1] & 0x80 ;
		m_bMouseCB = msMouse.rgbButtons[2] & 0x80 ;
		
		if( m_bMouseLB )
			m_FlightControls.bFireA = TRUE ;

		if( m_bMouseRB )
			m_FlightControls.bFireB = TRUE ;

	//
	// Map any change in rollwheel:
	//
		if( m_fMouseZ != m_fMousePrevZ && m_bScanForChangeInMouseWheel )
		{
			if( m_fMouseZ < m_fMousePrevZ )
				m_FlightControls.bNextWeapon = TRUE ;
			if( m_fMouseZ > m_fMousePrevZ )
				m_FlightControls.bPrevWeapon = TRUE ;
			
			m_bScanForChangeInMouseWheel = FALSE ;
		}
		if( m_fMouseZ == 0 )
			m_bScanForChangeInMouseWheel = TRUE ;

		m_fMousePrevZ = m_fMouseZ ;


		/*TCHAR              strElement[10] ;
		DIDEVICEOBJECTDATA didod[MOUSE_BUFFER_SIZE] ;
		DWORD              dwElements ;
		DWORD              i ;
		HRESULT            hr ;

		if (NULL == m_pMouse) 
			return S_OK ;
    
		dwElements = MOUSE_BUFFER_SIZE;
		hr = m_pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0) ;
		if (hr != DI_OK) 
		{
			// We got an error or we got DI_BUFFEROVERFLOW.
			//
			// Either way, it means that continuous contact with the
			// device has been lost, either due to an external
			// interruption, or because the buffer overflowed
			// and some events were lost.
			//
			// Consequently, if a button was pressed at the time
			// the buffer overflowed or the connection was broken,
			// the corresponding "up" message might have been lost.
			//
			// But since our simple sample doesn't actually have
			// any state associated with button up or down events,
			// there is no state to reset.  (In a real game, ignoring
			// the buffer overflow would result in the game thinking
			// a key was held down when in fact it isn't; it's just
			// that the "up" event got lost because the buffer
			// overflowed.)
			//
			// If we want to be cleverer, we could do a
			// GetDeviceState() and compare the current state
			// against the state we think the device is in,
			// and process all the states that are currently
			// different from our private state.
			hr = m_pMouse->Acquire() ;
			while(hr == DIERR_INPUTLOST) 
				hr = m_pMouse->Acquire() ;

			// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
			// may occur when the app is minimized or in the process of 
			// switching, so just try again later 
			return S_OK ;
		}

		if (FAILED(hr))  
			return hr ;

	// study each of the buffer elements and process them
		for (i = 0 ; i < dwElements ; i++) 
		{

			switch (didod[i].dwOfs)
			{
				case DIMOFS_BUTTON0:
					if (didod[i].dwData & 0x80)
						m_bMouseLB = FALSE ;
					else
						m_bMouseLB = TRUE ;
					break ;

				case DIMOFS_BUTTON1:
					if (didod[i].dwData & 0x80)
						m_bMouseRB = FALSE ;
					else
						m_bMouseRB = TRUE ;
					break ;

				case DIMOFS_BUTTON2:
					if (didod[i].dwData & 0x80)
						m_bMouseCB = FALSE ;
					else
						m_bMouseCB = TRUE ;
					break ;

				case DIMOFS_BUTTON3:
					break;

				case DIMOFS_X:
					m_fMouseX += (float)didod[i].dwData / 10.0f ;
					break ;

				case DIMOFS_Y:
					m_fMouseX += (float)didod[i].dwData / 10.0f ;
					break ;

				case DIMOFS_Z:
					//m_fMouseZ += (float)didod[i].dwData ;
					break ;
			}
		}*/

		return S_OK ;
	}


/*
 *
 * Mouse Status Polling:
 *
 * Mouse has a 'memory'; emulates a joystick. Don't need to keep moving the mouse to roll or loop 
 * repeatedly.
 *
 */
	HRESULT CUserInput::PollMouseSticky( bool bInGame )
	{
		DIMOUSESTATE msMouse ;
		float fMouseSensitivity = 0.5f * m_fMouseSensitivity ;

		m_fMouseX = 0.0f ;
		m_fMouseY = 0.0f ;
		m_fMouseZ = 0.0f ;
		m_bMouseLB = FALSE ;
		m_bMouseCB = FALSE ;
		m_bMouseRB = FALSE ;

	//
	// Get raw data:
	// 
		HRESULT hr ;
		if (!m_pMouse) 
			return E_FAIL ;

		ZeroMemory(&msMouse, sizeof(msMouse)) ;
		hr = m_pMouse->GetDeviceState(sizeof(msMouse), &msMouse) ;
		if (FAILED(hr)) 
		{
			hr = m_pMouse->Acquire() ;
			while (hr == DIERR_INPUTLOST) 
				hr = m_pMouse->Acquire() ;

			return E_FAIL ;
		}

	//
	// Mouse invert:
	//
		if( m_bMouseInvert )
			msMouse.lY *= -1 ;

	//
	// Mouse-smoothing:
	//
		int i ;
		xVec3 vA = xVec3( 0,0,0 );
		// move historical data through the buffer
		for (i = MOUSE_HISTORY_BUF - 2 ; i >= 0 ; i --)
			m_vMouseHistory[i+1] = m_vMouseHistory[i] ;

		// drop the new data into the front of the history buffer
		m_vMouseHistory[0].x = +(float)msMouse.lX ;
		m_vMouseHistory[0].y = -(float)msMouse.lY ;
		m_vMouseHistory[0].z = +(float)msMouse.lZ ;

		// get weighted average from the buffer
		float fWeight = 1.0f, fTotalWeight = 0.0f ;
		for (i = 0 ; i < MOUSE_HISTORY_BUF ; i ++)
		{
			vA += m_vMouseHistory[i] * fWeight ;
			fTotalWeight += fWeight ;
			fWeight *= MOUSE_SMOOTH_WEIGHT_D ;
		}
		vA /= fTotalWeight ; //(float)MOUSE_HISTORY_BUF ;
		
	
	// no mouse smoothing at all!
		vA.x = +(float)msMouse.lX ;
		vA.y = -(float)msMouse.lY ;
		vA.z = +(float)msMouse.lZ ;
		m_fMouseX = vA.x ;
		m_fMouseY = vA.y ;
		m_fMouseZ = vA.z ;
		
	//
	// Map Movement:
	//
		if( vA.x != 0 || vA.y != 0 || vA.z != 0 )
		{
			m_fMouseStickyX += vA.x ;
			m_fMouseStickyY += vA.y ;
			m_fMouseStickyZ += vA.z ;
			//ods( "%0.1f,%0.1f,%0.1f\n", m_fMouseStickyX, m_fMouseStickyY, m_fMouseStickyZ );
		}

	// allow quick changes of direction, to more properly emulate a joystick.
		if( vA.x < 0 && m_fMouseStickyX > 0 ) m_fMouseStickyX *= 0.5f ;
		if( vA.x > 0 && m_fMouseStickyX < 0 ) m_fMouseStickyX *= 0.5f ;
		if( vA.y < 0 && m_fMouseStickyY > 0 ) m_fMouseStickyY *= 0.5f ;
		if( vA.y > 0 && m_fMouseStickyY < 0 ) m_fMouseStickyY *= 0.5f ;

		if( m_fMouseStickyX < 0 && bInGame )
		{
			m_FlightControls.fSteerLeft += fabsf( m_fMouseStickyX ) * fMouseSensitivity ;
			if (m_FlightControls.fSteerLeft > MAX_FLIGHTMAP)
				m_FlightControls.fSteerLeft = MAX_FLIGHTMAP ;
		}
		if( m_fMouseStickyX > 0 && bInGame )
		{
			m_FlightControls.fSteerRight += fabsf( m_fMouseStickyX ) * fMouseSensitivity ;
			if (m_FlightControls.fSteerRight > MAX_FLIGHTMAP)
				m_FlightControls.fSteerRight = MAX_FLIGHTMAP ;
		}
		if( m_fMouseStickyY < 0 && bInGame )
		{
			m_FlightControls.fClimb += fabsf( m_fMouseStickyY ) * fMouseSensitivity ;
			if (m_FlightControls.fClimb > MAX_FLIGHTMAP)
				m_FlightControls.fClimb = MAX_FLIGHTMAP ;
		}
		if( m_fMouseStickyY > 0 && bInGame )
		{
			m_FlightControls.fDive += fabsf( m_fMouseStickyY ) * fMouseSensitivity ;
			if (m_FlightControls.fDive > MAX_FLIGHTMAP)
				m_FlightControls.fDive = MAX_FLIGHTMAP ;
		}

	//
	// Map Buttons:
	//
		m_bMouseLB = msMouse.rgbButtons[0] & 0x80 ;
		m_bMouseRB = msMouse.rgbButtons[1] & 0x80 ;
		m_bMouseCB = msMouse.rgbButtons[2] & 0x80 ;
		
		if( m_bMouseLB && bInGame )
			m_FlightControls.bFireA = TRUE ;

		if( m_bMouseRB && bInGame )
			m_FlightControls.bFireB = TRUE ;

	//
	// Map any change in rollwheel:
	//
		//ods( "%0.1f %0.1f\n", m_fMouseZ, m_fMousePrevZ );
		m_bMouseWheelUp = m_bMouseWheelDown = false ;
		if( m_fMouseZ != m_fMousePrevZ && m_bScanForChangeInMouseWheel )
		{
			if( m_fMouseZ < m_fMousePrevZ )
			{
				m_FlightControls.bNextWeapon = TRUE ;
				m_bMouseWheelDown = true ;
			}
			if( m_fMouseZ > m_fMousePrevZ )
			{
				m_FlightControls.bPrevWeapon = TRUE ;
				m_bMouseWheelUp = true ;
			}
			
			m_bScanForChangeInMouseWheel = FALSE ;
		}
		if( m_fMouseZ == 0 )
			m_bScanForChangeInMouseWheel = TRUE ;
		m_fMousePrevZ = m_fMouseZ ;

		return S_OK ;
	}