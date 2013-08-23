#include <QEventLoop>
#include <QDesktopServices>
#include "DdeTests.h"
#include <QMainWindow>
#include <QTimer>
#include "../../src/QDynamicDataExchange.h"

QTEST_MAIN(DdeTests);

bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000) {
  QEventLoop loop;
  QTimer timer;
  timer.setInterval(timeout);
  timer.setSingleShot(true);

  loop.connect(sender, signal, SLOT(quit()));
  loop.connect(&timer, SIGNAL(timeout()), SLOT(quit()));
  timer.start();
  loop.exec();

  return timer.isActive();
}

void DdeTests::should_be_able_to_call_protocol_handler() {
  {
    win32::QUrlProtocolHandler handler("dde4qt", "dde4qt_test");

    handler.install();

    QSettings registry(QString("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\dde4qt"), QSettings::NativeFormat);
    QCOMPARE(registry.value("URL Protocol").toString(), QString(""));
    QCOMPARE(registry.value("shell/open/command/.").toString(), QCoreApplication::instance()->arguments().at(0));
    QCOMPARE(registry.value("shell/open/ddeexec/.").toString(), QString("%1"));
    QCOMPARE(registry.value("shell/open/ddeexec/application/.").toString(), QString("dde4qt_test"));
    QCOMPARE(registry.value("shell/open/ddeexec/topic/.").toString(), QString("System"));

    handler.uninstall();
  }

  QSettings registry(QString("HKEY_CURRENT_USER\\SOFTWARE\\Classes"), QSettings::NativeFormat);
  QVERIFY(!registry.contains("dde4qt"));
}

void DdeTests::startProtocol() {
  //QDesktopServices::openUrl(QUrl("dde4qt:test"));
}