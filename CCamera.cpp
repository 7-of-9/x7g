#pragma warning( disable : 4183 )

#include "CCamera.h"

/*
 *
 * Construction / Destruction:
 *
 */
	CCamera::CCamera()
	{
		m_vEyePt = xVec3(0.0f, 300.0f, 0.0f) ;
		m_vLookAtPt = xVec3(0.0f, 300.0f, DIST_FROM_EYE_TO_LOOKAT) ;
		m_vUpVec = xVec3(0.0f, 1.0f, 0.0f) ;
		m_vOrientation = xVec3(0.0f, 0.0f, 1.0f) ;

		m_vMouseRelativeXAxis = xVec3(1.0f, 0.0f, 0.0f) ;
		m_fMouseLockCurRot = 0.0f ;
		m_vMouseRelativeYAxis = xVec3(0.0f, 1.0f, 0.0f) ;
	}
	CCamera::~CCamera()
	{
	}

/*
 *
 * D3D View Matrix:
 *
 */
	CCamera::SetViewMatrix(LPDIRECT3DDEVICE8 pDev, BOOL bFlipUpVec)
	{
		static xVec3 vPrevOrientation ;
		/*static xMat m ;
		static BOOL bFirstUse = TRUE ;
		static xMat matPrev ;

		if (bFirstUse)
		{
			bFirstUse = FALSE ;
			m._11 = 0.0f ;
			m._12 = 0.0f ;
			m._13 = 0.0f ;
			m._14 = 0.0f ;
			m._21 = 0.0f ;
			m._22 = 0.0f ;
			m._23 = 0.0f ;
			m._24 = 0.0f ;
			m._31 = 0.0f ;
			m._32 = 0.0f ;
			m._33 = 0.0f ;
			m._34 = 0.0f ;
			m._41 = 0.0f ;
			m._42 = 0.0f ;
			m._43 = 0.0f ;
			m._44 = 0.0f ;
		}*/

		m_vOrientation = m_vLookAtPt - m_vEyePt ;
		D3DXVec3Normalize(&m_vOrientation, &m_vOrientation) ;

		if (bFlipUpVec)
		{
			BOOL bXChanged = FALSE, bZChanged = FALSE ;

			if (m_vOrientation.x > 0.0f && vPrevOrientation.x < 0.0f) bXChanged = TRUE ;
			if (m_vOrientation.x < 0.0f && vPrevOrientation.x > 0.0f) bXChanged = TRUE ;
			if (m_vOrientation.x == 0.0f) bXChanged = TRUE ;
			
			if (m_vOrientation.z > 0.0f && vPrevOrientation.z < 0.0f) bZChanged = TRUE ;
			if (m_vOrientation.z < 0.0f && vPrevOrientation.z > 0.0f) bZChanged = TRUE ;
			if (m_vOrientation.z == 0.0f) bZChanged = TRUE ;

			if (bXChanged && bZChanged)
			{
				m_vUpVec.y *= -1.0f ;
			}

			vPrevOrientation = m_vOrientation ;
		}

		D3DXMatrixLookAtLH(&m_matView, &m_vEyePt, &m_vLookAtPt, &m_vUpVec) ;

		//m_vUpVec.y *= -1.0f ;
		//xMat matInv ;
		//D3DXMatrixLookAtLH(&matInv, &m_vEyePt, &m_vLookAtPt, &m_vUpVec) ;
		//m_vUpVec.y *= -1.0f ;

		/*if (m_matView._11 > 0.0f && matPrev._11 < 0.0f)
			m_matView._11 = -m_matView._11 ;
		if (m_matView._11 < 0.0f && matPrev._11 > 0.0f)
			m_matView._11 = +m_matView._11 ;*/

		//if (matInv._11 < 0.0f && m_matView._11 > 0.0f) m._11 += 1.0f ;
		//if (matInv._11 > 0.0f && m_matView._11 < 0.0f) m._12 += 1.0f ;
		/*if ((matInv._11 < 0.0f && m_matView._11 > 0.0f) || (matInv._11 > 0.0f && m_matView._11 < 0.0f)) m._11 += 1.0f ;
		if ((matInv._11 < 0.0f && m_matView._11 > 0.0f) || (matInv._11 > 0.0f && m_matView._11 < 0.0f)) m._11 += 1.0f ;
		if ((matInv._11 < 0.0f && m_matView._11 > 0.0f) || (matInv._11 > 0.0f && m_matView._11 < 0.0f)) m._11 += 1.0f ;*/

		/*if ((matInv._21 < 0.0f && m_matView._21 > 0.0f) || (matInv._21 > 0.0f && m_matView._21 < 0.0f)) m._21 += 1.0f ;
		if ((matInv._21 < 0.0f && m_matView._21 > 0.0f) || (matInv._21 > 0.0f && m_matView._21 < 0.0f)) m._21 += 1.0f ;
		if ((matInv._21 < 0.0f && m_matView._21 > 0.0f) || (matInv._21 > 0.0f && m_matView._21 < 0.0f)) m._21 += 1.0f ;
		if ((matInv._21 < 0.0f && m_matView._21 > 0.0f) || (matInv._21 > 0.0f && m_matView._21 < 0.0f)) m._21 += 1.0f ;*/

		/*if (m_matView._31 > 0.0f && matPrev._31 < 0.0f)
			m_matView._31 = -m_matView._31 ;
		if (m_matView._31 < 0.0f && matPrev._31 > 0.0f)
			m_matView._31 = +m_matView._31 ;*/

		//if (matInv._31 < 0.0f && m_matView._31 > 0.0f) m._31 += 1.0f ;
		//if (matInv._31 > 0.0f && m_matView._31 < 0.0f) m._32 += 1.0f ;
		/*if ((matInv._31 < 0.0f && m_matView._31 > 0.0f) || (matInv._31 > 0.0f && m_matView._31 < 0.0f)) m._31 += 1.0f ;
		if ((matInv._31 < 0.0f && m_matView._31 > 0.0f) || (matInv._31 > 0.0f && m_matView._31 < 0.0f)) m._31 += 1.0f ;
		if ((matInv._31 < 0.0f && m_matView._31 > 0.0f) || (matInv._31 > 0.0f && m_matView._31 < 0.0f)) m._31 += 1.0f ;*/

		/*if (m_matView._41 > 0.0f && matPrev._41 < 0.0f)
			m_matView._41 = -m_matView._41 ;
		if (m_matView._41 < 0.0f && matPrev._41 > 0.0f)
			m_matView._41 = +m_matView._41 ;*/

		//if (matInv._41 < 0.0f && m_matView._41 > 0.0f) m._41 += 1.0f ;
		//if (matInv._41 > 0.0f && m_matView._41 < 0.0f) m._42 += 1.0f ;
		/*if ((matInv._41 < 0.0f && m_matView._41 > 0.0f) || (matInv._41 > 0.0f && m_matView._41 < 0.0f)) m._41 += 1.0f ;
		if ((matInv._41 < 0.0f && m_matView._41 > 0.0f) || (matInv._41 > 0.0f && m_matView._41 < 0.0f)) m._41 += 1.0f ;
		if ((matInv._41 < 0.0f && m_matView._41 > 0.0f) || (matInv._41 > 0.0f && m_matView._41 < 0.0f)) m._41 += 1.0f ;*/

		//if (m_matView._11 < 0.0f) m_matView._11 = +m_matView._11 ;
		//if (m_matView._12 > 0.0f) m_matView._12 = -m_matView._12 ;
		//if (m_matView._22 < 0.0f) m_matView._22 = +m_matView._22 ;
		//if (m_matView._31 > 0.0f) m_matView._31 = -m_matView._31 ;
		//if (m_matView._32 > 0.0f) m_matView._32 = -m_matView._32 ;
		//if (m_matView._41 > 0.0f) m_matView._41 = -m_matView._41 ;
		//if (m_matView._42 > 0.0f) m_matView._42 = -m_matView._42 ;

		/*if (m_matView._32 > 0.0f)*/ //m_matView._32 = -m_matView._32 ;
		/*if (m_matView._41 > 0.0f)*/ //m_matView._41 = -m_matView._41 ;
		/*if (m_matView._42 > 0.0f)*/ //m_matView._42 = -m_matView._42 ;

			/*xVec3 vView, vUp, vRight;

			 // vView is the vector down which we are looking/traveling
			 // Already normalized
			 vView = m_vOrientation ;

			 // vUp is our camera's Y-Axis
			 // Already normalized
			 vUp = xVec3(0.0f, 1.0f, 0.0f) ;

			 // Create the vRight vector and Normalize
			 D3DXVec3Cross(&vRight, &vUp, &vView) ;
			 D3DXVec3Normalize(&vRight, &vRight) ;

			 // Start building the matrix. The first three rows contains the basis
			 // vectors used to rotate the view to point at the lookat point
			 D3DXMatrixIdentity(&m_matView) ;
			 
			 m_matView._11 = vRight.x;    m_matView._12 = vUp.x;
			 m_matView._13 = vView.x;
			 m_matView._21 = vRight.y;    m_matView._22 = vUp.y;
			 m_matView._23 = vView.y;
			 m_matView._31 = vRight.z;    m_matView._32 = vUp.z;
			 m_matView._33 = vView.z;

			 // Do the translation values (rotations are still about the eyepoint)
			 m_matView._41 = - D3DXVec3Dot(&m_vLookAtPt, &vRight);
			 m_matView._42 = - D3DXVec3Dot(&m_vLookAtPt, &vUp);
			 m_matView._43 = - D3DXVec3Dot(&m_vLookAtPt, &vView);*/

		pDev->SetTransform(D3DTS_VIEW, &m_matView) ;

		/*memcpy(&matPrev, &m_matView, sizeof(xMat)) ;*/
	}

