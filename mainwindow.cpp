/*
* Copyright (c) 2012-2015 Christian Surlykke, Petr Vanek
*
* This file is part of qt-lightdm-greeter 
* It is distributed under the LGPL 2.1 or later license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QPalette>
#include <QString>
#include <QDebug>

#include "mainwindow.h"
#include "loginform.h"
#include "settings.h"

MainWindow::MainWindow(int screen, QWidget *parent) :
    QWidget(parent),
    m_Screen(screen)
{
    setObjectName(QString("MainWindow_%1").arg(screen));
    
    
    QRect screenRect = QApplication::desktop()->screenGeometry(screen);
    setGeometry(screenRect);

    setBackground();

    // display login dialog only in the main screen
    
    if (showLoginForm()) {
        m_LoginForm = new LoginForm(this);

        /*
        int maxX = screenRect.width() - m_LoginForm->width();
        int maxY = screenRect.height() - m_LoginForm->height();
        int defaultX = 10*maxX/100;
        int defaultY = 50*maxY/100;
        int offsetX = getOffset(Settings().offsetX(), maxX, defaultX);
        int offsetY = getOffset(Settings().offsetY(), maxY, defaultY);
        m_LoginForm->move(offsetX, offsetY);
        */
        
        m_LoginForm->show();

        // This hack ensures that the primary screen will have focus
        // if there are more screens (move the mouse cursor in the center
        // of primary screen - not in the center of all X area). It
        // won't affect single-screen environments.
        int centerX = screenRect.width()/2 + screenRect.x();
        int centerY = screenRect.height()/2 + screenRect.y();
        QCursor::setPos(centerX, centerY);
    }
}

MainWindow::~MainWindow()
{
}

bool MainWindow::showLoginForm()
{
    return m_Screen == QApplication::desktop()->primaryScreen();
}

void MainWindow::setFocus(Qt::FocusReason reason)
{
    if (m_LoginForm) {
        m_LoginForm->setFocus(reason);
    }
    else  {
        QWidget::setFocus(reason);
    }
}

int MainWindow::getOffset(QString settingsOffset, int maxVal, int defaultVal)
{

    int offset = defaultVal > maxVal ? maxVal : defaultVal;

    if (! settingsOffset.isEmpty())  {
        if (QRegExp("^\\d+px$", Qt::CaseInsensitive).exactMatch(settingsOffset))  {
            offset = settingsOffset.left(settingsOffset.size() - 2).toInt();
            if (offset > maxVal) offset = maxVal;
        }
        else if (QRegExp("^\\d+%$", Qt::CaseInsensitive).exactMatch(settingsOffset)) {
            int offsetPct = settingsOffset.left(settingsOffset.size() -1).toInt();
            if (offsetPct > 100) offsetPct = 100;
            offset = (maxVal * offsetPct)/100;
        }
        else {
            qWarning() << "Could not understand" << settingsOffset
                       << "- must be of form <positivenumber>px or <positivenumber>%, e.g. 35px or 25%" ;
        }
    }

    return offset;
}

void MainWindow::setBackground()
{
    
    QImage backgroundImage;
    QSettings greeterSettings(CONFIG_FILE, QSettings::IniFormat);

    if (greeterSettings.contains(BACKGROUND_IMAGE_KEY)) {
        QString pathToBackgroundImage = greeterSettings.value(BACKGROUND_IMAGE_KEY).toString();
        
        backgroundImage = QImage(pathToBackgroundImage);
        if (backgroundImage.isNull()) {
            qWarning() << "Not able to read" << pathToBackgroundImage << "as image";
        }
             
    }
    
    QPalette palette;
    QRect rect = QApplication::desktop()->screenGeometry(m_Screen);
    if (backgroundImage.isNull()) {
        palette.setColor(QPalette::Background, Qt::black);
    }
    else {
        QBrush brush(backgroundImage.scaled(rect.width(), rect.height()));
        palette.setBrush(this->backgroundRole(), brush);
    }
    
    
    //====================
    /* test
    QImage backgroundImage;
    QPalette palette;
    QRect rect = QApplication::desktop()->screenGeometry(m_Screen);
    QString pathToBackgroundImage = ":/resources/Firewatch Mod v2.jpg";
    backgroundImage = QImage(pathToBackgroundImage);
    QBrush brush(backgroundImage.scaled(rect.width(), rect.height()));
    palette.setBrush(this->backgroundRole(), brush);
    */
    //====================

    this->setPalette(palette);
}


