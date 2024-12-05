#include <afxwin.h>
#include <afxext.h>
#include "resource.h"
class CMyView : public CView {
public:
	void OnDraw(CDC* pDC);
};
void CMyView::OnDraw(CDC* pDC) {
	pDC->TextOut(100,100,"Hello, World!");
}
class CMyFrameWnd : public CFrameWnd {
	DECLARE_MESSAGE_MAP()
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnNew();
	void OnQuit();
	void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	void OnContextMenu(CWnd* pWnd, CPoint point);
	void OnPaint();
	void OnRed();
public:
	CToolBar toolBar;
	CMenu menu;
};
void CMyFrameWnd::OnPaint() {
	
}
void CMyFrameWnd::OnRed() {
	AfxMessageBox("Red Color");
}
void CMyFrameWnd::OnContextMenu(CWnd* pWnd, CPoint point) {
	CMenu* pPopup = menu.GetSubMenu(0);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}
void CMyFrameWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) {
	pPopupMenu->CheckMenuItem(ID_NEW, MF_CHECKED);
}
void CMyFrameWnd::OnQuit() {
	::PostQuitMessage(0);
}
void CMyFrameWnd::OnNew() {
	AfxMessageBox("New File");
}
int CMyFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CMyView* pView = new CMyView;
	pView->Create(NULL,"MFC View",WS_CHILD|WS_VISIBLE,CRect(0,0,500,200),this,AFX_IDW_PANE_FIRST);
	menu.LoadMenu(IDR_MENU1);
	this->SetMenu(&menu);           //将对象与菜单句柄关联
	toolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_GRIPPER );
	toolBar.LoadToolBar(IDR_TOOLBAR1);
	toolBar.EnableDocking(CBRS_ALIGN_ANY);
	this->EnableDocking(CBRS_ALIGN_ANY);
	this->DockControlBar(&toolBar, AFX_IDW_DOCKBAR_BOTTOM);
	return CFrameWnd::OnCreate(lpCreateStruct);
}
BEGIN_MESSAGE_MAP(CMyFrameWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_NEW,OnNew)
	ON_COMMAND(ID_QUIT,OnQuit)
	ON_COMMAND(ID_RED,OnRed)
END_MESSAGE_MAP()
class CMyApp : public CWinApp {
public:
	virtual BOOL InitInstance();	
};

CMyApp theApp;//全局应用程序对象

BOOL CMyApp::InitInstance() {
	CMyFrameWnd* pFrame = new CMyFrameWnd;
	this->m_pMainWnd = pFrame;
	pFrame->Create(NULL, "MFC Application");
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}
