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

#include <state/state.h>
#include "navbar.h"
#include "ui_navbar.h"
#include "../state/statemachine.h"
#include "navmenuconfig.h"
#include "navmenuaccount.h"
#include "../state/x_events.h"
#include <QFileDialog>
#include "../state/g_Finalize.h"
#include "../util/Json.h"
#include "../control/messagebox.h"
#include "../dialogs/fileslateinfodlg.h"
#include "../core/Config.h"

namespace core {

NavBar::NavBar(QWidget *parent, state::StateContext * _context ) :
        QWidget(parent),
        ui(new Ui::NavBar),
        prntWnd(parent),
        context(_context)
{
    if (config::isOnlineNode()) {
        ui->setupUi(this);

        ui->accountButton->hide();
    }
    else {
        state::Events * evtState = (state::Events *)context->stateMachine->getState(state::STATE::EVENTS);
        Q_ASSERT(evtState);

        QObject::connect( evtState, &state::Events::updateNonShownWarnings,
                          this, &NavBar::onUpdateNonShownWarnings, Qt::QueuedConnection );

        ui->setupUi(this);

        onUpdateNonShownWarnings( evtState->hasNonShownWarnings() );
    }
}

NavBar::~NavBar() {
    delete ui;
}

void NavBar::checkButton(BTN b) {
    ui->accountButton->setChecked(b==BTN::ACCOUNTS);
    ui->notificationButton->setChecked(b==BTN::NOTIFICATION);
    ui->settingsButton->setChecked(b==BTN::SETTINGS);
}

void NavBar::onUpdateNonShownWarnings(bool hasNonShownWarns) {
    ui->notificationButton->setIcon( QIcon( QPixmap(
            hasNonShownWarns ? ":/img/NavNotificationActive@2x.svg" : ":/img/NavNotificationNormal@2x.svg" ) ) );
}

void NavBar::on_notificationButton_clicked()
{
    checkButton(BTN::NOTIFICATION);
    context->stateMachine->setActionWindow( state::STATE::EVENTS );
}

void NavBar::on_settingsButton_clicked()
{
    checkButton(BTN::SETTINGS);

    showNavMenu( new NavMenuConfig( prntWnd, context ) );
}

void NavBar::on_accountButton_clicked()
{
    checkButton(BTN::ACCOUNTS);

    showNavMenu( new NavMenuAccount( prntWnd, context ) );
}

void NavBar::onMenuDestroy() {
    checkButton(BTN::NONE);
}


void NavBar::showNavMenu( NavMenu * menu ) {
    menu->move( navMenuPos );
    QObject::connect( menu, SIGNAL(closed()), this, SLOT(onMenuDestroy()));
    menu->show();
}


}

