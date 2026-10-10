# Flow for macOS

Build natively on an Apple Silicon Mac, including M1 and M3
Use the same or a newer macOS version on the machine where you install the result

Install Xcode Command Line Tools and [Homebrew](https://brew.sh), then run:

```sh
xcode-select --install
brew install cmake ninja qt openssl@3
git clone --branch flow-macos https://github.com/crutch12/deskflow.git
cd deskflow
sh deploy/mac/build.sh
```

The installer is `build/macos/deskflow-<version>-macos-arm64.dmg`
It contains `Flow.app` and the `flow-core` worker, with Qt and OpenSSL bundled
Homebrew is only needed to build, not to run the installed app

Open the DMG and drag Flow into Applications
The default build is ad-hoc signed, not notarized
If macOS blocks it, use Privacy & Security > Open Anyway for your own build
Grant Flow Accessibility and Input Monitoring access when prompted
The existing Deskflow settings and bundle identifier are preserved

The installer build does not build or run tests
To build the tests separately:

```sh
sh deploy/mac/build.sh -DBUILD_TESTS=ON -DSKIP_BUILD_TESTS=ON
ctest --test-dir build/macos/src/unittests --output-on-failure
```

Those tests need a graphical session and Accessibility permissions
Qt packaging uses only the Cocoa, macOS style, SVG and native TLS plugins
See [Qt deployment](https://doc.qt.io/qt-6/macos-deployment.html) for the packaging options
