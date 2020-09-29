
// MFCNumericEditControlExampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCNumericEditControlExample.h"
#include "MFCNumericEditControlExampleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCNumericEditControlExampleDlg dialog

CMFCNumericEditControlExampleDlg::CMFCNumericEditControlExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCNumericEditControlExampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCNumericEditControlExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NUMERICINPUT, m_edtNumericInput);
}

BEGIN_MESSAGE_MAP(CMFCNumericEditControlExampleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCNumericEditControlExampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFCNumericEditControlExampleDlg message handlers

BOOL CMFCNumericEditControlExampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCNumericEditControlExampleDlg::OnPaint()
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
HCURSOR CMFCNumericEditControlExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCNumericEditControlExampleDlg::OnBnClickedOk()
{	
	//Get edit control numeric value and display
	LONGLONG llValue = m_edtNumericInput.AsValue();
	CString sValue;
	sValue.Format(L"The decimal value is: %llu\nThe hex value is: 0x%llx", llValue, llValue);
	MessageBox(sValue, L"Example", MB_OK);

	CDialogEx::OnOK();
}
