/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2022 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#pragma once

#include <Carbon/Carbon.h>
#include <QCoreApplication>
#include <QThread>
#include <memory>
#include <utility>

using CFDeallocator = decltype(&CFRelease);
using AutoCFArray = std::unique_ptr<const __CFArray, CFDeallocator>;
using AutoCFDictionary = std::unique_ptr<const __CFDictionary, CFDeallocator>;
using AutoTISInputSourceRef = std::unique_ptr<__TISInputSource, CFDeallocator>;

// TIS also runs inside Cocoa and Qt, so a private mutex cannot serialize it
template <typename Function> void runOSXInputSourceOperation(Function &&operation)
{
  auto *app = QCoreApplication::instance();
  if (QThread::currentThread() == app->thread()) {
    operation();
    return;
  }
  QMetaObject::invokeMethod(app, std::forward<Function>(operation), Qt::BlockingQueuedConnection);
}
