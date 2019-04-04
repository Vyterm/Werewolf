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

private:
	CString m_username;
	UINT_PTR m_friendFlag;
	int m_friendID;
	void ShowFriends(vyt::Packet &packet);
	void FriendRename(vyt::Packet &packet);
	void ChatAccess(vyt::Packet &packet);
	void AddFriend(vyt::Packet &packet);
	void DelFriend(vyt::Packet &packet);
public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_friends;
	afx_msg void OnShowDetails();
	afx_msg void InteractionWithFriend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChatToFriend();
	afx_msg void OnFileFriend();
	afx_msg void OnVideoFriend();
	afx_msg void OnAppendFriend();
	afx_msg void OnDeleteFriend();
};
