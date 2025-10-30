// configDlg.cpp : implementation file
//

#include "stdafx.h"
#include "config.h"
#include "configDlg.h"

    #pragma comment(lib, "Shlwapi.lib")
    #include <Shlwapi.h>

    CString
    GetModulePath(
        BOOL bAddBackslash = TRUE
      )
    {
        TCHAR MaxPathBuffer[MAX_PATH];
        CString str;

        if (GetModuleFileName(
            NULL, MaxPathBuffer, MAX_PATH))
        {
            if (PathRemoveFileSpec(MaxPathBuffer))
            {
                if (bAddBackslash)
                    PathAddBackslash(MaxPathBuffer);
                str = MaxPathBuffer;
            }
        }
        return str;
    }


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CconfigDlg dialog



CconfigDlg::CconfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CconfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CconfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CconfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_CHECK_FULLSCREEN, OnBnClickedCheckFullscreen)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_FXVOL, OnNMCustomdrawSliderFxvol)
	ON_BN_CLICKED(IDC_RADIO_MEDRES, OnBnClickedRadioMedres)
	ON_BN_CLICKED(IDC_RADIO_LOWRES, OnBnClickedRadioLowres)
	ON_BN_CLICKED(IDC_RADIO_HIRES, OnBnClickedRadioHires)
	ON_BN_CLICKED(IDC_CHECK_VSYNC, OnBnClickedCheckVsync)
	ON_BN_CLICKED(IDC_CHECK_FSAA, OnBnClickedCheckFsaa)
	ON_BN_CLICKED(IDC_RADIO_LOWDET, OnBnClickedRadioLowdet)
	ON_BN_CLICKED(IDC_RADIO_MEDDET, OnBnClickedRadioMeddet)
	ON_BN_CLICKED(IDC_RADIO_HIDET, OnBnClickedRadioHidet)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MUSICVOL, OnNMCustomdrawSliderMusicvol)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_MULTISAMPLE2, OnBnClickedRadioMultisample2)
	ON_BN_CLICKED(IDC_RADIO_MULTISAMPLE3, OnBnClickedRadioMultisample3)
	ON_BN_CLICKED(IDC_RADIO_MULTISAMPLE4, OnBnClickedRadioMultisample4)
	ON_BN_CLICKED(IDC_RADIO_DEPTH16, OnBnClickedRadioDepth16)
	ON_BN_CLICKED(IDC_RADIO_DEPTH24, OnBnClickedRadioDepth24)
	ON_BN_CLICKED(IDC_RADIO_DEPTH32, OnBnClickedRadioDepth32)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SENS, OnNMCustomdrawSliderSens)
	ON_BN_CLICKED(IDC_CHECK_INVERT, OnBnClickedCheckInvert)
	ON_EN_CHANGE(IDC_EDIT_MP3PATH, OnEnChangeEditMp3path)

END_MESSAGE_MAP()


// CconfigDlg message handlers

