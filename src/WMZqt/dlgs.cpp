
#include "WMZqt/dlgs.h"
#include "WMZqt/locale.h"
#include "WMZqt/threaded/log.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QCompleter>

#include <QMessageBox>

namespace WMZqt
{
	namespace Dlgs
	{
		class DlgInput : public QDialog
		{
			//Q_OBJECT

		public:
			DlgInput(const QString& qstrTitle, WMZqt_TVecNVItems& vecNVItems);
			virtual ~DlgInput();

			QString	GetValue(int nIndex);

		protected:
			QPushButton*	m_pPushButton_Ok;
			QPushButton*	m_pPushButton_Cancel;

			std::vector< QLineEdit* >	m_vecLineEdits;
		};
	};
};

WMZqt::Dlgs::DlgInput::DlgInput(const QString& qstrTitle, WMZqt_TVecNVItems& vecNVItems)
	:QDialog(nullptr)
{
	this->setWindowTitle(qstrTitle);

	QGridLayout*	pGridLayout_Main	=	new QGridLayout(this);
	this->setLayout(pGridLayout_Main);

	int	nCntItems	=	(int)vecNVItems.size();
	for (int i=0; i<nCntItems; ++i)
	{
		QLabel*		pLabel_Name = new QLabel(vecNVItems[i].qstrName, this);
		QLineEdit*	pLineEdit_Value = new QLineEdit(vecNVItems[i].qstrValue, this);
		if (vecNVItems[i].bIsPassword)
			pLineEdit_Value->setEchoMode(QLineEdit::Password);
		if (vecNVItems[i].bIsReadOnly)
		{
			pLineEdit_Value->setEnabled(false);
		}

		pGridLayout_Main->addWidget(pLabel_Name, i, 0);
		pGridLayout_Main->addWidget(pLineEdit_Value, i, 1);

		this->m_vecLineEdits.push_back(pLineEdit_Value);
	}

	this->m_pPushButton_Ok		=	new QPushButton(WMZqt_T("Ok"), this);
	this->m_pPushButton_Cancel	=	new QPushButton(WMZqt_T("Cancel"), this);
	pGridLayout_Main->addWidget(this->m_pPushButton_Ok, nCntItems, 1);
	pGridLayout_Main->addWidget(this->m_pPushButton_Cancel, nCntItems, 0);

	connect(this->m_pPushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
	connect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

WMZqt::Dlgs::DlgInput::~DlgInput()
{
	disconnect(this->m_pPushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
	disconnect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString	WMZqt::Dlgs::DlgInput::GetValue(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= this->m_vecLineEdits.size()))
	{
		return "";
	}

	return this->m_vecLineEdits[nIndex]->text();
}

int32_t WMZqt::Dlgs::GetInput(const QString& qstrTitle, WMZqt_TVecNVItems& vecNVItems)
{
	int	nCntItems = (int)vecNVItems.size();

	if (nCntItems <= 0)
	{
		return WMZqt::E_Errno_ERR_INVALID_PARAMS;
	}

	WMZqt::Dlgs::DlgInput	dlgInput(qstrTitle, vecNVItems);
	if (dlgInput.exec() != QDialog::Accepted)
	{
		return WMZqt::E_Errno_ERR_GENERAL;
	}

	for (int i = 0; i < nCntItems; ++i)
	{
		vecNVItems[i].qstrValue = dlgInput.GetValue(i);
	}

	return WMZqt::E_Errno_SUCCESS;
}

int32_t WMZqt::Dlgs::GetLoginInput(WMZqt_TLoginInfo& loginInfo,
								const WMZqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount)
{
	loginInfo.Clear();

	int32_t	nRetCode	=	WMZqt::Dlgs::DlgLogin::E_RetCode_Cancel;

	WMZqt::Dlgs::DlgLogin	dlgLogin(vecLoginInfos, qstrLastLogonAccount);
	nRetCode	=	dlgLogin.exec();
	if (nRetCode == WMZqt::Dlgs::DlgLogin::E_RetCode_Login)
	{
		dlgLogin.GetCurLoginInfo(loginInfo);
	}

	return nRetCode;
}

		// Yes - WMZqt::E_Errno_SUCCESS, No - else
void	WMZqt::Dlgs::ShowInformationMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxInfo(QMessageBox::Information, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxInfo.setButtonText(QMessageBox::Ok, WMZqt_T("Ok"));

	msgBoxInfo.exec();
}

void	WMZqt::Dlgs::ShowWarningMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxWarning(QMessageBox::Warning, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxWarning.setButtonText(QMessageBox::Ok, WMZqt_T("Ok"));

	msgBoxWarning.exec();
}

void	WMZqt::Dlgs::ShowCriticalMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxWarning(QMessageBox::Critical, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxWarning.setButtonText(QMessageBox::Ok, WMZqt_T("Ok"));

	msgBoxWarning.exec();
}

		// Yes - 0, No - else
int		WMZqt::Dlgs::ExecQuestionMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxQuestion(QMessageBox::Question, qstrTitle, qstrMsg, QMessageBox::Yes | QMessageBox::No);
	msgBoxQuestion.setButtonText(QMessageBox::Yes, WMZqt_T("Yes"));
	msgBoxQuestion.setButtonText(QMessageBox::No, WMZqt_T("No"));

	return (msgBoxQuestion.exec() == QMessageBox::Yes) ? WMZqt::E_Errno_SUCCESS : WMZqt::E_Errno_ERR_GENERAL;
}

WMZqt::Dlgs::DlgLogin::DlgLogin(const WMZqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount)
	:m_vecLoginInfos(vecLoginInfos)
{
	this->setWindowTitle(WMZqt_T("User Login"));

	QGridLayout*	pGridLayout_Main	=	new QGridLayout(this);
	this->setLayout(pGridLayout_Main);

	int	nLineNo	=	0;

	QLabel*	pLabel_Account		=	new QLabel(WMZqt_T("Account"), this);
	this->m_pComboBox_Account	=	new QComboBox(this);
	pGridLayout_Main->addWidget(pLabel_Account, nLineNo, 1);
	pGridLayout_Main->addWidget(this->m_pComboBox_Account, nLineNo, 2);
	++ nLineNo;

	QLabel*	pLabel_Password		=	new QLabel(WMZqt_T("Password"), this);
	this->m_pLineEdit_Password	=	new QLineEdit(this);
	pGridLayout_Main->addWidget(pLabel_Password, nLineNo, 1);
	pGridLayout_Main->addWidget(this->m_pLineEdit_Password, nLineNo, 2);
	++ nLineNo;

	this->m_pCheckBox_SaveAccount	=	new QCheckBox(WMZqt_T("save account"), this);
	this->m_pCheckBox_SavePassword	=	new QCheckBox(WMZqt_T("save password"), this);
	this->m_pCheckBox_AutoLogin		=	new QCheckBox(WMZqt_T("auto login"), this);
	pGridLayout_Main->addWidget(this->m_pCheckBox_SaveAccount, nLineNo, 0);
	pGridLayout_Main->addWidget(this->m_pCheckBox_SavePassword, nLineNo, 1);
	pGridLayout_Main->addWidget(this->m_pCheckBox_AutoLogin, nLineNo, 2);
	++ nLineNo;

	this->m_pPushButton_CreateAccount	=	new QPushButton(WMZqt_T("create account"), this);
	pGridLayout_Main->addWidget(this->m_pPushButton_CreateAccount, nLineNo, 2);
	++ nLineNo;

	this->m_pPushButton_Login	=	new QPushButton(WMZqt_T("Login"), this);
	this->m_pPushButton_Cancel	=	new QPushButton(WMZqt_T("Cancel"), this);
	pGridLayout_Main->addWidget(this->m_pPushButton_Login, nLineNo, 2);
	pGridLayout_Main->addWidget(this->m_pPushButton_Cancel, nLineNo, 1);
	++ nLineNo;

	this->m_pPushButton_Login->setDefault(true);

	// prepare data
	QStringList	qslLogonAccounts;
	WMZqt_TVecLoginInfos::const_iterator	iterVecLoginInfos;
	for (iterVecLoginInfos = vecLoginInfos.begin(); iterVecLoginInfos != vecLoginInfos.end(); ++iterVecLoginInfos)
	{
		qslLogonAccounts << iterVecLoginInfos->qstrAccount;
	}

	// set control attributes
	this->m_pComboBox_Account->setEditable(true);
	this->m_pComboBox_Account->addItems(qslLogonAccounts);
		QCompleter*	pCompleter	=	new QCompleter(qslLogonAccounts, this);
		pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
		this->m_pComboBox_Account->setCompleter(pCompleter);

	this->m_pLineEdit_Password->setEchoMode(QLineEdit::QLineEdit::Password);

	this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);

	connect(this->m_pComboBox_Account, SIGNAL(editTextChanged(const QString&)), this, SLOT(slot_OnComboBoxTextChanged_Account(const QString&)));
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	connect(this->m_pPushButton_CreateAccount, &QPushButton::clicked, this, &WMZqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_CreateAccount);

	connect(this->m_pPushButton_Login, &QPushButton::clicked, this, &WMZqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_Login);
	connect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &WMZqt::Dlgs::DlgLogin::slot_onPushButtonClicked_Cancel);

	if (!qstrLastLogonAccount.isEmpty())
	{
		this->m_pLineEdit_Password->setFocus();
	}

	// prepare last logon data
	this->m_pComboBox_Account->setCurrentText(qstrLastLogonAccount);
	emit this->m_pComboBox_Account->editTextChanged(qstrLastLogonAccount);
}

