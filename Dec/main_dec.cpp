#include <QtCore/QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include "Setting.h"
#include "Common.h"

static void printUsage()
{
    qDebug() << "Usage: dec.exe <file or path>";
}

static bool isEncFile(const QString& file)
{
    return true;
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray head = f.read(3);
    f.close();

    return head == "XXX";   // TODO
}

static QStringList filterEncFiles(const QStringList& files)
{
    QStringList results;
    for (const QString& f : files)
    {
        if (isEncFile(f))
            results << f;
    }

    return results;
}

static const int kBlockSize = 10 * 1024;
char block[kBlockSize];

static void doDecTask(QTcpSocket* client, const QString& file)
{
    QFileInfo fi(file);
    qDebug() << "Decrypting " << QDir::toNativeSeparators(fi.absoluteFilePath()) << "...";

    qint32 fileNameSize = (qint32)file.size();
    client->write((const char*)&fileNameSize, sizeof(fileNameSize));
    qDebug() << "write fileNameSize:" << fileNameSize;

    QByteArray fileNameBytes = file.toLocal8Bit();
    client->write(fileNameBytes);
	client->waitForBytesWritten();
    qDebug() << "write fileNameBytes:" << fileNameBytes;

    QString newFileName = file + ".dec_";
    QFile f(newFileName);
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);

    qint64 decContentSize = 0;
    blockRead(client, (char*)&decContentSize, sizeof(decContentSize));
    qDebug() << "read decContentSize:" << decContentSize;

    qint64 totalRecv = 0;
    while (true)
    {
        int recvSize = blockRead(client, block, kBlockSize);
        totalRecv += recvSize;

        if (recvSize == -1)
            break;

        f.write(block, recvSize);

        if (totalRecv >= decContentSize)
        {
            qDebug() << "recv finished.";
            break;
        }
    }
    
    f.close();
    // TODO: replace file and modify times
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList args = a.arguments();
    QStringList taskFiles;
    if (args.size() <= 1)
    {
        printUsage();
        return 0;
    }
    else
    {
        QString target = args.at(1);
        QFileInfo fi(target);
        if (!fi.exists())
        {
            qDebug() << "Target not exist.";
            return -1;
        }

        if (fi.isFile())
        {
            taskFiles << fi.absoluteFilePath();
        }
        else if (fi.isDir())
        {
            QDir dir(fi.absoluteFilePath());
            QFileInfoList fis = dir.entryInfoList(QDir::Files);
            for (const auto& fi : fis)
                taskFiles << fi.absoluteFilePath();
        }

        taskFiles = filterEncFiles(taskFiles);
        if (taskFiles.isEmpty())
        {
            qDebug() << "No file is encrypted.";
            return 0;
        }
    }

    Setting* setting = Setting::instance();
    setting->load();

    QTcpServer server;
    server.listen(QHostAddress::Any, setting->port());

    QString fakerProc = QCoreApplication::applicationDirPath() + "/" + setting->fakerProcesssName();
    QProcess::startDetached(fakerProc, QStringList{});

    bool ok = server.waitForNewConnection(30000);
    if (!ok)
    {
        qDebug() << "No connection.";
        return -1;
    }

    const QByteArray kHelloBytes = "HELLO";
    QTcpSocket* client = server.nextPendingConnection();
    char helloBuf[5];
    blockRead(client, helloBuf, 5);
    if (QByteArray(helloBuf, 5) != kHelloBytes)
    {
        qDebug() << "No hello response.";
        return -1;
    }

    for (const QString& file : taskFiles)
        doDecTask(client, file);

    qDebug() << taskFiles.size() << "file(s) decrypted.";

    qint32 byeCmd = -1;
    client->write((const char*)&byeCmd, sizeof(byeCmd));
    client->waitForBytesWritten();
    client->close();
    //qDebug() << "Tasks done, bye command sent.";

    server.close();

    return 0;
}
