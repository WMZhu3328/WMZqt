
#include "WMZqt/db_app_base.h"
#include "WMZqt/threaded/log.h"

WMZqt::DBAppBase::DBAppBase()
{
}

WMZqt::DBAppBase::~DBAppBase()
{
	this->Close();
}

int32_t	WMZqt::DBAppBase::Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword)
{
	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Trace,
		"WMZqt::DBAppBase::Open(\"%s\") available SQL drivers:\n",
		qstrDBName.toStdString().c_str());
	QStringList	listDrivers = QSqlDatabase::drivers();
	QStringList::ConstIterator	iter;
	for (iter = listDrivers.begin(); iter != listDrivers.end(); ++iter)
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Trace,
			"\t%s\n",
			iter->toStdString().c_str());
	}
	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Trace,
		"\n");

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::DBAppBase::Open(\"%s\") begin ...\n",
		qstrDBName.toStdString().c_str());

	// check parameters
	if (qstrDBName.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::DBAppBase::Open(\"%s\") failed: qstrAppName[\"\"] cannot be EMPTY !!!\n",
			qstrDBName.toStdString().c_str());

		return WMZqt::E_Errno_ERR_INVALID_PARAMS;
	}

	// close first if app database already opened
	if (this->IsOpen())
	{
		this->Close();
	}

	int32_t	nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QString	qstrFilePathDBApp;
		if (!qstrDBHome.isEmpty())
		{
			qstrFilePathDBApp	= qstrDBHome + QDir::separator() + qstrDBName + ".db";
		}
		else
		{
			qstrFilePathDBApp	=	QString(".") + QDir::separator() + qstrDBName + ".db";
		}
		this->m_qstrDBName	= qstrDBName;

		// open app database
		QSqlDatabase	dbApp = QSqlDatabase::addDatabase("QSQLITE", this->m_qstrDBName);
		dbApp.setDatabaseName(qstrFilePathDBApp);
		if (!qstrPassword.isEmpty())
		{
			dbApp.setPassword(qstrPassword);
		}
		if (!dbApp.open())
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::DBAppBase::Open(\"%s\") failed: failed to open app database[\"%s\"] !!!\n",
				this->m_qstrDBName.toStdString().c_str(),
				qstrFilePathDBApp.toStdString().c_str());

			nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

			break;
		}

		// open config table, create if not exists
		nErrno	=	this->OpenTableCfg();
		if (nErrno != WMZqt::E_Errno_SUCCESS)
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::DBAppBase::Open(\"%s\") failed: failed to create table[\"tbl_cfg\"] !!!\n",
				this->m_qstrDBName.toStdString().c_str());

			//nErrno	=	WMZqt::E_Errno_ERR_GENERAL;
			break;
		}

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::DBAppBase::Open(\"%s\") successfully .\n",
			this->m_qstrDBName.toStdString().c_str());

		nErrno	=	WMZqt::E_Errno_SUCCESS;
	}
	if (nErrno != WMZqt::E_Errno_SUCCESS)
	{
		this->Close();
	}

	return nErrno;
}

int32_t	WMZqt::DBAppBase::Close()
{
	if (!this->m_qstrDBName.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::DBAppBase::Close(\"%s\") begin ...\n",
			this->m_qstrDBName.toStdString().c_str());

		QSqlDatabase	dbApp	=	QSqlDatabase::database(this->m_qstrDBName);
		if (dbApp.isOpen())
		{
			QSqlDatabase::removeDatabase(this->m_qstrDBName);
		}

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::DBAppBase::Close(\"%s\") successfully !!!\n",
			this->m_qstrDBName.toStdString().c_str());

		this->m_qstrDBName.clear();
	}

	return WMZqt::E_Errno_SUCCESS;
}

bool	WMZqt::DBAppBase::IsOpen()
{
	if (this->m_qstrDBName.isEmpty())
	{
		return false;
	}

	return QSqlDatabase::database(this->m_qstrDBName).isOpen();
}

