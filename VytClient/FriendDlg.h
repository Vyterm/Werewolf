#pragma once


// FriendDlg 对话框

class FriendDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FriendDlg)

public:
	FriendDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FriendDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_H_FRIEND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
