
#include <QApplication>
#include <QDir>
#include "main_window.h"

int main(int argc, char* argv[])
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());

	QCoreApplication::addLibraryPath(".");
	qRegisterMetaType< WMZqt_SPLogItem >("WMZqt_SPLogItem");

	QApplication app(argc, argv);

	WMZqt::App::Instance()->SetAppGroupName("WMZqt");
	WMZqt::App::Instance()->SetAppName("WMZqt_locale");
	WMZqt::App::Instance()->SetAppVersion("1.0");
	WMZqt::App::Instance()->SetAppDisplayName("WMZqt locale");

	WMZqt::App::Instance()->SetVendor("Wasington Michael Zhu");
	WMZqt::App::Instance()->SetVendorDomain("https://github.com/WMZhu3328/WMZqt.git");
	WMZqt::App::Instance()->SetVendorEmail("WMZhu3328@hotmail.com");

	MainWindow	mainWindow;
	mainWindow.show();

	return app.exec();
}

