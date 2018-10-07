
// DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DemoMCCpp.h"
#include "DemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BUFFER_SIZE 12024 
static CDemoDlg* m_pDlg;

// callback function to receive async messages
void CALLBACK OnCallback( int type, LPCSTR message ) 
{
	// type is the frame type
	CStringA sMessageA( message );
	CString sMessage; sMessage.Format( _T( "Async Callback type:%d: %s\r\n"), type, CString( sMessageA ) );
	// calls a thread safe method to add to log 
	m_pDlg->AddToLogAsync( sMessage );
}

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDemoDlg dialog



CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DEMOMCCPP_DIALOG, pParent)
	, m_sIP( _T( "10.0.0.2" ) )
	, m_sPort( _T( "4001" ) )
	, m_sDevices( _T( "" ) )
	, m_sCmd( _T( "?ver" ) )
	, m_sLog( _T( "" ) )
	, m_sPassword( _T( "" ) )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDT_IP, m_sIP );
	DDX_Text( pDX, IDC_EDT_PORT, m_sPort );
	DDX_Text( pDX, IDC_EDT_DEVICES, m_sDevices );
	DDX_Text( pDX, IDC_EDT_CMD, m_sCmd );
	DDX_Text( pDX, IDC_EDT_LOG, m_sLog );
	DDX_Text( pDX, IDC_EDT_PASS, m_sPassword );
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_BN_CLICKED( IDC_BTN_CONNCT, &CDemoDlg::OnBnClickedBtnConnct )
	ON_BN_CLICKED( IDC_BTN_EXEC, &CDemoDlg::OnBnClickedBtnExec )
	ON_BN_CLICKED( IDC_BTN_GET_DEVICES, &CDemoDlg::OnBnClickedBtnGetDevices )
	ON_WM_TIMER()
	ON_BN_CLICKED( IDC_BTN_SEND, &CDemoDlg::OnBnClickedBtnSend )
	ON_BN_CLICKED( IDC_BTN_AUTHENTICATE, &CDemoDlg::OnBnClickedBtnAuthenticate )
	ON_BN_CLICKED( IDC_BTN_DISCONNCT, &CDemoDlg::OnBnClickedBtnDisconnct )
	
	ON_BN_CLICKED( IDC_BTN_SEND_FILE, &CDemoDlg::OnBnClickedBtnSendFile )
	ON_BN_CLICKED( IDC_BTN_GET_FILE, &CDemoDlg::OnBnClickedBtnGetFile )
END_MESSAGE_MAP()


// CDemoDlg message handlers

