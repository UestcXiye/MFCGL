

// MFCGLView.cpp: CMFCGLView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCGL.h"
#endif


#include "MFCGLView.h"

#include "MFCGLDoc.h"
#include "MFCGLView.h"

#include <iostream>
#include <string>

#include "CAddCubeDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCGLView

IMPLEMENT_DYNCREATE(CMFCGLView, CView)

BEGIN_MESSAGE_MAP(CMFCGLView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTON2, &CMFCGLView::OnAddCube)
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_UPDATEVIEW, &CMFCGLView::OnUpdateView)
	ON_MESSAGE(WM_CHANGECTRLER, &CMFCGLView::OnChangeController)
	ON_MESSAGE(WM_CLOSEDIALOG, &CMFCGLView::OnCloseDialog)
	ON_MESSAGE(WM_SELECTTREEITEM, &CMFCGLView::OnSelectTreeItem)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_BUTTON3, &CMFCGLView::OnOpenGraphCtrl)
	ON_COMMAND(ID_CTRL_OP, &CMFCGLView::OnOpenManipulator)
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()




float ambientLight1[] = { 0.1f,0.3f,0.8f,1.0f };  //环境光
float diffuseLight[] = { 0.25f,0.25f,0.25f,1.0f }; //散射光

float ambientLight2[] = { 0.1f,0.8f,0.3f,1.0f };  //环境光

float lightPosition[] = { 0.0f,0.2f,1.0f,0.0f }; //光源位置
//材质变量
float matAmbient[] = { 1.0f,1.0f,1.0f,1.0f };
float matDiff[] = { 1.0f,1.0f,1.0f,1.0f };
GLuint selectBuf[BUFSIZE];
CMFCGLView* CMFCGLView::p_View = nullptr;

// CMFCGLView 构造/析构
CMFCGLView::CMFCGLView() noexcept {
	// TODO: 在此处添加构造代码


	m_width = 1.0f;
	m_height = 1.0f;
	m_length = 1.0f;

	m_dragtype = 0;
	m_controllertype = 0;
	m_transform_method = 0;
	objClickedNum = -1;

	GLdouble m_lookAt[3] = { 1,1,1 };
	// GLdouble m_camTop[3];
	CalcLookVec(m_r, m_theta, m_phi);
	CalcCameraTopVec(m_r, m_theta, m_phi);

	GLdouble m_center[3] = { 0,0,0 };
	p_View = this;


}

CMFCGLView::~CMFCGLView() {}

BOOL CMFCGLView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCGLView 绘图

void CMFCGLView::OnDraw(CDC* /*pDC*/) {
	CMFCGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO:  在此处为本机数据添加绘制代码
	if (m_hglrc)
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hglrc);
	else
		return;

	ReDraw();

}

void CMFCGLView::OnRButtonUp(UINT /* nFlags */, CPoint point) {
	ClientToScreen(&point);
	OnContextMenu(this, point);

}

void CMFCGLView::OnContextMenu(CWnd* /* pWnd */, CPoint point) {
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->AddMenu(_T("IDR_MENU1"), IDR_MENU1);
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU1, point.x, point.y, this, TRUE);

#endif
}


// CMFCGLView 诊断

#ifdef _DEBUG
void CMFCGLView::AssertValid() const {
	CView::AssertValid();
}

void CMFCGLView::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}

CMFCGLDoc* CMFCGLView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCGLDoc)));
	return (CMFCGLDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCGLView 消息处理程序


void CMFCGLView::OnDestroy() {
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	::wglMakeCurrent(NULL, NULL);
	if (m_hglrc)
		::wglDeleteContext(m_hglrc);
	if (m_pDC)
		delete m_pDC;

}


int CMFCGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	int n;
	m_pDC = new CClientDC(this);
	ASSERT(m_pDC != NULL);
	//创建像素格式描述以匹配RC
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd  
		1,                              // version number  
		PFD_DRAW_TO_WINDOW |            // support window  
		PFD_SUPPORT_OPENGL |            // support OpenGL  
		PFD_DOUBLEBUFFER,                // double buffered  
		PFD_TYPE_RGBA,                  // RGBA type  
		24,                             // 24-bit color depth  
		0, 0, 0, 0, 0, 0,               // color bits ignored  
		0,                              // no alpha buffer  
		0,                              // shift bit ignored  
		0,                              // no accumulation buffer  
		0, 0, 0, 0,                     // accum bits ignored  
		16,                             // 16-bit z-buffer  
		0,                              // no stencil buffer  
		0,                              // no auxiliary buffer  
		PFD_MAIN_PLANE,                 // main layer  
		0,                              // reserved  
		0, 0, 0                         // layer masks ignored  

	};
	int m_nPixelFormat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (m_nPixelFormat == 0) {
		return FALSE;
	}

	if (::SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE) {
		return FALSE;
	}

	n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	//创建RC
	m_hglrc = ::wglCreateContext(m_pDC->GetSafeHdc());
	//创建RC失败
	if (m_hglrc == 0) {
		MessageBox(_T("Error Creating RC"));
		return FALSE;
	}
	//创建RC运行时
	if (::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hglrc) == FALSE) {
		MessageBox(_T("Error making RC Current"));
		return FALSE;
	}
	//白色为背景色  
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	return 0;
}


void CMFCGLView::OnSize(UINT nType, int cx, int cy) {
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_hglrc) {
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hglrc);
	} else {
		return;
	}

	if (0 == cy)         // 防止被零除
	{
		cy = 1;          // 将Height设为1
	}

	glViewport(0, 0, cx, cy);      // 重置当前的视口
	glMatrixMode(GL_PROJECTION);     // 选择投影矩阵
	glLoadIdentity();        // 重置投影矩阵

	// 设置视口的大小
	// gluPerspective(45.0f, (GLfloat)cx / (GLfloat)cy, 0.1f, 100.0f);

	glOrtho(-m_scale_factor * cx / cy, m_scale_factor * cx / cy, -m_scale_factor, m_scale_factor, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);      // 选择模型观察矩阵
	glLoadIdentity();        // 重置模型观察矩阵
	gluLookAt(m_eye[0], m_eye[1], m_eye[2], m_center[0], m_center[1], 0, m_camTop[0], m_camTop[1], m_camTop[2]);

}





