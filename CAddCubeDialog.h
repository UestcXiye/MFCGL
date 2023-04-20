#pragma once


#include <GL/gl.h>


#include "afxdialogex.h"
#include "MainFrm.h"


// CAddCubeDialog 对话框

class CAddCubeDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddCubeDialog)

public:
	CAddCubeDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAddCubeDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDCUBE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	GLfloat m_length;
	GLfloat m_width;
	GLfloat m_height;
	
	virtual BOOL OnInitDialog();
};
