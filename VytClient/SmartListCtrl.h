#pragma once

#include "stdafx.h"
#include "resource.h"

struct ColumnModel
{
public:
	CString Description;
	int Width;
};
// SmartListCtrl

class SmartListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(SmartListCtrl)

public:
	SmartListCtrl();
	virtual ~SmartListCtrl();

	void InsertItemWithTexts(int index, CString title, int textCount = 0, ...);
	void AppendColumn(int index, CString title, int width);

protected:
	DECLARE_MESSAGE_MAP()
};