BOOL CconfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//
// get control pointers
//
	pFullscreen		= (CButton*)GetDlgItem( IDC_CHECK_FULLSCREEN );
		pLowRes		= (CButton*)GetDlgItem( IDC_RADIO_LOWRES );
		pMedRes		= (CButton*)GetDlgItem( IDC_RADIO_MEDRES );
		pHiRes		= (CButton*)GetDlgItem( IDC_RADIO_HIRES );
		pClampVSync = (CButton*)GetDlgItem( IDC_CHECK_VSYNC );
		pFSAA       = (CButton*)GetDlgItem( IDC_CHECK_FSAA );
			pMultisample2= (CButton*)GetDlgItem( IDC_RADIO_MULTISAMPLE2 );
			pMultisample3= (CButton*)GetDlgItem( IDC_RADIO_MULTISAMPLE3 );
			pMultisample4= (CButton*)GetDlgItem( IDC_RADIO_MULTISAMPLE4 );
		pDepth16	= (CButton*)GetDlgItem( IDC_RADIO_DEPTH16 );
		pDepth24	= (CButton*)GetDlgItem( IDC_RADIO_DEPTH24 );
		pDepth32	= (CButton*)GetDlgItem( IDC_RADIO_DEPTH32 );	

	pLowDet			= (CButton*)GetDlgItem( IDC_RADIO_LOWDET );
	pMedDet			= (CButton*)GetDlgItem( IDC_RADIO_MEDDET );
	pHiDet			= (CButton*)GetDlgItem( IDC_RADIO_HIDET );
	
	pFXVol			= (CSliderCtrl*)GetDlgItem( IDC_SLIDER_FXVOL );
	pMusicVol		= (CSliderCtrl*)GetDlgItem( IDC_SLIDER_MUSICVOL );
	pMusicPath		= (CEdit*)GetDlgItem( IDC_EDIT_MP3PATH );
	pMP3Browse		= (CButton*)GetDlgItem( IDC_BUTTON_MP3BROWSE );
	pMP3AutoScan	= (CButton*)GetDlgItem( IDC_BUTTON_MP3AUTOSCAN );
	pFXVol->SetRange( 0, 255 );
	pMusicVol->SetRange( 0, 255 );
	pFXVol->SetPageSize( 25 );
	pMusicVol->SetPageSize( 25 );
	pMP3Browse->EnableWindow( FALSE );
	pMP3AutoScan->EnableWindow( FALSE );

	pInputSensitivity = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SENS );
	pMouseInvert	  = (CButton*)GetDlgItem( IDC_CHECK_INVERT );
	pInputSensitivity->SetRange( 0, 255 );
	pInputSensitivity->SetPageSize( 25 );

	pAutograb = (CButton*)GetDlgItem( IDC_CHECK_AUTOGRAB );
	pAutograb->SetCheck( 1 );
	pStart9 = (CButton*)GetDlgItem( IDC_CHECK_START9 );
	pStart19 = (CButton*)GetDlgItem( IDC_CHECK_START19 );
	pAutoStart = (CButton*)GetDlgItem( IDC_CHECK_AUTOSTART );
	pAutoStart->SetCheck( 1 );
	pMouseNonEx = (CButton*)GetDlgItem( IDC_CHECK_MOUSENONEX );
	pMouseNonEx->SetCheck( 1 );


//
// read config and set initial control values
//
	// video
	pFullscreen->SetCheck( cfg.getFullscreen() );
	switch( cfg.getRes() )
	{
		case CConfig::LOW_RES: pLowRes->SetCheck( 1 ); break ;
		case CConfig::MED_RES: pMedRes->SetCheck( 1 ); break ;
		case CConfig::HI_RES:   pHiRes->SetCheck( 1 ); break ;
	}

	pClampVSync->SetCheck( cfg.getClampVSync() );

	pFSAA->SetCheck( cfg.getFSAA() != 0 ? 1 : 0 );
	switch( cfg.getFSAA() )
	{
		case 2: pMultisample2->SetCheck( 1 ); break ;
		case 3: pMultisample3->SetCheck( 1 ); break ;
		case 4: pMultisample4->SetCheck( 1 ); break ;
	}

	switch( cfg.getDet() )
	{
		case CConfig::LOW_DET: pLowDet->SetCheck( 1 ); break ;
		case CConfig::MED_DET: pMedDet->SetCheck( 1 ); break ;
		case CConfig::HI_DET:   pHiDet->SetCheck( 1 ); break ;
	}

	switch( cfg.getDepthBuf() )
	{
		case 16: pDepth16->SetCheck( 1 ); break ;
		case 24: pDepth24->SetCheck( 1 ); break ;
		case 32: pDepth32->SetCheck( 1 ); break ;
	}
	ValidateVideoOptions() ;

	// sound
	pFXVol->SetPos( (int)( cfg.getFXVol() * 255 ) );
	pMusicVol->SetPos( (int)( cfg.getMusicVol() * 255 ) );
	pMusicPath->SetWindowText( cfg.getMusicPath() );

	// input
	pInputSensitivity->SetPos( (int)( cfg.getInputSensitivity() * 255 ) );
	pMouseInvert->SetCheck( cfg.getMouseInvert() != 0 ? 1 : 0 );

	return TRUE;
}