/*
 *
 * Manual Camera Movement and Rotation:
 *
 */
	CCamera::SetManualFromValues(LPDIRECT3DDEVICE8 pDev, float x, float y, float z)
	{
		m_vEyePt = xVec3(x, y, z) ;
		m_vLookAtPt = xVec3(x, y, z + DIST_FROM_EYE_TO_LOOKAT) ;
		m_vUpVec = xVec3(0.0f, 1.0f, 0.0f) ;
		m_vMouseRelativeXAxis = xVec3(1.0f, 0.0f, 0.0f) ;
		m_fMouseLockCurRot = 0.0f ;
		m_vMouseRelativeYAxis = xVec3(0.0f, 1.0f, 0.0f) ;
		
		SetViewMatrix(pDev, FALSE) ;
	}
	CCamera::SetManualFromInput(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput, float fFPS, float fMinY)
	{
		xMat matRot, matTrans1, matTrans2, matRes ;
		xVec4 vRes ;
		BOOL bChanges = FALSE ;
		static float fConstantForward = 0 ;
		static float fConstantBackward = 0 ;

		if( /*pInput->m_bMouseLB ||*/ pInput->m_FlightControls.fAccelerate > 0.0f)
		{
		//
		// Move towards the look at point:
		//
			xVec3 vDir ;

			if (fConstantForward < 250.0f)
				fConstantForward += (60.0f / fFPS) ;

			D3DXVec3Normalize(&vDir, &(m_vLookAtPt - m_vEyePt)) ;
			vDir *= (1.0f / fFPS) ;
			vDir *= fConstantForward ;
			D3DXMatrixTranslation(&matTrans1, vDir.x, vDir.y, vDir.z) ;

			D3DXVec3Transform(&vRes, &m_vLookAtPt, &matTrans1) ;
			if (vRes.y > fMinY)
			{
				m_vLookAtPt.x = vRes.x ;
				m_vLookAtPt.y = vRes.y ;
				m_vLookAtPt.z = vRes.z ;
				
				D3DXVec3Transform(&vRes, &m_vEyePt, &matTrans1) ;
				m_vEyePt.x = vRes.x ;
				m_vEyePt.y = vRes.y ;
				m_vEyePt.z = vRes.z ;

				bChanges = TRUE ;
			}
		}
		else
			fConstantForward = 0.0f ;

		if (pInput->m_FlightControls.fBrake > 0.0f)
		{
		//
		// Move away from the look at point:
		//
			xVec3 vDir ;

			if (fConstantForward < 250.0f)
				fConstantBackward += (60.0f / fFPS) ;

			D3DXVec3Normalize(&vDir, &(m_vLookAtPt - m_vEyePt)) ;
			vDir *= (1.0f / fFPS) ;
			vDir *= fConstantBackward ;
			D3DXMatrixTranslation(&matTrans1, -vDir.x, -vDir.y, -vDir.z) ;

			D3DXVec3Transform(&vRes, &m_vLookAtPt, &matTrans1) ;
			m_vLookAtPt.x = vRes.x ;
			m_vLookAtPt.y = vRes.y ;
			m_vLookAtPt.z = vRes.z ;
			
			D3DXVec3Transform(&vRes, &m_vEyePt, &matTrans1) ;
			m_vEyePt.x = vRes.x ;
			m_vEyePt.y = vRes.y ;
			m_vEyePt.z = vRes.z ;

			bChanges = TRUE ;
		}
		else
			fConstantBackward = 0.0f ;
		
		if (pInput->m_fMouseX != 0)
		{
			float d = (pInput->m_fMouseX / 250.0f) * (60.0f / fFPS) ;

		//
		// Apply the rotation to the mouse's Y axis (spin left and right):
		//
			D3DXMatrixTranslation(&matTrans1, -m_vEyePt.x, -m_vEyePt.y, -m_vEyePt.z) ;
			D3DXMatrixRotationY(&matRot, d) ;
			D3DXMatrixTranslation(&matTrans2, +m_vEyePt.x, +m_vEyePt.y, +m_vEyePt.z) ;
			D3DXMatrixMultiply(&matRes, &matTrans1, &matRot) ;
			D3DXMatrixMultiply(&matRes, &matRes, &matTrans2) ;
			D3DXVec3Transform(&vRes, &m_vLookAtPt, &matRes) ;
			m_vLookAtPt.x = vRes.x ;
			m_vLookAtPt.z = vRes.z ;
			m_vLookAtPt.y = vRes.y ;
			
			bChanges = TRUE ;

		//
		// Update the camera's relative X axis:
		//
			D3DXMatrixRotationY(&matRot, d) ;
			D3DXVec3Transform(&vRes, &m_vMouseRelativeXAxis, &matRot) ;
			m_vMouseRelativeXAxis.x = vRes.x ;
			m_vMouseRelativeXAxis.z = vRes.z ;
			m_vMouseRelativeXAxis.y = vRes.y ;
		}
		
		if (pInput->m_fMouseY != 0)
		{
			float fMaxRot = g_PI_DIV_2 * 0.999f /*g_PI * 0.49f*/ ;
			

			if (m_fMouseLockCurRot >= -fMaxRot && m_fMouseLockCurRot <= +fMaxRot)
			{
				float d = (pInput->m_fMouseY / 250.0f) * (60.0f / fFPS) ;
				
			//
			// Clamp the rotation:
			//
				if (m_fMouseLockCurRot + d > +fMaxRot)
					d = +fMaxRot - m_fMouseLockCurRot ;
				if (m_fMouseLockCurRot + d < -fMaxRot)
					d = -fMaxRot - m_fMouseLockCurRot ;
				
			//
			// Apply the rotation to the mouse's relative X axis:
			//
				m_fMouseLockCurRot += d ;

				D3DXMatrixTranslation(&matTrans1, -m_vEyePt.x, -m_vEyePt.y, -m_vEyePt.z) ;
				D3DXMatrixRotationAxis(&matRot, &m_vMouseRelativeXAxis, d) ;	
				D3DXMatrixTranslation(&matTrans2, +m_vEyePt.x, +m_vEyePt.y, +m_vEyePt.z) ;
				D3DXMatrixMultiply(&matRes, &matTrans1, &matRot) ;
				D3DXMatrixMultiply(&matRes, &matRes, &matTrans2) ;
				D3DXVec3Transform(&vRes, &m_vLookAtPt, &matRes) ;
				m_vLookAtPt.x = vRes.x ;
				m_vLookAtPt.z = vRes.z ;
				m_vLookAtPt.y = vRes.y ;

			//
			// Update the camera's relative Y axis:
			//
				D3DXMatrixRotationAxis(&matRot, &m_vMouseRelativeXAxis, d) ;
				D3DXVec3Transform(&vRes, &m_vMouseRelativeYAxis, &matRot) ;
				m_vMouseRelativeYAxis.x = vRes.x ;
				m_vMouseRelativeYAxis.z = vRes.z ;
				m_vMouseRelativeYAxis.y = vRes.y ;
				
				bChanges = TRUE ;
			}
		}
		
		if (bChanges)
			SetViewMatrix(pDev, FALSE) ;
	}

