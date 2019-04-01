#include "BaseDialog.h"

BaseDialog::BaseDialog(UINT dialogID, CWnd * pParent)
	: CDialogEx(dialogID, pParent)
{
}

BaseDialog::~BaseDialog()
{
}

IMPLEMENT_DYNAMIC(BaseDialog, CDialogEx)

BEGIN_MESSAGE_MAP(BaseDialog, CDialogEx)
END_MESSAGE_MAP()

void BaseDialog::DoDataExchange(CDataExchange * pDX)
{
}

INT_PTR BaseDialog::ModalDialog(CDialogEx & dialog)
{
	CDialogEx::ShowWindow(SW_HIDE);
	auto result = dialog.DoModal();
	CDialogEx::ShowWindow(SW_SHOW);
	return result;
}
