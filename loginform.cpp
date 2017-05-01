/*
* Copyright (c) 2012-2015 Christian Surlykke
*
* This file is part of qt-lightdm-greeter 
* It is distributed under the LGPL 2.1 or later license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <QDebug>
#include <QCompleter>
#include <QAbstractListModel>
#include <QModelIndex>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QPixmap>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QLightDM/UsersModel>
#include <QMetaMethod>
#include <QRegion>
#include <QTimer>
#include <QTime>

#include "loginform.h"
#include "ui_loginform.h"
#include "settings.h"
#include "mainwindow.h"

const int KeyRole = QLightDM::SessionsModel::KeyRole;

int rows(QAbstractItemModel& model) {
    return model.rowCount(QModelIndex());
}

QString displayData(QAbstractItemModel& model, int row, int role)
{
    QModelIndex modelIndex = model.index(row, 0);
    return model.data(modelIndex, role).toString();
}

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent), 
    ui(new Ui::LoginForm),
    m_Greeter(),
    power(this),
    sessionsModel()
{
    if (!m_Greeter.connectSync()) {
        close();
    }

    ui->setupUi(this);
    initialize();

    this->setGeometry(0, 0, parent->width(), parent->height());

    //frame
    int perfil_w = 100;
    int perfil_h = 100;
    ui->frame->setGeometry((this->width() / 2) - (perfil_w / 2), (this->height() / 4) - (perfil_h / 2), perfil_w, perfil_h);

    QRegion *region = new QRegion(*(new QRect(0, 0, perfil_w, perfil_h)),QRegion::Ellipse);
    ui->frame->setMask(*region);

    QPixmap userImage;
    QSettings greeterSettings(CONFIG_FILE, QSettings::IniFormat);

    if (greeterSettings.contains(USER_IMAGE_KEY)) {
        QString pathToUserImage = greeterSettings.value(USER_IMAGE_KEY).toString();
        userImage = QPixmap(pathToUserImage);     
    }

    /* test
    QPixmap bg_(":/resources/lain.jpg");
    bg_ = bg_.scaled(perfil_w, perfil_h, Qt::IgnoreAspectRatio);
    QPalette *palette_ = new QPalette();
    palette_->setBrush(QPalette::Background, bg_);
    ui->frame->setPalette(*palette_);
    */
    if (!userImage.isNull())
    {
        userImage = userImage.scaled(perfil_w, perfil_h, Qt::IgnoreAspectRatio);
        QPalette *palette_ = new QPalette();
        palette_->setBrush(QPalette::Background, userImage);
        ui->frame->setPalette(*palette_);
    }

    //frame_2
    int frame_w = 105;
    int frame_h = 105;
    ui->frame_2->setGeometry((this->width() / 2) - (frame_w / 2), (this->height() / 4) - (frame_h / 2), frame_w, frame_h);
    ui->frame_2->setStyleSheet("background-color: #fff; border-radius:" + QString::number(frame_w / 2) + "px");
    ui->frame_2->setAutoFillBackground(true);

    //frame_3
    int host_w = ui->frame_3->width();
    int host_h = ui->frame_3->height();
    ui->frame_3->setGeometry((this->width() / 2) - (host_w / 2), (this->height() / 4) + 30, host_w, host_h);

    //clock
    int clock_w = ui->clock->width();
    int clock_h = ui->clock->height();
    ui->clock->setGeometry(this->width() - (clock_w + 50), (this->height() / 2) + 100, clock_w, clock_h);

    //leaveComboBox
    int leave_w = ui->leaveComboBox->width();
    int leave_h = ui->leaveComboBox->height();
    ui->leaveComboBox->setGeometry(this->width() - 150, this->height() - 100, leave_w, leave_h);

    //sessionCombo
    int combo_w = ui->sessionCombo->width();
    int combo_h = ui->sessionCombo->height();
    ui->sessionCombo->setGeometry(this->width() - 310, this->height() - 100, combo_w, combo_h);

    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&LoginForm::updateTime);
    timer->start(1000);
    updateTime();
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::updateTime()
{
    QTime currentTime = QTime::currentTime();
    QString currentTimeText = currentTime.toString("hh:mm:ss");
    ui->clock->setText(currentTimeText);
}

void LoginForm::on_pushButton_clicked()
{
    respond();
}

void LoginForm::setFocus(Qt::FocusReason reason)
{
    if (ui->userInput->text().isEmpty()) {
        ui->userInput->setFocus(reason);
    } else {
        ui->passwordInput->setFocus(reason);
    }
}


