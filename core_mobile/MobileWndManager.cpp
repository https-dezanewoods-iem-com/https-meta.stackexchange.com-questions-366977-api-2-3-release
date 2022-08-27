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

#include "MobileWndManager.h"
#include <QQmlApplicationEngine>
#include "../state/state.h"

namespace core {

// NOTE: all  xxxxDlg methids must be modal. In mobile conetext it is mean that we should wait for closure and return back the result.
void MobileWndManager::init(QQmlApplicationEngine * _engine) {
    engine = _engine;
    Q_ASSERT(engine);

    engine->load(QUrl(QStringLiteral("qrc:/windows_mobile/main.qml")));
    mainWindow = engine->rootObjects().first();
    mainWindow->setProperty("currentState", 0);
}


void MobileWndManager::messageTextDlg( QString title, QString message, double widthScale) {
    if (engine== nullptr) {
        Q_ASSERT(false); // early crash, not much what we can do. May be do some logs (message is an error description)?
        return;
    }
    Q_ASSERT(false); // implement me
}

void MobileWndManager::messageHtmlDlg( QString title, QString message, double widthScale)  {
    Q_ASSERT(false); // implement me
}

// Two button box
WndManager::RETURN_CODE MobileWndManager::questionTextDlg( QString title, QString message, QString btn1, QString btn2, bool default1, bool default2, double widthScale) {
    Q_ASSERT(false); // implement me
    return WndManager::RETURN_CODE::BTN1;
}

WndManager::RETURN_CODE MobileWndManager::questionHTMLDlg( QString title, QString message, QString btn1, QString btn2, bool default1, bool default2, double widthScale )  {
    Q_ASSERT(false); // implement me
    return WndManager::RETURN_CODE::BTN1;
}

// Password accepted as a HASH. EMpty String mean that no password is set.
// After return, passwordHash value will have input raw Password value. So it can be user for wallet
WndManager::RETURN_CODE MobileWndManager::questionTextDlg( QString title, QString message, QString btn1, QString btn2, bool default1, bool default2, double widthScale, QString & passwordHash, WndManager::RETURN_CODE blockButton )  {
    Q_ASSERT(false); // implement me
    return WndManager::RETURN_CODE::BTN1;
}

// QFileDialog::getSaveFileName call
QString MobileWndManager::getSaveFileName(const QString &caption, const QString &dir, const QString &filter) {
    Q_ASSERT(false); // implement me
    return "";
}

// Ask for confirmation
bool MobileWndManager::sendConfirmationDlg( QString title, QString message, double widthScale, QString passwordHash ) {
    Q_ASSERT(false); // implement me
    return false;
}

    //---------------- Pages ------------------------
void MobileWndManager::pageNewWallet() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageInputPassword(QString pageTitle, bool lockMode) {
    Q_ASSERT(false); // implement me
    // ???
    mainWindow->setProperty("currentState", state::STATE::INPUT_PASSWORD);
}
void MobileWndManager::pageInitAccount() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageEnterSeed() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageNewSeed(QString pageTitle, QVector<QString> seed, bool hideSubmitButton) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageNewSeedTest(int wordIndex) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageProgressWnd(QString pageTitle, QString callerId, QString header, QString msgProgress, QString msgPlus, bool cancellable ) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageOutputs() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageFileTransaction(QString pageTitle, QString callerId,
                                     const QString & fileName, const util::FileTransactionInfo & transInfo,
                                     int nodeHeight,
                                     QString transactionType, QString processButtonName) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageRecieve() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageListening() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageFinalize() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageSendStarting() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageSendOnline( QString selectedAccount, int64_t amount ) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageSendOffline( QString selectedAccount, int64_t amount ) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageTransactions() {
    Q_ASSERT(false); // implement me
}

void MobileWndManager::pageHodl() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageHodlNode() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageHodlCold() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageHodlClaim(QString walletHash) {
    Q_ASSERT(false); // implement me
}
    // return true(first) if press OK and input signature value(second)
QPair<bool, QString> MobileWndManager::hodlGetSignatureDlg(QString challenge) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageAccounts() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageAccountTransfer() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageAirdrop() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageAirdropForBTC(QString btcAddress, QString challenge, QString identifier) {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageNodeInfo() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageContacts() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageEvents() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageWalletConfig() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageNodeConfig() {
    Q_ASSERT(false); // implement me
}
void MobileWndManager::pageSelectMode() {
    Q_ASSERT(false); // implement me
}

}
