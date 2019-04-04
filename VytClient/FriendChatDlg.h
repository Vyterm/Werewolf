#pragma once

#include "Handler.h"
// FriendChatDlg 对话框

class FriendChatDlg : public CDialogEx, public vyt::IHandler
{
	DECLARE_DYNAMIC(FriendChatDlg)

public:
	static const unsigned long CHAT_FLAG = 0;
	static const unsigned long FILE_FLAG = 1;
	static const unsigned long VIDEO_FLAG = 2;
	static void Create(CString selfname, CString friendname, CWnd* pParent = nullptr, unsigned long extraFlags = CHAT_FLAG);
private:
	static void Delete(FriendChatDlg *pDlg);
	static void Rename(FriendChatDlg *pDlg, CString &newname);
	static const UINT_PTR DIED_TIMER = 0x1234;
	static const UINT_PTR VIDEO_TIMER = 0x5678;
private:
	FriendChatDlg(CString selfname, CString friendname, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FriendChatDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HF_FRIENDCHAT };
#endif

private:
	CFile *m_chatrecord;
	CString m_selfname;
	CString m_friendname;
	void UpdateTitle();
	void ShowChat(CString &sender, CString &chat);
	void HandleRename(vyt::Packet &packet);
	void HandleChat(vyt::Packet &packet);
	void SendFile(CString filepath);
	void HandleFile(vyt::Packet &packet);
	void SendVideo();
	void HandleVideo(vyt::Packet &packet);
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
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL OnInitDialog();
};
