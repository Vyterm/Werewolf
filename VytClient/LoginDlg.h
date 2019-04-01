#pragma once

#include "BaseDialog.h"
#include "Handler.h"
#include "Packet.h"
// LoginDlg 对话框

class LoginDlg : public BaseDialog, public vyt::IHandler
{
	DECLARE_DYNAMIC(LoginDlg)

public:
	LoginDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~LoginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGINDLG };
#endif

public:
	static const int SUCCESS_FLAG = 0x12345678;
	void HandlePacket(vyt::Packet &packet) override;

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
