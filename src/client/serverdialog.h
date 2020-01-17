#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>
#include <QRegExpValidator>

namespace Ui {
class ServerDialog;
}

class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = nullptr);
    void done(int status) override;

    QString getServerIP();
    QString getServerPort();
    ~ServerDialog();

private:
    Ui::ServerDialog *ui;
};

#endif // SERVERDIALOG_H
