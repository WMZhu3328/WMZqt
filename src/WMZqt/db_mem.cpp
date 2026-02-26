
#include "WMZqt/db_mem.h"
#include "WMZqt/threaded/log.h"

WMZqt::DBMem::DBMem()
	:m_qstrDBName("WMZqt_DBInMem")
{
}

WMZqt::DBMem::~DBMem()
{
	this->Close();
}

WMZqt::DBMem*	WMZqt::DBMem::m_spInstance	=	NULL;

int32_t	WMZqt::DBMem::Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword)
{
	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Trace,
		"WMZqt::DBMem::Open(\"%s\") available SQL drivers:\n",
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
		"WMZqt::DBMem::Open(\"%s\") begin ...\n",
		qstrDBName.toStdString().c_str());

	// check parameters
	if (qstrDBName.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::DBMem::Open(\"%s\") failed: qstrAppName[\"\"] cannot be EMPTY !!!\n",
			qstrDBName.toStdString().c_str());

		return WMZqt::E_Errno_ERR_INVALID_PARAMS;
	}

	// close first if app database already opened
	if (this->IsOpen())
	{
		this->Close();
	}

	int32_t	nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

	QString	qstrDBName_FS;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QString	qstrFilePathDB_FS;
		if (!qstrDBHome.isEmpty())
		{
			qstrFilePathDB_FS	=	qstrDBHome + QDir::separator() + qstrDBName + ".db";
		}
		else
		{
			qstrFilePathDB_FS	=	QString(".") + QDir::separator() + qstrDBName + ".db";
		}
		qstrDBName_FS	=	qstrDBName + "_FS";

		// 1. open in-memory database
		QSqlDatabase	dbInMem	=	QSqlDatabase::addDatabase("QSQLITE", this->m_qstrDBName);
		dbInMem.setDatabaseName(":memory:");
		if (!dbInMem.open())
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::DBMem::Open(\"%s\") failed: failed to open in-memory database[\":memory:\"] !!!\n",
				qstrDBName.toStdString().c_str());

			nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

			break;
		}

		// 2. open database in FS to copy sql sechmas
		QSqlDatabase	db_FS	=	QSqlDatabase::addDatabase("QSQLITE", qstrDBName_FS);
		db_FS.setDatabaseName(qstrFilePathDB_FS);
		if (!qstrPassword.isEmpty())
		{
			db_FS.setPassword(qstrPassword);
		}
		if (!db_FS.open())
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::DBMem::Open(\"%s\") failed: failed to open database in FileSystem[\"%s\"] !!!\n",
				this->m_qstrDBName.toStdString().c_str(),
				qstrFilePathDB_FS.toStdString().c_str());

			nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

			break;
		}

		// 3. copy sql sechmas
		QString		qstrStmt_Query =
			QString("SELECT sql FROM sqlite_schema");
		QSqlQuery	query_Query(db_FS);
		if (!query_Query.exec(qstrStmt_Query))
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::DBMem::Open(\"%s\") failed: failed to query SqlSchemas from database in FileSystem[\"%s\"] !!!\n\tSql[\"%s\"], ErrorMessage[\"%s\"]\n",
				qstrDBName.toStdString().c_str(),
				qstrFilePathDB_FS.toStdString().c_str(),
				qstrStmt_Query.toStdString().c_str(),
				query_Query.lastError().text().toStdString().c_str());

			nErrno = WMZqt::E_Errno_ERR_GENERAL;

			break;
		}
		while (query_Query.next())
		{
			QString	qstrStmt_Copy	=	query_Query.value(0).toString();
			if (!qstrStmt_Copy.isEmpty())
			{
				QSqlQuery	query_Copy(dbInMem);
				if (!query_Copy.exec(qstrStmt_Copy))
				{
					WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
						"WMZqt::DBMem::Open(\"%s\") failed: failed to exec [\"%s\"] in memory database, error message[\"%s\"]!!!\n",
						qstrDBHome.toStdString().c_str(),
						qstrStmt_Copy.toStdString().c_str(),
						query_Copy.lastError().text().toStdString().c_str());

					nErrno = WMZqt::E_Errno_ERR_GENERAL;

					break;
				}
			}
		}

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::DBMem::Open(\"%s\") successfully .\n",
			this->m_qstrDBName.toStdString().c_str());

		nErrno	=	WMZqt::E_Errno_SUCCESS;
	}
	if (nErrno != WMZqt::E_Errno_SUCCESS)
	{
		this->Close();
	}

	this->CloseDB(qstrDBName_FS);

	return nErrno;
}

int32_t	WMZqt::DBMem::Close()
{
	if (!this->m_qstrDBName.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::DBMem::Close(\"%s\") begin ...\n",
			this->m_qstrDBName.toStdString().c_str());

		QSqlDatabase	dbApp	=	QSqlDatabase::database(this->m_qstrDBName);
		if (dbApp.isOpen())
		{
			QSqlDatabase::removeDatabase(this->m_qstrDBName);
		}

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::DBMem::Close(\"%s\") successfully !!!\n",
			this->m_qstrDBName.toStdString().c_str());

		this->m_qstrDBName.clear();
	}

	return WMZqt::E_Errno_SUCCESS;
}

bool	WMZqt::DBMem::IsOpen()
{
	if (this->m_qstrDBName.isEmpty())
	{
		return false;
	}

	return QSqlDatabase::database(this->m_qstrDBName).isOpen();
}

int32_t	WMZqt::DBMem::OpenDB(const QString& qstrDBName, const QString& qstrFilePathDB, const QString& qstrPassword)
{
	if (qstrDBName.isEmpty() || qstrFilePathDB.isEmpty())
	{
		return WMZqt::E_Errno_ERR_INVALID_PARAMS;
	}

	this->CloseDB(qstrDBName);

	int32_t	nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		// open database
		QSqlDatabase	db	=	QSqlDatabase::addDatabase("QSQLITE", qstrDBName);
		db.setDatabaseName(qstrFilePathDB);
		if (!qstrPassword.isEmpty())
		{
			db.setPassword(qstrPassword);
		}
		if (!db.open())
		{
			nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

			break;
		}

		nErrno	=	WMZqt::E_Errno_SUCCESS;
	}
	if (nErrno != WMZqt::E_Errno_SUCCESS)
	{
		this->Close();
	}

	return nErrno;
}

int32_t	WMZqt::DBMem::CloseDB(const QString& qstrDBName)
{
	if (qstrDBName.isEmpty())
	{
		return WMZqt::E_Errno_ERR_INVALID_PARAMS;
	}

	QSqlDatabase	db	=	QSqlDatabase::database(qstrDBName);
	if (db.isOpen())
	{
		db.close();
	}

	return WMZqt::E_Errno_SUCCESS;
}

int32_t WMZqt::DBMem::BeginDBTransaction()
{
	QSqlDatabase	db	=	QSqlDatabase::database(this->m_qstrDBName);

	return db.transaction() ? WMZqt::E_Errno_SUCCESS : WMZqt::E_Errno_ERR_EXEC_SQL;
}

int32_t WMZqt::DBMem::CommitDBTransaction()
{
	QSqlDatabase	db	=	QSqlDatabase::database(this->m_qstrDBName);

	return db.commit() ? WMZqt::E_Errno_SUCCESS : WMZqt::E_Errno_ERR_EXEC_SQL;
}

int32_t	WMZqt::DBMem::RollbackDBTransaction()
{
	QSqlDatabase	db	=	QSqlDatabase::database(this->m_qstrDBName);

	return db.rollback() ? WMZqt::E_Errno_SUCCESS : WMZqt::E_Errno_ERR_EXEC_SQL;
}

