// Graph.cpp : implementation file
//

#include "stdafx.h"
#include "Demo1.h"
#include "Graph.h"
#include "mpc2810.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraph dialog

CGraph::CGraph(CWnd* pParent /*=NULL*/)
	: CDialog(CGraph::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGraph)
	m_sMsgPos = _T("");
	//}}AFX_DATA_INIT
	m_lfFactor=0.01;
	m_iLastPosX=0;
	m_iLastPosY=0;
	m_bInitOK=FALSE;
}
CGraph::~CGraph()
{
}

void CGraph::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraph)
	DDX_Text(pDX, IDC_STATIC_MSGPOS, m_sMsgPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraph, CDialog)
	//{{AFX_MSG_MAP(CGraph)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraph message handlers

/////////////////////////
//画坐标轴
void CGraph::DrawAxes()
{
	CDC *pDc;
	CPen cPen,*cPenOld;
	RECT tWinRect;

	GetClientRect(&tWinRect);

	pDc=GetDC();

	cPen.CreatePen(PS_SOLID,PS_GEOMETRIC,RGB(255,0,0));
	cPenOld=pDc->SelectObject(&cPen);
	
	pDc->MoveTo(tWinRect.left,tWinRect.bottom/2);
	pDc->LineTo(tWinRect.right,tWinRect.bottom/2);
	pDc->MoveTo(tWinRect.right/2,tWinRect.top);
	pDc->LineTo(tWinRect.right/2,tWinRect.bottom);

	pDc->SelectObject(cPenOld);

	ReleaseDC(pDc);
}
/////////////////////////
//清除图形显示窗口内容
void CGraph::ClearAll()
{
	Invalidate(TRUE);
}
///////////////////////////////////
//将当前画图位置移到（x,y）点
void CGraph::MoveToPos(long x,long y)
{
	m_iLastPosX=(int)(x*m_lfFactor+m_CenX);
	m_iLastPosY=(int)(-y*m_lfFactor+m_CenY);

	m_sMsgPos.Format("X: %d  Y: %d",x,y);
	UpdateData(FALSE);
}
///////////////////////////////////
//从当前画图位置画线到（x,y）点
void CGraph::LineToPos(long x,long y)
{
	CString cStr;

	CDC *pDc;
	pDc=GetDC();
	pDc->MoveTo(m_iLastPosX,m_iLastPosY);
	m_iLastPosX=(int)(x*m_lfFactor+m_CenX);
	m_iLastPosY=(int)(-y*m_lfFactor+m_CenY);
	pDc->LineTo(m_iLastPosX,m_iLastPosY);

	m_sMsgPos.Format("X: %d  Y: %d",x,y);
	UpdateData(FALSE);
	ReleaseDC(pDc);
}

void CGraph::OnCancel() 
{
	m_bShowGraph=FALSE;
	
	CDialog::OnCancel();
}
void CGraph::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawAxes();
	// Do not call CDialog::OnPaint() for painting messages
}

void CGraph::OnBtnClear() 
{
	// TODO: Add your control notification handler code here
	ClearAll();
	DrawAxes();
}

BOOL CGraph::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	///////////////////////////
	//计算坐标原点
	RECT tWinRect;
	GetClientRect(&tWinRect);
	m_CenX=tWinRect.right/2;
	m_CenY=tWinRect.bottom/2;

	m_iLastPosX=m_CenX;
	m_iLastPosY=m_CenY;
	
	///////////////////////////
	//画坐标线
	DrawAxes();
	m_bInitOK=TRUE;

	//////////////////////////
	//初始化绘图比例
	GetDlgItem(IDC_EDIT_FACTOR)->SetWindowText("0.01");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

