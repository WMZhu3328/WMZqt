
#include "WMZqt/locale.h"
#include "WMZqt/app.h"
#include "WMZqt/threaded/log.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QPushButton>
#include <QGroupBox>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTabWidget>

WMZqt::Locale*	WMZqt::Locale::m_spInstance	=	NULL;

WMZqt::Locale::Locale()
	:WMZqt::DBAppBase()
	, m_qstrCurLocale("en_US")
{
}

WMZqt::Locale::~Locale()
{
	this->Close();
}

int32_t	WMZqt::Locale::Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword, const QString& qstrDataSourceDir)
{
	// close first if app database already opened
	if (this->IsOpen())
	{
		this->Close();
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::Open(\"%s\") begin ...\n",
		qstrDBName.toStdString().c_str());

	int32_t	nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		nErrno	=	WMZqt::DBAppBase::Open(qstrDBHome, qstrDBName, qstrPassword);
		if (nErrno != WMZqt::E_Errno_SUCCESS)
		{
			break;
		}

		// open supported locales' table, create if not exists
		nErrno	=	this->OpenTableSupportedLocales();
		if (nErrno != WMZqt::E_Errno_SUCCESS)
		{
			break;
		}

		// get configured CUR_LOCALE, initialize if not exists
		this->m_qstrCurLocale	=	this->GetCfgItem("CUR_LOCALE", "");
		if (this->m_qstrCurLocale.isEmpty())
		{
			this->m_qstrCurLocale	=	WMZqt::Locale::GetSystemLocale();

			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
				"WMZqt::Locale::Open(\"%s\") : NONE CUR_LOCALE configured, use system locale[\"%s\"] ...\n",
				this->m_qstrDBName.toStdString().c_str(),
				this->m_qstrCurLocale.toStdString().c_str());
		}
		if (!this->IsLocaleInitialized(this->m_qstrCurLocale))
		{
			QString	qstrFilePathLocale_App	=	WMZqt::App::Instance()->GetAppHome() + QDir::separator() + qstrDataSourceDir + QDir::separator() + "locale" + QDir::separator() + "locale." + this->m_qstrCurLocale + ".xml";
			this->m_qstrCurLocale	=	this->ImportLocale(qstrFilePathLocale_App);
			if (this->m_qstrCurLocale.isEmpty())
			{
				this->m_qstrCurLocale	=	"en_US";
			}

			QString	qstrFilePathLocale_pdsi	=	WMZqt::App::Instance()->GetAppHome() + QDir::separator() + qstrDataSourceDir + QDir::separator() + "locale" + QDir::separator() + "locale.pdsi." + this->m_qstrCurLocale + ".xml";
			if (QFile::exists(qstrFilePathLocale_pdsi))
			{
				this->ImportLocale(qstrFilePathLocale_pdsi, false);
			}
		}

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::Open(\"%s\") successfully with locale[\"%s\"] ...\n",
			this->m_qstrDBName.toStdString().c_str(),
			this->m_qstrCurLocale.toStdString().c_str());

		nErrno	=	WMZqt::E_Errno_SUCCESS;
	}
	if (nErrno != WMZqt::E_Errno_SUCCESS)
	{
		this->Close();
	}

	return nErrno;
}

int32_t	WMZqt::Locale::Close()
{
	if (this->IsOpen())
	{
		QString	qstrDBName	=	this->m_qstrDBName;

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::Close(\"%s\") begin ...\n",
			qstrDBName.toStdString().c_str());

		WMZqt::DBAppBase::Close();

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::Close(\"%s\") successfully !!!\n",
			qstrDBName.toStdString().c_str());
	}

	this->m_qstrCurLocale =	"en_US";

	return WMZqt::E_Errno_SUCCESS;
}

