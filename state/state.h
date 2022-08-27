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

#ifndef STATE_H
#define STATE_H

#include <QString>

namespace core {
    class WindowManager;
    class MainWindow;
    class AppContext;
    class HodlStatus;
}

namespace wallet {
    class Wallet;
}

namespace node {
class MwcNode;
}

namespace state {

class State;
class StateMachine;

enum STATE {
    NONE,
    START_WALLET, // Start backed mwc713. Check what it want and then delegate control to the next state
    STATE_INIT, // first run. Creating the password for a wallet
    INPUT_PASSWORD, // Input password from the wallet
    ACCOUNTS,       // Wallet accounts.              Note!!!  Keep accounts first Action page.
    ACCOUNT_TRANSFER, // Transfer funds from account to account
    EVENTS,         // Wallet events (logs)
    HODL,           // Hodl program.
    SEND,           // Send coins Page
    RECEIVE_COINS,  // Receive coins
    LISTENING,      // Listening API setting/status
    TRANSACTIONS,   // Transactions dialog
    OUTPUTS,        // Outputs for this wallet
    CONTACTS,       // Contact page. COntacts supported by wallet713
    WALLET_CONFIG,  // Wallet config
    AIRDRDOP_MAIN,  // Starting airdrop page
    SHOW_SEED,      // Show Seed
    NODE_INFO,      // Show node info
    RESYNC,         // Re-sync account with a node
    FINALIZE,       // Finalize transaction. Windowless state
    WALLET_RUNNING_MODE  // Running mode as a node, wallet or cold wallet
};

struct NextStateRespond {
    enum RESULT { NONE, WAIT_FOR_ACTION, DONE };

    NextStateRespond( RESULT res ) : result(res) {}
    NextStateRespond( RESULT res, STATE nextSt  ) : result(res), nextState(nextSt) {}

    RESULT result = RESULT::NONE;
    STATE nextState = STATE::NONE;
};


struct StateContext {
    core::AppContext    * const appContext;
    wallet::Wallet      * const wallet; //wallet caller interface
    node::MwcNode       * const mwcNode;
    core::WindowManager * const wndManager;
    core::MainWindow    * const mainWnd;
    StateMachine        * stateMachine = nullptr;
    core::HodlStatus    * hodlStatus = nullptr;

    StateContext(core::AppContext * _appContext, wallet::Wallet * _wallet,
                 node::MwcNode * _mwcNode,
                 core::WindowManager * _wndManager, core::MainWindow * _mainWnd) :
        appContext(_appContext), wallet(_wallet), mwcNode(_mwcNode), wndManager(_wndManager),
        mainWnd(_mainWnd), stateMachine(nullptr) {}

    void setStateMachine(StateMachine * sm) {stateMachine=sm;}
    void setHodlStatus(core::HodlStatus* hs) {hodlStatus=hs;}
};


// Single state of the app that is described with Dialog
class State
{
protected:
    StateContext * context;
    STATE stateId;

    //core::WalletWindow * wnd; //
    //State * prevState = nullptr;
    //State * nextState = nullptr;
public:
    State(StateContext * context, STATE stateId);
    virtual ~State();

    state::StateContext * getContext() {return context;}

    // process/verify the state
    virtual NextStateRespond execute() = 0;

    // State can block the stare change. Wallet config is the first usage.
    virtual bool canExitState() {return true;}

    // Executing another state
    virtual void exitingState() {};

    // Empty string - default document name
    virtual QString getHelpDocName() {return "";}
};

}

#endif // STATE_H
