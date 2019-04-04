#pragma once

#include "BaseDialog.h"
#include "SmartListCtrl.h"
#include "Handler.h"
// RoomlistDlg 对话框

class RoomlistDlg : public BaseDialog, public vyt::IHandler
{
	DECLARE_DYNAMIC(RoomlistDlg)

public:
	RoomlistDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RoomlistDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_H_ROOMLIST };
#endif

private:
	void Refresh();
public:
	void HandlePacket(vyt::Packet &packet) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	SmartListCtrl m_roomlist;
	afx_msg void OnBnClickedHrRefresh();
	afx_msg void OnBnClickedHrNewroom();
	virtual BOOL OnInitDialog();
	afx_msg void OnJoinRoom(NMHDR *pNMHDR, LRESULT *pResult);
};