bool	WMZqt::Locale::IsLocaleInitialized(const QString& qstrLocaleName)
{
	// check parameters
	if (qstrLocaleName.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::IsLocaleInitialized(\"%s\") failed: locale name is EMPTY !!!\n",
			qstrLocaleName.toStdString().c_str());

		return false;
	}

	// check locale is supported
	std::map< QString, QString >	mapSupportedLocales;
	this->GetSupportedLocales(mapSupportedLocales);
	if (mapSupportedLocales.find(qstrLocaleName) == mapSupportedLocales.end())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::IsLocaleInitialized(\"%s\") : locale NOT supported !!!\n",
			qstrLocaleName.toStdString().c_str());

		return false;
	}

	// check locale table exists or not
	QString	qstrLocaleTableName	=	QString("tbl_") + qstrLocaleName;
	if (!this->IsTableExists(qstrLocaleTableName))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::IsLocaleInitialized(\"%s\") : locale table[\"%s\"] NOT EXIST !!!\n",
			qstrLocaleName.toStdString().c_str(),
			qstrLocaleTableName.toStdString().c_str());

		return false;
	}

	return true;
}

QString	WMZqt::Locale::GetLocaleString(const QString& qstr_en_US)
{
	QString	qstrLocale(qstr_en_US);

	if (this->m_qstrCurLocale != "en_US")
	{
		if (this->IsOpen())
		{
			for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
			{
				QString		qstrStmt =
					QString("SELECT value FROM tbl_") + this->m_qstrCurLocale + " WHERE name = '" + qstr_en_US + "'";
				QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
				if (!query.exec(qstrStmt))
				{
					WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn,
						"WMZqt::Locale::GetLocaleString(\"%s\") : failed to execute SQL[\"%s\"], ERROR message[\"%s\"], return default value[\"%s\"] directly !!!\n",
						qstr_en_US.toStdString().c_str(),
						qstrStmt.toStdString().c_str(),
						query.lastError().text().toStdString().c_str(),
						qstr_en_US.toStdString().c_str());

					break;
				}
				if (!query.next())
				{
					WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Warn,
						"WMZqt::Locale::GetLocaleString(\"%s\") : NONE locale string queried, set and return default value[\"%s\"] !!!\n",
						qstr_en_US.toStdString().c_str(),
						qstr_en_US.toStdString().c_str());

					QString	qstrStmtSet =
						QString("INSERT INTO tbl_") + this->m_qstrCurLocale + "(name, value) VALUES('" + qstr_en_US + "', '" + qstr_en_US + "')";
					QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
					query.exec(qstrStmtSet);

					break;
				}

				qstrLocale =	query.value(0).toString();
			}
		}
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Trace,
		"WMZqt::Locale::GetLocaleString(\"%s\") successfully with value[\"%s\"].\n",
		qstr_en_US.toStdString().c_str(), qstrLocale.toStdString().c_str());

	return qstrLocale;
}

void	WMZqt::Locale::GetSupportedLocales(std::map< QString, QString >& mapSupportedLocales)
{
	mapSupportedLocales.clear();
	mapSupportedLocales.insert(std::make_pair("en_US", "English"));

	QString		qstrStmt	=
		"SELECT name, value FROM tbl_locale ORDER BY name";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (query.exec(qstrStmt))
	{
		while (query.next())
		{
			mapSupportedLocales.insert(std::make_pair(query.value(0).toString(), query.value(1).toString()));
		}
	}
}

QString	WMZqt::Locale::GetLocaleValue(const QString& qstrLocaleName)
{
	QString	qstrLocaleValue;

	QString		qstrStmt =
		QString("SELECT value FROM tbl_locale WHERE name='") + qstrLocaleName + "'";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (query.exec(qstrStmt))
	{
		if (query.next())
		{
			qstrLocaleValue	=	query.value(0).toString();
		}
	}

	return qstrLocaleValue;
}

QString	WMZqt::Locale::GetCurLocale()
{
	return this->m_qstrCurLocale;
}

