
#include "WMZqt/event.h"

void    WMZqt::PostQuitEvent(const QString& qstrHintMsg)
{
    emit WMZqt::EventMgr::Instance()->signal_QuitEvent(qstrHintMsg);
}

WMZqt::EventMgr* WMZqt::EventMgr::m_spInstance = NULL;

