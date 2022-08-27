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

#include "Log.h"
#include "ioutils.h"
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QDateTime>
#include "../wallet/mwc713task.h"
#include "../control/messagebox.h"

// 1 MB is a reasonable size limit.
#define LOG_SIZE_LIMIT  1000000

namespace logger {

static LogSender *   logClient = nullptr;
static LogReceiver * logServer = nullptr;

static bool logMwc713outBlocked = false;

const QString LOG_FILE_NAME = "mwcwallet.log";

void initLogger( bool logsEnabled) {
    logClient = new LogSender(true);

    enableLogs(logsEnabled);

    logClient->doAppend2logs(true, "", "mwc-qt-wallet is started..." );
}

void cleanUpLogs() {
    // Logs expected to be disabled first
    Q_ASSERT(logServer == nullptr );
    QString logPath = ioutils::getAppDataPath("logs");

    QFile::remove(logPath + "/" + LOG_FILE_NAME);
    QFile::remove(logPath + "/prev_" + LOG_FILE_NAME);
}

// enable/disable logs
void enableLogs( bool enableLogs ) {
    if (enableLogs) {
        if (logServer != nullptr )
            return;

        logServer = new LogReceiver(LOG_FILE_NAME);
        QObject::connect( logClient, &LogSender::doAppend2logs, logServer, &LogReceiver::onAppend2logs, Qt::DirectConnection); // Qt::QueuedConnection );
    }
    else {
        if (logServer == nullptr)
            return;

        delete logServer;
        logServer = nullptr;
    }
}


void LogSender::log(bool addDate, const QString & prefix, const QString & line) {
    if (asyncLogging) {
        emit doAppend2logs(addDate, prefix, line);
    }
    else {
        logServer->onAppend2logs(addDate, prefix, line );
    }
}

// Create logger file with some simplest rotation
LogReceiver::LogReceiver(const QString & filename) {
    QString logPath = ioutils::getAppDataPath("logs");

    QString logFn = logPath + "/" + filename;

    { // Check if need to rotate
        QFileInfo fi(logFn);

        if (fi.size() > LOG_SIZE_LIMIT) {
            const QString prevLogFn = "prev_"+filename;
            QDir logDir( logPath );
            logDir.remove(prevLogFn);
            logDir.rename(filename, prevLogFn);
        }
    }

    logFile = new QFile( logFn );
    if (! logFile->open( QFile::WriteOnly | QFile::Append ) ) {
        control::MessageBox::messageText(nullptr, "Critical Error", "Unable to open the logger file: " + logPath );
        QApplication::quit();
        return;
    }
}
LogReceiver::~LogReceiver() {
    delete logFile;
}

void LogReceiver::onAppend2logs(bool addDate, QString prefix, QString line ) {
    QString logLine;
    if (addDate)
        logLine += QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") + " ";

    logLine += prefix + " " + line + "\n";

    logFile->write( logLine.toUtf8() );
    logFile->flush();
}

// Global methods that do logging

void blockLogMwc713out(bool blockOutput) {
    logMwc713outBlocked = blockOutput;
}


// mwc713 IOs
void logMwc713out(QString str) {
    Q_ASSERT(logClient); // call initLogger first

    if (logMwc713outBlocked) {
        logClient->doAppend2logs(true, "mwc713>>", "CENSORED");
        return;
    }

    auto lns = str.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for (auto & l: lns) {
        logClient->doAppend2logs(true, "mwc713>>", l);
    }
}

void logMwc713in(QString str) {
    Q_ASSERT(logClient); // call initLogger first
    logClient->doAppend2logs(true, "mwc713<<", str);
}

void logMwcNodeOut(QString str) {
    Q_ASSERT(logClient);

    auto lns = str.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for (auto & l: lns) {
        logClient->doAppend2logs(true, "mwc-node>>", l);
    }

}


// Tasks to excecute on mwc713
void logTask( QString who, wallet::Mwc713Task * task, QString comment ) {
    Q_ASSERT(logClient); // call initLogger first
    if (task == nullptr)
        return;
    logClient->doAppend2logs(true, who, "Task " + task->toDbgString() + "  "+comment );
}

// Events activity
void logEmit(QString who, QString event, QString params) {
    // in tests there is no logger
    if (logClient) // call initLogger first
        logClient->doAppend2logs(true, who, "emit " + event + (params.length()==0 ? "" : (" with "+params)) );
}

void logInfo(QString who, QString message) {
    Q_ASSERT(logClient); // call initLogger first
    logClient->doAppend2logs(true, who, message );
}

void logParsingEvent(wallet::WALLET_EVENTS event, QString message ) {
    Q_ASSERT(logClient); // call initLogger first
    if (logMwc713outBlocked) { // Skipping event during block pahse as well
        logClient->doAppend2logs(true, "Event>", "CENSORED" );
        return;
    }

    if (event == wallet::WALLET_EVENTS::S_LINE) // Lines are reliable and not needed into the nogs. Let's keep logs less noisy.
        return;

    logClient->doAppend2logs(true, "mwc713-Event>", toString(event) + " [" + message + "]" );
}

void logNodeEvent( tries::NODE_OUTPUT_EVENT event, QString message ) {
    Q_ASSERT(logClient); // call initLogger first
    logClient->doAppend2logs(true, "mwc-node-Event>", toString(event) + " [" + message + "]" );
}


}


