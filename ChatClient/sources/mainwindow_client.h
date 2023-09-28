#include <QMainWindow>
#include <QTcpSocket>
#include "login_window.h"
#include "registration_window.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindowClient;
    class LoginWindow;
    class RegistrationWindow;
}
QT_END_NAMESPACE

class MainWindowClient : public QMainWindow

{
    Q_OBJECT
    Ui::MainWindowClient* _ui;
    QTcpSocket* _socket = new QTcpSocket();
    LoginWindow* _loginWindow;
    RegistrationWindow* _registrationWindow;
    bool _online = false;
    QString _curLogin;

public:
    MainWindowClient(QWidget *parent = nullptr);
    ~MainWindowClient();
    auto getSocket()->QTcpSocket* const;
    auto getCurLogin()->QString const;
    void setCurLogin(const QString& newCurLogin);
    void EnterUserInChat(const QString& login, const QString& users);
    void BanUser();
    void AddUsersInCombobox(QString str);
    void ReceiveCommandMessageFromServer(QString cmd);
    void ReceiveMessagesBetweenTwoUsers(QString str);

private slots:
    void disconnect();
    void openLoginWidget();
    void openRegistrationWidget();
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void readData();
    void exitUser();
};
