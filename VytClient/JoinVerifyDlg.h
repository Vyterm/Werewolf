#pragma once


// JoinVerifyDlg 对话框

class JoinVerifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(JoinVerifyDlg)

public:
	JoinVerifyDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~JoinVerifyDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HR_JOINVERIFY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
