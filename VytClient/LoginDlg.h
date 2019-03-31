#pragma once

#include <afxdialogex.h>
#include "Packet.h"
#include "RegisDlg.h"
#include "RegisVerifyDlg.h"
// LoginDlg 对话框

class LoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoginDlg)

public:
	LoginDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~LoginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGINDLG };
#endif

private:
	RegisDlg m_regisDlg;
	RegisVerifyDlg m_verifyDlg;
public:
	static const int SUCCESS_FLAG = 0x12345678;
	void HandleLoginMsg(byte msg);
	void HandleRegisMsg(byte msg);
	void HandleVerifyMsg(byte msg);
	void ShowLoginDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickLogin();
	afx_msg void ShowRegisDlg();
	CString m_username;
	CString m_password;
};
