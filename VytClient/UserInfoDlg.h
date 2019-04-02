#pragma once

#include "Handler.h"
// UserInfoDlg 对话框

class UserInfoDlg : public CDialogEx, public vyt::IHandler
{
	DECLARE_DYNAMIC(UserInfoDlg)

public:
	UserInfoDlg(CString username, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~UserInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HF_USERINFO };
#endif

private:
	CString m_beforeUsername;
public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_username;
	afx_msg void OnSetname();
	virtual BOOL OnInitDialog();
};
