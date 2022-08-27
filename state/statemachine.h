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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QMap>
#include "state.h"
#include <QObject>

namespace state {

// State machine that describes wallet application
class StateMachine : public QObject
{
    Q_OBJECT
public:
    StateMachine(StateContext * context);
    ~StateMachine();

    void start();

    // Check if current state agree to switch the state
    bool canSwitchState();

    void executeFrom( STATE nextState );

    // set current action winodow if possible
    bool setActionWindow( STATE actionWindowState, bool enforce = false );
    // get current action window
    STATE getActionWindow() const;

    // return true if action window will applicable
    bool isActionWindowMode() const;

    // Reset logout time.
    void resetLogoutLimit();

    // Logout must be blocked for modal dialogs
    void blockLogout();
    void unblockLogout();
    // logout now
    void logout();

    // Please use carefully, don't abuse this interface since no type control can be done
    State* getState(STATE state) const;

    State* getCurrentStateObj() const;

    STATE  getCurrentStateId() const {return currentState;}

private:
    // routine to process state into the loop
    bool processState(State* st);

    virtual void timerEvent(QTimerEvent *event) override;

    bool isLogoutOff( STATE state ) const { return state < STATE::ACCOUNTS || state==STATE::RESYNC; }

private:
    StateContext * context = nullptr;

    // Map is orders by Ids. It naturally define the priority of
    // all states
    QMap< STATE, State* > states;
    STATE currentState = STATE::NONE;

    int64_t logoutTime = 0; // 0 mean never logout...
    int     blockLogoutCounter = 0;
};


}

#endif // STATEMACHINE_H
