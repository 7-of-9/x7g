// configDlg.h : header file
//

#pragma once
#include "CConfig.h"


// CconfigDlg dialog
class CconfigDlg : public CDialog
{
// Construction
public:

	CConfig cfg ;

	CButton* pFullscreen ;
	CButton*	pLowRes ;
	CButton*	pMedRes ;
	CButton*	pHiRes ;
	CButton*	pClampVSync ;
	CButton*	pFSAA ;
	CButton*	   pMultisample2 ;
	CButton*	   pMultisample3 ;
	CButton*	   pMultisample4 ;
	CButton*	pDepth16 ;
	CButton*	pDepth24 ;
	CButton*	pDepth32 ;
	CButton* pLowDet ;
	CButton* pMedDet ;
	CButton* pHiDet ;

	CSliderCtrl* pFXVol ;
	CSliderCtrl* pMusicVol ;
	CEdit*		 pMusicPath ;
	CButton*	 pMP3Browse ;
	CButton*	 pMP3AutoScan ;

	CSliderCtrl* pInputSensitivity ;
	CButton*	 pMouseInvert ;

	CButton* pAutograb ;
	CButton* pStart9 ;
	CButton* pStart19 ;
	CButton* pAutoStart ;
	CButton* pMouseNonEx ;

	void ValidateVideoOptions();

	CconfigDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedCheckFullscreen();
	afx_msg void OnNMCustomdrawSliderFxvol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioMedres();
	afx_msg void OnBnClickedRadioLowres();
	afx_msg void OnBnClickedRadioHires();
	afx_msg void OnBnClickedCheckVsync();
	afx_msg void OnBnClickedCheckFsaa();
	afx_msg void OnBnClickedRadioLowdet();
	afx_msg void OnBnClickedRadioMeddet();
	afx_msg void OnBnClickedRadioHidet();
	afx_msg void OnNMCustomdrawSliderMusicvol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioMultisample2();
	afx_msg void OnBnClickedRadioMultisample3();
	afx_msg void OnBnClickedRadioMultisample4();
	afx_msg void OnBnClickedRadioDepth16();
	afx_msg void OnBnClickedRadioDepth24();
	afx_msg void OnBnClickedRadioDepth32();
	afx_msg void OnNMCustomdrawSliderSens(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckInvert();
	afx_msg void OnBnClickedCheckAutograb();
	afx_msg void OnEnChangeEditMp3path();
	afx_msg void OnBnClickedCheckMousenonex();
};
