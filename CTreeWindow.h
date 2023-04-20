#pragma once
#include "afxdialogex.h"

#include "resource.h"

// CTreeWindow 对话框
#define WM_SELECTTREEITEM WM_USER+104
#define WM_UPDATEVIEW WM_USER+100
#define WM_TREESELCHANGE WM_USER+108

class CTreeWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CTreeWindow)

public:
	CTreeWindow(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTreeWindow();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TREEDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_cTreeCtrl;
	virtual BOOL OnInitDialog();

	static CTreeWindow* m_pTreeWnd;
	static HTREEITEM RootItem;
	static int treeItemNum;





	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
};
