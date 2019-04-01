#pragma once

#include "resource.h"
#include <afxdialogex.h>
#include <vector>
// SmartTabCtrl

class SmartTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(SmartTabCtrl)

public:
	SmartTabCtrl();
	virtual ~SmartTabCtrl();

	template <typename TDlg>
	void InsertDlg(UINT idTemplate, CString text)
	{
		CDialogEx *pDialog = new TDlg;
		pDialog->Create(idTemplate, this);
		InsertItem(int(m_pDlgs.size()), text);
		m_pDlgs.push_back(pDialog);
	}

	template <typename TDlg>
	TDlg* GetDlg()
	{
		for (auto &pDlg : m_pDlgs)
		{
			TDlg *dlg = dynamic_cast<TDlg*>(pDlg);
			if (nullptr != dlg)
				return dlg;
		}
		return nullptr;
	}

	void ShowDlg(size_t index);
protected:
	std::vector<CDialogEx*> m_pDlgs;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
};


