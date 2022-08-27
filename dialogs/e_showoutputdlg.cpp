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

#include "dialogs/e_showoutputdlg.h"
#include "ui_e_showoutputdlg.h"
#include "../wallet/wallet.h"
#include "../util/stringutils.h"
#include "../util/execute.h"
#include "../core/global.h"

namespace dlg {

ShowOutputDlg::ShowOutputDlg(QWidget *parent, const wallet::WalletOutput &output, const wallet::WalletConfig &config) :
        control::MwcDialog(parent),
        ui(new Ui::ShowOutputDlg) {
    ui->setupUi(this);

    ui->status->setText(output.status);
    ui->height->setText(output.blockHeight);
    ui->confirms->setText(output.numOfConfirms);
    ui->mwc->setText(util::nano2one(output.valueNano));
    ui->locked->setText(output.lockedUntil);
    ui->coinBase->setText(output.coinbase ? "Yes" : "No");
    ui->tx->setText(QString::number(output.txIdx + 1));
    ui->commitment->setText(output.outputCommitment);

    blockExplorerUrl = (config.getNetwork() == "Mainnet") ? mwc::BLOCK_EXPLORER_URL_MAINNET
                                                          : mwc::BLOCK_EXPLORER_URL_FLOONET;
    commitment = output.outputCommitment;
}

ShowOutputDlg::~ShowOutputDlg() {
    delete ui;
}

void ShowOutputDlg::on_viewOutput_clicked() {
    util::openUrlInBrowser("https://" + blockExplorerUrl + "/#o" + commitment);
}

void ShowOutputDlg::on_pushButton_clicked() {
    accept();
}

}