BOOL CMFCGLView::DrawCubes(GLdouble length, GLdouble width, GLdouble height, GLdouble m_OriginX, GLdouble m_OriginY, GLdouble m_OriginZ, GLdouble m_xRads, GLdouble m_yRads, GLdouble m_zRads, int isChosen, int cubeid, glm::mat4 m_rotate) {

	{
		glShadeModel(GL_SMOOTH);     //使用平滑明暗处理
		glEnable(GL_DEPTH_TEST);     //剔除隐藏面
		glEnable(GL_CULL_FACE);      //不计算多边形背面
		glFrontFace(GL_CCW);      //多边形逆时针方向为正面
		glEnable(GL_LIGHTING);      //启用光照
		glEnable(GL_BLEND);
		//启用光0
		glEnable(GL_LIGHT0);
	}

	if (isChosen) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		//现在开始调协LIGHT0
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight2); //设置环境光分量
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight); //设置散射光分量
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); //设置光源在场景中的位置
		//启用光
		glEnable(GL_COLOR_MATERIAL);//材质跟踪当前绘图色
		glColor4d(0, 1, 0, 0.8);
	} else {
		//为LIGHT0设置析质
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		//现在开始调协LIGHT0
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight1); //设置环境光分量
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight); //设置散射光分量
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); //设置光源在场景中的位置
	}

	GLdouble tempM[16] = { m_rotate[0][0],m_rotate[0][1],m_rotate[0][2],m_rotate[0][3],
						m_rotate[1][0],m_rotate[1][1],m_rotate[1][2],m_rotate[1][3],
						m_rotate[2][0],m_rotate[2][1],m_rotate[2][2],m_rotate[2][3],
						m_rotate[3][0],m_rotate[3][1],m_rotate[3][2],m_rotate[3][3] };

	glPushMatrix();


	if (m_controllertype == 0) {
		glTranslated(m_OriginX, m_OriginY, m_OriginZ);
		glTranslated(-m_OriginX + m_op_loc[0], -m_OriginY + m_op_loc[1], -m_OriginZ + m_op_loc[2]);
		//4.旋转轴设定为现有坐标系，旋转
		glPushMatrix();
		glRotated(m_cube_rad[0] * 180 / MATHPI, op_coordX[0], op_coordX[1], op_coordX[2]);
		glRotated(m_cube_rad[1] * 180 / MATHPI, op_coordY[0], op_coordY[1], op_coordY[2]);
		glRotated(m_cube_rad[2] * 180 / MATHPI, op_coordZ[0], op_coordZ[1], op_coordZ[2]);
		//3.与坐标轴同步
		glPushMatrix();
		glTranslated(m_OriginX - m_op_loc[0], m_OriginY - m_op_loc[1], m_OriginZ - m_op_loc[2]);
		//2.然后旋转
		glPushMatrix();
		glMultMatrixd(tempM);
		//1.移动到原点先
		glPushMatrix();
		glTranslated(-m_OriginX, -m_OriginY, -m_OriginZ);

	} else {  //正方体不应该动
		glTranslated(m_OriginX, m_OriginY, m_OriginZ);
		glPushMatrix();

		glMultMatrixd(tempM);

		glPushMatrix();
		glTranslated(-m_OriginX, -m_OriginY, -m_OriginZ);

		glPushMatrix();
	}


	glPushName(S_CUBE + cubeid);

	glBegin(GL_QUADS);
	//视图的右侧
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3d(length + m_OriginX, width + m_OriginY, m_OriginZ);     //右上顶点
	glVertex3d(m_OriginX, width + m_OriginY, m_OriginZ);    //左上顶点
	glVertex3d(m_OriginX, width + m_OriginY, height + m_OriginZ);     //左下顶点
	glVertex3d(length + m_OriginX, width + m_OriginY, height + m_OriginZ);      //右下顶点
	//视图的左侧
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3d(length + m_OriginX, m_OriginY, height + m_OriginZ);     //右上顶点
	glVertex3d(m_OriginX, m_OriginY, height + m_OriginZ);    //左上顶点
	glVertex3d(m_OriginX, m_OriginY, m_OriginZ);   //左下顶点
	glVertex3d(length + m_OriginX, m_OriginY, m_OriginZ);    //右下顶点
	//前面
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3d(length + m_OriginX, width + m_OriginY, height + m_OriginZ);      //右上顶点
	glVertex3d(m_OriginX, width + m_OriginY, height + m_OriginZ);     //左上顶点
	glVertex3d(m_OriginX, m_OriginY, height + m_OriginZ);    //左下顶点
	glVertex3d(length + m_OriginX, m_OriginY, height + m_OriginZ);     //右下顶点
	//后面
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3d(length + m_OriginX, m_OriginY, m_OriginZ);    //右上顶点
	glVertex3d(m_OriginX, m_OriginY, m_OriginZ);   //左上顶点
	glVertex3d(m_OriginX, width + m_OriginY, m_OriginZ);    //左下顶点
	glVertex3d(length + m_OriginX, width + m_OriginY, m_OriginZ);     //右下顶点
	//左侧面
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3d(m_OriginX, width + m_OriginY, height + m_OriginZ);     //右上顶点
	glVertex3d(m_OriginX, width + m_OriginY, m_OriginZ);    //左上顶点
	glVertex3d(m_OriginX, m_OriginY, m_OriginZ);   //左下顶点
	glVertex3d(m_OriginX, m_OriginY, height + m_OriginZ);     //右下顶点
	//右侧面
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3d(length + m_OriginX, width + m_OriginY, m_OriginZ);     //右上顶点
	glVertex3d(length + m_OriginX, width + m_OriginY, height + m_OriginZ);      //左上顶点
	glVertex3d(length + m_OriginX, m_OriginY, height + m_OriginZ);     //左下顶点
	glVertex3d(length + m_OriginX, m_OriginY, m_OriginZ);    //右下顶点
	glEnd();

	glPopName();
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);

	return true;
}




BOOL CMFCGLView::Draw3DCoord() {
	// TODO: 在此处添加实现代码.
	//glDisable(GL_DEPTH_TEST);
	//绘制坐标轴

	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);

	glLineWidth(0.5);
	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 1000.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 1000.0, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(1000.0, 0.0, 0.0);
	glEnd();

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0x0f0f);
	glBegin(GL_LINE_STRIP);
	glColor3f(0, 0, 1);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(-100.0, 0.0, 0.0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(0, 1, 0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, -100.0, 0.0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, -100.0);
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	return true;
}

