#pragma once

#include "BaseDialog.h"
#include "Handler.h"

// RegisDlg 对话框
class RegisDlg : public BaseDialog, public vyt::IHandler
{
	DECLARE_DYNAMIC(RegisDlg)

public:
	RegisDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RegisDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISDLG };
#endif

public:
	static const INT_PTR SUCCESS_FLAG = 0x12345678;
	void HandlePacket(vyt::Packet &packet) override;

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
