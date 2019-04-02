#pragma once

#include "Handler.h"
// FriendChatDlg 对话框

class FriendChatDlg : public CDialogEx, public vyt::IHandler
{
	DECLARE_DYNAMIC(FriendChatDlg)

public:
	static void Create(CString selfname, CString friendname, CWnd* pParent = nullptr);
private:
	static void Delete(FriendChatDlg *pDlg);
private:
	FriendChatDlg(CString selfname, CString friendname, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FriendChatDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HF_FRIENDCHAT };
#endif

private:
	CString m_selfname;
	CString m_friendname;
	void HandleAccess(byte access);
	void ShowChat(CString &sender, CString &chat);
public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_chatview;
	CString m_inputchat;
	afx_msg void SendChat();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
