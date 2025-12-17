
#ifndef WMZqt_LOG_H
#define WMZqt_LOG_H

#include "WMZqt/types.h"
#include <map>
class QTextEdit;

#define WMZqt_Log_Header_Info	__FILE__, __LINE__

namespace WMZqt
{
	enum ET_LogLevel
	{
		E_LogLevel_Error = 10,
		E_LogLevel_Warn = 20,
		E_LogLevel_Info = 30,
		E_LogLevel_Debug = 40,
		E_LogLevel_Trace = 50,
		E_LogLevel_None = 99,
	};

	struct TLogItem
	{
		ET_LogLevel	eLogLevel;
		char*		pcLog;
		int32_t		lenLog;

		TLogItem()
			:eLogLevel(WMZqt::E_LogLevel_None), pcLog(NULL), lenLog(0)
		{}
		TLogItem(WMZqt::ET_LogLevel eLogLevelParam, char* pcLogParam, int32_t lenLogParam)
			:eLogLevel(eLogLevelParam), pcLog(pcLogParam), lenLog(lenLogParam)
		{}
		~TLogItem()
		{
			eLogLevel	=	WMZqt::E_LogLevel_None;
			if (pcLog != NULL) { delete[] pcLog; pcLog = NULL; }
			lenLog = 0;
		}
	};

	// private functions
	WMZqt_API ET_LogLevel	GetLogLevel(const char* pstrLogLevel);
	WMZqt_API const char*	GetLogLevelString(WMZqt::ET_LogLevel eLogLevel);
};
typedef std::shared_ptr< WMZqt::TLogItem >	WMZqt_SPLogItem;

namespace WMZqt
{
	class WMZqt_API LogMgr : public QObject
	{
		Q_OBJECT

	public:
		LogMgr();
		~LogMgr();

		int32_t	Init(const QString& qstrLogDir, const QString& qstrLogFile, const QString& qstrLogLevelMax, bool bTruncate = false);
		int32_t	Fini();
		void	SetLogLevelMax(const QString& qstrLogLevelMax);
		void	SetCmdLineOutputOn(bool bCmdLineOutputOn);

		int32_t	SetModuleLogFile(const QString& qstrModule, const QString& qstrModuleLogDir, const QString& qstrModuleLogFile, bool bTruncate = false);

		int32_t	Log(const char* pstrModule, const char* pstrSourceFile, int32_t nLineNo,
			ET_LogLevel eLogLevel,
			const char* pcFormat, ...);

		static void	LogToTextCtrl(QTextEdit* pTextEdit, WMZqt_SPLogItem spLogItem);
		static void	LogToCmdLine(WMZqt_SPLogItem spLogItem);

	signals:
		// async signal for log event
		void	signal_DoInit(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString, QString, QString, bool);
		void	signal_DoFini(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*);	// Errno, ErrMsg
		void	signal_DoSetLogLevelMax(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString);
		void	signal_DoSetCmdLineOutputOn(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			bool);
		void	signal_DoSetModuleLogFile(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString, QString, QString, bool);

        void	signal_WriteLog(QString, WMZqt_SPLogItem);

	private slots:
		// switch thread to handle log event
		void	slot_DoInit(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString, QString, QString, bool);
		void	slot_DoFini(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*);	// Errno, ErrMsg
		void	slot_DoSetLogLevelMax(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString);
		void	slot_DoSetCmdLineOutputOn(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			bool);
		void	slot_DoSetModuleLogFile(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString, QString, QString, bool);

        void	slot_WriteLog(QString, WMZqt_SPLogItem);

		// async event handler
		void	slot_OnThreadStarted();

	private:
		QThread*		m_pThread;
		QWaitCondition	m_condThread;
		QMutex			m_mutexThread;

		ET_LogLevel	m_eLogLevelMax;

		QFile		m_mainFileLog;
		std::map< QString, QFile* >	m_mapModuleLogFiles;

		bool		m_bIsCmdLineOutputOn;

	public:
		static LogMgr*	Instance();
		static void		Destroy();

	private:
		static LogMgr*	m_spInstance;
	};
};

inline
WMZqt::LogMgr*	WMZqt::LogMgr::Instance()
{
    if (WMZqt::LogMgr::m_spInstance == NULL)
	{
		WMZqt::LogMgr::m_spInstance	=	new WMZqt::LogMgr;
	}

    return WMZqt::LogMgr::m_spInstance;
}

inline
void	WMZqt::LogMgr::Destroy()
{
    if (WMZqt::LogMgr::m_spInstance != NULL)
	{
        delete WMZqt::LogMgr::m_spInstance;
		WMZqt::LogMgr::m_spInstance	=	NULL;
	}
}

#endif //end WMZqt_LOG_H