WMZqt::Dlgs::DlgLogin::~DlgLogin()
{
	disconnect(this->m_pComboBox_Account, SIGNAL(editTextChanged(const QString&)), this, SLOT(slot_OnComboBoxTextChanged_Account(const QString&)));
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	disconnect(this->m_pPushButton_Login, &QPushButton::clicked, this, &QDialog::accept);
	disconnect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

void	WMZqt::Dlgs::DlgLogin::GetCurLoginInfo(WMZqt_TLoginInfo& curLoginInfo) const
{
	curLoginInfo.Clear();

	curLoginInfo.qstrAccount	=	this->m_pComboBox_Account->currentText();
	curLoginInfo.qstrPassword	=	this->m_pLineEdit_Password->text();
	curLoginInfo.bSaveAccount	=	(this->m_pCheckBox_SaveAccount->checkState() == Qt::Checked) ? true : false;
	curLoginInfo.bSavePassword	=	(this->m_pCheckBox_SavePassword->checkState() == Qt::Checked) ? true : false;
	curLoginInfo.bAutoLogin		=	(this->m_pCheckBox_AutoLogin->checkState() == Qt::Checked) ? true : false;
}

void	WMZqt::Dlgs::DlgLogin::slot_OnComboBoxTextChanged_Account(const QString& text)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	int	nIndex	=	-1;
	int	nCntLoginInfos	=	(int)(this->m_vecLoginInfos.size());
	for (int i=0; i<nCntLoginInfos; ++i)
	{
		if (this->m_vecLoginInfos[i].qstrAccount == text)
		{
			nIndex	=	i;
			break;
		}
	}

	if (nIndex != -1)
	{
		this->m_pLineEdit_Password->setText(this->m_vecLoginInfos[nIndex].qstrPassword);

		this->m_pCheckBox_SaveAccount->setCheckState(this->m_vecLoginInfos[nIndex].bSaveAccount ? Qt::Checked : Qt::Unchecked);
		this->m_pCheckBox_SavePassword->setCheckState(this->m_vecLoginInfos[nIndex].bSavePassword ? Qt::Checked : Qt::Unchecked);
		this->m_pCheckBox_AutoLogin->setCheckState(this->m_vecLoginInfos[nIndex].bAutoLogin ? Qt::Checked : Qt::Unchecked);
	}
	else
	{
		this->m_pLineEdit_Password->setText("");
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	WMZqt::Dlgs::DlgLogin::slot_OnCheckBoxStateChanged_SaveAccount(int state)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	if (state == Qt::Checked)
	{
		//do-nothing
	}
	else
	{
		this->m_pCheckBox_SavePassword->setCheckState(Qt::Unchecked);
		this->m_pCheckBox_AutoLogin->setCheckState(Qt::Unchecked);
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	WMZqt::Dlgs::DlgLogin::slot_OnCheckBoxStateChanged_SavePassword(int state)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	if (state == Qt::Checked)
	{
		this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);
	}
	else
	{
		this->m_pCheckBox_AutoLogin->setCheckState(Qt::Unchecked);
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	WMZqt::Dlgs::DlgLogin::slot_OnCheckBoxStateChanged_AutoLogin(int state)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	if (state == Qt::Checked)
	{
		this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);
		this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);
	}
	else
	{
		//do-nothing
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	WMZqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_CreateAccount()
{
	QDialog::done(WMZqt::Dlgs::DlgLogin::E_RetCode_CreateAccount);
}

void	WMZqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_Login()
{
	if (this->m_pComboBox_Account->currentText().isEmpty())
	{
		WMZqt::Dlgs::ShowWarningMsgBox(WMZqt_T("User Login"), WMZqt_T("account cannot be EMPTY"));

		this->m_pComboBox_Account->setFocus();
	}
	else if (this->m_pLineEdit_Password->text().isEmpty())
	{
		WMZqt::Dlgs::ShowWarningMsgBox(WMZqt_T("User Login"), WMZqt_T("password cannot be EMPTY"));

		this->m_pLineEdit_Password->setFocus();
	}
	else
	{
		QDialog::done(WMZqt::Dlgs::DlgLogin::E_RetCode_Login);
	}
}

void	WMZqt::Dlgs::DlgLogin::slot_onPushButtonClicked_Cancel()
{
	QDialog::done(WMZqt::Dlgs::DlgLogin::E_RetCode_Cancel);
}

