
#include "WMZqt/common.h"
#include <QApplication>

namespace WMZqt
{
	static volatile bool	s_bIsMainWindowCreated	=	false;
};

bool	WMZqt::IsZSTR(const char* pstr)
{
	if (pstr == NULL)
	{
		return true;
	}

	if (::strlen(pstr) == 0)
	{
		return true;
	}

	return false;
}

