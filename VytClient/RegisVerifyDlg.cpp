// RegisVerifyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "RegisVerifyDlg.h"
#include "afxdialogex.h"
#include "ClientPeer.h"
#include "Commands.h"

using namespace vyt;

// RegisVerifyDlg 对话框

IMPLEMENT_DYNAMIC(RegisVerifyDlg, CDialogEx)

RegisVerifyDlg::RegisVerifyDlg(CString phone, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_R_VERIFY, pParent), m_phone(phone), IHandler(command(OpCommand::User), command(UserCommand::Verify))
	, m_code(_T(""))
{
}

RegisVerifyDlg::~RegisVerifyDlg()
{
}

void RegisVerifyDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getMessageSize() != 1)
		MessageBox(_T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
	{
		auto msg = packet->getMessage()[0];
		if (msg == 0)
		{
			MessageBox(GetStringByTable(IDS_LOGINSUCCESSHINT), GetStringByTable(IDS_REGISSUCCESSTITLE), MB_ICONINFORMATION);
			EndDialog(SUCCESS_FLAG);
		}
		else if (msg == 1)
		{
			MessageBox(_T("输入的不是有效的验证码"));
		}
	}
}

void RegisVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_code);
}


BEGIN_MESSAGE_MAP(RegisVerifyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &RegisVerifyDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// RegisVerifyDlg 消息处理程序


void RegisVerifyDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::Verify), "ss", m_phone, m_code));
}
