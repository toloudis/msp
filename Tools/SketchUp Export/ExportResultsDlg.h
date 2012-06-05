#pragma once


// CExportResultsDlg dialog
class CExportResultsDlg : public CDialog
{
	DECLARE_DYNAMIC(CExportResultsDlg)

public:
	CExportResultsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExportResultsDlg();

    // Dialog Data
	enum { IDD = IDD_DIALOG_EXPORT_RESULTS };

	void SetStats( const std::wstring& stats );

protected:

    CString	m_StatsMessage;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