bool	WMZqt::Locale::SetCurLocale(const QString& qstrCurLocale)
{
	this->m_qstrCurLocale	=	qstrCurLocale;

	this->SetCfgItem("CUR_LOCALE", this->m_qstrCurLocale);
	this->OpenTableLocale(this->m_qstrCurLocale);

	this->OnLocaleChanged();
	emit WMZqt::EventMgr::Instance()->signal_OnLocaleChanged(qstrCurLocale);

	return true;
}

bool	WMZqt::Locale::GenerateLocaleTemplate(const QString& qstrFilePathLocale)
{
	// check parameters
	if (qstrFilePathLocale.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::Locale::GenerateLocaleTemplate(\"%s\") failed: qstrFilePathLocale is EMPTY !!!\n",
			qstrFilePathLocale.toStdString().c_str());

		return false;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::GenerateLocaleTemplate(\"%s\") begin ...\n",
		qstrFilePathLocale.toStdString().c_str());

	QString	qstrLocaleName(this->m_qstrCurLocale);
	QString	qstrLocaleValue	=	this->GetLocaleValue(qstrLocaleName);
	if (qstrLocaleValue.isEmpty())
	{
		qstrLocaleName	=	"en_US";
		qstrLocaleValue	=	"English";
	}

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QFile	xmlFile(qstrFilePathLocale);
		if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::Locale::GenerateLocaleTemplate(\"%s\") failed: open file to write failed !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			break;
		}

		QXmlStreamWriter	xmlWriter(&xmlFile);
		xmlWriter.setAutoFormatting(true);
		//xmlWriter.setAutoFormattingIndent(4);
		xmlWriter.writeStartDocument("1.0", true);

			xmlWriter.writeStartElement("Locale");
			xmlWriter.writeAttribute("name", qstrLocaleName);
			xmlWriter.writeAttribute("value", qstrLocaleValue);

			QString	qstrStmt =
				QString("SELECT name, value FROM tbl_") + this->m_qstrCurLocale + " ORDER BY name ASC";
			QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
			if (!query.exec(qstrStmt))
			{
				WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
					"WMZqt::Locale::GenerateLocaleTemplate(\"%s\") failed: failed to execute SQL[\"%s\"], ERROR message[\"%s\"] !!!\n",
					qstrFilePathLocale.toStdString().c_str(),
					qstrStmt.toStdString().c_str(),
					query.lastError().text().toStdString().c_str());

				break;
			}
			while (query.next())
			{
				xmlWriter.writeStartElement("Item");
				xmlWriter.writeAttribute("en_US", query.value(0).toString());
				xmlWriter.writeAttribute("locale", query.value(1).toString());
				xmlWriter.writeEndElement();
			}

			xmlWriter.writeEndElement();

		xmlWriter.writeEndDocument();

		xmlFile.close();

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::GenerateLocaleTemplate(\"%s\") finished successfully !!!\n",
			qstrFilePathLocale.toStdString().c_str());
	}

	return true;
}

