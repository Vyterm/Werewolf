#pragma once

#include "Handler.h"
// ChatroomDlg 对话框

class PlayerListHandler : public vyt::IHandler
{
private:
	CListCtrl &m_players;
	void PlayerJoin(CString player);
	void PlayerLeave(CString player);
	void PlayerRename(CString oldname, CString newname);
public:
	PlayerListHandler(CListCtrl &players);
	virtual ~PlayerListHandler();
	void HandlePacket(vyt::Packet &packet);
};

class ChatroomDlg : public CDialogEx, public vyt::IHandler
{
	DECLARE_DYNAMIC(ChatroomDlg)

public:
	ChatroomDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ChatroomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_H_CHATROOM };
#endif

private:
	int m_friendID;
	CString m_roomID;
public:
	void HandlePacket(vyt::Packet &packet);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_players;
	PlayerListHandler m_playerHandler;
	CString m_message;
	afx_msg void DoSend();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_chats;
	CEdit m_chatscroll;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectPlayer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAddFriend();
};
