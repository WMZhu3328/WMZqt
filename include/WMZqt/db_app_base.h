
#ifndef WMZqt_DB_APP_BASE_H
#define WMZqt_DB_APP_BASE_H

#include "WMZqt/types.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace WMZqt
{
	class WMZqt_API DBAppBase
	{
	public:
		DBAppBase();
		virtual ~DBAppBase();

		virtual int32_t	Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword = "");
		virtual int32_t	Close();
		virtual bool	IsOpen();

		virtual int32_t	SetCfgItem(const QString& qstrName, const QString& qstrValue);
				int32_t	SetIntCfgItem(const QString& qstrName, const int32_t& nValue)				{ return this->SetCfgItem(qstrName, QString::number(nValue)); }
				int32_t	SetInt64CfgItem(const QString& qstrName, const int64_t& llnValue)			{ return this->SetCfgItem(qstrName, QString::number(llnValue)); }

		virtual QString	GetCfgItem(const QString& qstrName, const QString& qstrDftValue = "");
				int32_t	GetIntCfgItem(const QString& qstrName, const int32_t& nDftValue = 0)		{ return this->GetCfgItem(qstrName, QString::number(nDftValue)).toInt(); }
				int64_t	GetInt64CfgItem(const QString& qstrName, const int64_t& llnDftValue = 0)	{ return this->GetCfgItem(qstrName, QString::number(llnDftValue)).toLongLong(); }

	protected:
		bool	IsTableExists(const QString& qstrTableName);
		int32_t	DropTable(const QString& qstrTableName);

		int32_t	OpenTableCfg();

	protected:
		QString	m_qstrDBName;
	};
};

#endif //end WMZqt_DB_APP_BASE_H

