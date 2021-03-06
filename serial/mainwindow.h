/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QTimer>

#include "calib.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void frameEnd();
    void clear();

    void handleError(QSerialPort::SerialPortError error);

    void on_cBox_chn_currentTextChanged(const QString &arg1);

    void on_pBt_enter_clicked();

    void on_pBt_exit_clicked();

    void on_pBt_calib_clicked();

    void on_pBt_readcal_clicked();

    void on_cBox_seg_currentIndexChanged(int index);

private:
    void initActionsConnections();
    void initButtons();
    void initEdit();
    void initComboBoxs();
    void initTable();
    void modbus(QByteArray &adu, calib_t *data);
    unsigned short crc16(unsigned char *pucFrame, unsigned short usLen);
    unsigned short chksum(unsigned short *buf, int size);
    void calib_do_rsp_received(calib_t *rsp);
    void calib_sam_rsp_received(calib_sam_t *rsp);

private:
    Ui::MainWindow *ui;

    SettingsDialog *settings;
    QSerialPort *serial;
    QByteArray txbuf;
    QByteArray rxbuf;
    QTimer rxcomp;
};

#endif // MAINWINDOW_H
