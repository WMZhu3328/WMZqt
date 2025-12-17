
#ifndef DB_APP_H
#define DB_APP_H

#include "WMZqt/db_app_base.h"

class DBApp : public WMZqt::DBAppBase
{
public:
	DBApp();
	virtual ~DBApp();

	virtual int32_t	Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword = "");
	virtual int32_t	Close();

public:
	static DBApp*	Instance();
	static void		Destroy();

private:
	static DBApp*	m_spInstance;
};

inline
DBApp*	DBApp::Instance()
{
	if (DBApp::m_spInstance == NULL)
	{
		DBApp::m_spInstance	=	new DBApp();
	}

	return DBApp::m_spInstance;
}

inline
void	DBApp::Destroy()
{
	if (DBApp::m_spInstance != NULL)
	{
		delete DBApp::m_spInstance;
		DBApp::m_spInstance	=	NULL;
	}
}

#endif //end DB_APP_H

