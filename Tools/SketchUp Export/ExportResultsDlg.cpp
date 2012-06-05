#include "stdafx.h"
#include "resource.h"
#include "ExportResultsDlg.h"

// CExportResultsDlg dialog

IMPLEMENT_DYNAMIC(CExportResultsDlg, CDialog)
CExportResultsDlg::CExportResultsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportResultsDlg::IDD, pParent)
{
    m_StatsMessage = _T("");
}

CExportResultsDlg::~CExportResultsDlg()
{
}

void CExportResultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_STATS_MESSAGE, m_StatsMessage);
}


BEGIN_MESSAGE_MAP(CExportResultsDlg, CDialog)
END_MESSAGE_MAP()

// CExportResultsDlg message handlers

void CExportResultsDlg::SetStats( const std::wstring& stats )
{
    CString exported;
    exported.LoadString(IDS_EXPORT_TITLE);

    
    m_StatsMessage = stats.c_str();
}
