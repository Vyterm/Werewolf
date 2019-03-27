#pragma once

#include <afxdialogex.h>
// RegisDlg 对话框
class LoginDlg;
class RegisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisDlg)

public:
	RegisDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RegisDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISDLG };
#endif

private:
	LoginDlg *m_pLoginDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickRegis();
	afx_msg void OnShowLogin();
	CString m_username;
	CString m_password;
	virtual BOOL OnInitDialog();
};
