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

#ifndef MWC_QT_WALLET_SWAP_S_H
#define MWC_QT_WALLET_SWAP_S_H

#include "state.h"
#include "../wallet/wallet.h"
#include <QMap>
#include <QSet>
#include "../util/httpclient.h"

namespace state {

struct AutoswapTask {
    QString swapId;
    int64_t lastUpdatedTime = 0;

    void setData(QString _swapId, int64_t _lastUpdatedTime) { swapId = _swapId; lastUpdatedTime = _lastUpdatedTime; }
};

class Swap : public util::HttpClient, public State {
Q_OBJECT
public:
    Swap(StateContext * context);
    virtual ~Swap() override;

    // Show first page with trade List
    void pageTradeList();
    // Edit/View Trade Page
    void viewTrade(QString swapId);
    // Show trade details page
    void showTradeDetails(QString swapId);

    bool isTradeRunning(const QString & swapId) const;

    // Reset the new trade data and switch to the first page.
    void initiateNewTrade();
    // Show the trade page 1
    void showNewTrade1();
    // Show the trade page 2
    void showNewTrade2();

    // Apply params from trade1 and switch to trade2 panel. Expected that params are validated by the wallet(bridge)
    void applyNewTrade1Params(QString acccount, QString secCurrency, QString mwcAmount, QString secAmount,
                                          QString secAddress, QString sendToAddress );
    // Apply part1 params from trade2 panel. Expected that params are validated by the windows
    void applyNewTrade21Params(QString secCurrency, int offerExpTime, int redeemTime, int mwcBlocks, int secBlocks,
                                          double secTxFee);
    // Apply [part2 params from trade2 panel and switch to the review (panel3).
    // Expected that params are validated by the wallet(bridge)
    void applyNewTrade22Params(QString electrumXUrl);


    // Create and start a new swap. The data must be submitted by that moment
    // Response: onCreateStartSwap(bool ok, QString errorMessage)
    void createStartSwap();

    // Account that is related to this swap trade
    QString getAccount() const {return  newSwapAccount;}

    // List of the secondary currencies that wallet support
    QVector<QString> secondaryCurrencyList();

    // Current selected currency to trade
    QString getCurrentSecCurrency() const {return newSwapCurrency;}
    void setCurrentSecCurrency(QString secCurrency);

    QString getCurrentSecCurrencyFeeUnits() const;

    // Current trade parameters.
    QString getMwc2Trade() const {return newSwapMwc2Trade;}
    QString getSec2Trade() const {return newSwapSec2Trade;}
    QString getSecAddress() const {return newSwapSecAddress;}
    bool isLockMwcFirst() const {return newSwapSellectLockFirst;}
    QString getBuyerAddress() const {return newSwapBuyerAddress;}

    // Return pairs of the expiration interval combo:
    // <Interval is string> <Value in minutes>
    QVector<QString> getExpirationIntervals() const;

    int getOfferExpirationInterval() const {return newSwapOfferExpirationTime;}
    int getSecRedeemTime() const {return newSwapRedeemTime;}
    double getSecTransactionFee() const {return newSwapSecTxFee;}
    int getMwcConfNumber() const {return newSwapMwcConfNumber;}
    int getSecConfNumber() const {return newSwapSecConfNumber;}
    QString getElectrumXprivateUrl() const {return newSwapElectrumXUrl;}

    // Calculate the locking time for a NEW not yet created swap offer.
    Q_INVOKABLE QVector<QString> getLockTime( QString secCurrency, int offerExpTime, int redeemTime, int mwcBlocks, int secBlocks );

private:
signals:

    void onSwapTradeStatusUpdated(QString swapId, QString currentAction, QString currentState,
                               QVector<wallet::SwapExecutionPlanRecord> executionPlan,
                               QVector<wallet::SwapJournalMessage> tradeJournal);

    void onCreateStartSwap(bool ok, QString errorMessage);

protected:
    virtual NextStateRespond execute() override;


    virtual void onProcessHttpResponse(bool requestOk, const QString & tag, QJsonObject & jsonRespond,
                                       const QString & param1,
                                       const QString & param2,
                                       const QString & param3,
                                       const QString & param4) override;
private:
    virtual void timerEvent(QTimerEvent *event) override;

    void resetNewSwapData();

    // Start trade to run
    void runTrade(QString swapId);

    int calcConfirmationsForMwcAmount(double mwcAmount);

    // Request latest fees for the coins
    void updateFeesIsNeeded();
private
slots:

    void onPerformAutoSwapStep(QString swapId, bool swapIsDone, QString currentAction, QString currentState,
                               QVector<wallet::SwapExecutionPlanRecord> executionPlan,
                               QVector<wallet::SwapJournalMessage> tradeJournal,
                               QString error );

    void onNewSwapTrade(QString currency, QString swapId);

    // Response from createNewSwapTrade, SwapId on OK,  errMsg on failure
    void onCreateNewSwapTrade(QString tag, bool dryRun, QVector<QString> params, QString swapId, QString errMsg);

private:
    // Key: swapId,  Value: running Task
    QMap<QString, AutoswapTask> runningSwaps;
    QString  runningTask;

    QSet<QString> shownMessages;

    // New trade data.
    QString newSwapAccount;
    QString newSwapCurrency;
    QString newSwapMwc2Trade;
    QString newSwapSec2Trade;
    QString newSwapSecAddress;
    bool    newSwapSellectLockFirst = true;
    QString newSwapBuyerAddress;
    int     newSwapOfferExpirationTime = -1;
    int     newSwapRedeemTime = -1;
    double  newSwapSecTxFee = 0.0;
    int     newSwapMwcConfNumber = -1;
    int     newSwapSecConfNumber = -1;
    QString newSwapElectrumXUrl;
};

}

#endif //MWC_QT_WALLET_SWAP_H
