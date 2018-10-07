
// DemoDlg.h : header file
//

#pragma once
#pragma comment(lib, "Kmapi.lib")
#include "KMApi.h"

// CDemoDlg dialog
class CDemoDlg : public CDialogEx
{
// Construction
public:
	CDemoDlg(CWnd* pParent = NULL);	// standard constructor
	
	KMApi* m_pKMApi;
	CString m_sAsyncLog;
	CMutex m_mutx;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEMOMCCPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	

public:
	
	CString m_sIP;
	CString m_sPort;
	afx_msg	void AddToLog( CString sMessage );
	void AddToLogAsync( CString sMessage );
	void CheckError();
	void OnBnClickedBtnConnct();
	CString m_sDevices;
	CString m_sCmd;
	CString m_sLog;
	CString m_sPassword;
	afx_msg void OnBnClickedBtnExec();
	afx_msg void OnBnClickedBtnGetDevices();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnAuthenticate();
	afx_msg void OnBnClickedBtnDisconnct();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnSendFile();
	afx_msg void OnBnClickedBtnGetFile();
};