void CconfigDlg::ValidateVideoOptions()
{
	pLowRes->EnableWindow( cfg.getFullscreen() );
	pMedRes->EnableWindow( cfg.getFullscreen() );
	pHiRes->EnableWindow( cfg.getFullscreen() );
	pClampVSync->EnableWindow( cfg.getFullscreen() );

	pMultisample2->EnableWindow( 1==pFSAA->GetCheck() );
	pMultisample3->EnableWindow( 1==pFSAA->GetCheck() );
	pMultisample4->EnableWindow( 1==pFSAA->GetCheck() );
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CconfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CconfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//
// Video
//
void CconfigDlg::OnBnClickedCheckFullscreen()
{
	cfg.setFullscreen( (1==pFullscreen->GetCheck()) );
	ValidateVideoOptions() ;
}
void CconfigDlg::OnBnClickedRadioMedres() {	cfg.setRes( CConfig::MED_RES ); }
void CconfigDlg::OnBnClickedRadioLowres() {	cfg.setRes( CConfig::LOW_RES ); }
void CconfigDlg::OnBnClickedRadioHires() {	cfg.setRes( CConfig::HI_RES ); }
void CconfigDlg::OnBnClickedCheckVsync() {	cfg.setClampVSync( (1==pClampVSync->GetCheck()) ); }
void CconfigDlg::OnBnClickedCheckFsaa()
{
	if( 1==pFSAA->GetCheck() )
	{
		pMultisample2->SetCheck( 1 );
		cfg.setFSAA( 2 );
	}
	else
		cfg.setFSAA( 0 );
	ValidateVideoOptions() ;
}
void CconfigDlg::OnBnClickedRadioMultisample2() { cfg.setFSAA( 2 ); }
void CconfigDlg::OnBnClickedRadioMultisample3() { cfg.setFSAA( 3 ); }
void CconfigDlg::OnBnClickedRadioMultisample4() { cfg.setFSAA( 4 ); }
void CconfigDlg::OnBnClickedRadioDepth16() { cfg.setDepthBuf( 16 ); }
void CconfigDlg::OnBnClickedRadioDepth24() { cfg.setDepthBuf( 24 ); }
void CconfigDlg::OnBnClickedRadioDepth32() { cfg.setDepthBuf( 32 ); }
void CconfigDlg::OnBnClickedRadioLowdet() {	cfg.setDet( CConfig::LOW_DET ); }
void CconfigDlg::OnBnClickedRadioMeddet() { cfg.setDet( CConfig::MED_DET ); }
void CconfigDlg::OnBnClickedRadioHidet() {  cfg.setDet( CConfig::HI_DET ); }

//
// Sound
//
void CconfigDlg::OnNMCustomdrawSliderFxvol(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR); *pResult = 0;
	cfg.setFXVol( (float)pFXVol->GetPos() / 255 );
}

void CconfigDlg::OnNMCustomdrawSliderMusicvol(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR); *pResult = 0;
	cfg.setMusicVol( (float)pMusicVol->GetPos() / 255 );
}
void CconfigDlg::OnEnChangeEditMp3path()
{
	char szPath[ 1024 ];
	pMusicPath->GetWindowText( szPath, 1023 );
	cfg.setMusicPath( szPath );
}

//
// Input
//
void CconfigDlg::OnNMCustomdrawSliderSens(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR); *pResult = 0;
	cfg.setInputSensitivity( (float)pInputSensitivity->GetPos() / 255 );
}
void CconfigDlg::OnBnClickedCheckInvert() { cfg.setMouseInvert( pMouseInvert->GetCheck() ? true : false ); }

