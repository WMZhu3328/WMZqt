
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
class QCloseEvent;
class QToolButton;
class NavMenu;
class QGroupBox;
class QLabel;
class QDateTimeEdit;
class QLineEdit;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class QTabWidget;
class QTextEdit;

#include "WMZqt/app.h"
#include "db_app.h"
#include "WMZqt/locale.h"
#include "WMZqt/threaded/log.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

signals:
	// async signal for MainWindow created event
	void	signal_OnCreateDialog();
	void	signal_OnInitDialog();

private slots:
	// switch thread to handle MainWindow created event
	void	slot_OnCreateDialog();
	void	slot_OnInitDialog();

	void	slot_OnLogWindowClosed();

	// switch thread to handle log event
	void	slot_WriteLog(QString qstrModule, WMZqt_SPLogItem spLogItem);

protected:
	virtual void	closeEvent(QCloseEvent* pEvt);

private:
	void	CreateMenuBar();
	void	CreateToolBar();
	void	CreateMainPanel();
	void	CreateStatusBar();

	//MenuBar
		//File
			//Exit
	void	OnMenuExit();
		// Tools
			//GenerateLocaleTemplate
	void	OnMenuGenerateLocaleTemplate();
			//ImportLocale
	void	OnMenuImportLocale();
		//Help
			//About
	void	OnMenuAbout();

	//ToolBar
		//Log
			//ShowOrHideLogWindow
	void	OnToolBar_ShowOrHideLogWindow();

private:
	//MenuBar
		//File
			//Exit
	QAction*	m_pMenuItem_Exit;

	//ToolBar
		//Log
			//ShowOrHideLogWindow
	QToolButton*	m_pToolButton_LogInfo;

	//Console
	QDockWidget*	m_pDockWidget_Log;
	QTextEdit*		m_pTextEdit_Log;
};

#endif //end MAIN_WINDOW_H

