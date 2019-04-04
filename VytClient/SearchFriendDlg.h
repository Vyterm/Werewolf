#pragma once

#include "Handler.h"
// SearchFriendDlg 对话框

class SearchFriendDlg : public CDialogEx, public vyt::IHandler
{
	DECLARE_DYNAMIC(SearchFriendDlg)

public:
	SearchFriendDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SearchFriendDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HF_SEARCHFRIEND };
#endif

public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_friendname;
	afx_msg void OnBnClickedOk();
};
