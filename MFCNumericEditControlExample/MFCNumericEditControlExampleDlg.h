
// MFCNumericEditControlExampleDlg.h : header file
//

#pragma once

#include "CNumericEditControl.h"

// CMFCNumericEditControlExampleDlg dialog
class CMFCNumericEditControlExampleDlg : public CDialogEx
{
// Construction
public:
	CMFCNumericEditControlExampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCNUMERICEDITCONTROLEXAMPLE_DIALOG };

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
private:
	CNumericEditControl m_edtNumericInput; //CNumericEditControl
public:
	afx_msg void OnBnClickedOk();
};