/*
 *
 * Preset Positions and Rotations:
 *
 */
	CCamera::SetPresetFromInput(LPDIRECT3DDEVICE8 pDev, CUserInput *pInput)
	{
		if (pInput->m_bF5)
		{
			m_vEyePt = xVec3(0.0f, 200.0f, 0.0f) ;
			m_vLookAtPt = xVec3(0.0f, 200.0f, DIST_FROM_EYE_TO_LOOKAT) ;
			m_vUpVec = xVec3(0.0f, 1.0f, 0.0f) ;
			m_vOrientation = xVec3(0.0f, 0.0f, 1.0f) ;
			m_vMouseRelativeXAxis = xVec3(1.0f, 0.0f, 0.0f) ;
			m_fMouseLockCurRot = 0.0f ;
			m_vMouseRelativeYAxis = xVec3(0.0f, 1.0f, 0.0f) ;
			SetViewMatrix(pDev, FALSE) ;
		}
		if (pInput->m_bF6)
		{
			m_vEyePt = xVec3(0.0f, 9000.0f, 0.0f) ;
			m_vLookAtPt = xVec3(0.0f, 9000.0f, DIST_FROM_EYE_TO_LOOKAT) ;
			m_vUpVec = xVec3(0, 1, 0) ;
			m_vOrientation = xVec3(0.0f, 0.0f, 1.0f) ;
			m_vMouseRelativeXAxis = xVec3(1.0f, 0.0f, 0.0f) ;
			m_fMouseLockCurRot = 0.0f ;
			m_vMouseRelativeYAxis = xVec3(0.0f, 1.0f, 0.0f) ;
			SetViewMatrix(pDev, FALSE) ;
		}
		if (pInput->m_bF7)
		{
			m_vEyePt = xVec3(0.0f, 100000.0f, 0.0f) ;
			m_vLookAtPt = xVec3(0.0f, 100000.0f, DIST_FROM_EYE_TO_LOOKAT) ;
			m_vUpVec = xVec3(0.0f, 1.0f, 0.0f) ;
			m_vOrientation = xVec3(0.0f, 0.0f, 1.0f) ;
			m_vMouseRelativeXAxis = xVec3(1.0f, 0.0f, 0.0f) ;
			m_fMouseLockCurRot = 0.0f ;
			m_vMouseRelativeYAxis = xVec3(0.0f, 1.0f, 0.0f) ;
			SetViewMatrix(pDev, FALSE) ;
		}
		if (pInput->m_bF8)
		{
			m_vEyePt = xVec3(0.0f, 0.0f, -2.0f) ;
			m_vLookAtPt = xVec3(0.0f, 0.0f, -2.0f + DIST_FROM_EYE_TO_LOOKAT) ;
			m_vUpVec = xVec3(0.0f, 1.0f, 0.0f) ;
			m_vOrientation = xVec3(0.0f, 0.0f, 1.0f) ;
			m_vMouseRelativeXAxis = xVec3(1.0f, 0.0f, 0.0f) ;
			m_fMouseLockCurRot = 0.0f ;
			m_vMouseRelativeYAxis = xVec3(0.0f, 1.0f, 0.0f) ;
			SetViewMatrix(pDev, FALSE) ;
		}
	}

/*
 *
 * Rotation around a point:
 *
 */
	CCamera::SetShowoff(LPDIRECT3DDEVICE8 pDev, xVec3 *vCenter, float fRadius, float fFPS, float fYaw, float fPitch, float fRoll)
	{
		xMat matRot ;
		xVec4 v ;
		static xVec3 vOrientation = xVec3(0.0f, 0.0f, -1.0f) ;

		D3DXMatrixRotationYawPitchRoll(&matRot, fYaw * (60.0f / fFPS), fPitch * (60.0f / fFPS), fRoll * (60.0f / fFPS)) ;

		m_vLookAtPt = *vCenter ;
		m_vEyePt = *vCenter - ( vOrientation * fRadius );

		D3DXVec3Transform(&v, &vOrientation, &matRot) ;
		vOrientation.x = v.x ; vOrientation.y = v.y ; vOrientation.z = v.z ;

		SetViewMatrix(pDev, FALSE) ;
	}
