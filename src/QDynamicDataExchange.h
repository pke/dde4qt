#pragma once

#include <QCoreApplication>
#include <QAbstractNativeEventFilter>
#include <QSettings>
#include <QUrl>
#include <QStringList>

#define WIN32_LEAN_AND_MEAN
#include <wtypes.h>
#include <winnt.h>
#include <winbase.h>
#include <winuser.h>
#include <dde.h>

namespace win32 {
  class Atom {
  public:
    Atom(const QString& name) : atom(GlobalAddAtomW((PCWSTR)name.utf16())) {
    }

    ~Atom() {
      ::GlobalDeleteAtom(atom);
    }

    operator ATOM() const { return atom; }
  private:
    ATOM atom;
  };

  class QDdeFilter : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT
  Q_SIGNALS:
    void command(const QString& command);
  
  public:
    QDdeFilter(const QString& application, const QString& topic, QObject* parent=nullptr) 
      : QObject(parent), 
      application(application), topic(topic) {
    }

  private:
    bool nativeEventFilter(const QByteArray& /*eventType*/, void* message, long* /*result*/) override {
      auto msg = reinterpret_cast<LPMSG>(message);
      if (msg->message == WM_DDE_INITIATE) {
        if (LOWORD(msg->lParam) == application && HIWORD(msg->lParam) == topic) {
          ::SendMessageW((HWND)msg->wParam, WM_DDE_ACK, (WPARAM)msg->hwnd, ::ReuseDDElParam(msg->lParam, WM_DDE_INITIATE, WM_DDE_ACK, application, topic));
          return true;
        }
      } else if (msg->message == WM_DDE_EXECUTE) {
        HGLOBAL hcommand;
        ::UnpackDDElParam(msg->message, msg->lParam, 0, (PUINT_PTR)&hcommand);
        const QString commandString(QString::fromUtf16((const ushort*)::GlobalLock((HGLOBAL)hcommand)));
        ::GlobalUnlock(hcommand);
        ::PostMessageW((HWND)msg->wParam, WM_DDE_ACK, (WPARAM)msg->hwnd, ::ReuseDDElParam(msg->lParam, WM_DDE_EXECUTE, WM_DDE_ACK, (UINT)0x8000, (UINT_PTR)hcommand));
        emit command(commandString);
        return true;
      } else if (msg->message == WM_DDE_TERMINATE) {
        ::PostMessageW((HWND)msg->wParam, WM_DDE_TERMINATE, (WPARAM)msg->hwnd, 0);
      }
      return false;
    }
  
    Atom application;
    Atom topic;
  };

  class QUrlProtocolHandler : public QObject {
    Q_OBJECT
  Q_SIGNALS:
    void activate(const QUrl& url);
    
  public:
    QUrlProtocolHandler(const QString& schema, const QString& application = QCoreApplication::applicationName(), const QString& topic = QStringLiteral("System"), QObject* parent=nullptr) :
      QObject(parent),
      ddeFilter(application, topic, this),
      schema(schema),
      application(application),
      topic(topic) {
      connect(&ddeFilter, &QDdeFilter::command, this, &QUrlProtocolHandler::onCommand);
      QCoreApplication::instance()->installNativeEventFilter(&ddeFilter);
    }

    ~QUrlProtocolHandler() {
      QCoreApplication::instance()->removeNativeEventFilter(&ddeFilter);
    }

    void install(const QString& applicationPath = QCoreApplication::instance()->arguments().at(0)) {
      if (!schema.isEmpty()) {
        QSettings registry(QString("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\%1").arg(schema), QSettings::NativeFormat);
        registry.setValue("URL Protocol", "");
        registry.setValue("shell/open/command/.", applicationPath);
        registry.setValue("shell/open/ddeexec/.", "%1");
        registry.setValue("shell/open/ddeexec/application/.", application);
        registry.setValue("shell/open/ddeexec/topic/.", topic);
        registry.sync();        
      }
    }

    void uninstall() {
      // Prevent catastrophic removal of all Classes subkey
      if (!schema.isEmpty()) {
        QSettings registry(QString("HKEY_CURRENT_USER\\SOFTWARE\\Classes"), QSettings::NativeFormat);
        registry.remove(schema);
      }
    }

  private Q_SLOTS:
    void onCommand(const QString& command) {
      emit activate(QUrl(command));
    }

  private:
    const QString schema;
    const QString application;
    const QString topic;
    QDdeFilter ddeFilter;
  };
} // win32