//画操作器，包含更新操作器图像
BOOL CMFCGLView::DrawOperator(GLdouble originX, GLdouble originY, GLdouble originZ, GLdouble m_xRads, GLdouble m_yRads, GLdouble m_zRads, glm::mat4 m_rotate) {
	// glDisable(GL_BLEND);
	GLdouble tempM[16] = { m_rotate[0][0],m_rotate[0][1],m_rotate[0][2],m_rotate[0][3],
					m_rotate[1][0],m_rotate[1][1],m_rotate[1][2],m_rotate[1][3],
					m_rotate[2][0],m_rotate[2][1],m_rotate[2][2],m_rotate[2][3],
					m_rotate[3][0],m_rotate[3][1],m_rotate[3][2],m_rotate[3][3] };

	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslated(originX, originY, originZ);

	glPushMatrix();
	glRotated(m_op_rad[0] * 180 / MATHPI, op_coordX[0], op_coordX[1], op_coordX[2]);
	glRotated(m_op_rad[1] * 180 / MATHPI, op_coordY[0], op_coordY[1], op_coordY[2]);
	glRotated(m_op_rad[2] * 180 / MATHPI, op_coordZ[0], op_coordZ[1], op_coordZ[2]);

	glPushMatrix();
	glMultMatrixd(tempM);

	glPushMatrix();
	// glTranslated(originX, originY, originZ);
	{
		GLUquadric* GLUq = gluNewQuadric();
		glColor4d(0.9, 0.4, 0.4, 1);

		//axis 1  arrow1   Z_Axis
		if (objClickedNum == S_AXIS + 1 || objClickedNum == S_ARROW + 1) {
			glColor4d(0.1, 0.1, 0.1, 1);
			joystickType = 1;
		} else {
			glColor4d(1.0, 0.4, 0.4, 1);
		}
		glPushName(S_AXIS + 1);
		gluCylinder(GLUq, 0.1, 0.1, 3, 20, 20);
		glPopName();


		glPushMatrix();
		glTranslated(0, 0, 3);
		glPushName(S_ARROW + 1);
		gluCylinder(GLUq, 0.2, 0, 0.8, 20, 20);
		glPopName();
		glPopMatrix();

		//axis 2 arrow 2 Y_Axis
		if (objClickedNum == S_AXIS + 2 || objClickedNum == S_ARROW + 2) {
			glColor4d(0.1, 0.1, 0.1, 1);
			joystickType = 2;

		} else {
			glColor4d(0.1, 0.9, 0.1, 1);
		}
		glPushMatrix();
		glRotated(90, -1, 0, 0);
		glPushName(S_AXIS + 2);
		gluCylinder(GLUq, 0.1, 0.1, 3, 20, 20);
		glPopName();

		glPushMatrix();
		glTranslated(0, 0, 3);
		glPushName(S_ARROW + 2);
		gluCylinder(GLUq, 0.2, 0, 0.8, 20, 20);
		glPopName();

		glPopMatrix();
		glPopMatrix();

		//axis 3 arrow 3 X_Axis
		if (objClickedNum == S_AXIS + 3 || objClickedNum == S_ARROW + 3) {
			glColor4d(0.1, 0.1, 0.1, 1);
			joystickType = 3;
		} else {
			glColor4d(0.2, 0.2, 0.9, 1);
		}
		glPushMatrix();
		glRotated(90, 0, 1, 0);
		glPushName(S_AXIS + 3);
		gluCylinder(GLUq, 0.1, 0.1, 3, 20, 20);
		glPopName();

		glPushMatrix();
		glTranslated(0, 0, 3);
		glPushName(S_ARROW + 3);
		gluCylinder(GLUq, 0.2, 0, 0.8, 20, 20);
		glPopName();

		glPopMatrix();
		glPopMatrix();

		//ball1 disk1 Y向
		if (objClickedNum == S_BALL + 1 || objClickedNum == S_DISK + 1) {
			glColor4d(0.1, 0.1, 0.1, 1);
			joystickType = 4;
		} else {
			glColor4d(0.2, 0.8, 0.1, 1);
		}
		glPushMatrix();
		glRotatef(45, 0, -1, 0);
		glTranslated(1.8, 0, 0);
		glPushName(S_BALL + 1);
		gluSphere(GLUq, 0.3, 20, 20);
		glPopName();
		glPopMatrix();

		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glPushName(S_DISK + 1);
		gluPartialDisk(GLUq, 1.7, 1.9, 20, 1, 0, 90);
		glPopName();
		glPopMatrix();

		//ball2 disk2 X向
		if (objClickedNum == S_BALL + 2 || objClickedNum == S_DISK + 2) {
			glColor4d(0.1, 0.1, 0.1, 1);
			joystickType = 5;
		} else {
			glColor4d(0.1, 0.1, 0.9, 1);
		}
		glPushMatrix();
		glRotatef(45, 1, 0, 0);
		glTranslated(0, 1.8, 0);
		glPushName(S_BALL + 2);
		gluSphere(GLUq, 0.3, 20, 20);
		glPopName();
		glPopMatrix();

		glPushMatrix();
		glRotated(90, 0, -1, 0);
		glPushName(S_DISK + 2);
		gluPartialDisk(GLUq, 1.7, 1.9, 20, 1, 0, 90);
		glPopName();
		glPopMatrix();

		//ball3 disk3 Z向
		if (objClickedNum == S_BALL + 3 || objClickedNum == S_DISK + 3) {
			glColor4d(0.1, 0.1, 0.1, 1);
			joystickType = 6;
		} else {
			glColor4d(0.8, 0.1, 0.1, 1);
		}
		glPushMatrix();
		glRotatef(45, 0, 0, 1);
		glTranslated(1.8, 0, 0);
		glPushName(S_BALL + 3);
		gluSphere(GLUq, 0.3, 20, 20);
		glPopName();
		glPopMatrix();

		glPushMatrix();
		glPushName(S_DISK + 3);
		gluPartialDisk(GLUq, 1.7, 1.9, 20, 1, 0, 90);
		glPopName();
		glPopMatrix();


	}
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	// axis in canonical coords
	glDisable(GL_COLOR_MATERIAL);
	return 0;
}


LRESULT CMFCGLView::OnUpdateView(WPARAM wParam, LPARAM lParam) {

	ReDraw();
	return 0;
}