BOOL CDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
		
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	// KMApi initialization here
	m_pDlg = this;
	m_pKMApi = KMNewKMAPI();
	m_pKMApi->SetCallback( OnCallback );
	
	// 10 seconds execute timeout
	m_pKMApi->SetTimeout( KMTO_Exec, 10*1000 ); 

	// 7 seconds connect timeout
	m_pKMApi->SetTimeout( KMTO_Connect, 7 * 1000 ); 

	// start the check async buffer timer
	SetTimer( 11, 500, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDemoDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


///////////////////////////////////////////////////////////////
///   KMAPI Code
///////////////////////////////////////////////////////////////


void CDemoDlg::AddToLog( CString sMessage ) {
	UpdateData();
	m_sLog += sMessage;
	UpdateData( 0 );
}
void CDemoDlg::AddToLogAsync( CString sMessage ) {
	m_mutx.Lock();
	m_sAsyncLog += sMessage; 
	m_mutx.Unlock();
}

void CDemoDlg::CheckError() {
	int errorCode;
	char buffer[BUFFER_SIZE];
	KMResponse res = m_pKMApi->GetLastError( errorCode, buffer, BUFFER_SIZE );
	// if there is something to report
	if (errorCode == KMRet_OK) 
		return;
	CString sMessage;
	CStringA sBuffer(buffer);
	sMessage.Format( _T( "Error number %d , \r\nError Message: %s\r\n" ),  errorCode, CString( sBuffer ) );
	AddToLog( sMessage );
}
void CDemoDlg::OnBnClickedBtnConnct()
{
	UpdateData();
	CStringA sIP(m_sIP);
	KMResponse ret = m_pKMApi->Connect( sIP.GetBuffer(), _ttoi( m_sPort ) );
	if (ret != KMRet_OK)
	{
		CheckError();
	}
	else
	{
		AddToLog( _T( "Connected\r\n" ) );
	}
}


void CDemoDlg::OnBnClickedBtnExec()
{
	UpdateData();
	CHAR buffer[BUFFER_SIZE];
	CStringA sCommand( m_sCmd ); 
	KMResponse resp = m_pKMApi->ExecCommand( sCommand.GetBuffer() , buffer, BUFFER_SIZE );
	if (resp != KMRet_OK)
	{
		CheckError();
	}
	else
	{
		AddToLog( CString(buffer) );
	}
	UpdateData( 0 );
}

// send command
void CDemoDlg::OnBnClickedBtnSend()
{
	UpdateData();
	CStringA sCommand( m_sCmd );
	KMResponse resp = m_pKMApi->SendCommand( sCommand.GetBuffer() );
	if (resp != KMRet_OK)
	{
		CheckError();
	}
	else
	{
		CString sMessage;
		sMessage.Format( _T( "Command '%s' sent\r\n" ), m_sCmd );
		AddToLog( sMessage );
	}
	UpdateData( 0 );
}

// authenticate
void CDemoDlg::OnBnClickedBtnAuthenticate()
{
	UpdateData();
	CStringA sPassword( m_sPassword );
	KMResponse resp = m_pKMApi->Authenticate( sPassword.GetBuffer() );
	// MC does not return error in case of wrong password
	if (resp != KMRet_OK)
	{
		CheckError();
	}
	else
	{
		AddToLog( _T("Authenticated\r\n") );
	}
	UpdateData( 0 );
}

// disconnect from controller (and create a new KMAPI object)
void CDemoDlg::OnBnClickedBtnDisconnct()
{
	// drop/delete old kmapi object 
	MCFreeKMApi( m_pKMApi );
	AddToLog( _T( "Disconnected\r\n" ) );
	// create a new one
	m_pKMApi = KMNewKMAPI();
}

void CDemoDlg::OnBnClickedBtnGetDevices()
{
	// Detect devices on the same network
	m_pKMApi->DetectDevices( 2000 ); // wait 2 seconds to get all responses from all devices on network


	// you can get all list as a CSV list 
	CStringA sDeviceList = m_pKMApi->GetDevicesDescriptor( true );
	if (sDeviceList.IsEmpty())
		sDeviceList = "No devices found";
	m_sDevices =  CString( sDeviceList);
	UpdateData( 0 );

	// or get a pointer to a list of objects (devices)
	MCDevice* pDevices; 

	pDevices = m_pKMApi->GetDeviceList(); // gets a list of MCDevice struct
	if (pDevices[0].ip[0] != 0) // it is safe since the last IP is always ""
	{
		// show the 1st ip 
		AfxMessageBox( _T( "First IP is " ) + CString( pDevices[0].ip ) );
	}
}


void CDemoDlg::OnTimer( UINT_PTR nIDEvent )
{
	if (nIDEvent == 11)
	{
		CString tmpLog;
		// need to thread protect 
		//since m_sAsyncLog is changed in callback
		m_mutx.Lock();
		if (!m_sAsyncLog.IsEmpty())
		{
			tmpLog = m_sAsyncLog;
			m_sAsyncLog.Empty();
		}
		m_mutx.Unlock();
		if (!tmpLog.IsEmpty())
		{
			AddToLog( tmpLog );
		}
	}
	CDialogEx::OnTimer( nIDEvent );
}







void CDemoDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}


void CDemoDlg::OnBnClickedBtnSendFile()
{
	// Takes a.prg from temp folder and upload it to MC
	KMResponse resp = m_pKMApi->UploadFile( "c:\\temp\\a.prg" );
	if (resp != KMRet_OK)
	{
		CheckError();
	}
	else
	{
		AddToLog( _T( "File uploaded successfully\r\n" ) );
	}

}


void CDemoDlg::OnBnClickedBtnGetFile()
{
	// Gets a.prg and puts it in temp folder
	KMResponse resp = m_pKMApi->DownloadFile( "c:\\temp", "a.prg" );
	if (resp != KMRet_OK)
	{
		CheckError();
	}
	else
	{
		AddToLog( _T( "File downloaded successfully\r\n" ) );
	}


}
