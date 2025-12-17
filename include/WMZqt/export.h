
#ifndef WMZqt_EXPORT_H
#define WMZqt_EXPORT_H

#include <QObject>

#ifdef WMZqt_BUILD_DLL
#	define WMZqt_API	Q_DECL_EXPORT
#else //!WMZqt_BUILD_DLL
#	define WMZqt_API	Q_DECL_IMPORT
#endif //end WMZqt_BUILD_DLL + !WMZqt_BUILD_DLL

#endif //end WMZqt_EXPORT_H

