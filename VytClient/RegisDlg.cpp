// RegisDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "RegisDlg.h"
#include "LoginDlg.h"
#include "ClientPeer.h"
#include "Commands.h"
#include "RegisVerifyDlg.h"

using namespace vyt;

// RegisDlg 对话框

IMPLEMENT_DYNAMIC(RegisDlg, BaseDialog)

RegisDlg::RegisDlg(CWnd* pParent /*=nullptr*/)
	: BaseDialog(IDD_REGISDLG, pParent), IHandler(command(OpCommand::User), command(UserCommand::Regis))
	, m_username(_T(""))
	, m_password(_T(""))
{
}

RegisDlg::~RegisDlg()
{
}

void RegisDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getMessageSize() != 1)
		MessageBox(_T("非法的消息"), _T("错误"), MB_ICONERROR);
	else
	{
		auto msg = packet->getMessage()[0];
		if (msg == 0)
		{
			RegisVerifyDlg verify(m_username, this);
			if (RegisVerifyDlg::SUCCESS_FLAG == ModalDialog(verify))
				EndDialog(SUCCESS_FLAG);
		}
		else if (msg == 1)
			MessageBox(GetStringByTable(IDS_REGISERROR1), GetStringByTable(IDS_REGISERRORTITLE), MB_ICONERROR);
	}
}

void RegisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_username);
	DDX_Text(pDX, IDC_EDIT2, m_password);
}


BEGIN_MESSAGE_MAP(RegisDlg, BaseDialog)
	ON_BN_CLICKED(IDOK, &RegisDlg::OnClickRegis)
	ON_BN_CLICKED(IDC_BUTTON1, &RegisDlg::OnShowLogin)
END_MESSAGE_MAP()


// RegisDlg 消息处理程序


void RegisDlg::OnClickRegis()
{
	UpdateData(TRUE);
	vyt::ClientPeer::Get().Send(_Packet(command(OpCommand::User), command(UserCommand::Regis), "ss", m_username, m_password));
}


void RegisDlg::OnShowLogin()
{
	EndDialog(0);
}


BOOL RegisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(GetStringByTable(IDS_REGIS));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
