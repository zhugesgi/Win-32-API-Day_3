#include<afxwin.h>
#include<afxext.h>
#include"resource.h"
class CMyDoc :public CDocument {

};





class CMyView :public CView {
	DECLARE_DYNCREATE(CMyView)
public:
	virtual void OnDraw(CDC* pDC);
};
IMPLEMENT_DYNCREATE(CMyView, CView)
void CMyView::OnDraw(CDC* pDC) {
}





class CMyFrame :public CFrameWnd {
	DECLARE_MESSAGE_MAP();
public:
};
BEGIN_MESSAGE_MAP(CMyFrame, CFrameWnd)
END_MESSAGE_MAP()





class CMyApp :public CWinApp {
public:
	virtual BOOL InitInstance();
};




BOOL CMyApp::InitInstance() {
	CMyFrame* pFrame = new CMyFrame;
	CMyDoc* pDoc = new CMyDoc;


	CCreateContext cct;
	cct.m_pCurrentDoc = pDoc;//文档类对象的地址
	cct.m_pNewViewClass = RUNTIME_CLASS(CMyView);


	pFrame->LoadFrame(IDR_MENU1, WS_OVERLAPPEDWINDOW, NULL, &cct);
	this->m_pMainWnd = pFrame;
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}
CMyApp theApp;