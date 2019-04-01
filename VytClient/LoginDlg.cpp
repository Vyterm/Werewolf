// LoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "LoginDlg.h"
#include "ClientPeer.h"
#include "afxdialogex.h"
#include "Commands.h"
#include "RegisDlg.h"

using namespace vyt;

// LoginDlg 对话框

IMPLEMENT_DYNAMIC(LoginDlg, BaseDialog)

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: BaseDialog(IDD_LOGINDLG, pParent), IHandler(command(OpCommand::User), command(UserCommand::Login))
	, m_username(_T(""))
	, m_password(_T(""))
{
}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getMessageSize() != 1)
		MessageBox(_T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
	{
		auto msg = packet->getMessage()[0];
		if (msg == 0)
		{
			MessageBox(GetStringByTable(IDS_LOGINSUCCESSHINT), GetStringByTable(IDS_LOGINSUCCESSTITLE), MB_ICONINFORMATION);
			EndDialog(SUCCESS_FLAG);
		}
		else if (msg == 1)
			MessageBox(GetStringByTable(IDS_LOGINERROR1), GetStringByTable(IDS_LOGINERRORTITLE), MB_ICONERROR);
		else if (msg == 2)
			MessageBox(GetStringByTable(IDS_LOGINERROR2), GetStringByTable(IDS_LOGINERRORTITLE), MB_ICONERROR);
		else if (msg == 3)
			MessageBox(GetStringByTable(IDS_LOGINERROR3), GetStringByTable(IDS_LOGINERRORTITLE), MB_ICONERROR);
	}
}

void LoginDlg::ShowRegisDlg()
{
	RegisDlg m_regisDlg;
	if (RegisDlg::SUCCESS_FLAG == ModalDialog(m_regisDlg))
		EndDialog(SUCCESS_FLAG);
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_username);
	DDX_Text(pDX, IDC_EDIT2, m_password);
}


BEGIN_MESSAGE_MAP(LoginDlg, BaseDialog)
	ON_BN_CLICKED(IDOK, &LoginDlg::OnClickLogin)
	ON_BN_CLICKED(IDC_BUTTON1, &LoginDlg::ShowRegisDlg)
END_MESSAGE_MAP()


// LoginDlg 消息处理程序


BOOL LoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(GetStringByTable(IDS_LOGIN));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void LoginDlg::OnClickLogin()
{
	UpdateData(TRUE);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::Login), "ss", m_username, m_password));
}