QString	WMZqt::Locale::ImportLocale(const QString& qstrFilePathLocale, bool bTruncate)
{
	// check parameters
	if (qstrFilePathLocale.isEmpty())
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::Locale::ImportLocale(\"%s\") failed: qstrFilePathLocale is EMPTY !!!\n",
			qstrFilePathLocale.toStdString().c_str());

		return "";
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::ImportLocale(\"%s\") begin ...\n",
		qstrFilePathLocale.toStdString().c_str());

	QString	qstrLocaleName;
	QString	qstrLocaleValue;
	std::map< QString, QString >	mapLocaleStrings;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QFile	xmlFile(qstrFilePathLocale);
		if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::Locale::ImportLocale(\"%s\") failed: open file to read failed !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			break;
		}

		QXmlStreamReader	xmlReader(&xmlFile);
		while (!xmlReader.atEnd() && !xmlReader.hasError())
		{
			QXmlStreamReader::TokenType	tokenType	=	xmlReader.readNext();
			if (tokenType == QXmlStreamReader::StartElement)
			{
				QString					qstrElementName	=	xmlReader.name().toString();
				QXmlStreamAttributes	attributes		=	xmlReader.attributes();
				if (qstrElementName == "Locale")
				{
					qstrLocaleName	=	attributes.value("name").toString();
					qstrLocaleValue	=	attributes.value("value").toString();

					WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
						"rootNode[\"Locale\"] read, LocaleName[\"%s\"], LocaleValue[\"%s\"] ...\n",
						qstrLocaleName.toStdString().c_str(), qstrLocaleValue.toStdString().c_str());
				}
				else if (qstrElementName == "Item")
				{
					QString	qstr_en_US	=	attributes.value("en_US").toString();
					QString	qstr_locale	=	attributes.value("locale").toString();
					mapLocaleStrings[qstr_en_US]	=	qstr_locale;

					WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
						"itemNode[\"Item\"] read, English[\"%s\"], Locale[\"%s\"] ...\n",
						qstr_en_US.toStdString().c_str(), qstr_locale.toStdString().c_str());
				}
			}
			else if (tokenType == QXmlStreamReader::StartDocument)
			{
				WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
					"WMZqt::Locale::ImportLocale(\"%s\") begin read locale file ...\n",
					qstrFilePathLocale.toStdString().c_str());
			}
			else if (tokenType == QXmlStreamReader::EndDocument)
			{
				WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
					"WMZqt::Locale::ImportLocale(\"%s\") finished read locale file !!!\n",
					qstrFilePathLocale.toStdString().c_str());
			}
		}
		if (xmlReader.hasError())
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::Locale::ImportLocale(\"%s\") failed: XML file is corrupted !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			qstrLocaleName.clear();
			qstrLocaleValue.clear();
			mapLocaleStrings.clear();

			break;
		}

		if (this->PersistenceLocale(qstrLocaleName, qstrLocaleValue, mapLocaleStrings, bTruncate) != WMZqt::E_Errno_SUCCESS)
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::Locale::ImportLocale(\"%s\") failed: failed on persistence !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			qstrLocaleName.clear();
			qstrLocaleValue.clear();
			mapLocaleStrings.clear();

			break;
		}

		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
			"WMZqt::Locale::ImportLocale(\"%s\") finished successfully !!!\n",
			qstrFilePathLocale.toStdString().c_str());
		xmlFile.close();
	}

	return qstrLocaleName;
}

QString	WMZqt::Locale::GetSystemLocale()
{
	QLocale	localeSys;
	QString	qstrLangSys;

	switch (localeSys.language())
	{
	case QLocale::Chinese:
		qstrLangSys	=	"zh_CN";
		break;
	default:
		qstrLangSys	=	"en_US";
		break;
	}

	return qstrLangSys;
}