BOOL CMFCGLView::ReDraw() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Draw3DCoord();
	//遍历立方体，以绘制立方体和操作轴
	int count = m_GraphList.GetCount();
	for (int i = 0; i < count; i++) {
		POSITION pos = m_GraphList.FindIndex(i);
		m_GraphObject tmp = m_GraphList.GetAt(pos);
		//如果已选中，则操作器需要绘制
		if (tmp.objectType == 0 && tmp.isChosen == 1 && OpIsExist == 1) {
			DrawOperator(
				// m_op_loc[0]+0.5*tmp.m_length,
				// m_op_loc[1]+0.5*tmp.m_width,
				// m_op_loc[2]+0.5*tmp.m_height,

				m_op_loc[0],
				m_op_loc[1],
				m_op_loc[2],

				m_op_rad[0],
				m_op_rad[1],
				m_op_rad[2],
				op_rotate
			);

			DrawCubes(
				tmp.m_length,
				tmp.m_width,
				tmp.m_height,

				tmp.m_tmpOriginPoint.x,
				tmp.m_tmpOriginPoint.y,
				tmp.m_tmpOriginPoint.z,


				0,
				0,
				0,
				// tmp.m_xRads,
				// tmp.m_yRads,
				// tmp.m_zRads,
				tmp.isChosen,
				i + 1,//cube序号从1开始,
				tmp.rotation
			);

		} else if (tmp.objectType == 0) {
			DrawCubes(
				tmp.m_length,
				tmp.m_width,
				tmp.m_height,

				tmp.m_tmpOriginPoint.x,
				tmp.m_tmpOriginPoint.y,
				tmp.m_tmpOriginPoint.z,

				tmp.m_xRads,
				tmp.m_yRads,
				tmp.m_zRads,
				tmp.isChosen,
				i + 1,//cube序号从1开始
				tmp.rotation
			);
		}
	}

	glLoadIdentity();
	CalcLookVec(m_r, m_theta, m_phi);
	CalcCameraTopVec(m_r, m_theta, m_phi);
	gluLookAt(m_eye[0], m_eye[1], m_eye[2], m_center[0], m_center[1], 0, m_camTop[0], m_camTop[1], m_camTop[2]);

	SwapBuffers(m_pDC->GetSafeHdc());
	return true;
}

BOOL CMFCGLView::CalcLookVec(GLdouble radius, GLdouble theta, GLdouble phi) {

	m_eye[0] = radius * cos(phi) * sin(theta) + m_center[0];
	m_eye[1] = radius * sin(phi) * sin(theta) + m_center[1];
	m_eye[2] = radius * cos(theta);

	return true;
}

BOOL CMFCGLView::CalcCameraTopVec(GLdouble radius, GLdouble theta, GLdouble phi) {

	if (theta <= MATHPI / 2) {
		m_camTop[0] = radius * cos(phi + MATHPI) * sin(MATHPI / 2 - theta);
		m_camTop[1] = radius * sin(phi + MATHPI) * sin(MATHPI / 2 - theta);
		m_camTop[2] = radius * cos(theta);
	} else {
		m_camTop[0] = radius * cos(phi) * sin(MATHPI - theta);
		m_camTop[1] = radius * sin(phi) * sin(MATHPI - theta);
		m_camTop[2] = radius * cos(MATHPI - theta);
	}

	return true;
}


BOOL CMFCGLView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_scale_factor -= 0.003 * zDelta;
	m_scale_factor < 5 ? m_scale_factor = 5 : m_scale_factor;

	//glLoadIdentity();
	CalcLookVec(m_r, m_theta, m_phi);
	CalcCameraTopVec(m_r, m_theta, m_phi);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	CRect tmp;
	GetClientRect(&tmp);

	//计算屏幕长宽比
	GLfloat aspect = (GLfloat)(tmp.right - tmp.left) / (GLfloat)(tmp.bottom - tmp.top);
	glOrtho(-m_scale_factor * aspect, m_scale_factor * aspect, -m_scale_factor, m_scale_factor, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_eye[0], m_eye[1], m_eye[2], m_center[0], m_center[1], 0, m_camTop[0], m_camTop[1], m_camTop[2]);

	ReDraw();

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMFCGLView::OnMouseMove(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (isClicked) {
		switch (m_dragtype) {
			//移动视角
		case 1:
		{
			m_ptEnd = point;
			GLdouble diffX = m_ptStart.x - m_ptEnd.x;
			GLdouble diffY = m_ptStart.y - m_ptEnd.y;

			m_theta = (m_theta + 0.005 * diffY);
			m_theta >= MATHPI ? m_theta = MATHPI : m_theta;
			m_theta <= 0 ? m_theta = 0 : m_theta;

			m_phi = (m_phi + 0.005 * diffX);
			m_phi > 2 * MATHPI ? m_phi -= 2 * MATHPI : m_phi;
			m_phi < 0 ? m_phi += 2 * MATHPI : m_phi;


			CalcLookVec(m_r, m_theta, m_phi);
			CalcCameraTopVec(m_r, m_theta, m_phi);
			glLoadIdentity();

			gluLookAt(m_eye[0], m_eye[1], m_eye[2], m_center[0], m_center[1], 0, m_camTop[0], m_camTop[1], m_camTop[2]);
			ReDraw();

			break;
		}
		//想要对立方体的位置做出调整
		case 2:
		{
			m_ptEnd = point;
			int count = m_GraphList.GetCount();

			for (int i = 0; i < count; ++i) {

				POSITION pos = m_GraphList.FindIndex(i);
				if (m_GraphList.GetAt(pos).isChosen) {
					tmp = &m_GraphList.GetAt(pos);
					break;
				}
			}

			if (objClickedNum != -1) {
				GLfloat offsetX = m_ptStart.x - point.x;
				GLfloat offsetY = -m_ptStart.y + point.y;
				TRACE("Offset: %f %f \n",offsetX,offsetY);
				switch (joystickType) {
					//Z
				case 1:
				{
					if (!m_controllertype) {
						tmp->m_tmpOriginPoint.x -= 0.03 * (offsetX * dragvector[2][0] + offsetY * dragvector[2][1]) * (op_coordZ[0]);
						tmp->m_tmpOriginPoint.y -= 0.03 * (offsetX * dragvector[2][0] + offsetY * dragvector[2][1]) * (op_coordZ[1]);
						tmp->m_tmpOriginPoint.z-= 0.03 * (offsetX * dragvector[2][0] + offsetY * dragvector[2][1]) * (op_coordZ[2]);
					}

					m_op_loc[0] -= 0.03 * (offsetX * dragvector[2][0] + offsetY * dragvector[2][1]) * (op_coordZ[0]);
					m_op_loc[1] -= 0.03 * (offsetX * dragvector[2][0] + offsetY * dragvector[2][1]) * (op_coordZ[1]);
					m_op_loc[2] -= 0.03 * (offsetX * dragvector[2][0] + offsetY * dragvector[2][1]) * (op_coordZ[2]);
					break;
				}
					//Y
				case 2:
				{
					if (!m_controllertype) {
						tmp->m_tmpOriginPoint.x -= 0.03 * (offsetX * dragvector[1][0] + offsetY * dragvector[1][1]) * (op_coordY[0]);
						tmp->m_tmpOriginPoint.y -= 0.03 * (offsetX * dragvector[1][0] + offsetY * dragvector[1][1]) * (op_coordY[1]);
						tmp->m_tmpOriginPoint.z -= 0.03 * (offsetX * dragvector[1][0] + offsetY * dragvector[1][1]) * (op_coordY[2]);
					}

					m_op_loc[0] -= 0.03 * (offsetX * dragvector[1][0] + offsetY * dragvector[1][1]) * (op_coordY[0]);
					m_op_loc[1] -= 0.03 * (offsetX * dragvector[1][0] + offsetY * dragvector[1][1]) * (op_coordY[1]);
					m_op_loc[2] -= 0.03 * (offsetX * dragvector[1][0] + offsetY * dragvector[1][1]) * (op_coordY[2]);
					break;
				}
				case 3:
				{
					if (!m_controllertype) {
						tmp->m_tmpOriginPoint.x -= 0.03 * (offsetX * dragvector[0][0] + offsetY * dragvector[0][1]) * (op_coordX[0]);
						tmp->m_tmpOriginPoint.y -= 0.03 * (offsetX * dragvector[0][0] + offsetY * dragvector[0][1]) * (op_coordX[1]);
						tmp->m_tmpOriginPoint.z -= 0.03 * (offsetX * dragvector[0][0] + offsetY * dragvector[0][1]) * (op_coordX[2]);

					}

					m_op_loc[0] -= 0.03 * (offsetX * dragvector[0][0] + offsetY * dragvector[0][1]) * (op_coordX[0]);
					m_op_loc[1] -= 0.03 * (offsetX * dragvector[0][0] + offsetY * dragvector[0][1]) * (op_coordX[1]);
					m_op_loc[2] -= 0.03 * (offsetX * dragvector[0][0] + offsetY * dragvector[0][1]) * (op_coordX[2]);
					break;
				}
				//Y轴旋转
				case 4:
				{
					if (!m_controllertype) {
						tmp->m_yRads -= 0.01 * (offsetX * dragvector[4][0] + offsetY * dragvector[4][1]);
						m_cube_rad[1] -= 0.01 * (offsetX * dragvector[4][0] + offsetY * dragvector[4][1]);
					}

					m_op_rad[1] -= 0.01 * (offsetX * dragvector[4][0] + offsetY * dragvector[4][1]);
					break;
				}
				//X轴旋转
				case 5:
				{
					if (!m_controllertype) {
						tmp->m_xRads -= 0.01 * (offsetX * dragvector[3][0] + offsetY * dragvector[3][1]);
						m_cube_rad[0] -= 0.01 * (offsetX * dragvector[3][0] + offsetY * dragvector[3][1]);
					}

					m_op_rad[0] -= 0.01 * (offsetX * dragvector[3][0] + offsetY * dragvector[3][1]);

					break;
				}
				//Z轴旋转
				case 6:
				{
					if (!m_controllertype) {
						tmp->m_zRads -= 0.01 * (offsetX * dragvector[5][0] + offsetY * dragvector[5][1]);
						m_cube_rad[2] -=  0.01 * (offsetX * dragvector[5][0] + offsetY * dragvector[5][1]);
					}

					m_op_rad[2] -=  0.01 * (offsetX * dragvector[5][0] + offsetY * dragvector[5][1]);
					break;
				}
				default:
				{
					break;
				}
				}
			}
			ReDraw();
			break;
		}

		}
	}

	
	m_ptStart = m_ptEnd;
	CView::OnMouseMove(nFlags, point);
}



