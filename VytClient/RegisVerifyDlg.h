#pragma once


// RegisVerifyDlg 对话框

class RegisVerifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisVerifyDlg)

public:
	RegisVerifyDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RegisVerifyDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_R_VERIFY };
#endif

private:
	CString m_phone;
public:
	void ShowWindow(CString phone);
	void HideWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_code;
	afx_msg void OnBnClickedOk();
};
