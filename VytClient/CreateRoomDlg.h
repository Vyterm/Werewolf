#pragma once

#include "Handler.h"
// CreateRoomDlg 对话框

class CreateRoomDlg : public CDialogEx, public vyt::IHandler
{
	DECLARE_DYNAMIC(CreateRoomDlg)

public:
	CreateRoomDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CreateRoomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HR_CREATEROOM };
#endif

public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_roomname;
	afx_msg void PasswordSetting();
	CString m_password;
	afx_msg void OnCreateRoom();
	afx_msg void OnClickCancel();
	CEdit m_passwordInputfield;
	BOOL m_ispassword;
};