void	WMZqt::Locale::OnLocaleChanged()
{
	// Label
	std::map< QLabel*, const char* >::iterator	iterMapLocales_Label;
	for (iterMapLocales_Label = this->m_mapLocales_Label.begin(); iterMapLocales_Label != this->m_mapLocales_Label.end(); ++iterMapLocales_Label)
	{
		iterMapLocales_Label->first->setText(WMZqt_T(iterMapLocales_Label->second));
	}

	// Menu
	std::map< QMenu*, const char* >::iterator	iterMapLocales_Menu;
	for (iterMapLocales_Menu = this->m_mapLocales_Menu.begin(); iterMapLocales_Menu != this->m_mapLocales_Menu.end(); ++iterMapLocales_Menu)
	{
		iterMapLocales_Menu->first->setTitle(WMZqt_T(iterMapLocales_Menu->second));
	}

	// MenuItem
	std::map< QAction*, const char* >::iterator	iterMapLocales_MenuItem;
	for (iterMapLocales_MenuItem = this->m_mapLocales_MenuItem.begin(); iterMapLocales_MenuItem != this->m_mapLocales_MenuItem.end(); ++iterMapLocales_MenuItem)
	{
		iterMapLocales_MenuItem->first->setText(WMZqt_T(iterMapLocales_MenuItem->second));
	}

	// ToolButton
	std::map< QToolButton*, const char* >::iterator	iterMapLocales_ToolButton;
	for (iterMapLocales_ToolButton = this->m_mapLocales_ToolButton.begin(); iterMapLocales_ToolButton != this->m_mapLocales_ToolButton.end(); ++iterMapLocales_ToolButton)
	{
		iterMapLocales_ToolButton->first->setText(WMZqt_T(iterMapLocales_ToolButton->second));
	}

	// PushButton
	std::map< QPushButton*, const char* >::iterator	iterMapLocales_PushButton;
	for (iterMapLocales_PushButton = this->m_mapLocales_PushButton.begin(); iterMapLocales_PushButton != this->m_mapLocales_PushButton.end(); ++iterMapLocales_PushButton)
	{
		iterMapLocales_PushButton->first->setText(WMZqt_T(iterMapLocales_PushButton->second));
	}

	// GroupBox
	std::map< QGroupBox*, const char* >::iterator	iterMapLocales_GroupBox;
	for (iterMapLocales_GroupBox = this->m_mapLocales_GroupBox.begin(); iterMapLocales_GroupBox != this->m_mapLocales_GroupBox.end(); ++iterMapLocales_GroupBox)
	{
		iterMapLocales_GroupBox->first->setTitle(WMZqt_T(iterMapLocales_GroupBox->second));
	}

	// DockWidget
	std::map< QDockWidget*, const char* >::iterator	iterMapLocales_DockWidget;
	for (iterMapLocales_DockWidget = this->m_mapLocales_DockWidget.begin(); iterMapLocales_DockWidget != this->m_mapLocales_DockWidget.end(); ++iterMapLocales_DockWidget)
	{
		iterMapLocales_DockWidget->first->setWindowTitle(WMZqt_T(iterMapLocales_DockWidget->second));
	}

	// TreeWidgetItem
	std::map< QTreeWidgetItem*, const char* >::iterator	iterMapLocales_TreeWidgetItem;
	for (iterMapLocales_TreeWidgetItem = this->m_mapLocales_TreeWidgetItem.begin(); iterMapLocales_TreeWidgetItem != this->m_mapLocales_TreeWidgetItem.end(); ++iterMapLocales_TreeWidgetItem)
	{
		iterMapLocales_TreeWidgetItem->first->setText(0, WMZqt_T(iterMapLocales_TreeWidgetItem->second));
	}

	// TabPage
	std::multimap< QTabWidget*, WMZqt::TTabHeaderLocaleInfo >::iterator	iterMapLocales_TabPage;
	for (iterMapLocales_TabPage = this->m_multimapLocales_TabPage.begin(); iterMapLocales_TabPage != this->m_multimapLocales_TabPage.end(); ++iterMapLocales_TabPage)
	{
		iterMapLocales_TabPage->first->setTabText(iterMapLocales_TabPage->second.nIndex, WMZqt_T(iterMapLocales_TabPage->second.pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnLabelCreated(QLabel* pLabel, const char* pstrLabel_en_US)
{
	this->m_mapLocales_Label.insert(std::make_pair(pLabel, pstrLabel_en_US));
}

void	WMZqt::Locale::OnMenuCreated(QMenu* pMenu, const char* pstrLabel_en_US)
{
	this->m_mapLocales_Menu.insert(std::make_pair(pMenu, pstrLabel_en_US));
}

void	WMZqt::Locale::OnMenuItemCreated(QAction* pMenuItem, const char* pstrLabel_en_US)
{
	this->m_mapLocales_MenuItem.insert(std::make_pair(pMenuItem, pstrLabel_en_US));
}

void	WMZqt::Locale::OnToolButtonCreated(QToolButton* pToolButton, const char* pstrLabel_en_US)
{
	this->m_mapLocales_ToolButton.insert(std::make_pair(pToolButton, pstrLabel_en_US));
}

void	WMZqt::Locale::OnPushButtonCreated(QPushButton* pPushButton, const char* pstrLabel_en_US)
{
	this->m_mapLocales_PushButton.insert(std::make_pair(pPushButton, pstrLabel_en_US));
}

void	WMZqt::Locale::OnGroupBoxCreated(QGroupBox* pGroupBox, const char* pstrLabel_en_US)
{
	this->m_mapLocales_GroupBox.insert(std::make_pair(pGroupBox, pstrLabel_en_US));
}

void	WMZqt::Locale::OnDockWidgetCreated(QDockWidget* pDockWidget, const char* pstrLabel_en_US)
{
	this->m_mapLocales_DockWidget.insert(std::make_pair(pDockWidget, pstrLabel_en_US));
}

void	WMZqt::Locale::OnTreeWidgetItemCreated(QTreeWidgetItem* pTreeWidgetItem, const char* pstrLabel_en_US)
{
	this->m_mapLocales_TreeWidgetItem.insert(std::make_pair(pTreeWidgetItem, pstrLabel_en_US));
}

void	WMZqt::Locale::OnTabPageCreated(QTabWidget* pTabWidget, int nIndex, const char* pstrLabel_en_US)
{
	WMZqt::TTabHeaderLocaleInfo	tabHeaderLocalInfo(pTabWidget, nIndex, pstrLabel_en_US);
	this->m_multimapLocales_TabPage.insert(std::make_pair(pTabWidget, tabHeaderLocalInfo));
}

void	WMZqt::Locale::OnLabelUpdated(QLabel* pLabel, const char* pstrLabel_en_US)
{
	std::map< QLabel*, const char* >::iterator	iterMapLocales_Label	=
		this->m_mapLocales_Label.find(pLabel);
	if (iterMapLocales_Label != this->m_mapLocales_Label.end())
	{
		iterMapLocales_Label->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_Label.insert(std::make_pair(pLabel, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnMenuUpdated(QMenu* pMenu, const char* pstrLabel_en_US)
{
	std::map< QMenu*, const char* >::iterator	iterMapLocales_Menu	=
		this->m_mapLocales_Menu.find(pMenu);
	if (iterMapLocales_Menu != this->m_mapLocales_Menu.end())
	{
		iterMapLocales_Menu->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_Menu.insert(std::make_pair(pMenu, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnMenuItemUpdated(QAction* pMenuItem, const char* pstrLabel_en_US)
{
	std::map< QAction*, const char* >::iterator	iterMapLocales_MenuItem =
		this->m_mapLocales_MenuItem.find(pMenuItem);
	if (iterMapLocales_MenuItem != this->m_mapLocales_MenuItem.end())
	{
		iterMapLocales_MenuItem->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_MenuItem.insert(std::make_pair(pMenuItem, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnToolButtonUpdated(QToolButton* pToolButton, const char* pstrLabel_en_US)
{
	std::map< QToolButton*, const char* >::iterator	iterMapLocales_ToolButton =
		this->m_mapLocales_ToolButton.find(pToolButton);
	if (iterMapLocales_ToolButton != this->m_mapLocales_ToolButton.end())
	{
		iterMapLocales_ToolButton->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_ToolButton.insert(std::make_pair(pToolButton, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnPushButtonUpdated(QPushButton* pPushButton, const char* pstrLabel_en_US)
{
	std::map< QPushButton*, const char* >::iterator	iterMapLocales_PushButton =
		this->m_mapLocales_PushButton.find(pPushButton);
	if (iterMapLocales_PushButton != this->m_mapLocales_PushButton.end())
	{
		iterMapLocales_PushButton->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_PushButton.insert(std::make_pair(pPushButton, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnGroupBoxUpdated(QGroupBox* pGroupBox, const char* pstrLabel_en_US)
{
	std::map< QGroupBox*, const char* >::iterator	iterMapLocales_GroupBox =
		this->m_mapLocales_GroupBox.find(pGroupBox);
	if (iterMapLocales_GroupBox != this->m_mapLocales_GroupBox.end())
	{
		iterMapLocales_GroupBox->second = pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_GroupBox.insert(std::make_pair(pGroupBox, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnDockWidgetUpdated(QDockWidget* pDockWidget, const char* pstrLabel_en_US)
{
	std::map< QDockWidget*, const char* >::iterator	iterMapLocales_DockWidget =
		this->m_mapLocales_DockWidget.find(pDockWidget);
	if (iterMapLocales_DockWidget != this->m_mapLocales_DockWidget.end())
	{
		iterMapLocales_DockWidget->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_DockWidget.insert(std::make_pair(pDockWidget, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnTreeWidgetItemUpdated(QTreeWidgetItem* pTreeWidgetItem, const char* pstrLabel_en_US)
{
	std::map< QTreeWidgetItem*, const char* >::iterator	iterMapLocales_TreeWidgetItem	=
		this->m_mapLocales_TreeWidgetItem.find(pTreeWidgetItem);
	if (iterMapLocales_TreeWidgetItem != this->m_mapLocales_TreeWidgetItem.end())
	{
		iterMapLocales_TreeWidgetItem->second = pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_TreeWidgetItem.insert(std::make_pair(pTreeWidgetItem, pstrLabel_en_US));
	}
}

void	WMZqt::Locale::OnTabPageUpdated(QTabWidget* pTabWidget, int nIndex, const char* pstrLabel_en_US)
{
	std::multimap< QTabWidget*, WMZqt::TTabHeaderLocaleInfo >::iterator	iterMultimapLocales_TabPage	=
		this->m_multimapLocales_TabPage.find(pTabWidget);
	for (; iterMultimapLocales_TabPage != this->m_multimapLocales_TabPage.end(); ++iterMultimapLocales_TabPage)
	{
		if (iterMultimapLocales_TabPage->second.nIndex == nIndex)
		{
			break;
		}
		else
		{
			iterMultimapLocales_TabPage	=	this->m_multimapLocales_TabPage.end();
			break;
		}
	}

	if (iterMultimapLocales_TabPage != this->m_multimapLocales_TabPage.end())
	{
		iterMultimapLocales_TabPage->second.pstrLabel_en_US	=	pstrLabel_en_US;
	}
	else
	{
		WMZqt::TTabHeaderLocaleInfo	tabHeaderLocalInfo(pTabWidget, nIndex, pstrLabel_en_US);
		this->m_multimapLocales_TabPage.insert(std::make_pair(pTabWidget, tabHeaderLocalInfo));
	}
}

int32_t	WMZqt::Locale::OpenTableSupportedLocales()
{
	QString	qstrTableName	=	QString("tbl_locale");
	if (this->IsTableExists(qstrTableName))
	{
		return WMZqt::E_Errno_SUCCESS;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::OpenTableSupportedLocales(\"%s\") begin ...\n",
		qstrTableName.toStdString().c_str());

	QString		qstrStmt =
		QString("CREATE TABLE ") + qstrTableName + "(name VARCHAR(255) PRIMARY KEY, value VARCHAR(255))";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::Locale::OpenTableSupportedLocales(\"%s\") failed: SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrTableName.toStdString().c_str(),
			qstrStmt.toStdString().c_str(),
			query.lastError().text().toStdString().c_str());

		return WMZqt::E_Errno_ERR_GENERAL;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::OpenTableSupportedLocales(\"%s\") successfully .\n",
		qstrTableName.toStdString().c_str());

	return WMZqt::E_Errno_SUCCESS;
}

int32_t	WMZqt::Locale::OpenTableLocale(const QString& qstrLocale, bool bTruncate)
{
	QString	qstrLocaleTableName	=	QString("tbl_") + qstrLocale;
	if (this->IsTableExists(qstrLocaleTableName))
	{
		if (!bTruncate)
		{
			return WMZqt::E_Errno_SUCCESS;
		}

		QString		qstrStmt =
			QString("DROP TABLE ") + qstrLocaleTableName;
		QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
		if (!query.exec(qstrStmt))
		{
			WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
				"WMZqt::Locale::OpenTableLocale(\"%s\") failed: failed on DROP TABLE, SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
				qstrLocaleTableName.toStdString().c_str(),
				qstrStmt.toStdString().c_str(),
				query.lastError().text().toStdString().c_str());

			return WMZqt::E_Errno_ERR_GENERAL;
		}
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::OpenTableLocale(\"%s\") begin ...\n",
		qstrLocaleTableName.toStdString().c_str());

	QString		qstrStmt	=
		QString("CREATE TABLE ") + qstrLocaleTableName + "(name VARCHAR(255) PRIMARY KEY, value VARCHAR(255))";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Error,
			"WMZqt::Locale::OpenTableLocale(\"%s\") failed: SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrLocaleTableName.toStdString().c_str(),
			qstrStmt.toStdString().c_str(),
			query.lastError().text().toStdString().c_str());

		return WMZqt::E_Errno_ERR_GENERAL;
	}

	WMZqt::LogMgr::Instance()->Log(NULL, WMZqt_Log_Header_Info, WMZqt::E_LogLevel_Debug,
		"WMZqt::Locale::OpenTableLocale(\"%s\") successfully .\n",
		qstrLocaleTableName.toStdString().c_str());

	return WMZqt::E_Errno_SUCCESS;
}

int32_t	WMZqt::Locale::PersistenceLocale(const QString& qstrLocaleName, const QString& qstrLocaleValue,
	std::map< QString, QString >& mapLocaleStrings, bool bTruncate)
{
	int32_t	nErrno	=	WMZqt::E_Errno_ERR_GENERAL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		nErrno	=	this->OpenTableLocale(qstrLocaleName, bTruncate);
		if (nErrno != WMZqt::E_Errno_SUCCESS)
		{
			break;
		}

		QSqlDatabase	dbLocale = QSqlDatabase::database(this->m_qstrDBName);

		if (!bTruncate)
		{
			QString	qstrStmtList	=
				QString("SELECT name, value FROM tbl_") + qstrLocaleName;
			QSqlQuery	queryList(dbLocale);
			if (queryList.exec(qstrStmtList))
			{
				while (queryList.next())
				{
					mapLocaleStrings.insert(std::make_pair(queryList.value(0).toString(), queryList.value(1).toString()));
				}
			}

			this->OpenTableLocale(qstrLocaleName, true);
		}

		// save locale strings to DB
		// {
			// begin transaction
			dbLocale.transaction();

			std::map< QString, QString >::const_iterator	iterMapLocaleStrings;
			for (iterMapLocaleStrings = mapLocaleStrings.begin(); iterMapLocaleStrings != mapLocaleStrings.end(); ++iterMapLocaleStrings)
			{
				QString	qstrStmt	=
					QString("INSERT INTO tbl_") + qstrLocaleName + "(name, value) VALUES('" + iterMapLocaleStrings->first + "', '" + iterMapLocaleStrings->second + "')";
				QSqlQuery	query(dbLocale);
				query.exec(qstrStmt);
			}

			// finish transaction
			dbLocale.commit();
		// }

		// update supported locale
		{
			QString	qstrStmt_set;

			QString	qstrStmt_get	=
				QString("SELECT value FROM tbl_locale WHERE name='") + qstrLocaleName + "'";
			QSqlQuery	query_get(dbLocale);
			query_get.exec(qstrStmt_get);
			if (query_get.next())
			{
				qstrStmt_set	=	QString("UPDATE tbl_locale SET value='") + qstrLocaleValue + "' WHERE name='" + qstrLocaleName + "'";
			}
			else
			{
				qstrStmt_set	=	QString("INSERT INTO tbl_locale(name, value) VALUES ('") + qstrLocaleName + "', '" + qstrLocaleValue + "')";
			}

			QSqlQuery	query_set(dbLocale);
			query_set.exec(qstrStmt_set);
		}

		nErrno	=	WMZqt::E_Errno_SUCCESS;
	}

	return nErrno;
}

