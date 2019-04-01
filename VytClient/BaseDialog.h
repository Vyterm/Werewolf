#ifndef BASE_DIALOG_H_INCLUDED
#define BASE_DIALOG_H_INCLUDED

#include <afxdialogex.h>

class BaseDialog : public CDialogEx
{
protected:
	BaseDialog(UINT dialogID, CWnd* pParent = nullptr);   // ��׼���캯��
	virtual ~BaseDialog();
private:
	DECLARE_DYNAMIC(BaseDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()

	INT_PTR ModalDialog(CDialogEx &dialog);
public:
};

#endif