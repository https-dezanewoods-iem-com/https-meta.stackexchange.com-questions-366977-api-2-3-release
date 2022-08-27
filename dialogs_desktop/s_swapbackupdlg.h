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

#ifndef S_SWAPBACKUPDLG_H
#define S_SWAPBACKUPDLG_H

#include "../control_desktop/mwcdialog.h"

namespace Ui {
class SwapBackupDlg;
}

namespace bridge {
class Config;
class Swap;
}

namespace dlg {

class SwapBackupDlg : public control::MwcDialog
{
    Q_OBJECT

public:
    // QString swapId - swap to backup
    // int backupId - id for this backup. On success it will be updated.
    explicit SwapBackupDlg(QWidget *parent, QString swapId, int backupId);
    ~SwapBackupDlg();

private slots:
    void sgnBackupSwapTradeData(QString swapId, QString exportedFileName, QString errorMessage);

    void on_selectPath_clicked();
    void on_skipButton_clicked();
    void on_backupButton_clicked();
    void on_backupDataPath_textEdited(const QString &str);

private:
    void updateButtons();

private:
    Ui::SwapBackupDlg *ui;
    QString swapId;
    int     backupId = 0;
    bridge::Config * config = nullptr;
    bridge::Swap * swap = nullptr;
};

}

#endif // S_SWAPBACKUPDLG_H
