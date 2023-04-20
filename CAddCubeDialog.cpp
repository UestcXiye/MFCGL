// CAddCubeDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFCGL.h"
#include "afxdialogex.h"
#include "CAddCubeDialog.h"


#define WM_UPDATEVIEW WM_USER + 100


// CAddCubeDialog 对话框

IMPLEMENT_DYNAMIC(CAddCubeDialog, CDialogEx)

CAddCubeDialog::CAddCubeDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADDCUBE, pParent)
	, m_length(3), m_width(3), m_height(3) {

}

CAddCubeDialog::~CAddCubeDialog() {}

void CAddCubeDialog::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_length);
	DDX_Text(pDX, IDC_EDIT2, m_width);
	DDX_Text(pDX, IDC_EDIT3, m_height);

}


BEGIN_MESSAGE_MAP(CAddCubeDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddCubeDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddCubeDialog 消息处理程序


void CAddCubeDialog::OnBnClickedOk() {
	// TODO: 在此添加控件通知处理程序代码
	
	CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView* actV = pMF->GetActiveView();
	actV->PostMessage(WM_UPDATEVIEW);
	CDialogEx::OnOK();
}


BOOL CAddCubeDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
