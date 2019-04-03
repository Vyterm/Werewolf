// SmartListCtrl.cpp: 实现文件
//

#include "stdafx.h"
#include "SmartListCtrl.h"

IMPLEMENT_DYNAMIC(SmartListCtrl, CListCtrl)

SmartListCtrl::SmartListCtrl()
{

}

SmartListCtrl::~SmartListCtrl()
{
}

void SmartListCtrl::InsertItemWithTexts(int index, CString title, int textCount, ...)
{
	InsertItem(index, title);
	va_list texts;
	va_start(texts, textCount);
	for (int i = 0; i < textCount; ++i)
		SetItemText(index, i + 1, va_arg(texts, CString));
	va_end(texts);
}

void SmartListCtrl::AppendColumn(int index, CString title, int width)
{
	InsertColumn(index, title, 0, width);
}

BEGIN_MESSAGE_MAP(SmartListCtrl, CListCtrl)
END_MESSAGE_MAP()



// SmartListCtrl 消息处理程序


