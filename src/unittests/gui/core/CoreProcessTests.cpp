// SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception

#include "gui/core/CoreProcess.h"
#include "common/ExitCodes.h"

#include <QTemporaryDir>
#include <QTest>

namespace deskflow::gui {

class CoreProcessTests : public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void retriesUnexpectedExit_data()
  {
    QTest::addColumn<int>("exitCode");
    QTest::addColumn<QProcess::ExitStatus>("exitStatus");
    QTest::newRow("abort") << 6 << QProcess::CrashExit;
    QTest::newRow("crash-with-config-exit-code") << s_exitConfig << QProcess::CrashExit;
    QTest::newRow("runtime-failure") << s_exitFailed << QProcess::NormalExit;
    QTest::newRow("normal-reconnect") << s_exitSuccess << QProcess::NormalExit;
  }

  void retriesUnexpectedExit()
  {
    QFETCH(int, exitCode);
    QFETCH(QProcess::ExitStatus, exitStatus);
    ServerConfig config;
    CoreProcess process(config);
    process.m_processState = core::ProcessState::Started;
    process.onProcessFinished(exitCode, exitStatus);
    QCOMPARE(process.processState(), core::ProcessState::RetryPending);
    QVERIFY(process.m_retryTimer.isActive());
  }

  void doesNotRetryRequestedStop()
  {
    ServerConfig config;
    CoreProcess process(config);
    process.m_processState = core::ProcessState::Stopping;
    process.onProcessFinished(15, QProcess::CrashExit);
    QCOMPARE(process.processState(), core::ProcessState::Stopped);
    QVERIFY(!process.m_retryTimer.isActive());
  }

  void doesNotRetryInvalidConfig()
  {
    ServerConfig config;
    CoreProcess process(config);
    process.m_processState = core::ProcessState::Started;
    process.onProcessFinished(s_exitConfig, QProcess::NormalExit);
    QCOMPARE(process.processState(), core::ProcessState::Stopped);
    QVERIFY(!process.m_retryTimer.isActive());
  }
};

} // namespace deskflow::gui

int main(int argc, char **argv)
{
  QTemporaryDir settings;
  qputenv("XDG_CONFIG_HOME", settings.path().toUtf8());
  QApplication app(argc, argv);
  deskflow::gui::CoreProcessTests tests;
  return QTest::qExec(&tests, argc, argv);
}

#include "CoreProcessTests.moc"
