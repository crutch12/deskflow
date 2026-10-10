// SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception

#include "platform/OSXComputer.h"
#include "arch/Arch.h"
#include "base/EventQueue.h"
#include "base/Log.h"
#include "common/Settings.h"
#include "deskflow/unix/AppUtilUnix.h"

#include <QApplication>
#include <QTemporaryDir>
#include <QTest>

namespace {
int hideCount = 0;
CGError cursorError = kCGErrorSuccess;

class ReadyEventQueue : public EventQueue
{
public:
  void waitForReady() const override
  {
  }
};
}

// Model Quartz's hide count without changing the user's cursor
extern "C" CGError CGDisplayHideCursor(CGDirectDisplayID)
{
  if (cursorError == kCGErrorSuccess)
    ++hideCount;
  return cursorError;
}

extern "C" CGError CGDisplayShowCursor(CGDirectDisplayID)
{
  if (cursorError == kCGErrorSuccess && hideCount > 0)
    --hideCount;
  return cursorError;
}

extern "C" CGError CGAssociateMouseAndMouseCursorPosition(boolean_t)
{
  return kCGErrorSuccess;
}

class OSXComputerTests : public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void repeatedLeaveDoesNotKeepCursorHidden()
  {
    ReadyEventQueue events;
    AppUtilUnix util(&events);
    OSXComputer screen(&events, false);
    screen.waitForCarbonLoop();
    QTRY_VERIFY(CFRunLoopIsWaiting(screen.m_pmRunloop));
    screen.hideCursor();
    screen.hideCursor();
    screen.showCursor();
    QCOMPARE(hideCount, 0);
    QVERIFY(!screen.m_cursorHidden);
  }

  void failedShowCanBeRetried()
  {
    ReadyEventQueue events;
    AppUtilUnix util(&events);
    OSXComputer screen(&events, false);
    screen.waitForCarbonLoop();
    QTRY_VERIFY(CFRunLoopIsWaiting(screen.m_pmRunloop));
    screen.hideCursor();
    cursorError = kCGErrorFailure;
    screen.showCursor();
    const bool stillHidden = screen.m_cursorHidden;
    cursorError = kCGErrorSuccess;
    screen.showCursor();
    QVERIFY(stillHidden);
    QCOMPARE(hideCount, 0);
  }

  void init()
  {
    hideCount = 0;
    cursorError = kCGErrorSuccess;
  }
};

int main(int argc, char **argv)
{
  QTemporaryDir settings;
  qputenv("XDG_CONFIG_HOME", settings.path().toUtf8());
  QApplication app(argc, argv);
  Arch arch;
  arch.init();
  Log log;
  Settings::setValue(Settings::Core::PreventSleep, false);
  OSXComputerTests tests;
  return QTest::qExec(&tests, argc, argv);
}

#include "OSXComputerTests.moc"
