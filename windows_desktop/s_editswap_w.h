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

#ifndef S_EDITSWAP_W_H
#define S_EDITSWAP_W_H

#include "../core_desktop/navwnd.h"

namespace Ui {
class EditSwap;
}

namespace bridge {
class Swap;
class Config;
}

namespace wnd {

class EditSwap : public core::NavWnd {
Q_OBJECT

public:
    // swapId - what trade deal we are editing
    explicit EditSwap(QWidget *parent, QString swapId, QString stateCmd);
    ~EditSwap();

private:
    void updateButtons(bool first_call = false);
    bool isCanUpdate() const;

    // Validate the data and call for update. Return number of update calls.
    int requestUpdateData();

private slots:
    void sgnRequestTradeDetails(  QVector<QString> swapInfo,
                                  QVector<QString> executionPlan,
                                  QString currentAction,
                                  QVector<QString> tradeJournal,
                                  QString errMsg );
    void sgnUpdateXXX(QString swapId, QString errorMsg);

    void on_redeemAddressEdit_textEdited(const QString &arg1);
    void on_secondaryFeeEdit_textEdited(const QString &arg1);
    void on_updateBtn_clicked();
    void on_tradeDetailsBtn_clicked();
    void on_backButton_clicked();

    void on_electrumXEdit_textEdited(const QString &arg1);

    void on_noteEdit_textEdited(const QString &arg1);

private:
    Ui::EditSwap *ui;
    bridge::Swap * swap = nullptr;
    bridge::Config * config = nullptr;
    QString swapId;
    QString redeemAddress;
    QString secondaryCurrency;
    QString secondaryFee;
    QString electrumX;

    bool    acceptanceMode = false;
    int     requests2accept = -1;
};

}

#endif // S_EDITSWAP_W_H