//进入选择模式；
void CMFCGLView::OnLButtonDown(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	transformDragVector();
	isClicked = 1;
	m_ptStart = point;
	m_tmpPtStart = point;
	int hit;
	if (m_dragtype == 2 && OpIsExist && CTreeWindow::treeItemNum != -1) {


		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glSelectBuffer(BUFSIZE, selectBuf);
		glRenderMode(GL_SELECT);
		glInitNames();
		//计算屏幕长宽比
		CRect tmp;
		GetClientRect(&tmp);
		GLfloat aspect = (GLfloat)(tmp.right - tmp.left) / (GLfloat)(tmp.bottom - tmp.top);

		gluPickMatrix(point.x, viewport[3] - point.y, 1, 1, viewport);

		glOrtho(-m_scale_factor * aspect,
			m_scale_factor * aspect,
			-m_scale_factor,
			m_scale_factor,
			-100.0,
			100.0);
		gluLookAt(m_eye[0], m_eye[1], m_eye[2], m_center[0], m_center[1], 0, m_camTop[0], m_camTop[1], m_camTop[2]);

		ReDraw();
		SwapBuffers(m_pDC->GetSafeHdc());


		hit = glRenderMode(GL_RENDER);
		CString temp;
		temp.Format("hit is %d", hit);

		if (hit > 0) {
			ProcessHit(hit, selectBuf);
		} else {
			objClickedNum = -1;
		}
		ReDraw();
	}

	CView::OnLButtonDown(nFlags, point);

}