int32_t	WMZqt::DBAppBase::SetCfgItem(const QString& qstrName, const QString& qstrValue)
{
	QString		qstrStmt_get	=
		QString("SELECT value FROM tbl_cfg WHERE name='") + qstrName + "'";
	QSqlQuery	query_get(QSqlDatabase::database(this->m_qstrDBName));
	if (!query_get.exec(qstrStmt_get))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::DBAppBase::SetCfgItem(\"%s\", \"%s\") failed: failed to execute SQL[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrName.toStdString().c_str(), qstrValue.toStdString().c_str(),
			qstrStmt_get.toStdString().c_str(),
			query_get.lastError().text().toStdString().c_str());

		return WMZqt::E_Errno_ERR_GENERAL;
	}

	QString	qstrStmt_set;
	if (query_get.next())
	{ // config item already set, update
		qstrStmt_set = QString("UPDATE tbl_cfg SET value='") + qstrValue + "' WHERE name='" + qstrName + "'";
	}
	else
	{ // config item not set, add
		qstrStmt_set = QString("INSERT INTO tbl_cfg(name, value) VALUES ('") + qstrName + "', '" + qstrValue + "')";
	}
	QSqlQuery	query_set(QSqlDatabase::database(this->m_qstrDBName));
	if (!query_set.exec(qstrStmt_set))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::DBAppBase::SetCfgItem(\"%s\", \"%s\") failed: failed to execute SQL[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrName.toStdString().c_str(), qstrValue.toStdString().c_str(),
			qstrStmt_get.toStdString().c_str(),
			query_get.lastError().text().toStdString().c_str());

		return WMZqt::E_Errno_ERR_GENERAL;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::DBAppBase::SetCfgItem(\"%s\", \"%s\") successfully.\n",
		qstrName.toStdString().c_str(), qstrValue.toStdString().c_str());

	return WMZqt::E_Errno_SUCCESS;
}

QString	WMZqt::DBAppBase::GetCfgItem(const QString& qstrName, const QString& qstrDftValue)
{
	QString	qstrValue;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QString		qstrStmt =
			QString("SELECT value FROM tbl_cfg WHERE name = '") + qstrName + "'";
		QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
		if (!query.exec(qstrStmt))
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::DBAppBase::GetCfgItem(\"%s\") failed: failed to execute SQL[\"%s\"], ERROR message[\"%s\"], return default value[\"%s\"] directly !!!\n",
				qstrName.toStdString().c_str(),
				qstrStmt.toStdString().c_str(),
				query.lastError().text().toStdString().c_str(),
				qstrDftValue.toStdString().c_str());

			break;
		}

		if (query.next())
		{
			qstrValue	=	query.value(0).toString();

			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
				"WMZqt::DBAppBase::GetCfgItem(\"%s\") successfully with value[\"%s\"].\n",
				qstrName.toStdString().c_str(), qstrValue.toStdString().c_str());
		}
		else if (!qstrDftValue.isEmpty())
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn,
				"WMZqt::DBAppBase::GetCfgItem(\"%s\") : NONE config item queried, set and return default value[\"%s\"] !!!\n",
				qstrName.toStdString().c_str(),
				qstrDftValue.toStdString().c_str());

			qstrValue	=	qstrDftValue;
			this->SetCfgItem(qstrName, qstrDftValue);
		}
		else
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn,
				"WMZqt::DBAppBase::GetCfgItem(\"%s\") : NONE config item queried, and NONE default value given, return EMPTY !!!\n",
				qstrName.toStdString().c_str());
		}
	}

	return qstrValue;
}

bool	WMZqt::DBAppBase::IsTableExists(const QString& qstrTableName)
{
	if (!this->IsOpen())
	{
		return false;
	}

	QString	qstrStmt	=	QString("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='") + qstrTableName + "'";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		return false;
	}

	if (!query.next())
	{
		return false;
	}

	return (query.value(0).toInt() != 0);
}

int32_t	WMZqt::DBAppBase::DropTable(const QString& qstrTableName)
{
	QString	qstrStmt	=
		QString("DROP TABLE IF EXISTS ") + qstrTableName;
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::DBAppBase::DropTable() failed: SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrStmt.toStdString().c_str(),
			query.lastError().text().toStdString().c_str());

		return WMZqt::E_Errno_ERR_GENERAL;
	}

	return WMZqt::E_Errno_SUCCESS;
}

int32_t	WMZqt::DBAppBase::OpenTableCfg()
{
	if (this->IsTableExists("tbl_cfg"))
	{
		return WMZqt::E_Errno_SUCCESS;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::DBAppBase::OpenTableCfg() begin ...\n");

	QString		qstrStmt	=
		"CREATE TABLE tbl_cfg(name VARCHAR(255) PRIMARY KEY, value VARCHAR(255))";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::DBAppBase::OpenTableCfg() failed: SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrStmt.toStdString().c_str(),
			query.lastError().text().toStdString().c_str());

		return WMZqt::E_Errno_ERR_GENERAL;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::DBAppBase::OpenTableCfg() successfully .\n");

	return WMZqt::E_Errno_SUCCESS;
}

