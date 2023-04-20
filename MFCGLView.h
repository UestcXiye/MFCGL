

// MFCGLView.h: CMFCGLView 类的接口
//

#pragma once

#include <GL/glut.h>
#include <GL/GL.h>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CGraphCtrlDialog.h"
#include "MFCGLDoc.h"

#define MATHPI 3.14159265358979323846f
#define WM_UPDATEVIEW WM_USER+100
#define WM_CHANGECTRLER WM_USER+101
#define WM_CHANGEAXIS WM_USER+102
#define WM_CLOSEDIALOG WM_USER+103
#define WM_SELECTTREEITEM WM_USER+104
#define WM_CHANGEDRAG WM_USER+106
#define WM_CHANGEDATA WM_USER+107

#define WM_TREESELCHANGE WM_USER+108
//S for Select
#define S_AXIS 63
#define S_ARROW 66
#define S_BALL 69
#define S_DISK 72
#define S_CUBE 0

#define BUFSIZE 512
class CMFCGLView : public CView
{
protected: // 仅从序列化创建
	CMFCGLView() noexcept;
	DECLARE_DYNCREATE(CMFCGLView)


public:
	CClientDC* m_pDC;
	HGLRC m_hglrc;
	CGraphCtrlDialog* m_GCtrl;

	// 特性
public:
	CMFCGLDoc* GetDocument() const;

public:
	struct m_GraphObject
	{
		//0：立方体，1：待定
		int objectType;
		//物体属性
		GLdouble radius;

		GLdouble m_width;
		GLdouble m_height;
		GLdouble m_length;

		//物体位置，用于LButtonUp
		glm::vec4 originPoint = { 0,0,0,1 };

		//上一次移动前的物体位置，用于MouseMove
		glm::vec4 m_tmpOriginPoint = { 0,0,0,1 };
		
		//旋转系数
		GLdouble m_xRads;
		GLdouble m_yRads;
		GLdouble m_zRads;

		int isChosen = 0;

		glm::mat4 rotation = glm::mat4(1.0f);
	};

	CList<m_GraphObject, m_GraphObject> m_GraphList;


	//0：无  1：鼠标中键拖视角  2：对单个立方体操作(包含拾取)
	int m_dragtype;

	//0：移动物体  1：移动坐标轴
	int m_controllertype;


	//0：坐标轴  1：物体
	int m_transform_method;

	//1-3:x/y/z平移  4-6:x/y/z旋转
	int joystickType;

	//鼠标是否按下
	int isClicked;
	//操作器是否存在
	int OpIsExist;
	//-1:未选中
	int objClickedNum;



	m_GraphObject* tmp;

	//缩放因子
	GLdouble m_scale_factor = 10;


	//视角的位置
	GLdouble m_width;
	GLdouble m_height;
	GLdouble m_length;
	//视角的球面坐标
	GLdouble m_r = 10;
	GLdouble m_theta = MATHPI / 4;
	GLdouble m_phi = MATHPI / 4;
	//转化后的相机向量
	GLdouble m_eye[3];
	GLdouble m_camTop[3];
	GLdouble m_center[3] = { 0.0,0.0,0.0 };


	CPoint m_ptEnd;
	CPoint m_ptStart;


	CPoint m_tmpPtStart;
	CPoint m_tmpPtEnd;

	//轴向量
	const glm::vec4 coordX = { 1.0,0.0,0.0,0.0 };
	const glm::vec4 coordY = { 0.0,1.0,0.0 ,0.0 };
	const glm::vec4 coordZ = { 0.0,0.0,1.0,0.0 };

	//操作器的向量坐标
	glm::vec4 op_coordX = { 1.0,0.0,0.0, 0.0 };
	glm::vec4 op_coordY = { 0.0,1.0,0.0, 0.0 };
	glm::vec4 op_coordZ = { 0.0,0.0,1.0,0.0 };



	//操作器的旋转矩阵
	glm::mat4 op_rotate = glm::mat4(1.0f);

	//操作器的临时位置/角度信息，将在下一次鼠标抬起时重新赋值。
	GLdouble m_op_loc[3] = { 0,0,0 };
	GLdouble m_op_rad[3] = { 0,0,0 };

	//立方体的临时位置调整信息
	GLdouble m_cube_rad[3] = { 0,0,0 };

	//神奇操作
	GLdouble dragvector[6][2];


	// 操作
public:

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	// 实现
public:
	virtual ~CMFCGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	static CMFCGLView* p_View;


	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	BOOL CMFCGLView::ReDraw();

	BOOL DrawCubes(GLdouble length, GLdouble width, GLdouble height, GLdouble m_OriginX, GLdouble m_OriginY, GLdouble m_OriginZ, GLdouble
		m_xRads, GLdouble m_yRads, GLdouble m_zRads, int isChosen, int cubeid, glm::mat4 m_rotate);
	BOOL Draw3DCoord();
	BOOL DrawOperator(GLdouble originX, GLdouble originY, GLdouble originZ, GLdouble m_xRads, GLdouble m_yRads, GLdouble m_zRads, glm::mat4 m_rotate);

	afx_msg void OnAddCube();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	//自定义消息函数
	afx_msg LRESULT OnUpdateView(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeController(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeAxis(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloseDialog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectTreeItem(WPARAM wParam, LPARAM lParam);


	BOOL CalcLookVec(GLdouble radius, GLdouble theta, GLdouble phi);
	BOOL CalcCameraTopVec(GLdouble radius, GLdouble theta, GLdouble phi);

	//WM消息响应函数
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOpenGraphCtrl();
	afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	void refreshTree();

	afx_msg void OnOpenManipulator();
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	void ProcessHit(int hit, GLuint* buf);
	void transformDragVector();
};



#ifndef _DEBUG  // MFCGLView.cpp 中的调试版本
inline CMFCGLDoc* CMFCGLView::GetDocument() const {
	return reinterpret_cast<CMFCGLDoc*>(m_pDocument);
}
#endif

