#pragma once
#include "afxdialogex.h"
#include "MainFrm.h"


// CGraphCtrlDialog 对话框

class CGraphCtrlDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGraphCtrlDialog)

public:
	CGraphCtrlDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGraphCtrlDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTRLPANEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
		virtual void PostNcDestroy();
public:
	int m_controller;
	int m_axis;

	//0：不做动作  1：移动视角  2：移动物体
	int m_dragtype;

	CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView* actV = pMF->GetActiveView();
	
	static int ctrltype;


	static double x_offset;
	static double y_offset;
	static double z_offset;


	static CGraphCtrlDialog *m_pGDialog;

	afx_msg void OnBnClickedRadioCtrlGroup();
	afx_msg void OnBnClickedRadioAxisGroup();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();

	afx_msg void OnOpenByMenu();
	afx_msg void OnBnClickedDragGroup();

		afx_msg LRESULT OnChangeData(WPARAM wParam, LPARAM lParam);

};