void CMFCGLView::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isClicked = 0;

	//初始化旋转矩阵
	{
		glm::mat4 op_rotateX = glm::mat4(1.0f);
		glm::mat4 op_rotateY = glm::mat4(1.0f);
		glm::mat4 op_rotateZ = glm::mat4(1.0f);
		glm::mat4 cube_rotateX = glm::mat4(1.0f);
		glm::mat4 cube_rotateY = glm::mat4(1.0f);
		glm::mat4 cube_rotateZ = glm::mat4(1.0f);
		glm::mat4 trans1 = glm::mat4(1.0f);
		glm::mat4 trans2 = glm::mat4(1.0f);

		glm::mat4 transform = glm::mat4(1.0f);
		//计算坐标系旋转矩阵。由于坐标系永远处于原点，不需计算位移。
		op_rotateX = glm::rotate(op_rotateX, (float)m_op_rad[0], glm::vec3(op_coordX.x, op_coordX.y, op_coordX.z));
		op_rotateY = glm::rotate(op_rotateY, (float)m_op_rad[1], glm::vec3(op_coordY.x, op_coordY.y, op_coordY.z));
		op_rotateZ = glm::rotate(op_rotateZ, (float)m_op_rad[2], glm::vec3(op_coordZ.x, op_coordZ.y, op_coordZ.z));


		//算轴
		op_rotate = op_rotateZ * op_rotateY * op_rotateX * op_rotate;

		op_coordX = op_rotate * coordX;
		op_coordY = op_rotate * coordY;
		op_coordZ = op_rotate * coordZ;

		//计算立方体的旋转矩阵，需要计算位移。
		cube_rotateX = glm::rotate(cube_rotateX, (float)m_cube_rad[0], glm::vec3(op_coordX[0], op_coordX[1], op_coordX[2]));
		cube_rotateY = glm::rotate(cube_rotateY, (float)m_cube_rad[1], glm::vec3(op_coordY[0], op_coordY[1], op_coordY[2]));
		cube_rotateZ = glm::rotate(cube_rotateZ, (float)m_cube_rad[2], glm::vec3(op_coordZ[0], op_coordZ[1], op_coordZ[2]));


		//计算新的xyz轴，便于平移使用
		// transform_matrix = transform* rotateZ * rotateY * rotateX * transform_matrix;


		//如果选中，且选定了物体，而不是单独坐标轴移动
		auto count = m_GraphList.GetCount();
		for (int i = 0; i < count; ++i) {
			POSITION pos = m_GraphList.FindIndex(i);
			// m_GraphList.GetAt(pos);
			if (m_GraphList.GetAt(pos).isChosen == 1 && m_controllertype != 1) {

				trans1 = glm::translate(trans1, glm::vec3(-m_op_loc[0], -m_op_loc[1], -m_op_loc[2]));
				trans2 = glm::translate(trans2, glm::vec3(m_op_loc[0], m_op_loc[1], m_op_loc[2]));
				// {
				// 	TRACE("操作器在 : %f %f %f\n", m_op_loc[0], m_op_loc[1], m_op_loc[2]);
				// 	TRACE("旋转轴X为：%f,%f,%f \n", op_coordX[0], op_coordX[1], op_coordX[2]);
				// 	TRACE("旋转轴Y为：%f,%f,%f \n", op_coordY[0], op_coordY[1], op_coordY[2]);
				// 	TRACE("旋转轴Z为：%f,%f,%f \n", op_coordZ[0], op_coordZ[1], op_coordZ[2]);
				//
				// 	TRACE("旋转矩阵X为：\n %f,%f,%f,%f\n %f,%f,%f,%f\n %f,%f,%f,%f\n %f,%f,%f,%f\n ",
				// 		cube_rotateX[0][0], cube_rotateX[0][1], cube_rotateX[0][2], cube_rotateX[0][3],
				// 		cube_rotateX[1][0], cube_rotateX[1][1], cube_rotateX[1][2], cube_rotateX[1][3],
				// 		cube_rotateX[2][0], cube_rotateX[2][1], cube_rotateX[2][2], cube_rotateX[2][3],
				// 		cube_rotateX[3][0], cube_rotateX[3][1], cube_rotateX[3][2], cube_rotateX[3][3]);
				// 	TRACE("旋转矩阵Y为：\n %f,%f,%f,%f\n %f,%f,%f,%f\n %f,%f,%f,%f\n %f,%f,%f,%f\n ",
				// 		cube_rotateY[0][0], cube_rotateY[0][1], cube_rotateY[0][2], cube_rotateY[0][3],
				// 		cube_rotateY[1][0], cube_rotateY[1][1], cube_rotateY[1][2], cube_rotateY[1][3],
				// 		cube_rotateY[2][0], cube_rotateY[2][1], cube_rotateY[2][2], cube_rotateY[2][3],
				// 		cube_rotateY[3][0], cube_rotateY[3][1], cube_rotateY[3][2], cube_rotateY[3][3]);
				// 	TRACE("旋转矩阵Z为：\n %f,%f,%f,%f\n %f,%f,%f,%f\n %f,%f,%f,%f\n %f,%f,%f,%f\n ",
				// 		cube_rotateZ[0][0], cube_rotateZ[0][1], cube_rotateZ[0][2], cube_rotateZ[0][3],
				// 		cube_rotateZ[1][0], cube_rotateZ[1][1], cube_rotateZ[1][2], cube_rotateZ[1][3],
				// 		cube_rotateZ[2][0], cube_rotateZ[2][1], cube_rotateZ[2][2], cube_rotateZ[2][3],
				// 		cube_rotateZ[3][0], cube_rotateZ[3][1], cube_rotateZ[3][2], cube_rotateZ[3][3]);
				// }
				m_GraphList.GetAt(pos).rotation = op_rotateZ * op_rotateY * op_rotateX * m_GraphList.GetAt(pos).rotation;

				m_GraphList.GetAt(pos).originPoint = trans2 * cube_rotateZ * cube_rotateY * cube_rotateX * trans1 * glm::vec4(
					m_GraphList.GetAt(pos).m_tmpOriginPoint.x,
					m_GraphList.GetAt(pos).m_tmpOriginPoint.y,
					m_GraphList.GetAt(pos).m_tmpOriginPoint.z, 1);


				m_GraphList.GetAt(pos).m_tmpOriginPoint.x = m_GraphList.GetAt(pos).originPoint.x;
				m_GraphList.GetAt(pos).m_tmpOriginPoint.y = m_GraphList.GetAt(pos).originPoint.y;
				m_GraphList.GetAt(pos).m_tmpOriginPoint.z = m_GraphList.GetAt(pos).originPoint.z;

				TRACE("临时位置在: %f %f %f\n", m_GraphList.GetAt(pos).m_tmpOriginPoint.x, m_GraphList.GetAt(pos).m_tmpOriginPoint.y, m_GraphList.GetAt(pos).m_tmpOriginPoint.z);
				TRACE("立方体位置在: %f %f %f\n", m_GraphList.GetAt(pos).originPoint.x, m_GraphList.GetAt(pos).originPoint.y, m_GraphList.GetAt(pos).originPoint.z);

				break;
			} else if (m_GraphList.GetAt(pos).isChosen == 1) {
				TRACE("立方体位置在: %f %f %f\n", m_GraphList.GetAt(pos).originPoint.x, m_GraphList.GetAt(pos).originPoint.y, m_GraphList.GetAt(pos).originPoint.z);
			}


		}

		TRACE("操作器在 : %f %f %f\n", m_op_loc[0], m_op_loc[1], m_op_loc[2]);


		//清空操作器角度/位移数组，便于下一次使用
		m_op_rad[0] = 0;
		m_op_rad[1] = 0;
		m_op_rad[2] = 0;

		m_cube_rad[0] = 0;
		m_cube_rad[1] = 0;
		m_cube_rad[2] = 0;


	}
	m_tmpPtStart = m_tmpPtEnd;
	CView::OnLButtonUp(nFlags, point);
}


