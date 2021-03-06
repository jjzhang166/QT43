#include "mwworker.h"
#include "mainwindow.h"
#include "printer.h"
#include "msgproto.h"
#include <QTcpSocket>

mwworker::mwworker(QWidget *parent)
{
    ui = (MainWindow*)parent;
    isrun = false;
}

void mwworker::msgq_push(int msg)
{
    msgq.push(msg);
}

bool mwworker::msgq_get(int &msg)
{
    if (msgq.empty())
        return false;

    msg = msgq.front();
    msgq.pop();

    return true;
}

void mwworker::ShowStatus(const char *s)
{
   string ss = s;

   emit ShowStatus(ss);
}

void mwworker::run()
{
    QTcpSocket *client;
    msgproto pkt;
    char *rxbuf, *txbuf;
    int size;
    printer pt;

    isrun = true;

    server = new QTcpServer;
    if (!server->listen(QHostAddress::Any, 2012))
    {
        qDebug("err");
        return;
    }

WAIT:
    ui->ShowTip("等待设备连接");
    server->waitForNewConnection(-1);
    client = server->nextPendingConnection();

    rxbuf = new char[1024];
    txbuf = new char[1024];

    ui->ShowTip("设备已连接");

    while (isrun)
    {
        int uimsg = 0;

        if (client->state() == QTcpSocket::UnconnectedState)
        {
            ui->ShowTip("设备已断开");
            client->close();
            goto WAIT;
        }

        if (client->waitForReadyRead(500))
        {
            int cmd = 0;

            size = client->read(rxbuf, 1024);
            if (pkt.in(rxbuf, size, cmd))
            {
                bool ret = false;

                switch (cmd)
                {
                case 0x21:
                    emit ShowStatus("收到心跳");
                    ret = pkt.makeheartbeat(txbuf, size);
                    break;
                case 0x2F:
                    emit ShowStatus("收到登陆");
                    ret = pkt.makelogin(txbuf, size);
                    break;
                case 0x88:
                    emit ShowStatus("收到打印回复");
                    break;
                }

                if (ret)
                {
                    client->write(txbuf, size);
                    client->flush();
                }
            }
        }

        msgq_get(uimsg);

        switch (uimsg)
        {
        case 1:
        {
            string text;

            ui->GetText(text);
            size = text.size();
            if (size == 0)
            {
                emit ShowStatus("请输入内容");
            }
            else
            {
                text = pt.format(text);
                pkt.makeprint(text, txbuf, size);
                client->write(txbuf, size);
                client->flush();
            }
        }
        break;
        }
    }
}
