// SearchFriendDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "SearchFriendDlg.h"
#include "afxdialogex.h"
#include "ClientPeer.h"
#include "Commands.h"
using namespace vyt;

// SearchFriendDlg 对话框

IMPLEMENT_DYNAMIC(SearchFriendDlg, CDialogEx)

SearchFriendDlg::SearchFriendDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HF_SEARCHFRIEND, pParent)
	, IHandler(command(OpCommand::Friend), command(FriendCommand::Search))
	, m_friendname(_T(""))
{

}

SearchFriendDlg::~SearchFriendDlg()
{
}

void SearchFriendDlg::HandlePacket(vyt::Packet & packet)
{
	byte access = packet->getMessage()[0];
	if (1 == access)
		MessageBox(_T("未找到指定的好友"));
	else if (0 == access)
	{
		ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Add), "s", m_friendname));
		EndDialog(0);
	}
}

void SearchFriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HFS_FRIENDNAME, m_friendname);
}


BEGIN_MESSAGE_MAP(SearchFriendDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SearchFriendDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SearchFriendDlg 消息处理程序


void SearchFriendDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Search), "s", m_friendname));
}
