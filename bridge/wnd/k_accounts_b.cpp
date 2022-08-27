// Copyright 2020 The MWC Developers
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

#include "k_accounts_b.h"
#include "../BridgeManager.h"
#include "../../state/state.h"
#include "../../wallet/wallet.h"
#include "../../state/k_accounts.h"
#include "../../core/global.h"
#include "../../core/HodlStatus.h"
#include <QMap>

namespace bridge {

static state::Accounts * getState() { return (state::Accounts *) state::getState(state::STATE::ACCOUNTS); }

Accounts::Accounts(QObject * parent) : QObject(parent) {
    bridge::getBridgeManager()->addAccounts(this);
}

Accounts::~Accounts() {
    bridge::getBridgeManager()->removeAccounts(this);
}

// Return data ready to insert into the Accounts table
// Data returned in series of 6 (isHodl==true) or 5 (isHodl==false)
// [accountName, Spendable, Awaiting, Locked, Total, [in Hodl]]
QVector<QString> Accounts::getAccountsBalancesToShow(bool inHodl) {
    wallet::Wallet * wallet = state::getStateContext()->wallet;
    core::HodlStatus * hodlStatus = state::getStateContext()->hodlStatus;

    QVector<wallet::AccountInfo> accounts = wallet->getWalletBalance();
    const QMap<QString, QVector<wallet::WalletOutput> > & walletOutputs = wallet->getwalletOutputs();
    QVector<QString> result;

    for (auto & acc : accounts) {
        QVector<QString> data{ acc.accountName, util::nano2one(acc.currentlySpendable), util::nano2one(acc.awaitingConfirmation),
                               util::nano2one(acc.lockedByPrevTransaction), util::nano2one(acc.total) };

        if ( inHodl ) {
            QVector<wallet::WalletOutput> accountOutputs = walletOutputs.value(acc.accountName);

            int64_t hodlBalancePerClass = 0;

            for (const auto & out : accountOutputs) {
                core::HodlOutputInfo hOut = hodlStatus->getHodlOutput("", out.outputCommitment );
                if ( hOut.isValid() ) {
                    hodlBalancePerClass += out.valueNano;
                }
            }

            data.push_back( util::nano2one(hodlBalancePerClass) );
        }

        result.append(data);
    }
    return result;
}

// Validate new account name. Return Empty string for ok.
// Otherwise return error message
QString Accounts::validateNewAccountName(QString accountName) {
    if (accountName.startsWith("-"))
        return "You can't start account name from '-' symbol.";

    for ( auto & pref : mwc::BANNED_ACCOUT_PREFIXES ) {
        if (accountName.startsWith(pref))
            return "Please specify account name without prefix '" + pref + "'";
    }

    QPair <bool, QString> valRes = util::validateMwc713Str( accountName );
    if (!valRes.first) {
        return valRes.second;
    }

    // Check for account names
    {
        wallet::Wallet * wallet = state::getStateContext()->wallet;
        QVector<wallet::AccountInfo> accounts = wallet->getWalletBalance();

        for (auto & acc : accounts) {
            if (acc.accountName == accountName) {
                return "Account with name '" + accountName + "' already exists. Please specify a unique account name to create.";
            }
        }
    }
    return "";
}

// Check if can delete the account
bool Accounts::canDeleteAccount(QString accountName) {
    wallet::Wallet * wallet = state::getStateContext()->wallet;
    QVector<wallet::AccountInfo> accounts = wallet->getWalletBalance();

    for (auto & acc : accounts) {
        if (acc.accountName == accountName)
            return acc.canDelete();
    }

    return true;
}

// Switch to transfer funds page
void Accounts::doTransferFunds() {
    getState()->doTransferFunds();
}

// Delete account (wallet can do rename only with prefix that will be hidden after)
// Check Signal: sgnAccountRenamed(bool success, QString errorMessage);
void Accounts::deleteAccount( QString accountName ) {
    getState()->deleteAccount(accountName);
}



}
