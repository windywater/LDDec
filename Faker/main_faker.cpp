#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QHostAddress>
#include <QDebug>
#include "Setting.h"
#include "Common.h"

static bool doReadTask(QTcpSocket* client)
{
    qint32 fileNameSize = 0;
    qint64 ret = blockRead(client, (char*)&fileNameSize, sizeof(fileNameSize));
    qDebug() << "fileNameSize:" << fileNameSize;
    if (fileNameSize == -1 || ret == -1)
        return false;

    char* buf = new char[fileNameSize];
    blockRead(client, buf, fileNameSize);
    QString fileName = QString::fromLocal8Bit(buf, fileNameSize);
    qDebug() << "fileName:" << fileName;
    delete[] buf;
    
    QFile f(fileName);
    qint64 fileSize = f.size();
    client->write((const char*)&fileSize, sizeof(fileSize));
    client->waitForBytesWritten();

    f.open(QIODevice::ReadOnly);
    const int kBlockSize = 10 * 1024;
    while (!f.atEnd())
    {
        QByteArray block = f.read(kBlockSize);
        client->write(block);
        client->waitForBytesWritten();
    }
    f.close();

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Setting* setting = Setting::instance();
    setting->load();

    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, setting->port());
    bool ok = client.waitForConnected(3000);
    if (!ok)
    {
        qDebug() << "No server found.";
        return -1;
    }

    client.write("HELLO");
    client.waitForBytesWritten();
    while (true)
    {
        bool cont = doReadTask(&client);
        if (!cont)
            break;
    }

    client.close();

    return 0;
}
