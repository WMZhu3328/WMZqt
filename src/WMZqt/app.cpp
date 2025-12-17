
#include "WMZqt/app.h"
#include <QApplication>

WMZqt::App*	WMZqt::App::m_spInstance	=	NULL;

WMZqt::App::App()
{
}

WMZqt::App::~App()
{
}

void	WMZqt::App::SetCustomSetting(const QString& qstrSettingName, const QString& qstrSettingValue)
{
	if (!qstrSettingName.isEmpty())
	{
		this->m_mapSettings[qstrSettingName]	=	qstrSettingValue;
	}
}

QString	WMZqt::App::GetCustomSetting(const QString& qstrSettingName, const QString& qstrDftSettingValue)
{
	if (qstrSettingName.isEmpty())
	{
		return "";
	}

	std::map< QString, QString >::iterator	iterMapSettings	=
		this->m_mapSettings.find(qstrSettingName);
	if (iterMapSettings != this->m_mapSettings.end())
	{
		return iterMapSettings->second;
	}
	else
	{
		this->m_mapSettings[qstrSettingName]	=	qstrDftSettingValue;

		return qstrDftSettingValue;
	}
}

QString	WMZqt::App::GetAppPath() const
{
	return QApplication::applicationFilePath();
}

QString	WMZqt::App::GetAppHome() const
{
	QString	qstrBinDir	=	QApplication::applicationDirPath();
	QDir	dirBinDir(qstrBinDir);

	qstrBinDir.resize(qstrBinDir.length() - dirBinDir.dirName().length() - 1);

	return qstrBinDir;
}

QString	WMZqt::App::GetAppGroupDataHome() const
{
	QString	qstrAppGroupName(this->m_qstrAppGroupName);
	if (qstrAppGroupName.isEmpty())
	{
		qstrAppGroupName	=	"WMZqt";
	}

	return QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "." + qstrAppGroupName);
}

QString	WMZqt::App::GetAppDataHome() const
{
	QString	qstrAppName(this->m_qstrAppName);
	if (qstrAppName.isEmpty())
	{
		qstrAppName	=	"WMZqt";
	}

	return QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "." + qstrAppName);
}

