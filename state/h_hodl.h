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

#ifndef HODL_H
#define HODL_H


#include "state.h"
#include "../wallet/wallet.h"
#include <QVector>

class QNetworkAccessManager;
class QNetworkReply;

namespace wnd {
class Hodl;
class HodlCold;
class HodlNode;
class HodlClaim;
}

namespace state {

class Hodl : public QObject, public State
{
Q_OBJECT
private:
    // Different event driven workflows shared the same path.
    // Fortunatelly complexity is low and never parallel, so just a flag will be OK.
    enum class HODL_WORKFLOW { INIT, REGISTER, CLAIM };

public:
    Hodl(StateContext * context);
    virtual ~Hodl() override;

    void deleteHodlNormWnd(wnd::Hodl * wnd) { if (wnd == hodlNormWnd) {hodlNormWnd = nullptr;} }
    void deleteHodlColdWnd(wnd::HodlCold * wnd) { if (wnd == hodlColdWnd) {hodlColdWnd = nullptr;} }
    void deleteHodlNodeWnd(wnd::HodlNode * wnd) { if (wnd == hodlNodeWnd) {hodlNodeWnd = nullptr;} }
    void deleteHodlClaimWnd(wnd::HodlClaim * wnd) { if (wnd == hodlClaimWnd) {hodlClaimWnd = nullptr;} }

    // Set cold wallet public key. That can initiate all status workflow.
    // Epmty String  - reset
    void setColdWalletPublicKey(QString pubKey);

    // Request registration for HODL program, normal online automatic way
    void registerAccountForHODL();

    void moveToClaimPage();
    void moveToStartHODLPage();

    void claimMWC();

    QVector<int> getColumnsWidhts();
    void updateColumnsWidhts(QVector<int> widths);

    // request message to sign, respond expected to be delivered to the window
    void requestSignMessage(const QString & message);

protected:
    virtual NextStateRespond execute() override;
    virtual QString getHelpDocName() override {return "hodl.html";}

    // Respond will be back at replyFinished.  replyFinished will process it accordingly
    enum HTTP_CALL {GET, POST};
    void sendRequest(HTTP_CALL call, const QString & url,
                     const QVector<QString> & urlParams,
                     const QByteArray & body, //
                     const QString & tag, const QString & param1="",const QString & param2="",
                     const QString & param3="", const QString & param4="");

    void reportMessageToUI( const QString & title, const QString & message );
    void hideWaitingStatus();

    // Note, publicKey & claimAmount are for claiming only!!!
    void startChallengeWorkflow(HODL_WORKFLOW workflow, QString publicKey, int64_t claimAmount );

    void retrieveHodlBalance();

private slots:
    // Need to get a network info
    void onLoginResult(bool ok);
    void replyFinished(QNetworkReply* reply);
    void onGetNextKeyResult( bool success, QString identifier, QString publicKey, QString errorMessage, QString btcaddress, QString airDropAccPasswor);
    void onReceiveFile( bool success, QStringList errors, QString inFileName, QString outFn );
    void onRootPublicKey( bool success, QString errMsg, QString rootPubKey, QString message, QString signature );

    void onHodlStatusWasChanged();
private:
    QNetworkAccessManager *nwManager = nullptr;

    wnd::Hodl * hodlNormWnd = nullptr;
    wnd::HodlCold * hodlColdWnd = nullptr;
    wnd::HodlNode * hodlNodeWnd = nullptr;
    wnd::HodlClaim * hodlClaimWnd = nullptr;

    QString hodlUrl; // Url for airdrop requests. Url depend on current network.

private:
    // Local contexts
    HODL_WORKFLOW hodlWorkflow = HODL_WORKFLOW::INIT;

    // Claim workflow states
    QString claimNextTransIdentifier;
    QString claimNextTransPubKey;
    int requestCounter = 0;
};

}

#endif // HODL_H
