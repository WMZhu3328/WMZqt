TEMPLATE	=	lib
DEFINES		+=	WMZqt_BUILD_DLL
QT			+=	core gui widgets sql xml
INCLUDEPATH	+=	$$PWD/../../include
DESTDIR		=	$$PWD/../../lib
win32: DLLDESTDIR	=	$$PWD/../../bin
#win32 {
#	CONFIG(debug, debug|release):{
#		DESTDIR	=	$$PWD/../../lib/debug
#	}
#	else:CONFIG(release, debug|release):{
#		DESTDIR	=	$$PWD/../../lib/release
#	}
#}

HEADERS += \
	$$PWD/../../include/WMZqt/errno.h \
	$$PWD/../../include/WMZqt/consts.h \
	$$PWD/../../include/WMZqt/export.h \
	$$PWD/../../include/WMZqt/event.h \
	$$PWD/../../include/WMZqt/types.h \
	$$PWD/../../include/WMZqt/tree_widget.h \
	$$PWD/../../include/WMZqt/common.h \
	$$PWD/../../include/WMZqt/app.h \
	$$PWD/../../include/WMZqt/db_app_base.h \
	$$PWD/../../include/WMZqt/locale.h \
	$$PWD/../../include/WMZqt/threaded/log.h \
	$$PWD/../../include/WMZqt/dlgs.h

SOURCES += \
	$$PWD/event.cpp \
	$$PWD/common.cpp \
	$$PWD/app.cpp \
	$$PWD/db_app_base.cpp \
	$$PWD/locale.cpp \
	$$PWD/threaded/log.cpp \
	$$PWD/dlgs.cpp

