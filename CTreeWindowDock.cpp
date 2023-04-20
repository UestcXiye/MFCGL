#include "pch.h"
#include "CTreeWindowDock.h"

#include "resource.h"
BEGIN_MESSAGE_MAP(CTreeWindowDock, CDockablePane)
		ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


int CTreeWindowDock::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	//ģ�������ڴ���
	if (!m_TreeBaseWind.Create(IDD_TREEDIALOG, this))
	{
		TRACE0("δ�ܴ���ģ��������!");
		return -1;
	}
	m_TreeBaseWind.ShowWindow(SW_SHOW);

	return 0;
}


void CTreeWindowDock::OnSize(UINT nType, int cx, int cy) {
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
		if (m_TreeBaseWind.GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		m_TreeBaseWind.MoveWindow(rect);
	}
}


void CTreeWindowDock::OnDestroy() {
	CDockablePane::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	m_TreeBaseWind.DestroyWindow();
}
