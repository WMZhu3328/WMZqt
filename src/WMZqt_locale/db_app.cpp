
#include "db_app.h"
#include "WMZqt/app.h"
#include "WMZqt/threaded/log.h"

DBApp* DBApp::m_spInstance = NULL;

DBApp::DBApp()
	:DBAppBase()
{
}

DBApp::~DBApp()
{
	this->Close();
}

int32_t	DBApp::Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword)
{
	int32_t	nErrno = WMZqt::E_Errno_ERR_GENERAL;

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Info,
		"DBApp::Open(\"%s\") begin ...\n",
		qstrDBName.toStdString().c_str());

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		nErrno = DBAppBase::Open(qstrDBHome, qstrDBName, qstrPassword);
		if (nErrno != WMZqt::E_Errno_SUCCESS)
		{
			break;
		}

		nErrno = WMZqt::E_Errno_SUCCESS;
	}
	if (nErrno == WMZqt::E_Errno_SUCCESS)
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Info,
			"DBApp::Open(\"%s\") successfully .\n",
			qstrDBName.toStdString().c_str());
	}
	else
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn,
			"DBApp::Open(\"%s\") failed !!!\n",
			qstrDBName.toStdString().c_str());

		this->Close();
	}

	return nErrno;
}

int32_t	DBApp::Close()
{
	int32_t	nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

	if (this->IsOpen())
	{
		QString	qstrDBName	=	this->m_qstrDBName;

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Info,
			"DBApp::Close(\"%s\") begin ...\n",
			qstrDBName.toStdString().c_str());

		for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
		{
			nErrno = DBAppBase::Close();
			if (nErrno != WMZqt::E_Errno_SUCCESS)
			{
				break;
			}

			nErrno = WMZqt::E_Errno_SUCCESS;
		}
		if (nErrno == WMZqt::E_Errno_SUCCESS)
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Info,
				"DBApp::Close(\"%s\") successfully !!!\n",
				qstrDBName.toStdString().c_str());
		}
		else
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn,
				"DBApp::Close(\"%s\") failed !!!\n",
				qstrDBName.toStdString().c_str());

			this->Close();
		}
	}

	return nErrno;
}

