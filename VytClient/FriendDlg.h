#pragma once

#include "BaseDialog.h"
#include "Handler.h"
// FriendDlg 对话框

class FriendDlg : public BaseDialog, public vyt::IHandler
{
	DECLARE_DYNAMIC(FriendDlg)

public:
	FriendDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FriendDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_H_FRIEND };
#endif

public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_friends;
	afx_msg void OnShowDetails();
};
