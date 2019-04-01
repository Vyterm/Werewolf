#pragma once


// RoomlistDlg 对话框

class RoomlistDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RoomlistDlg)

public:
	RoomlistDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RoomlistDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_H_ROOMLIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
