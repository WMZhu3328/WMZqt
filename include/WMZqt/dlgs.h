
#ifndef WMZqt_DLGS_H
#define WMZqt_DLGS_H

#include "WMZqt/types.h"
#include <QDialog>

class QComboBox;
class QLineEdit;
class QCheckBox;
class QPushButton;
namespace WMZqt
{
	namespace Dlgs
	{
		WMZqt_API	int32_t GetInput(const QString& qstrTitle, WMZqt_TVecNVItems& vecNVItems);
		WMZqt_API	int32_t	GetLoginInput(WMZqt_TLoginInfo& loginInfo,
								const WMZqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount);

		WMZqt_API	void	ShowInformationMsgBox(const QString& qstrTitle, const QString& qstrMsg);
		WMZqt_API	void	ShowWarningMsgBox(const QString& qstrTitle, const QString& qstrMsg);
		WMZqt_API	void	ShowCriticalMsgBox(const QString& qstrTitle, const QString& qstrMsg);
			// Yes - WMZqt::E_Errno_SUCCESS, No - else
		WMZqt_API	int32_t	ExecQuestionMsgBox(const QString& qstrTitle, const QString& qstrMsg);

		class WMZqt_API DlgLogin: public QDialog
		{
			Q_OBJECT

		public:
			DlgLogin(const WMZqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount);
			~DlgLogin();

			void	GetCurLoginInfo(WMZqt_TLoginInfo& curLoginInfo) const;

		enum ET_RetCode
		{
			E_RetCode_CreateAccount	=	101,
			E_RetCode_Login			=	102,
			E_RetCode_Cancel		=	103,
		};

		protected:
			QPushButton*	m_pPushButton_Login;
			QPushButton*	m_pPushButton_Cancel;

			QComboBox*		m_pComboBox_Account;
			QLineEdit*		m_pLineEdit_Password;

			QCheckBox*		m_pCheckBox_SaveAccount;
			QCheckBox*		m_pCheckBox_SavePassword;
			QCheckBox*		m_pCheckBox_AutoLogin;

			QPushButton*	m_pPushButton_CreateAccount;

			const WMZqt_TVecLoginInfos&	m_vecLoginInfos;

		protected slots:
			void	slot_OnComboBoxTextChanged_Account(const QString& text);

			void	slot_OnCheckBoxStateChanged_SaveAccount(int state);
			void	slot_OnCheckBoxStateChanged_SavePassword(int state);
			void	slot_OnCheckBoxStateChanged_AutoLogin(int state);

			void	slot_OnPushButtonClicked_CreateAccount();

			void	slot_OnPushButtonClicked_Login();
			void	slot_onPushButtonClicked_Cancel();
		};
	};
};

#endif //end WMZqt_DLGS_H

