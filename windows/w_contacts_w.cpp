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

#include "w_contacts_w.h"
#include "ui_w_contacts.h"
#include "../state/w_contacts.h"
#include "../dialogs/w_contacteditdlg.h"
#include "../control/messagebox.h"
#include "../state/timeoutlock.h"

namespace wnd {

Contacts::Contacts(QWidget *parent, state::Contacts * _state) :
    core::NavWnd(parent, _state->getContext()),
    ui(new Ui::Contacts),
    state(_state)
{
    ui->setupUi(this);

    initTableHeaders();
    updateContactTable();

    ui->contactsTable->setFocus();
    updateButtons();
}

Contacts::~Contacts()
{
    saveTableHeaders();
    delete ui;
}

void Contacts::updateButtons() {
    int idx = getSelectedContactIndex();

    ui->addButton->setEnabled(true);
    ui->editButton->setEnabled(idx>=0);
    ui->deleteButton->setEnabled(idx>=0);
}


void Contacts::initTableHeaders() {

    // Disabling to show the grid
    // Creatign columns
    QVector<int> widths = state->getColumnsWidhts();
    if ( widths.size() != 3 ) {
        widths = QVector<int>{30,100,400};
    }
    Q_ASSERT( widths.size() == 3 );
    ui->contactsTable->setColumnWidths( widths );
}

void Contacts::saveTableHeaders() {
    state->updateColumnsWidhts( ui->contactsTable->getColumnWidths() );
}

void Contacts::updateContactTable() {
    contacts = state->getContacts();

    ui->contactsTable->clearData();
    int idx = 0;
    for ( const core::ContactRecord & cont : contacts ) {
        ui->contactsTable->appendRow( QVector<QString>{
                                          QString::number(++idx),
                                          cont.name,
                                          cont.address
                                      } );
    }
}

int  Contacts::getSelectedContactIndex() const {
    QList<QTableWidgetItem*> selList = ui->contactsTable->selectedItems();
    if (selList.size()==0)
        return -1;

    int idx = selList[0]->row();
    if (idx>=0 && idx<contacts.size())
        return idx;

    return -1;
}


void Contacts::on_addButton_clicked()
{
    state::TimeoutLockObject to( state );

    dlg::ContactEditDlg dlg(this, core::ContactRecord(),
                                   contacts, false );
    if (dlg.exec() == QDialog::Accepted) {
        QPair<bool, QString> res = state->addContact(dlg.getContact());

        if (!res.first) {
            control::MessageBox::messageText(this, "Error", "Unable to add a new contact.\n" + res.second);
        }

        updateContactTable();
    }

}

void Contacts::on_editButton_clicked()
{
    state::TimeoutLockObject to( state );

    int idx = getSelectedContactIndex();
    if (idx<0) // expected to be disabled
        return;

    core::ContactRecord oldContact = contacts[idx];
    QVector<core::ContactRecord> contacts2check = contacts;
    contacts2check.remove(idx);

    dlg::ContactEditDlg dlg(this, oldContact,
                       contacts2check, true );
    if (dlg.exec() == QDialog::Accepted) {
        auto contact = dlg.getContact();

        // Update is not atomic. There is a chnce to lost the contact.
        QPair<bool, QString> res = state->updateContact( oldContact, contact );

        if (!res.first)
            control::MessageBox::messageText(this, "Error", "Unable to update the contact data. Error: " + res.second);

        updateContactTable();
    }

}

void wnd::Contacts::on_contactsTable_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    on_editButton_clicked();
}


void Contacts::on_deleteButton_clicked()
{
    state::TimeoutLockObject to( state );

    int idx = getSelectedContactIndex();
    if (idx<0) // expected to be disabled
        return;

    core::ContactRecord contact2del = contacts[idx];

    if (control::MessageBox::questionText(this, "Remove a contact", "Remove the selected contact for " + contact2del.name +
                              "? Press 'Yes' to delete.", "Yes", "No", false,true) == control::MessageBox::RETURN_CODE::BTN1 ) {
        QPair<bool, QString> res = state->deleteContact(contact2del);
        if (!res.first) {
            control::MessageBox::messageText(this, "Error", "Unable to remove the contact '"+ contact2del.name +"'.\nError: " + res.second);
        }

        updateContactTable();
    }
}


void Contacts::on_contactsTable_itemSelectionChanged()
{
    updateButtons();

}

}



