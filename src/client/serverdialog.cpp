#include "serverdialog.h"
#include "ui_serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);

    // setup regex
    QRegExp serverIPRegex("[0-9]{3}.[0-9]{3}.[0-9]{3}.[0-9]{3}");
    QRegExp serverPortRegex("[0-9]{4}");

    QValidator *ip_validator = new QRegExpValidator(serverIPRegex, this);
    QValidator *port_validator = new QRegExpValidator(serverPortRegex, this);

    // assert regex to inputs
    ui->serverDialogInputIP->setValidator(ip_validator);
    ui->serverDialogInputPort->setValidator(port_validator);
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

void ServerDialog::done(int status) {
    if( status == QDialog::Accepted ) {
        if( ui->serverDialogInputIP->hasAcceptableInput() &&
            ui->serverDialogInputPort->hasAcceptableInput() ) {
            QDialog::done(status);
            return;
        } else {
            ui->serverDialogPrompt->setText("Your input is malformed, please try again!\nIP: ___.___.___.___ Port: ____");
            return;
        }
    } else {
        QDialog::done(status);
        return;
    }
}

QString ServerDialog::getServerIP() {
    return ui->serverDialogInputIP->text();
}

QString ServerDialog::getServerPort() {
    return ui->serverDialogInputPort->text();
}
