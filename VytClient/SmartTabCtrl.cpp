// SmartTabCtrl.cpp: 实现文件
//

#include "stdafx.h"
#include "SmartTabCtrl.h"

// SmartTabCtrl

IMPLEMENT_DYNAMIC(SmartTabCtrl, CTabCtrl)

SmartTabCtrl::SmartTabCtrl()
{

}

SmartTabCtrl::~SmartTabCtrl()
{
}

void SmartTabCtrl::ShowDlg(size_t index)
{
	if (GetCurFocus() != index)
		SetCurFocus(int(index));
	for (size_t i = 0; i < m_pDlgs.size(); ++i)
	{
		if (i == index)
		{
			CRect rt;
			GetClientRect(rt);
			rt.DeflateRect(2, 35, -20, -20);
			m_pDlgs[i]->ShowWindow(SW_SHOW);
			m_pDlgs[i]->MoveWindow(rt);
		}
		else
			m_pDlgs[i]->ShowWindow(SW_HIDE);
	}
}


BEGIN_MESSAGE_MAP(SmartTabCtrl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &SmartTabCtrl::OnTcnSelchange)
END_MESSAGE_MAP()



// SmartTabCtrl 消息处理程序

void SmartTabCtrl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowDlg(GetCurSel());
	*pResult = 0;
}
