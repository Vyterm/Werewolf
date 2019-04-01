#pragma once

#include "Handler.h"
// ChatroomDlg 对话框

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

public:
	void HandlePacket(vyt::Packet &packet);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_chats;
	CString m_message;
	afx_msg void DoSend();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
