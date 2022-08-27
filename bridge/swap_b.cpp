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

#include "swap_b.h"
#include "../wallet/wallet.h"
#include "../state/state.h"
#include "../state/s_swap.h"
#include "../state/u_nodeinfo.h"
#include "../core/appcontext.h"
#include "../util/address.h"

namespace bridge {

static wallet::Wallet * getWallet() { return state::getStateContext()->wallet; }
static core::AppContext * getAppContext() { return state::getStateContext()->appContext; }
static state::Swap * getSwap() {return (state::Swap *) state::getState(state::STATE::SWAP); }
//static state::NodeInfo * getNodeInfo() {return (state::NodeInfo *) state::getState(state::STATE::NODE_INFO); }


Swap::Swap(QObject *parent) : QObject(parent) {
    wallet::Wallet *wallet = state::getStateContext()->wallet;

    QObject::connect(wallet, &wallet::Wallet::onRequestSwapTrades,
                     this, &Swap::onRequestSwapTrades, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onDeleteSwapTrade,
                     this, &Swap::onDeleteSwapTrade, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onCreateNewSwapTrade,
                     this, &Swap::onCreateNewSwapTrade, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onRequestTradeDetails,
                     this, &Swap::onRequestTradeDetails, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onAdjustSwapData,
                     this, &Swap::onAdjustSwapData, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onCancelSwapTrade,
                     this, &Swap::onCancelSwapTrade, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onNewSwapTrade,
                     this, &Swap::onNewSwapTrade, Qt::QueuedConnection);

    QObject::connect(wallet, &wallet::Wallet::onBackupSwapTradeData,
                     this, &Swap::onBackupSwapTradeData, Qt::QueuedConnection);
    QObject::connect(wallet, &wallet::Wallet::onRestoreSwapTradeData,
                     this, &Swap::onRestoreSwapTradeData, Qt::QueuedConnection);

    state::Swap * swap = getSwap();
    QObject::connect(swap, &state::Swap::onSwapTradeStatusUpdated,
                     this, &Swap::onSwapTradeStatusUpdated, Qt::QueuedConnection);
    QObject::connect(swap, &state::Swap::onCreateStartSwap,
                     this, &Swap::onCreateStartSwap, Qt::QueuedConnection);

}

Swap::~Swap() {}

// Return back to the trade list page
void Swap::pageTradeList() {
    getSwap()->pageTradeList();
}

// request the list of swap trades
void Swap::requestSwapTrades() {
    getWallet()->requestSwapTrades();
}

void Swap::onRequestSwapTrades(QVector<wallet::SwapInfo> swapTrades) {
    // Result comes in series of 9 item tuples:
    // < <bool is Seller>, <mwcAmount>, <sec+amount>, <sec_currency>, <Trade Id>, <State>, <initiate_time_interval>, <expire_time_interval>  <secondary_address> >, ....
    QVector<QString> trades;
    int64_t timestampSec = QDateTime::currentSecsSinceEpoch();
    for (const auto & st : swapTrades) {
        trades.push_back( st.isSeller ? "true" : "false" );
        trades.push_back( st.mwcAmount );
        trades.push_back( st.secondaryAmount );
        trades.push_back( st.secondaryCurrency );
        trades.push_back(st.swapId);
        if (st.action.isEmpty() || st.action=="None")
            trades.push_back(st.state);
        else
            trades.push_back(st.action);

        trades.push_back( util::interval2String( timestampSec - st.startTime, false, 2 ));
        if (st.expiration>0) {
            trades.push_back( util::interval2String( st.expiration - timestampSec, true, 2 ));
        }
        else {
            trades.push_back("");
        }
        trades.push_back(st.secondaryAddress);
    }

    emit sgnSwapTradesResult( trades );
}

// Cancel the trade. Send signal to the cancel the trade.
// Trade cancelling might take a while.
void Swap::cancelTrade(QString swapId) {
    getWallet()->cancelSwapTrade(swapId);
}


// Switch to editTrade Window
void Swap::viewTrade(QString swapId) {
    getSwap()->viewTrade(swapId);
}

// Switch to trade Details Window
void Swap::showTradeDetails(QString swapId) {
    getSwap()->showTradeDetails(swapId);
}

// Deleting swapId
void Swap::deleteSwapTrade(QString swapId) {
    getWallet()->deleteSwapTrade(swapId);
}

void Swap::onDeleteSwapTrade(QString swapId, QString errMsg) {
    emit sgnDeleteSwapTrade(swapId, errMsg);
}

void Swap::onCancelSwapTrade(QString swapId, QString error) {
    emit sgnCancelTrade(swapId, error);
}

// Requesting all details about the single swap Trade
// Respond will be with sent back with sgnRequestTradeDetails
void Swap::requestTradeDetails(QString swapId) {
    getWallet()->requestTradeDetails(swapId);
}

QString calcTimeLeft(int64_t time) {
    int64_t curTime = QDateTime::currentSecsSinceEpoch();
    if (curTime > time)
        return 0;

    int64_t secondsLeft = time - curTime;
    int hours = secondsLeft / 3600;
    int minutes = (secondsLeft % 3600) / 60;

    return QString::number(hours) + " hours and " + QString::number(minutes) + " minutes";
}

// executionPlan, array of triplets: <active: "true"|"false">, <end_time>, <Name> >, ....
static QVector<QString> convertExecutionPlan(const QVector<wallet::SwapExecutionPlanRecord> & executionPlan) {
    QVector<QString> res;
    for (const wallet::SwapExecutionPlanRecord & exPl : executionPlan ) {
        res.push_back( exPl.active ? "true" : "false");
        res.push_back(util::timestamp2ThisTime(exPl.end_time));
        res.push_back(exPl.name);
    }
    return res;
}

static QVector<QString> convertTradeJournal( const QVector<wallet::SwapJournalMessage> & tradeJournal ) {
    QVector<QString> res;
    for ( const wallet::SwapJournalMessage & tj : tradeJournal ) {
        res.push_back(tj.message);
        res.push_back( util::timestamp2ThisTime(tj.time));
    }
    return res;
}

void Swap::onRequestTradeDetails( wallet::SwapTradeInfo swap,
                                   QVector<wallet::SwapExecutionPlanRecord> executionPlan,
                                   QString currentAction,
                                   QVector<wallet::SwapJournalMessage> tradeJournal,
                                   QString errMsg ) {
    QVector<QString> swapInfo;

    // Response from requestTradeDetails call
    // [0] - swapId
    swapInfo.push_back(swap.swapId);

    if (!errMsg.isEmpty()) {
        emit sgnRequestTradeDetails(swapInfo, {}, currentAction, {}, errMsg);
        return;
    }

    // [1] - Description in HTML format. Role can be calculated form here as "Selling ..." or "Buying ..."


    QString reportStr = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                        "<html><body style=\"font-family:'Open Sans'; font-size:medium; font-weight:normal; font-style:normal; color:white; background-color:transparent;\">";


    reportStr += QString("<p>") + (swap.isSeller ? "Selling" : "Buying") + " <b style=\"color:yellow;\">" + util::trimStrAsDouble(QString::number( swap.mwcAmount, 'f' ), 10) +
            " MWC</b> for <b style=\"color:yellow;\">" + util::trimStrAsDouble(QString::number( swap.secondaryAmount, 'f' ), 10) + " " + swap.secondaryCurrency + "</b>.</p>";

    reportStr += "<p>";
    reportStr += "Required confirmations: <b style=\"color:yellow;\">" + QString::number(swap.mwcConfirmations) +
                 " for MWC</b> and <b style=\"color:yellow;\">" + QString::number(swap.secondaryConfirmations) + " for " + swap.secondaryCurrency + "</b>.</p>";


    reportStr += "<p>Time limits: <b style=\"color:yellow;\">" + util::interval2String(swap.messageExchangeTimeLimit, false, 2) +
            "</b> for messages exchange and <b style=\"color:yellow;\">" +  util::interval2String(swap.redeemTimeLimit, false, 2)+"</b> for redeem or refund<p/>";

    reportStr += "<p>Locking order: " + (swap.sellerLockingFirst ? "<b style=\"color:yellow;\">Lock MWC</b> first" : ("<b style=\"color:yellow;\">Lock " + swap.secondaryCurrency + "</b> first")) + "</p>";

    QString lockTime = calcTimeLeft(swap.mwcLockTime);
    reportStr += "<p>";
    if (lockTime.size()>0)
        reportStr += "MWC funds locked until block <b style=\"color:yellow;\">" + QString::number(swap.mwcLockHeight) + "</b>, expected to be mined in <b style=\"color:yellow;\">" + lockTime + "</b>.<br/>";
    else
        reportStr += "MWC Lock expired.<br/>";

    QString secLockTime = calcTimeLeft(swap.secondaryLockTime);
    if (secLockTime.size()>0) {
        reportStr += swap.secondaryCurrency + " funds locked for <b style=\"color:yellow;\">" + secLockTime + "</b>.";
    }
    else {
        reportStr += swap.secondaryCurrency + " lock expired";
    }
    reportStr += "</p>";

    QString address = swap.communicationAddress;
    if (swap.communicationMethod=="mwcmqs")
        address = "mwcmqs://" + address;
    else if (swap.communicationMethod=="tor")
        address = "http://" + address + ".onion";
    else {
        Q_ASSERT(false); // QT wallet doesn't expect anything else.
    }
    reportStr += "<p>Trading with : <b style=\"color:yellow;\">" +  address + "</b></p>";
    reportStr += "</body></html>";
    swapInfo.push_back(reportStr);



/*    QString description;
    if (swap.isSeller) {
        description += "Selling <b>" + QString::number( swap.mwcAmount ) + "</b> MWC for <b>" + QString::number(swap.secondaryAmount) + "</b> " +
                swap.secondaryCurrency + "." + swap.secondaryCurrency + " redeem address: " + swap.secondaryAddress + "<br/>";
    }
    else {
        description += "Buying <b>" + QString::number( swap.mwcAmount ) + "</b> MWC for <b>" + QString::number(swap.secondaryAmount) + "</b> " + swap.secondaryCurrency + "<br/>";
    }

    description += "Requied lock confirmations: <b>" + QString::number(swap.mwcConfirmations)+"</b> for MWC and <b>"+ QString::number(swap.secondaryConfirmations) +"</b> for " + swap.secondaryCurrency + "<br/>";
    description += "Time limits: <b>"+QString::number(swap.messageExchangeTimeLimit/60)+"</b> minutes for messages exchange and <b>"+QString::number(swap.redeemTimeLimit/60)+"</b> minutes for redeem or refund<br/>";
    description += "Locking order: " + (swap.sellerLockingFirst ? "Seller lock MWC first" : ("Buyer lock " + swap.secondaryCurrency + " first")) + "<br/>";
    QString lockTime = calcTimeLeft(swap.mwcLockTime);
    if (lockTime.size()>0)
        description += "MWC funds locked until block " + QString::number(swap.mwcLockHeight) + ", expected to be mined in " + lockTime + "<br/>";
    else
        description += "MWC Lock expired.<br/>";

    QString secLockTime = calcTimeLeft(swap.secondaryLockTime);
    if (secLockTime.size()>0) {
        description += swap.secondaryCurrency + " funds locked for " + secLockTime + "<br/>";
    }
    else {
        description += swap.secondaryCurrency + " lock expired<br/>";
    }
    description += (swap.isSeller ? "Buyer" : "Seller" ) + QString(" address: ") + swap.communicationMethod + ", " + swap.communicationAddress + "<br/>";
    swapInfo.push_back(description);*/

    // [2] - Redeem address
    swapInfo.push_back(swap.secondaryAddress);
    // [3] - secondary currency name
    swapInfo.push_back(swap.secondaryCurrency);
    // [4] - secondary fee
    swapInfo.push_back(QString::number(swap.secondaryFee));
    // [5] - secondary fee units
    swapInfo.push_back(swap.secondaryFeeUnits);
    // [6] - communication method: mwcmqc|tor
    swapInfo.push_back(swap.communicationMethod);
    // [7] - Communication address
    swapInfo.push_back(swap.communicationAddress);

    emit sgnRequestTradeDetails( swapInfo, convertExecutionPlan(executionPlan), currentAction, convertTradeJournal(tradeJournal), errMsg );
}


// Check if this Trade is running in auto mode now
bool Swap::isRunning(QString swapId) {
    return getSwap()->isTradeRunning(swapId);
}

// Update communication method.
// Respond will be at sgnUpdateCommunication
void Swap::updateCommunication(QString swapId, QString communicationMethod, QString communicationAddress) {
    getWallet()->adjustSwapData(swapId, "destination", communicationMethod, communicationAddress);
}

// Update redeem adress.
// Respond will be at sgnUpdateRedeemAddress
void Swap::updateSecondaryAddress(QString swapId, QString secondaryAddress) {
    getWallet()->adjustSwapData(swapId, "secondary_address", secondaryAddress );
}

// Update secondary fee value for the transaction.
// Respond will come with sgnUpdateSecondaryFee
void Swap::updateSecondaryFee(QString swapId, double fee) {
    getWallet()->adjustSwapData(swapId, "secondary_fee", QString::number(fee) );
}

void Swap::onAdjustSwapData(QString swapId, QString adjustCmd, QString errMsg) {
    if (adjustCmd == "destination") {
        emit sgnUpdateCommunication(swapId, errMsg);
    }
    else if (adjustCmd=="secondary_address") {
        emit sgnUpdateSecondaryAddress(swapId,errMsg);
    }
    else if (adjustCmd=="secondary_fee") {
        emit sgnUpdateSecondaryFee(swapId,errMsg);
    }
}

void Swap::onSwapTradeStatusUpdated(QString swapId, QString currentAction, QString currentState,
                              QVector<wallet::SwapExecutionPlanRecord> executionPlan,
                              QVector<wallet::SwapJournalMessage> tradeJournal) {
    emit sgnSwapTradeStatusUpdated( swapId, currentAction, currentState,
            convertExecutionPlan(executionPlan),
            convertTradeJournal(tradeJournal));
}

void Swap::onNewSwapTrade(QString currency, QString swapId) {
    emit sgnNewSwapTrade(currency, swapId);
}

// Backup/export swap trade data into the file
// Respond with sgnBackupSwapTradeData
void Swap::backupSwapTradeData(QString swapId, QString backupFileName) {
    getWallet()->backupSwapTradeData(swapId, backupFileName);
}

// Import/restore swap trade data from the file
// Respond with sgnRestoreSwapTradeData
void Swap::restoreSwapTradeData(QString filename) {
    getWallet()->restoreSwapTradeData(filename);
}

void Swap::onBackupSwapTradeData(QString swapId, QString exportedFileName, QString errorMessage) {
    emit sgnBackupSwapTradeData(swapId, exportedFileName, errorMessage);
}

void Swap::onRestoreSwapTradeData(QString swapId, QString importedFilename, QString errorMessage) {
    emit sgnRestoreSwapTradeData(swapId, importedFilename, errorMessage);
}


// Switch to New Trade Window
void Swap::initiateNewTrade() {
    getSwap()->initiateNewTrade();
}

void Swap::showNewTrade1() {
    getSwap()->showNewTrade1();
}
void Swap::showNewTrade2() {
    getSwap()->showNewTrade2();
}

// Apply params from trade1 and switch to trade2 panel
// Response: sgnApplyNewTrade1Params(bool ok, QString errorMessage)
void Swap::applyNewTrade1Params(QString account, QString secCurrency, QString mwcAmount, QString secAmount,
                                      QString secAddress, QString sendToAddress ) {

    QPair< bool, util::ADDRESS_TYPE > addressRes = util::verifyAddress(sendToAddress);
    if ( !addressRes.first ) {
        emit sgnApplyNewTrade1Params (false, "Unable to parse the other wallet address " + sendToAddress);
        return;
    }

    QString addrType;

    switch (addressRes.second) {
        case util::ADDRESS_TYPE::MWC_MQ: {
            addrType = "mwcmqs";
            break;
        }
        case util::ADDRESS_TYPE::TOR: {
            addrType = "tor";
            break;
        }
        default: {
            emit sgnApplyNewTrade1Params(false, "Automated swaps working only with MQS and TOR addresses.");
            return;

        }
    }

    // We need to use wallet for verification. start new with a dry run will work great
    getWallet()->createNewSwapTrade(account,
                                    getAppContext()->getSendCoinsParams().inputConfirmationNumber,
                                    mwcAmount, secAmount, secCurrency,
                                    secAddress,
                                    true,
                                    60,
                                    60,
                                    60,
                                    6,
                                    addrType,
                                    sendToAddress,
                                    "",
                                    "",
                                    true,
                                    "applyNewTrade1Params",
                                    {account, secCurrency, mwcAmount, secAmount, secAddress, sendToAddress} );


    // Respond at onCreateNewSwapTrade, tag applyNewTrade1Params
}

// Apply params from trade2 panel and switch to the review (panel3)
// Response: sgnApplyNewTrade2Params(bool ok, QString errorMessage)
void Swap::applyNewTrade2Params(QString secCurrency, int offerExpTime, int redeemTime, int mwcBlocks, int secBlocks,
                                      double secTxFee, QString electrumXUrl) {

    QString sendToAddress = getSwap()->getBuyerAddress();
    QPair< bool, util::ADDRESS_TYPE > addressRes = util::verifyAddress(sendToAddress);
    Q_ASSERT(addressRes.first);

    QString addrType;
    switch (addressRes.second) {
        case util::ADDRESS_TYPE::MWC_MQ: {
            addrType = "mwcmqs";
            break;
        }
        case util::ADDRESS_TYPE::TOR: {
            addrType = "tor";
            break;
        }
        default: {
            Q_ASSERT(false);
            return;
        }
    }


    // We need to use wallet for verification. start new with a dry run will work great
    getWallet()->createNewSwapTrade( getSwap()->getAccount(),
                                    getAppContext()->getSendCoinsParams().inputConfirmationNumber,
                                    getSwap()->getMwc2Trade(), getSwap()->getSec2Trade(), getSwap()->getCurrentSecCurrency(),
                                    getSwap()->getSecAddress(),
                                    true,
                                    offerExpTime,
                                    redeemTime,
                                    mwcBlocks,
                                    secBlocks,
                                    addrType,
                                    sendToAddress,
                                    electrumXUrl,
                                    "",
                                    true,
                                    "applyNewTrade2Params",
                                    {electrumXUrl} );

    getSwap()->applyNewTrade21Params(secCurrency, offerExpTime, redeemTime, mwcBlocks, secBlocks,
            secTxFee);

    // Respond at onCreateNewSwapTrade, tag applyNewTrade2Params
}

void Swap::onCreateNewSwapTrade(QString tag, bool dryRun, QVector<QString> params, QString swapId, QString errMsg) {
    Q_UNUSED(swapId)
    if (tag == "applyNewTrade1Params") {
        Q_ASSERT(dryRun);
        Q_ASSERT(params.size()==6);
        emit sgnApplyNewTrade1Params(errMsg.isEmpty(), errMsg);
        if (errMsg.isEmpty()) {
            getSwap()->applyNewTrade1Params( params[0], params[1], params[2], params[3], params[4], params[5] );
        }
        return;
    }
    else if (tag == "applyNewTrade2Params") {
        Q_ASSERT(dryRun);
        Q_ASSERT(params.size()==1);
        emit sgnApplyNewTrade2Params(errMsg.isEmpty(), errMsg);
        if (errMsg.isEmpty()) {
            getSwap()->applyNewTrade22Params( params[0] );
        }
        return;
    }
    // Just a message from different request.
}


// Create and start a new swap. The data must be submitted by that moment
// Response: sgnCreateStartSwap(bool ok, QString errorMessage)
void Swap::createStartSwap() {
    getSwap()->createStartSwap();
}

void Swap::onCreateStartSwap(bool ok, QString errorMessage) {
    emit sgnCreateStartSwap(ok, errorMessage);
}

// Account that is related to this swap trade
QString Swap::getAccount() {
    return getSwap()->getAccount();
}

// List of the secondary currencies that wallet support
QVector<QString> Swap::secondaryCurrencyList() {
    return getSwap()->secondaryCurrencyList();
}

QString Swap::getCurrentSecCurrency() {
    return getSwap()->getCurrentSecCurrency();
}

QString Swap::getCurrentSecCurrencyFeeUnits() {
    return getSwap()->getCurrentSecCurrencyFeeUnits();
}

void Swap::setCurrentSecCurrency(QString secCurrency) {
    getSwap()->setCurrentSecCurrency(secCurrency);
}

QString Swap::getMwc2Trade() {
    return getSwap()->getMwc2Trade();
}

QString Swap::getSec2Trade() {
    return getSwap()->getSec2Trade();
}

QString Swap::getSecAddress() {
    return getSwap()->getSecAddress();
}

bool    Swap::isLockMwcFirst() {
    return getSwap()->isLockMwcFirst();
}

QString Swap::getBuyerAddress() {
    return getSwap()->getBuyerAddress();
}

// Return pairs of the expiration interval combo:
// <Interval is string> <Value in minutes>
QVector<QString> Swap::getExpirationIntervals() {
    return getSwap()->getExpirationIntervals();
}

int Swap::getOfferExpirationInterval() {
    return getSwap()->getOfferExpirationInterval();
}
int Swap::getSecRedeemTime() {
    return getSwap()->getSecRedeemTime();
}
double Swap::getSecTransactionFee() {
    return getSwap()->getSecTransactionFee();
}

int Swap::getMwcConfNumber() {
    return getSwap()->getMwcConfNumber();
}

int Swap::getSecConfNumber() {
    return getSwap()->getSecConfNumber();
}

QString Swap::getElectrumXprivateUrl() {
    return getSwap()->getElectrumXprivateUrl();
}

// Calculate the locking time for a NEW not yet created swap offer.
QVector<QString> Swap::getLockTime( QString secCurrency, int offerExpTime, int redeemTime, int mwcBlocks, int secBlocks ) {
    return getSwap()->getLockTime( secCurrency, offerExpTime, redeemTime, mwcBlocks, secBlocks );
}


}


