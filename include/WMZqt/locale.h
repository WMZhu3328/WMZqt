
#ifndef WMZqt_LOCALE_H
#define WMZqt_LOCALE_H

#include "WMZqt/db_app_base.h"

class QLabel;
class QMenu;
class QAction;
class QToolButton;
class QPushButton;
class QGroupBox;
class QDockWidget;
class QTabWidget;
class QTreeWidgetItem;

namespace WMZqt
{
	struct TTabHeaderLocaleInfo
	{
		QTabWidget*	pTabWidget;
		int			nIndex;
		const char* pstrLabel_en_US;

		TTabHeaderLocaleInfo()
			:pTabWidget(NULL), nIndex(-1), pstrLabel_en_US(NULL)
		{}
		TTabHeaderLocaleInfo(QTabWidget* pTabWidgetParam, int nIndexParam, const char* pstrLabel_en_USParam)
			:pTabWidget(pTabWidgetParam), nIndex(nIndexParam), pstrLabel_en_US(pstrLabel_en_USParam)
		{}
	};

	class WMZqt_API Locale: public DBAppBase
	{
	public:
		Locale();
		virtual ~Locale();

		virtual int32_t	Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword, const QString& qstrDataSourceDir = "data");
		virtual int32_t	Close();

		bool	IsLocaleInitialized(const QString& qstrLocaleName);

		QString	GetLocaleString(const QString& qstr_en_US);

		void	GetSupportedLocales(std::map< QString, QString >& mapSupportedLocales);
		QString	GetLocaleValue(const QString& qstrLocaleName);
		QString	GetCurLocale();
		bool	SetCurLocale(const QString& qstrCurLocale);

		bool	GenerateLocaleTemplate(const QString& qstrFilePathLocale);
		QString	ImportLocale(const QString& qstrFilePathLocale, bool bTruncate = true);

		static	QString	GetSystemLocale();

		void	OnLocaleChanged();

		void	OnLabelCreated(QLabel* pLabel, const char* pstrLabel_en_US);
		void	OnMenuCreated(QMenu* pMenu, const char* pstrLabel_en_US);
		void	OnMenuItemCreated(QAction* pMenuItem, const char* pstrLabel_en_US);
		void	OnToolButtonCreated(QToolButton* pToolButton, const char* pstrLabel_en_US);
		void	OnPushButtonCreated(QPushButton* pPushButton, const char* pstrLabel_en_US);
		void	OnGroupBoxCreated(QGroupBox* pGroupBox, const char* pstrLabel_en_US);
		void	OnDockWidgetCreated(QDockWidget* pDockWidget, const char* pstrLabel_en_US);
		void	OnTreeWidgetItemCreated(QTreeWidgetItem* pTreeWidgetItem, const char* pstrLabel_en_US);
		void	OnTabPageCreated(QTabWidget* pTabWidget, int nIndex, const char* pstrLabel_en_US);

		void	OnLabelUpdated(QLabel* pLabel, const char* pstrLabel_en_US);
		void	OnMenuUpdated(QMenu* pMenu, const char* pstrLabel_en_US);
		void	OnMenuItemUpdated(QAction* pMenuItem, const char* pstrLabel_en_US);
		void	OnToolButtonUpdated(QToolButton* pToolButton, const char* pstrLabel_en_US);
		void	OnPushButtonUpdated(QPushButton* pPushButton, const char* pstrLabel_en_US);
		void	OnGroupBoxUpdated(QGroupBox* pGroupBox, const char* pstrLabel_en_US);
		void	OnDockWidgetUpdated(QDockWidget* pDockWidget, const char* pstrLabel_en_US);
		void	OnTreeWidgetItemUpdated(QTreeWidgetItem* pTreeWidgetItem, const char* pstrLabel_en_US);
		void	OnTabPageUpdated(QTabWidget* pTabWidget, int nIndex, const char* pstrLabel_en_US);

