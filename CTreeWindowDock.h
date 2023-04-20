#pragma once
#include <afxdockablepane.h>
#include "CTreeWindow.h"

class CTreeWindowDock :
    public CDockablePane
{
public:
    CTreeWindow m_TreeBaseWind;



    DECLARE_MESSAGE_MAP()
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
};

