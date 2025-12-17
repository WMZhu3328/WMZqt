
#ifndef WMZqt_ERRNO_H
#define WMZqt_ERRNO_H

namespace WMZqt
{
	enum ET_Errno
	{
		E_Errno_SUCCESS		=	0,
		E_Errno_ERR_GENERAL	=	-1,

		E_Errno_ERR_INVALID_PARAMS	=	-2,
		E_Errno_ERR_INVALID_STATUS	=	-3,
		E_Errno_ERR_NO_MORE_MEM		=	-4,
		E_Errno_ERR_NO_MORE_RES		=	-5,
		E_Errno_ERR_ACCESS_DENIED	=	-6,
		E_Errno_ERR_EXEC_SQL		=	-7,
	};
};

#endif //end WMZqt_ERRNO_H

