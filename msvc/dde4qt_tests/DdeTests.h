#include <QTest>

class DdeTests : public QObject {
  Q_OBJECT

private Q_SLOTS:
  void should_be_able_to_call_protocol_handler();
  //void onActivate(const QUrl& url);
  void startProtocol();
};