	protected:
		int32_t	OpenTableSupportedLocales();
		int32_t	OpenTableLocale(const QString& qstrLocale, bool bTruncate = false);

		int32_t	PersistenceLocale(const QString& qstrLocaleName, const QString& qstrLocaleValue,
			std::map< QString, QString >& mapLocaleStrings, bool bTruncate);

	protected:
		QString	m_qstrCurLocale;

		std::map< QLabel*, const char* >			m_mapLocales_Label;
		std::map< QMenu*, const char* >				m_mapLocales_Menu;
		std::map< QAction*, const char* >			m_mapLocales_MenuItem;
		std::map< QToolButton*, const char* >		m_mapLocales_ToolButton;
		std::map< QPushButton*, const char* >		m_mapLocales_PushButton;
		std::map< QGroupBox*, const char* >			m_mapLocales_GroupBox;
		std::map< QDockWidget*, const char* >		m_mapLocales_DockWidget;
		std::map< QTreeWidgetItem*, const char* >	m_mapLocales_TreeWidgetItem;
		std::multimap< QTabWidget*, WMZqt::TTabHeaderLocaleInfo >	m_multimapLocales_TabPage;

	public:
		static Locale*	Instance();
		static void		Destroy();

	private:
		static Locale*	m_spInstance;
	};
};

#define	WMZqt_T(x)	WMZqt::Locale::Instance()->GetLocaleString(x)

#define WMZqt_OpLogBegin(pstrOpCmd) \
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Info, \
			"%s [\"%s\"] %s%s\n", \
			WMZqt_T("execute command").toStdString().c_str(), \
			WMZqt_T(pstrOpCmd).toStdString().c_str(), \
			WMZqt_T("begin").toStdString().c_str(), \
			WMZqt_T("...").toStdString().c_str());

#define WMZqt_OpLogSuccess(pstrOpCmd) \
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Info, \
			"%s [\"%s\"] %s%s%s[%s]%s\n", \
			WMZqt_T("execute command").toStdString().c_str(), \
			WMZqt_T(pstrOpCmd).toStdString().c_str(), \
			WMZqt_T("finished").toStdString().c_str(), \
			WMZqt_T(":").toStdString().c_str(), \
			WMZqt_T("result").toStdString().c_str(), \
			WMZqt_T("successfully").toStdString().c_str(), \
			WMZqt_T(".").toStdString().c_str());

#define	WMZqt_OpLogFailed(pstrOpCmd, nErrno, qstrErrMsg) \
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn, \
			"%s [\"%s\"] %s%s%s[%s]%s%s[%d]%s%s[\"%s\"]%s\n", \
			WMZqt_T("execute command").toStdString().c_str(), \
			WMZqt_T(pstrOpCmd).toStdString().c_str(), \
			WMZqt_T("finished").toStdString().c_str(), \
			WMZqt_T(":").toStdString().c_str(), \
			WMZqt_T("result").toStdString().c_str(), \
			WMZqt_T("failed").toStdString().c_str(), \
			WMZqt_T(",").toStdString().c_str(), \
			WMZqt_T("errno").toStdString().c_str(), \
			nErrno, \
			WMZqt_T(",").toStdString().c_str(), \
			WMZqt_T("error message").toStdString().c_str(), \
			WMZqt_T(qstrErrMsg).toStdString().c_str(), \
			WMZqt_T("!!!").toStdString().c_str());

inline
WMZqt::Locale*	WMZqt::Locale::Instance()
{
	if (WMZqt::Locale::m_spInstance == NULL)
	{
		WMZqt::Locale::m_spInstance	=	new WMZqt::Locale();
	}

	return WMZqt::Locale::m_spInstance;
}

inline
void	WMZqt::Locale::Destroy()
{
	if (WMZqt::Locale::m_spInstance != NULL)
	{
		delete WMZqt::Locale::m_spInstance;
		WMZqt::Locale::m_spInstance	=	NULL;
	}
}

#endif //end WMZqt_LOCALE_H

