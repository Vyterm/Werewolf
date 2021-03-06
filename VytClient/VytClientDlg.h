
// VytClientDlg.h: 头文件
//

#pragma once
#include "SmartTabCtrl.h"

// CVytClientDlg 对话框
class CVytClientDlg : public CDialogEx
{
// 构造
public:
	CVytClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VYTCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	bool LoginToServer();
	void InitUI();

// 实现
protected:
	HICON m_hIcon;
	SmartTabCtrl m_channelTab;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStatusChanged();
};