//
// Build command line and run the app.
//
void CconfigDlg::OnBnClickedOk()
{
	char szCmdLine[ 4096 ];
	char szEXEPath[ 4096 ];

	sprintf( szEXEPath, "%s%s", GetModulePath(), GAME_EXE );

// base
	//sprintf( szCmdLine, "%s\\%s", GetModulePath(), GAME_EXE );
	szCmdLine[0] = 0 ;

// video
// TODO: /nonstickymouse:0 /level:1 /playerdamage:1 /bots:0 /autostart:0 /terrain1:8 /terrain2:16 /fullscreen:1 /winheight:300 /winwidth:400 /winx:800 /winy:800 /god:0 /blacksky:0 /fps:1 /avgstats:1 /soundmusic:50 /soundfx:50
	sprintf( szCmdLine, "%s /FullScreen:%d", szCmdLine, cfg.getFullscreen() ? 1 : 0 );
	switch( cfg.getRes() )
	{
	case CConfig::LOW_RES: sprintf( szCmdLine, "%s /FullscreenWidth:400 /FullscreenHeight:300", szCmdLine ); break ;
	case CConfig::MED_RES: sprintf( szCmdLine, "%s /FullscreenWidth:640 /FullscreenHeight:480", szCmdLine ); break ;
	case CConfig::HI_RES:  sprintf( szCmdLine, "%s /FullscreenWidth:800 /FullscreenHeight:600", szCmdLine ); break ;
	}
	switch( cfg.getDet() )
	{
	case CConfig::LOW_DET: sprintf( szCmdLine, "%s /Terrain1:8 /Terrain2:16", szCmdLine ); break ;
	case CConfig::MED_DET: sprintf( szCmdLine, "%s /Terrain1:8 /Terrain2:32", szCmdLine ); break ;
	case CConfig::HI_DET:  sprintf( szCmdLine, "%s /Terrain1:8 /Terrain2:64", szCmdLine ); break ;
	}
	sprintf( szCmdLine, "%s /FSAA:%d", szCmdLine, cfg.getFSAA() );
	sprintf( szCmdLine, "%s /DepthBufMinBits:%d", szCmdLine, cfg.getDepthBuf() );
	sprintf( szCmdLine, "%s /VSync:%d", szCmdLine, cfg.getClampVSync() ? 1 : 0 );


// sound
	sprintf( szCmdLine, "%s /SoundFX:%d", szCmdLine, (int)( cfg.getFXVol() * 255 ) );
	sprintf( szCmdLine, "%s /SoundMusic:%d", szCmdLine, (int)( cfg.getMusicVol() * 255 ) );
	sprintf( szCmdLine, "%s /MusicPath:%s", szCmdLine, cfg.getMusicPath() );

// input
	sprintf( szCmdLine, "%s /InputSens:%d", szCmdLine, (int)( cfg.getInputSensitivity() * 255 ) );
	sprintf( szCmdLine, "%s /MouseInvert:%d", szCmdLine, (int)( cfg.getMouseInvert() ? 1 : 0 ) );

// debug
	sprintf( szCmdLine, "%s /AutoGrab:%d", szCmdLine, pAutograb->GetCheck() );
	if( pStart9->GetCheck() )
		sprintf( szCmdLine, "%s /Level:9", szCmdLine, pAutograb->GetCheck() );
	if( pStart19->GetCheck() )
		sprintf( szCmdLine, "%s /Level:19", szCmdLine, pAutograb->GetCheck() );
	if( pAutoStart->GetCheck() )
		sprintf( szCmdLine, "%s /Autostart:1", szCmdLine );
	if( pMouseNonEx->GetCheck() )
		sprintf( szCmdLine, "%s /MouseExclusive:0", szCmdLine );

	//MessageBox( szCmdLine, szEXEPath );
	HINSTANCE hi ;
	hi = ShellExecute( NULL, "open", szEXEPath, szCmdLine, GetModulePath( FALSE ), SW_SHOW );
	int i = (int)hi ;
	switch(i)
	{
	case ERROR_FILE_NOT_FOUND: MessageBox("ERROR_FILE_NOT_FOUND"); break ;
	case ERROR_PATH_NOT_FOUND: MessageBox("ERROR_PATH_NOT_FOUND"); break ;
	case ERROR_BAD_FORMAT: MessageBox("ERROR_BAD_FORMAT"); break ;
	case SE_ERR_ACCESSDENIED: MessageBox("SE_ERR_ACCESSDENIED"); break ;
	case SE_ERR_ASSOCINCOMPLETE: MessageBox("SE_ERR_ASSOCINCOMPLETE"); break ;
	case SE_ERR_DDEBUSY: MessageBox("SE_ERR_DDEBUSY"); break ;
	case SE_ERR_DDEFAIL: MessageBox("SE_ERR_DDEFAIL"); break ;
	case SE_ERR_DDETIMEOUT: MessageBox("SE_ERR_DDETIMEOUT"); break ;
	case SE_ERR_DLLNOTFOUND: MessageBox("SE_ERR_DLLNOTFOUND"); break ;
	case SE_ERR_NOASSOC: MessageBox("SE_ERR_NOASSOC"); break ;
	case SE_ERR_OOM: MessageBox("SE_ERR_OOM"); break ;
	case SE_ERR_SHARE: MessageBox("SE_ERR_SHARE"); break ;
	case 0: MessageBox("ERROR 0"); break ;
	}

	OnOK();
}