void CMFCGLView::OnOpenGraphCtrl() {
	// TODO: 在此添加命令处理程序代码
	m_dragtype = 2;
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT_VALID(pWnd);    //定义父窗口指针pWnd
	m_GCtrl = new CGraphCtrlDialog(pWnd);
	m_GCtrl->Create(IDD_CTRLPANEL, pWnd);
	m_GCtrl->ShowWindow(SW_SHOW);
	ReDraw();
}
LRESULT CMFCGLView::OnChangeController(WPARAM wParam, LPARAM lParam) {
	m_controllertype = CGraphCtrlDialog::ctrltype;

	return 0;
}


LRESULT CMFCGLView::OnCloseDialog(WPARAM wParam, LPARAM lParam) {
	m_dragtype = 0;
	m_controllertype = 0;
	m_transform_method = 0;

	OpIsExist = 0;
	glLoadIdentity();
	gluLookAt(m_eye[0], m_eye[1], m_eye[2], m_center[0], m_center[1], 0, m_camTop[0], m_camTop[1], m_camTop[2]);
	ReDraw();
	return 0;
}
void CMFCGLView::OnAddCube() {
	// TODO: 在此添加命令处理程序代码
	CAddCubeDialog addCubeDlg;

	if (addCubeDlg.DoModal() == IDOK) {
		m_width = addCubeDlg.m_width;
		m_length = addCubeDlg.m_length;
		m_height = addCubeDlg.m_height;
	}
	m_GraphList.AddTail(m_GraphObject{ 0,0,m_width,m_height,m_length,glm::vec4(0,0,0,1),glm::vec4(0,0,0,1),0,0,0,0,glm::mat4(1.0f) });

	refreshTree();
	ReDraw();
}
//此函数为选中立方体之后执行。
LRESULT CMFCGLView::OnSelectTreeItem(WPARAM wParam, LPARAM lParam) {
	m_GraphObject tmp = m_GraphList.GetHead();
	int index = CTreeWindow::treeItemNum;
	int count = m_GraphList.GetCount();


	//刷新数据
	for (int i = 0; i < count; ++i) {
		POSITION pos = m_GraphList.FindIndex(i);
		m_GraphList.GetAt(pos).isChosen = 0;
	}
	if (index > 0) {

		POSITION pos = m_GraphList.FindIndex(index - 1);
		//TRACE("%s\r\n", (m_GraphList.GetAt(pos)));
		auto i = m_GraphList.GetAt(pos);
		m_GraphList.GetAt(pos).isChosen = 1;
		ReDraw();
	} else if (index == -1) {
		for (int i = 0; i < count; ++i) {
			POSITION pos = m_GraphList.FindIndex(i);
			m_GraphList.GetAt(pos).isChosen = 0;
		}
		OpIsExist = 0;
		ReDraw();
	}

	return 0;
}

void CMFCGLView::refreshTree() {
	CTreeWindow::m_pTreeWnd->m_cTreeCtrl.DeleteAllItems();
	CTreeWindow::RootItem = CTreeWindow::m_pTreeWnd->m_cTreeCtrl.InsertItem(_T("几何体"));

	int count = m_GraphList.GetCount();
	//树控件操作
	CString TreeStr;


	for (int i = 0; i < count; ++i) {
		TreeStr.Format(_T("立方体 %d"), i + 1);
		CTreeWindow::m_pTreeWnd->m_cTreeCtrl.InsertItem(TreeStr, CTreeWindow::RootItem);
	}

	CTreeWindow::m_pTreeWnd->m_cTreeCtrl.Expand(CTreeWindow::RootItem, TVE_EXPAND);
}


void CMFCGLView::OnOpenManipulator() {
	// TODO: 在此添加命令处理程序代码

	m_dragtype = 2;

	m_op_loc[0] = 0;
	m_op_loc[1] = 0;
	m_op_loc[2] = 0;

	m_op_rad[0] = 0;
	m_op_rad[1] = 0;
	m_op_rad[2] = 0;

	int chosenFlag = 0;
	int count = m_GraphList.GetCount();
	m_GraphObject tmp;
	for (int i = 0; i < count; ++i) {
		POSITION pos = m_GraphList.FindIndex(i);
		// m_GraphList.GetAt(pos);
		if (m_GraphList.GetAt(pos).isChosen == 1) {
			tmp = m_GraphList.GetAt(pos);
			chosenFlag = 1;
			break;
		}
	}
	if (chosenFlag) {
		OpIsExist = 1;
		m_op_loc[0] = tmp.originPoint.x;
		m_op_loc[1] = tmp.originPoint.y;
		m_op_loc[2] = tmp.originPoint.z;
		// m_op_rad[0] = tmp.m_xRads;
		// m_op_rad[1] = tmp.m_yRads;
		// m_op_rad[2] = tmp.m_zRads;

		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		ASSERT_VALID(pWnd);    //定义父窗口指针pWnd
		m_GCtrl = new CGraphCtrlDialog(pWnd);
		m_GCtrl->Create(IDD_CTRLPANEL, pWnd);
		m_GCtrl->ShowWindow(SW_SHOW);
		ReDraw();
	} else {
		AfxMessageBox("没有指定移动哪个立方体！");
	}

}


void CMFCGLView::OnMButtonUp(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isClicked = 0;

	if (OpIsExist) {
		m_dragtype = 2;
	} else {
		m_dragtype = 0;
	}

	CView::OnMButtonUp(nFlags, point);
}


void CMFCGLView::OnMButtonDown(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isClicked = 1;
	m_dragtype = 1;
	m_ptStart = point;
	CView::OnMButtonDown(nFlags, point);
}




