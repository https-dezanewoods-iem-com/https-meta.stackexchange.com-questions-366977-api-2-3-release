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

#ifndef MWC_QT_WALLET_RECEIVE_H
#define MWC_QT_WALLET_RECEIVE_H

#include "state.h"
#include "../wallet/wallet.h"

namespace state {

const QString RECEIVE_CALLER_ID = "Receive";

class Receive : public QObject, public State {
    Q_OBJECT
public:
    Receive( StateContext * context );
    virtual ~Receive() override;

    void signTransaction( QString fileName );

public:
    void ftBack();
    void ftContinue(QString fileName, QString resultTxFileName, bool fluff);

    bool needResultTxFileName() {return false;}

    QString getResultTxPath() {return "";}
    virtual void updateResultTxPath(QString path) {Q_UNUSED(path)}

    virtual bool isNodeHealthy() const {return true;}
protected:
    virtual NextStateRespond execute() override;
    virtual QString getHelpDocName() override {return "receive.html";}

    void respReceiveFile( bool success, QStringList errors, QString inFileName );

    bool isActive() const;
private slots:
    void onNodeStatus( bool online, QString errMsg, int nodeHeight, int peerHeight, int64_t totalDifficulty, int connections );
private:
    int lastNodeHeight = 0;
};


}

#endif //MWC_QT_WALLET_RECEIVE_H
