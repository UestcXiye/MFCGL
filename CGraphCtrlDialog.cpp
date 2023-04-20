// CGraphCtrlDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFCGL.h"
#include "afxdialogex.h"
#include "CGraphCtrlDialog.h"


#define WM_CHANGECTRLER WM_USER+101
#define WM_CHANGEAXIS WM_USER+102
#define WM_CLOSEDIALOG WM_USER+103
#define WM_CHANGEDRAG WM_USER+106
#define WM_CHANGEDATA WM_USER+107

// CGraphCtrlDialog 对话框



IMPLEMENT_DYNAMIC(CGraphCtrlDialog, CDialogEx)

CGraphCtrlDialog::CGraphCtrlDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTRLPANEL, pParent)
	, m_controller(0), m_axis(0), m_dragtype(0) {

}

CGraphCtrlDialog::~CGraphCtrlDialog() {}

void CGraphCtrlDialog::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_controller);
	// DDX_Text(pDX,IDC_EDIT1,x_offset);
}


BEGIN_MESSAGE_MAP(CGraphCtrlDialog, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, &CGraphCtrlDialog::OnBnClickedRadioCtrlGroup)
	ON_BN_CLICKED(IDC_RADIO2, &CGraphCtrlDialog::OnBnClickedRadioCtrlGroup)
	ON_WM_CLOSE()
	ON_COMMAND(IDR_MENU1, &CGraphCtrlDialog::OnOpenByMenu)
	ON_MESSAGE(WM_CHANGEDATA, &CGraphCtrlDialog::OnChangeData)

END_MESSAGE_MAP()


// CGraphCtrlDialog 消息处理程序
int CGraphCtrlDialog::ctrltype = 0;
double CGraphCtrlDialog::x_offset = 0;
double CGraphCtrlDialog::y_offset = 0;
double CGraphCtrlDialog::z_offset = 0;

void CGraphCtrlDialog::PostNcDestroy() {
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CDialogEx::PostNcDestroy();
}


void CGraphCtrlDialog::OnBnClickedRadioCtrlGroup() {
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	switch (m_controller) {
	case 0:
		ctrltype = 0;
		// MessageBox(_T("0"));
		break;
	case 1:
		ctrltype = 1;
		// MessageBox(_T("1"));
		break;
	}
	actV->PostMessage(WM_CHANGECTRLER);
}





void CGraphCtrlDialog::OnOK() {
	// TODO: 在此添加专用代码和/或调用基类
	actV->PostMessage(WM_CLOSEDIALOG);
	CDialogEx::OnOK();
}


void CGraphCtrlDialog::OnCancel() {
	// TODO: 在此添加专用代码和/或调用基类
	actV->PostMessage(WM_CLOSEDIALOG);
	CDialogEx::OnCancel();
}


void CGraphCtrlDialog::OnClose() {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	actV->PostMessage(WM_CLOSEDIALOG);
	CDialogEx::OnClose();
}


void CGraphCtrlDialog::OnOpenByMenu() {
	// TODO: 在此添加命令处理程序代码
	AfxMessageBox("hola");
}



LRESULT CGraphCtrlDialog::OnChangeData(WPARAM wParam, LPARAM lParam) {
	AfxMessageBox("hola!");

	return 0;
}
