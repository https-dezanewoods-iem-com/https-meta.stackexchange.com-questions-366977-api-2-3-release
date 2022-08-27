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

#ifndef ENTERSEED_H
#define ENTERSEED_H

#include "../core_desktop/PanelBaseWnd.h"

namespace Ui {
class EnterSeed;
class Util;
}

namespace bridge {
class InitAccount;
class Util;
}

namespace wnd {

class EnterSeed : public core::PanelBaseWnd
{
    Q_OBJECT

public:
    explicit EnterSeed(QWidget *parent);
    virtual ~EnterSeed() override;

private slots:
    void on_Enter();
    void on_cancelButton_clicked();
    void on_continueButton_clicked();
    void on_addWordButton_clicked();
private:
    Ui::EnterSeed *ui;
    bridge::InitAccount * accountInit = nullptr;
    bridge::Util * util = nullptr;
};

}

#endif // ENTERSEED_H
