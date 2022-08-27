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

#include "global.h"
#include <QApplication>
#include <QMainWindow>
#include "../core/Config.h"

namespace mwc {

#ifdef WALLET_DESKTOP
static QApplication * mwcApp = nullptr;
static QMainWindow * mwcMainWnd = nullptr;
#endif

QString get_APP_NAME() {
    if (config::isOnlineWallet())
        return "MWC Qt Wallet";
    else if (config::isOnlineNode())
        return "MWC Node";
    else if (config::isColdWallet())
        return "MWC Cold Wallet";
    else
        return "Unknown mode";
}

static bool nonClosed = true;

bool isAppNonClosed() {return nonClosed;}

#ifdef WALLET_DESKTOP
void setApplication(QApplication * app, QMainWindow * mainWindow) {
    mwcApp = app;
    mwcMainWnd = mainWindow;
}

void closeApplication() {
    if (mwcApp == nullptr || mwcMainWnd==nullptr) {
        nonClosed = false;
        return; // posible if error detected during the start of the app. Nothing is there yet
    }

    Q_ASSERT(mwcApp);
    Q_ASSERT(mwcMainWnd);

    if (nonClosed) {

        nonClosed = false;

        // Async call is the only way to close App nicely !!!!
        // Alternatively we can call app::quit slot
        QMetaObject::invokeMethod(mwcMainWnd, "close", Qt::QueuedConnection);
    }
}
#endif

#ifdef WALLET_MOBILE
void closeApplication() {
    if (nonClosed) {

        nonClosed = false;

        // Suppose to close QML application. I am not sure if it will be friendly enough
        QCoreApplication::quit();
    }
}
#endif

static int repostId = -1;
static QString repostAccount;
static bool finalize = false;

int getRepostId() {
    return repostId;
}

void setRepostId(int id) {
    repostId = id;
}

QString getRepostAccount() {
    return repostAccount;
}

void setRepostAccount(QString account) {
    repostAccount = account;
}

bool isFinalize() {
    return finalize;
}

void setFinalize(bool finalizeValue) {
    finalize = finalizeValue;
}

static bool walletLocked = false;

void setWalletLocked(bool locked) {
    walletLocked = locked;
}

bool isWalletLocked() {
    return walletLocked;
}

}
