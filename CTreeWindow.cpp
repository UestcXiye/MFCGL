// CTreeWindow.cpp: 实现文件
//

#include "pch.h"
#include "MFCGL.h"
#include "afxdialogex.h"
#include "CTreeWindow.h"
#include "MFCGLView.h"
#include "MainFrm.h"


// CTreeWindow 对话框


CTreeWindow* CTreeWindow::m_pTreeWnd = NULL;
HTREEITEM CTreeWindow::RootItem = 0;
int CTreeWindow::treeItemNum = 0;

IMPLEMENT_DYNAMIC(CTreeWindow, CDialogEx)

CTreeWindow::CTreeWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TREEDIALOG, pParent) {

}

CTreeWindow::~CTreeWindow() {}

void CTreeWindow::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_cTreeCtrl);
}


BEGIN_MESSAGE_MAP(CTreeWindow, CDialogEx)


	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CTreeWindow::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CTreeWindow::OnNMRClickTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CTreeWindow::OnNMClickTree1)
END_MESSAGE_MAP()


// CTreeWindow 消息处理程序


BOOL CTreeWindow::OnInitDialog() {
	CDialogEx::OnInitDialog();

	m_pTreeWnd = this;
	//往模型树插入节点
	RootItem = m_pTreeWnd->m_cTreeCtrl.InsertItem(_T("几何体"));
	m_cTreeCtrl.Expand(RootItem, TVE_EXPAND);//设置模型树节点hTreeItem为展开状态

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}





void CTreeWindow::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CString temp = m_cTreeCtrl.GetItemText(m_cTreeCtrl.GetSelectedItem());
	if (temp != "几何体") {
		CString temp = m_cTreeCtrl.GetItemText(m_cTreeCtrl.GetSelectedItem());
		CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CView* actV = pMF->GetActiveView();
		actV->PostMessage(WM_SELECTTREEITEM);
	} else if (temp == "几何体" && m_cTreeCtrl.GetCount() > 1) {
		CString temp = m_cTreeCtrl.GetItemText(m_cTreeCtrl.GetSelectedItem());
		CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CView* actV = pMF->GetActiveView();
		actV->PostMessage(WM_SELECTTREEITEM);
	}

	//求数字子串
	CString strTemp;
	for (int i = 0; i < temp.GetLength(); i++) {
		if (temp[i] >= '0' && temp[i] <= '9')
			strTemp += temp[i];
	}
	strTemp.IsEmpty() ? treeItemNum = -1 : treeItemNum = atoi(strTemp);
	*pResult = 0;
}


void CTreeWindow::OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;

	GetCursorPos(&point);
	CPoint pointInTree = point;
	m_cTreeCtrl.ScreenToClient(&pointInTree);
	HTREEITEM hItem;
	UINT flag = TVHT_ONITEM;
	hItem = m_cTreeCtrl.HitTest(pointInTree, &flag);

	if (hItem != NULL && hItem != RootItem) {
		m_cTreeCtrl.SelectItem(hItem);
		CMenu menu;

		int i = menu.LoadMenu(IDR_MENU1);

		// menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |
		// TPM_RIGHTBUTTON, point.x, point.y, this, NULL);

	}

	*pResult = 0;
}





void CTreeWindow::OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: 在此添加控件通知处理程序代码


	*pResult = 0;
}
