TEMPLATE = subdirs

# project names
SUBDIRS += \
	WMZqt \
	WMZqt_locale

# where to find the projects - give the folders
WMZqt.subdir			=	src/WMZqt
WMZqt_locale.subdir	=	src/WMZqt_locale

# projects' dependencies
WMZqt_locale.depends	=	WMZqt
