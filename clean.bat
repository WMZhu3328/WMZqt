@echo off

:: clean project files for Visual Studio
echo clean project files for Visual Studio

SET FILE_ITEM=.qmake.stash
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=WMZqt.sln
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET DIR_ITEM=.vs
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=bin
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=lib
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)

SET DIR_ITEM=src\WMZqt\debug
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=src\WMZqt\release
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET FILE_ITEM=src\WMZqt\WMZqt.vcxproj
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\WMZqt\WMZqt.vcxproj.filters
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\WMZqt\WMZqt.vcxproj.user
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)

SET DIR_ITEM=src\WMZqt_locale\debug
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=src\WMZqt_locale\release
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET FILE_ITEM=src\WMZqt_locale\WMZqt_locale.vcxproj
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\WMZqt_locale\WMZqt_locale.vcxproj.filters
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\WMZqt_locale\WMZqt_locale.vcxproj.user
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\WMZqt_locale\WMZqt_locale_resource.rc
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