void LoginForm::initialize()
{
    ui->hostnameLabel->setText(m_Greeter.hostname());

    ui->sessionCombo->setModel(&sessionsModel);

    addLeaveEntry(power.canShutdown(), "system-shutdown", tr("Shutdown"), "shutdown");
    addLeaveEntry(power.canRestart(), "system-reboot", tr("Restart"), "restart");
    addLeaveEntry(power.canHibernate(), "system-suspend-hibernate", tr("Hibernate"), "hibernate");
    addLeaveEntry(power.canSuspend(), "system-suspend", tr("Suspend"), "suspend");
    ui->leaveComboBox->setDisabled(ui->leaveComboBox->count() <= 1);

    ui->sessionCombo->setCurrentIndex(0);
    setCurrentSession(m_Greeter.defaultSessionHint());

    connect(ui->userInput, SIGNAL(editingFinished()), this, SLOT(userChanged()));
    connect(ui->leaveComboBox, SIGNAL(activated(int)), this, SLOT(leaveDropDownActivated(int)));
    connect(&m_Greeter, SIGNAL(showPrompt(QString, QLightDM::Greeter::PromptType)), this, SLOT(onPrompt(QString, QLightDM::Greeter::PromptType)));
    connect(&m_Greeter, SIGNAL(authenticationComplete()), this, SLOT(authenticationComplete()));

    //ui->passwordInput->setEnabled(false);
    //ui->passwordInput->clear();

    if (! m_Greeter.hideUsersHint()) {
        QStringList knownUsers;
        QLightDM::UsersModel usersModel;
        for (int i = 0; i < usersModel.rowCount(QModelIndex()); i++) {
            knownUsers << usersModel.data(usersModel.index(i, 0), QLightDM::UsersModel::NameRole).toString();
        }
        ui->userInput->setCompleter(new QCompleter(knownUsers));
        ui->userInput->completer()->setCompletionMode(QCompleter::InlineCompletion);
    }

    QString user = Cache().getLastUser();
    if (user.isEmpty()) {
        user = m_Greeter.selectUserHint();
    }
    ui->userInput->setText(user);
    userChanged();
}

void LoginForm::userChanged()
{
    setCurrentSession(Cache().getLastSession(ui->userInput->text()));

    if (m_Greeter.inAuthentication()) {
        m_Greeter.cancelAuthentication();
    }
    if (! ui->userInput->text().isEmpty()) {
        m_Greeter.authenticate(ui->userInput->text());
        ui->passwordInput->setFocus();
    }
    else {
        ui->userInput->setFocus();
    }
}

void LoginForm::leaveDropDownActivated(int index)
{
    QString actionName = ui->leaveComboBox->itemData(index).toString();
    if      (actionName == "shutdown") power.shutdown();
    else if (actionName == "restart") power.restart();
    else if (actionName == "hibernate") power.hibernate();
    else if (actionName == "suspend") power.suspend();
}

void LoginForm::respond()
{
    m_Greeter.respond(ui->passwordInput->text().trimmed());
    //ui->passwordInput->clear();
    //ui->passwordInput->setEnabled(false);
}

void LoginForm::onPrompt(QString prompt, QLightDM::Greeter::PromptType promptType)
{
    //ui->passwordInput->setEnabled(true);
    ui->passwordInput->setFocus();
}


void LoginForm::addLeaveEntry(bool canDo, QString iconName, QString text, QString actionName)
{
    if (canDo) {
        ui->leaveComboBox->addItem(QIcon::fromTheme(iconName), text, actionName);
    }
}

QString LoginForm::currentSession()
{
    QModelIndex index = sessionsModel.index(ui->sessionCombo->currentIndex(), 0, QModelIndex());
    return sessionsModel.data(index, QLightDM::SessionsModel::KeyRole).toString();
}

void LoginForm::setCurrentSession(QString session)
{
    for (int i = 0; i < ui->sessionCombo->count(); i++) {
        if (session == sessionsModel.data(sessionsModel.index(i, 0), KeyRole).toString()) {
            ui->sessionCombo->setCurrentIndex(i);
            return;
        }
    }
}


void LoginForm::authenticationComplete()
{
    if (m_Greeter.isAuthenticated()) {
        Cache().setLastUser(ui->userInput->text());
        Cache().setLastSession(ui->userInput->text(), currentSession());
        Cache().sync();
        m_Greeter.startSessionSync(currentSession());
    }
    else  {
        ui->passwordInput->clear();
        userChanged();
    }
}

void LoginForm::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        respond();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