void CMFCGLView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	transformDragVector();
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMFCGLView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//归一化二维前进向量
	GLdouble look_vec[3] = { m_center[0] - m_eye[0] ,m_center[1] - m_eye[1],m_center[2] - m_eye[2] };
	GLdouble norm_length = sqrt(pow(look_vec[0], 2) + pow(look_vec[1], 2));
	GLdouble norm_look_vec[2] = { look_vec[0] / norm_length,look_vec[1] / norm_length };

	// CalcCameraTopVec(m_r, m_theta, m_phi);
	// CalcLookVec(m_r, m_theta, m_phi);
	// norm_look_vec;
	switch (nChar) {
	case VK_LEFT:
	{
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		break;
	}
	case 0x41:
	{
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[1];
		m_center[1] += 0.1 * norm_look_vec[0];
		break;
	}

	case VK_RIGHT:
	{
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		break;
	}
	case 0x44:
	{
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[1];
		m_center[1] -= 0.1 * norm_look_vec[0];
		break;
	}

	case VK_UP:
	{
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		break;
	}
	case 0x57:
	{
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] += 0.1 * norm_look_vec[0];
		m_center[1] += 0.1 * norm_look_vec[1];
		break;
	}

	case VK_DOWN:
	{
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		break;

	}
	case 0x53:
	{
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		ReDraw();
		m_center[0] -= 0.1 * norm_look_vec[0];
		m_center[1] -= 0.1 * norm_look_vec[1];
		break;
	}
	}

	ReDraw();
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CMFCGLView::ProcessHit(int hit, GLuint* buf) {

	for (int i = 1; i <= hit; ++i) {
		GLuint nameNum = *buf;

		CString t1 = "";
		t1.Format("这次点击到的东西有 %d 个名字\n", *buf);
		// AfxMessageBox(t1);
		TRACE(t1);
		buf++;
		CString t2 = "";
		t2.Format("minZ is %g\n", (float)*buf / 0x7FFFFFFF);
		// AfxMessageBox(t2);
		TRACE(t2);
		buf++;

		CString t3 = "";
		t3.Format("maxZ is %g\n", (float)*buf / 0x7FFFFFFF);
		// AfxMessageBox(t3);
		TRACE(t3);
		buf++;


		for (int j = 0; j < (int)nameNum; ++j) {

			CString t4 = "";
			t4.Format("name is %d\n", *buf);
			// AfxMessageBox(t4);
			TRACE(t4);

			if (*buf > 63 && *buf < 75) {
				objClickedNum = *buf;
			}
			buf++;
		}

	}

}



void CMFCGLView::transformDragVector() {
	//方向向量
	GLdouble tmpdragvector[12][3];
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject(m_op_loc[0] + 3 * op_coordX[0], m_op_loc[1] + 3 * op_coordX[1], m_op_loc[2] + 3 * op_coordX[2], modelMatrix, projMatrix, viewport, &tmpdragvector[0][0], &tmpdragvector[0][1], &tmpdragvector[0][2]);
	gluProject(m_op_loc[0], m_op_loc[1], m_op_loc[2], modelMatrix, projMatrix, viewport, &tmpdragvector[1][0], &tmpdragvector[1][1], &tmpdragvector[1][2]);

	gluProject(m_op_loc[0] + 3 * op_coordY[0], m_op_loc[1] + 3 * op_coordY[1], m_op_loc[2] + 3 * op_coordY[2], modelMatrix, projMatrix, viewport, &tmpdragvector[2][0], &tmpdragvector[2][1], &tmpdragvector[2][2]);
	gluProject(m_op_loc[0], m_op_loc[1], m_op_loc[2], modelMatrix, projMatrix, viewport, &tmpdragvector[3][0], &tmpdragvector[3][1], &tmpdragvector[3][2]);

	gluProject(m_op_loc[0] + 3 * op_coordZ[0], m_op_loc[1] + 3 * op_coordZ[1], m_op_loc[2] + 3 * op_coordZ[2], modelMatrix, projMatrix, viewport, &tmpdragvector[4][0], &tmpdragvector[4][1], &tmpdragvector[4][2]);
	gluProject(m_op_loc[0], m_op_loc[1], m_op_loc[2], modelMatrix, projMatrix, viewport, &tmpdragvector[5][0], &tmpdragvector[5][1], &tmpdragvector[5][2]);

	gluProject(m_op_loc[0] + 3 * op_coordZ[0], m_op_loc[1] + 3 * op_coordZ[1], m_op_loc[2] + 3 * op_coordZ[2], modelMatrix, projMatrix, viewport, &tmpdragvector[6][0], &tmpdragvector[6][1], &tmpdragvector[6][2]);
	gluProject(m_op_loc[0] + 3 * op_coordY[0], m_op_loc[1] + 3 * op_coordY[1], m_op_loc[2] + 3 * op_coordY[2], modelMatrix, projMatrix, viewport, &tmpdragvector[7][0], &tmpdragvector[7][1], &tmpdragvector[7][2]);

	gluProject(m_op_loc[0] + 3 * op_coordX[0], m_op_loc[1] + 3 * op_coordX[1], m_op_loc[2] + 3 * op_coordX[2], modelMatrix, projMatrix, viewport, &tmpdragvector[8][0], &tmpdragvector[8][1], &tmpdragvector[8][2]);
	gluProject(m_op_loc[0] + 3 * op_coordZ[0], m_op_loc[1] + 3 * op_coordZ[1], m_op_loc[2] + 3 * op_coordZ[2], modelMatrix, projMatrix, viewport, &tmpdragvector[9][0], &tmpdragvector[9][1], &tmpdragvector[9][2]);

	gluProject(m_op_loc[0] + 3 * op_coordY[0], m_op_loc[1] + 3 * op_coordY[1], m_op_loc[2] + 3 * op_coordY[2], modelMatrix, projMatrix, viewport, &tmpdragvector[10][0], &tmpdragvector[10][1], &tmpdragvector[10][2]);
	gluProject(m_op_loc[0] + 3 * op_coordX[0], m_op_loc[1] + 3 * op_coordX[1], m_op_loc[2] + 3 * op_coordX[2], modelMatrix, projMatrix, viewport, &tmpdragvector[11][0], &tmpdragvector[11][1], &tmpdragvector[11][2]);

	dragvector[0][0] = tmpdragvector[0][0] - tmpdragvector[1][0];
	dragvector[0][1] = tmpdragvector[0][1] - tmpdragvector[1][1];

	dragvector[1][0] = tmpdragvector[2][0] - tmpdragvector[3][0];
	dragvector[1][1] = tmpdragvector[2][1] - tmpdragvector[3][1];

	dragvector[2][0] = tmpdragvector[4][0] - tmpdragvector[5][0];
	dragvector[2][1] = tmpdragvector[4][1] - tmpdragvector[5][1];

	dragvector[3][0] = tmpdragvector[6][0] - tmpdragvector[7][0];
	dragvector[3][1] = tmpdragvector[6][1] - tmpdragvector[7][1];

	dragvector[4][0] = tmpdragvector[8][0] - tmpdragvector[9][0];
	dragvector[4][1] = tmpdragvector[8][1] - tmpdragvector[9][1];

	dragvector[5][0] = tmpdragvector[10][0] - tmpdragvector[11][0];
	dragvector[5][1] = tmpdragvector[10][1] - tmpdragvector[11][1];

	//归一化
	for (int i = 0; i < 6; ++i) {
		GLdouble len = sqrt(pow(dragvector[i][0], 2) + pow(dragvector[i][1], 2));
		dragvector[i][0] = dragvector[i][0] / len;
		dragvector[i][1] = dragvector[i][1] / len;
		TRACE("dragvector %d 是 %f, %f\n", i, dragvector[i][0], dragvector[i][1]);
	}

}


