
#ifndef WMZqt_EVENT_H
#define WMZqt_EVENT_H

#include "WMZqt/export.h"
#include <QString>

namespace WMZqt
{
    WMZqt_API void   PostQuitEvent(const QString& qstrHint = "");

    class WMZqt_API EventMgr: public QObject
    {
        Q_OBJECT

    public:
        EventMgr()          {}
        virtual ~EventMgr() {}

    signals:
        void    signal_QuitEvent(QString);

        void    signal_OnLocaleChanged(QString);

    public:
        static EventMgr*    Instance();
        static void         Destroy();

    private:
        static EventMgr*    m_spInstance;
    };
};

inline
WMZqt::EventMgr* WMZqt::EventMgr::Instance()
{
    if (WMZqt::EventMgr::m_spInstance == NULL)
    {
        WMZqt::EventMgr::m_spInstance    =   new WMZqt::EventMgr();
    }

    return WMZqt::EventMgr::m_spInstance;
}

inline
void    WMZqt::EventMgr::Destroy()
{
    if (WMZqt::EventMgr::m_spInstance != NULL)
    {
        delete WMZqt::EventMgr::m_spInstance;
        WMZqt::EventMgr::m_spInstance    =   NULL;
    }
}

#endif //end WMZqt_EVENT_H

