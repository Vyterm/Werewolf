// FriendChatDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "VytClient.h"
#include "FriendChatDlg.h"
#include "afxdialogex.h"
#include "Commands.h"
#include "ClientPeer.h"
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>

using namespace vyt;

// Send
static cv::Mat img_encode;
static cv::VideoCapture capture(0);
static std::vector<uchar> video_encode;
// Recv
static cv::Mat img_decode;
static std::vector<uchar> video_decode;

static std::map<CString, FriendChatDlg*> pFriends;

inline CString GetDocumentPath()
{
	TCHAR homePath[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(AfxGetMainWnd()->GetSafeHwnd(), homePath, 5, false);
	return homePath;
}

void FriendChatDlg::Create(CString selfname, CString friendname, CWnd* pParent, unsigned long extraFlags)
{
	FriendChatDlg *fcd = nullptr;
	if (pFriends.find(friendname) == pFriends.end())
	{
		fcd = new FriendChatDlg(selfname, friendname);
		pFriends.emplace(friendname, fcd);
	}
	else
		fcd = pFriends.find(friendname)->second;
	fcd->ShowWindow(SW_SHOW);
	if (extraFlags & FILE_FLAG)
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		TCHAR path[MAX_PATH] = {};
		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = _T("./");
		ofn.lpstrFile = path;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = _T("所有文件(*.*)\0*.*\0\0");
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn))
			fcd->SendFile(path);
	}
	else if (extraFlags & VIDEO_FLAG)
		fcd->SetTimer(VIDEO_TIMER, 100, nullptr);
}

void FriendChatDlg::Delete(FriendChatDlg * pDlg)
{
	pFriends.erase(pFriends.find(pDlg->m_friendname));
	pDlg->KillTimer(VIDEO_TIMER);
	pDlg->ShowWindow(SW_HIDE);
	pDlg->SetTimer(DIED_TIMER, 1000, nullptr);
}

void FriendChatDlg::Rename(FriendChatDlg *pDlg, CString &newname)
{
	pFriends.erase(pFriends.find(pDlg->m_friendname));
	pDlg->m_friendname = newname;
	pFriends[pDlg->m_friendname] = pDlg;
}

IMPLEMENT_DYNAMIC(FriendChatDlg, CDialogEx)

FriendChatDlg::FriendChatDlg(CString selfname, CString friendname, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HF_FRIENDCHAT, pParent)
	, IHandler(command(OpCommand::Friend), command(FriendCommand::Chat))
	, m_selfname(selfname)
	, m_friendname(friendname)
	, m_chatview(_T(""))
	, m_inputchat(_T(""))
{
	CString recordpath;
	recordpath.Format(_T(".\\ChatRecords\\%s_%s.vcr"), m_selfname, m_friendname);
	m_chatrecord = new CFile(recordpath, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeBinary | CFile::osSequentialScan);

	CDialogEx::Create(IDD_HF_FRIENDCHAT, pParent);
	UpdateTitle();
	NetHandler::Get().RegisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Offline), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::File), *this);
	NetHandler::Get().RegisterHandler(command(OpCommand::Friend), command(FriendCommand::Video), *this);
}

FriendChatDlg::~FriendChatDlg()
{
	NetHandler::Get().UnregisterHandler(command(OpCommand::User), command(UserCommand::Rename), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Offline), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::File), *this);
	NetHandler::Get().UnregisterHandler(command(OpCommand::Friend), command(FriendCommand::Video), *this);

	m_chatrecord->Close();
	delete m_chatrecord;
}

void FriendChatDlg::UpdateTitle()
{
	CString title;
	title.Format(_T("%s 与 %s 的对话"), m_selfname, m_friendname);
	CDialogEx::SetWindowText(title);
}

void FriendChatDlg::ShowChat(CString &sender, CString & chat)
{
	UpdateData(TRUE);
	CString message = sender + _T(":") + chat + _T("\r\n");;
	m_chatview += message;
	m_chatrecord->Write(message.GetString(), message.GetLength() * vytsize(sizeof(TCHAR)));
	m_chatrecord->Flush();
	UpdateData(FALSE);
}

void FriendChatDlg::HandleRename(vyt::Packet & packet)
{
	CString oldname, newname;
	packet->Decode("ss", &oldname, &newname);
	if (oldname == m_selfname)
		m_selfname = newname;
	else if (oldname == m_friendname)
		Rename(this, newname);
	UpdateTitle();
}

void FriendChatDlg::HandleChat(vyt::Packet & packet)
{
	CString sender, chat;
	packet->Decode("ss", &sender, &chat);
	ShowChat(sender, chat);
}

