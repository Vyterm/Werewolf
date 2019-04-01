#ifndef BASE_DIALOG_H_INCLUDED
#define BASE_DIALOG_H_INCLUDED

#include <afxdialogex.h>

class BaseDialog : public CDialogEx
{
protected:
	BaseDialog(UINT dialogID, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~BaseDialog();
private:
	DECLARE_DYNAMIC(BaseDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

	INT_PTR ModalDialog(CDialogEx &dialog);
public:
};

#endif