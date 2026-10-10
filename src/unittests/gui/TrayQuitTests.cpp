// SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception

#include "gui/MainWindow.h"
#include "common/Settings.h"

#include <QAction>
#include <QApplication>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>

class TrayQuitTests : public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void quitsWhileWindowIsHidden()
  {
    Settings::setValue(Settings::Gui::CloseToTray, true);
    Settings::setValue(Settings::Gui::CloseReminder, false);
    Settings::setValue(Settings::Gui::AutoStartCore, false);
    Settings::setValue(Settings::Gui::AutoUpdateCheck, false);
    Settings::setValue(Settings::Security::TlsEnabled, false);
    MainWindow window;
    window.hide();
    QAction *quit = nullptr;
    for (auto *action : window.findChildren<QAction *>()) {
      if (action->text() == "&Quit" && action->menuRole() == QAction::NoRole) {
        quit = action;
        break;
      }
    }
    QVERIFY(quit);
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect(&watchdog, &QTimer::timeout, qApp, [] { qApp->exit(1); });
    watchdog.start(2000);
    QTimer::singleShot(0, quit, &QAction::trigger);
    QCOMPARE(QApplication::exec(), 0);
  }
};

int main(int argc, char **argv)
{
  QTemporaryDir settings;
  qputenv("XDG_CONFIG_HOME", settings.path().toUtf8());
  qputenv("TMPDIR", settings.path().toUtf8());
  QApplication app(argc, argv);
  TrayQuitTests tests;
  return QTest::qExec(&tests, argc, argv);
}

#include "TrayQuitTests.moc"
