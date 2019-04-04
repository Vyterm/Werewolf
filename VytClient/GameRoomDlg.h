#pragma once
#include "ChatroomDlg.h"

// GameRoomDlg 对话框

class GameRoomDlg : public ChatroomDlg
{
	DECLARE_DYNAMIC(GameRoomDlg)

public:
	static void Create(CString lobbyID);
private:
	static void Delete(GameRoomDlg *dlg);

private:
	GameRoomDlg(CString lobbyID, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~GameRoomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HR_GAMEROOM };
#endif

public:
	CString getLobbyID() const { return m_lobbyID; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnCancel();
};
