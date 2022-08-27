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

#include "k_accounts.h"
#include "../wallet/wallet.h"
#include "../windows/k_accounts_w.h"
#include "../core/windowmanager.h"
#include "../core/appcontext.h"
#include "../state/statemachine.h"
#include "../core/global.h"
#include "../core/Config.h"


namespace state {


Accounts::Accounts(StateContext * context) :
    State(context, STATE::ACCOUNTS)
{
    connect( context->wallet, &wallet::Wallet::onWalletBalanceUpdated, this, &Accounts::onWalletBalanceUpdated, Qt::QueuedConnection );
    connect( context->wallet, &wallet::Wallet::onAccountCreated, this, &Accounts::onAccountCreated, Qt::QueuedConnection );
    connect( context->wallet, &wallet::Wallet::onAccountRenamed, this, &Accounts::onAccountRenamed, Qt::QueuedConnection );
    connect( context->wallet, &wallet::Wallet::onLoginResult, this, &Accounts::onLoginResult, Qt::QueuedConnection );

    connect(context->wallet, &wallet::Wallet::onNodeStatus,
                     this, &Accounts::onNodeStatus, Qt::QueuedConnection);

    startingTime = 0;

    startTimer(37000);
}

Accounts::~Accounts() {}

NextStateRespond Accounts::execute() {
    if (context->appContext->getActiveWndState() != STATE::ACCOUNTS)
        return NextStateRespond(NextStateRespond::RESULT::DONE);

    if (wnd==nullptr) {
        wnd  = (wnd::Accounts*)context->wndManager->switchToWindowEx( mwc::PAGE_K_ACCOUNTS,
                new wnd::Accounts( context->wndManager->getInWndParent(), this ));
    }

    return NextStateRespond( NextStateRespond::RESULT::WAIT_FOR_ACTION );
}

// get balance for current account
QVector<wallet::AccountInfo> Accounts::getWalletBalance() {
    return context->wallet->getWalletBalance();
}

void Accounts::updateWalletBalance() {
    context->wallet->updateWalletBalance(true,true);
}

void Accounts::doTransferFunds() {
    // Calling the next page
    context->stateMachine->setActionWindow( STATE::ACCOUNT_TRANSFER, true );
}

// add new account
void Accounts::createAccount(QString account) {
    context->wallet->createAccount(account);
}

void Accounts::onAccountCreated( QString newAccountName) {
    Q_UNUSED(newAccountName);
    if (wnd) {
        wnd->refreshWalletBalance();
    }
}

void Accounts::onWalletBalanceUpdated() {
    if (wnd) {
        wnd->refreshWalletBalance();
    }
}

void Accounts::onAccountRenamed(bool success, QString errorMessage) {
    if (wnd) {
         wnd->onAccountRenamed(success, errorMessage);
    }
}

QVector<int> Accounts::getColumnsWidhts() const {
    return context->appContext->getIntVectorFor("AccountTblColWidth");
}

void Accounts::updateColumnsWidhts(const QVector<int> & widths) {
    context->appContext->updateIntVectorFor("AccountTblColWidth", widths);
}

// ui caller must be at waiting state
void Accounts::renameAccount( const wallet::AccountInfo & account, QString newName ) {
    context->wallet->renameAccount( account.accountName, newName );
}

void Accounts::deleteAccount( const wallet::AccountInfo & account ) {
    // Delete is rename. Checking for names collision

    QVector<wallet::AccountInfo> allAccounts = context->wallet->getWalletBalance(false);

    QString newName;

    for (int t=0;t<100;t++) {
        newName = mwc::DEL_ACCONT_PREFIX + account.accountName + (t==0?"":("_" + QString::number(t)));
        bool collision = false;
        for ( auto & acc : allAccounts ) {
            if (acc.accountName ==  newName) {
                collision = true;
                break;
            }
        }
        if (!collision)
            break;
    }

    renameAccount( account, newName );
}

void Accounts::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);

    // Skipping first 5 seconds after start. Let's mwc-node get online
    if ( startingTime==0 || QDateTime::currentMSecsSinceEpoch() - startingTime < 5000 )
        return;

    if ( !context->wallet->isWalletRunningAndLoggedIn() ) {
        startingTime=0;
        return;
    }


    if ( isNodeHealthy() ) {
        if (!lastNodeIsHealty) {
            notify::appendNotificationMessage(notify::MESSAGE_LEVEL::INFO,
                                              "MWC-Node that wallet connected to is healthy now. Wallet can validate stored data with blockchain.");
            lastNodeIsHealty = true;
        }
        else {
            context->wallet->updateWalletBalance(true, false);
        }
    }
    else {
        if (lastNodeIsHealty) {
            notify::appendNotificationMessage(notify::MESSAGE_LEVEL::WARNING,
                                              "Wallet connected to not healthy MWC-Node. Your balance, transactions and output status might be not accurate");
            lastNodeIsHealty = false;
        }
        context->wallet->updateWalletBalance(false, false);
    };

}

void Accounts::onLoginResult(bool ok) {
    Q_UNUSED(ok)
    startingTime = QDateTime::currentMSecsSinceEpoch();
}


void Accounts::onNodeStatus( bool online, QString errMsg, int nodeHeight, int peerHeight, int64_t totalDifficulty, int connections ) {
    Q_UNUSED(errMsg)
    nodeIsHealthy = online &&
                    ((config::isColdWallet() || connections > 0) && totalDifficulty > 0 && nodeHeight > peerHeight - 5);
}



}