void FriendChatDlg::SendFile(CString filepath)
{
	CString filename = filepath.Mid(filepath.ReverseFind('\\') + 1);
	CFile file;
	file.Open(filepath, CFile::modeRead);
	vytsize length = vytsize(file.GetLength());
	char *pBuffer = new char[length];
	file.Read(pBuffer, length);
	Buffer buffer = _Buffer("ss", m_friendname, filename);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::File), {
		{ buffer->m_buffer, buffer->m_size },
		{ pBuffer, length },
		}));
	file.Close();
}

void FriendChatDlg::HandleFile(vyt::Packet & packet)
{
	CString filename;
	vytsize size = packet->Decode("ss", nullptr, &filename);
	CString filepath = GetDocumentPath() + _T("\\");
	CString fullpath = filepath + filename;
	char path[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, fullpath, -1, path, MAX_PATH, nullptr, FALSE);
	FILE *file = nullptr;
	fopen_s(&file, path, "w+");
	if (nullptr != file)
	{
		fwrite(packet->getMessage() + size, packet->getMessageSize() - size, 1, file);
		fclose(file);
		ShellExecute(nullptr, _T("explore"), filepath, nullptr, nullptr, SW_SHOW);
		CString message;
		message.Format(_T("收到来自 %s 的文件 %s"), m_friendname, filename);
		MessageBox(message);
	}
}

void FriendChatDlg::SendVideo()
{
	if (!capture.read(img_encode)) return;
	cv::imencode(".jpg", img_encode, video_encode);
	vytsize len_encode = vytsize(video_encode.size());
	__Buffer buffer(len_encode);
	for (vytsize i = 0; i < len_encode; ++i)
		buffer.m_buffer[i] = video_encode[i];
	auto userpack = _Buffer("s", m_friendname);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Video), {
		{ userpack->m_buffer, userpack->m_size },
		{ buffer.m_buffer, buffer.m_size },
	}));
}

void FriendChatDlg::HandleVideo(vyt::Packet & packet)
{
	vytsize len_name = packet->Decode("s", nullptr);
	pByte buffer = packet->getMessage() + len_name;
	vytsize len_buffer = packet->getMessageSize() - len_name;
	video_decode.resize(len_buffer);
	for (vytsize i = 0; i < len_buffer; i++)
		video_decode[i] = buffer[i];
	img_decode = imdecode(video_decode, cv::ImreadModes::IMREAD_COLOR);
	char name[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, m_friendname, -1, name, MAX_PATH, nullptr, FALSE);
	imshow(name, img_decode);
}

void FriendChatDlg::HandlePacket(vyt::Packet & packet)
{
	if (packet->getOpCommand() == command(OpCommand::Friend))
	{
		CString player;
		packet->Decode("s", &player);
		if (player != m_friendname) return;
		if (packet->getSubCommand() == command(FriendCommand::Offline))
			Delete(this);
		else if (packet->getSubCommand() == command(FriendCommand::Chat))
			HandleChat(packet);
		else if (packet->getSubCommand() == command(FriendCommand::File))
			HandleFile(packet);
		else if (packet->getSubCommand() == command(FriendCommand::Video))
			HandleVideo(packet);
	}
	else if (packet->getOpCommand() == command(OpCommand::User))
	{
		if (packet->getSubCommand() == command(UserCommand::Rename))
			HandleRename(packet);
	}
}

void FriendChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HFF_CHATVIEW, m_chatview);
	DDX_Text(pDX, IDC_HFF_CHATINPUT, m_inputchat);
}


BEGIN_MESSAGE_MAP(FriendChatDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &FriendChatDlg::SendChat)
	ON_WM_TIMER()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// FriendChatDlg 消息处理程序


void FriendChatDlg::SendChat()
{
	UpdateData(TRUE);
	if (m_inputchat.IsEmpty()) return;
	ShowChat(m_selfname, m_inputchat);
	ClientPeer::Get().Send(_Packet(command(OpCommand::Friend), command(FriendCommand::Chat), "ss", m_friendname, m_inputchat));
	m_inputchat = _T("");
	UpdateData(FALSE);
}


BOOL FriendChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		SendChat();

	return __super::PreTranslateMessage(pMsg);
}


void FriendChatDlg::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
	if (nIDEvent == VIDEO_TIMER)
		SendVideo();
	else if (nIDEvent == DIED_TIMER)
		delete this;
}


void FriendChatDlg::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);
	TCHAR path[MAX_PATH];
	DragQueryFile(hDropInfo, 0, path, MAX_PATH);
	SendFile(path);
	UpdateData(FALSE);

	__super::OnDropFiles(hDropInfo);
}


BOOL FriendChatDlg::OnInitDialog()
{
	__super::OnInitDialog();

	vytsize length = vytsize(m_chatrecord->GetLength());
	if (length != 0)
	{
		char *buffer = new char[length+sizeof(TCHAR)]();
		m_chatrecord->Read(buffer, length);
		m_chatview = CString((LPCTSTR)buffer);
		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
