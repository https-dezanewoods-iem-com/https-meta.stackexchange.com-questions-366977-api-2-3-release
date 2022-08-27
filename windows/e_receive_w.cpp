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

#include "e_receive_w.h"
#include "ui_e_receive.h"
#include <QFileInfo>
#include "../state/e_Receive.h"
#include <QFileDialog>
#include "../control/messagebox.h"
#include "../state/timeoutlock.h"
#include "../core/Config.h"

namespace wnd {

Receive::Receive(QWidget *parent, state::Receive * _state, bool mwcMqStatus, bool keybaseStatus, bool httpStatus,
                 QString mwcMqAddress, const wallet::WalletConfig & _walletConfig ) :
        core::NavWnd(parent, _state->getContext() ),
        ui(new Ui::Receive),
        state(_state),
        walletConfig(_walletConfig)
{
    ui->setupUi(this);

    ui->progress->initLoader(false);

    updateAccountList();

    if (config::isColdWallet()) {
        ui->frameQs->hide();
        ui->mwcmqAddress->hide();
    }

    if (walletConfig.hasTls())
        ui->httpLabel->setText("Https");

    updateMwcMqState(mwcMqStatus);
    updateKeybaseState(keybaseStatus);
    updateHttpState(httpStatus);
    updateMwcMqAddress(mwcMqAddress);
}

Receive::~Receive() {
    state->deletedWnd(this);
    delete ui;
}

void Receive::updateMwcMqAddress(QString address) {

    QString mqHost = walletConfig.getMwcMqHostNorm();
    if (!mqHost.isEmpty())
        mqHost = "@" + mqHost;

    mwcAddress = (config::getUseMwcMqS() ? "mwcmqs://" : "mwcmq://") + address + mqHost;
    ui->mwcmqAddress->setText( mwcAddress );
}

void Receive::updateMwcMqState(bool online) {
    ui->mwcmqStatusImg->setPixmap( QPixmap(online ? ":/img/StatusOk@2x.svg" : ":/img/StatusEmpty@2x.svg") );
    ui->mwcmqStatusLabel->setText( online ? "Online" : "Offline" );
}

void Receive::updateKeybaseState(bool online) {
    ui->keybaseStatusImg->setPixmap( QPixmap(online ? ":/img/StatusOk@2x.svg" : ":/img/StatusEmpty@2x.svg") );
    ui->keybaseStatusLabel->setText( online ? "Online" : "Offline" );
}

void Receive::updateHttpState(bool online) {
    ui->httpStatusImg->setPixmap( QPixmap(online ? ":/img/StatusOk@2x.svg" : ":/img/StatusEmpty@2x.svg") );
    ui->httpStatusLabel->setText( online ? "Online" : "Offline" );
}


void wnd::Receive::on_recieveFileButton_clicked()
{
    state::TimeoutLockObject to( state );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open initial transaction file"),
                                                    state->getFileGenerationPath(),
                                                    tr("MWC transaction (*.tx *.input);;All files (*.*)"));

    if (fileName.length()==0)
        return;

    // Update path
    QFileInfo flInfo(fileName);
    state->updateFileGenerationPath( flInfo.path() );

    ui->progress->show();

    state->signTransaction(fileName);
    // Expected respond from state with result
}

void Receive::onTransactionActionIsFinished( bool success, QString message ) {
    state::TimeoutLockObject to( state );

    ui->progress->hide();
    control::MessageBox::messageText(this, success ? "Success" : "Failure", message );
}

void Receive::stopWaiting() {
    ui->progress->hide();
}


void Receive::on_accountComboBox_activated(int index)
{
    if (index>=0 && index < accountInfo.size() )
        state->setReceiveAccount( accountInfo[index].accountName );
}

void Receive::updateAccountList() {
    accountInfo = state->getWalletBalance();
    QString selectedAccount = state->getReceiveAccount();

    int selectedAccIdx = 0;

    ui->accountComboBox->clear();

    int idx=0;
    for (auto & info : accountInfo) {
        if (info.accountName == selectedAccount)
            selectedAccIdx = idx;

        ui->accountComboBox->addItem( info.getLongAccountName(), QVariant(idx++) );
    }
    ui->accountComboBox->setCurrentIndex(selectedAccIdx);
}

void Receive::updateWalletBalance() {
    updateAccountList();
}


}


