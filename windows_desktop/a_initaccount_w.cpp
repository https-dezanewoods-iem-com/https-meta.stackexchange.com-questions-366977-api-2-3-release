// Copyright 2019 The MWC Developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "a_initaccount_w.h"
#include "ui_a_initaccount.h"
#include "../util/passwordanalyser.h"
#include "../state/a_initaccount.h"
#include "../util_desktop/widgetutils.h"
#include "../control_desktop/messagebox.h"
#include <QShortcut>
#include <QKeyEvent>
#include "../util_desktop/timeoutlock.h"
#include "../dialogs_desktop/x_walletinstances.h"
#include "../bridge/util_b.h"
#include "../bridge/wnd/a_initaccount_b.h"
#include "../bridge/wnd/y_selectmode_b.h"
#include "../core/Config.h"

namespace wnd {

InitAccount::InitAccount(QWidget *parent) :
    core::PanelBaseWnd(parent),
    ui(new Ui::InitAccount)
{
    ui->setupUi(this);

    selectMode = new bridge::SelectMode(this);
    initAccount = new bridge::InitAccount(this);
    util = new bridge::Util(this);

    util->passwordQualitySet( ui->password1Edit->text() );
    ui->strengthLabel->setText( util->passwordQualityComment() );
    ui->submitButton->setEnabled( util->passwordQualityIsAcceptable() );

    ui->password1Edit->installEventFilter(this);

    utils::defineDefaultButtonSlot(this, SLOT(on_submitButton_clicked()) );
    updatePassState();
}

InitAccount::~InitAccount()
{
    util->releasePasswordAnalyser();
    delete ui;
}

void InitAccount::on_password1Edit_textChanged(const QString &text)
{
    QPair <bool, QString> valRes = util::validateMwc713Str(text, true);
    if (!valRes.first) {
        ui->strengthLabel->setText( valRes.second );
        ui->submitButton->setEnabled( false );
    }

    util->passwordQualitySet(text);
    ui->strengthLabel->setText( util->passwordQualityComment() );
    ui->submitButton->setEnabled( util->passwordQualityIsAcceptable() );

    updatePassState();
}

void InitAccount::on_password2Edit_textChanged( const QString &text )
{
    Q_UNUSED(text)
    updatePassState();
}

void InitAccount::updatePassState() {
    QString pswd1 = ui->password1Edit->text();
    QString pswd2 = ui->password2Edit->text();

    if (pswd2.isEmpty()) {
        ui->confirmPassLable->hide();
    }
    else {
        ui->confirmPassLable->show();
        ui->confirmPassLable->setPixmap( QPixmap( pswd1==pswd2 ? ":/img/PassOK@2x.svg" : ":/img/PassNotMatch@2x.svg") );
    }
}



void InitAccount::on_submitButton_clicked()
{
    util::TimeoutLockObject to("InitAccount");

    QString pswd1 = ui->password1Edit->text();
    QString pswd2 = ui->password2Edit->text();

    QPair <bool, QString> valRes = util::validateMwc713Str(pswd1, true);
    if (!valRes.first) {
        control::MessageBox::messageText(this, "Password", valRes.second );
        return;
    }

    util->passwordQualitySet(pswd1);

    if (!util->passwordQualityIsAcceptable())
        return;

    if (pswd1!=pswd2) {
        control::MessageBox::messageText(this, "Password", "Password doesn't match confirm string. Please retype the password correctly");
        return;
    }

    util->releasePasswordAnalyser();

    initAccount->setPassword(pswd1);
}

void InitAccount::on_instancesButton_clicked()
{
    util::TimeoutLockObject to("InitAccount");

    dlg::WalletInstances  walletInstances(this);
    walletInstances.exec();
}

void wnd::InitAccount::on_runOnlineNodeButton_clicked()
{
    util::TimeoutLockObject to("InitAccount");
    if ( core::WndManager::RETURN_CODE::BTN2 == control::MessageBox::questionText(this, "Running Mode",
                          "You are switching to 'Online Node'.\nOnline Node can be used as a data provider for the Cold Wallet.",
                                  "Cancel", "Continue", false, true) ) {
        // Restarting wallet in a right mode...
        // First, let's upadte a config
        selectMode->updateWalletRunMode( int(config::WALLET_RUN_MODE::ONLINE_NODE) );
    }
}

}


