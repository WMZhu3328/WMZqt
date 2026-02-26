
#ifndef WMZqt_DB_MEM_H
#define WMZqt_DB_MEM_H

#include "WMZqt/types.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace WMZqt
{
	class WMZqt_API DBMem
	{
	public:
		DBMem();
		~DBMem();

		int32_t	Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword = "");
		int32_t	Close();
		bool	IsOpen();

		const QString&	GetDBName() const	{ return this->m_qstrDBName; }

		int32_t BeginDBTransaction();
		int32_t CommitDBTransaction();
		int32_t RollbackDBTransaction();

	private:
		int32_t	OpenDB(const QString& qstrDBName, const QString& qstrFilePathDB, const QString& qstrPassword);
		int32_t	CloseDB(const QString& qstrDBName);

	private:
		QString	m_qstrDBName;

	public:
		static DBMem*	Instance();
		static void		Destroy();

	private:
		static DBMem*	m_spInstance;
	};
};

inline
WMZqt::DBMem*	WMZqt::DBMem::Instance()
{
	if (WMZqt::DBMem::m_spInstance == NULL)
	{
		WMZqt::DBMem::m_spInstance	=	new WMZqt::DBMem();
	}

	return WMZqt::DBMem::m_spInstance;
}

inline
void	WMZqt::DBMem::Destroy()
{
	if (WMZqt::DBMem::m_spInstance != NULL)
	{
		delete WMZqt::DBMem::m_spInstance;
		WMZqt::DBMem::m_spInstance	=	NULL;
	}
}

#endif //end WMZqt_DB_MEM_H

