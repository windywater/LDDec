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
    qDebug() << "Usage: dec <file or path>";
}

static const QByteArray kEncryptedTag = "\x88\x7d\x1c";
static bool isEncFile(const QString& file)
{
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray head = f.read(3);
    f.close();

    return head == kEncryptedTag;
}

#ifdef Q_OS_WIN
#include <Windows.h>
static bool getFileTimes(const QString& file, FILETIME& create, FILETIME& access, FILETIME& modified)
{
	std::wstring wstr = file.toStdWString();
	HANDLE handle = CreateFileW(wstr.c_str(),
		FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	BOOL ok = GetFileTime(handle, &create, &access, &modified);
	CloseHandle(handle);

	return (ok == TRUE);
}

static bool setFileTimes(const QString& file, const FILETIME& create, const FILETIME& access, const FILETIME& modified)
{
	std::wstring wstr = file.toStdWString();
	HANDLE handle = CreateFileW(wstr.c_str(),
		FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	BOOL ok = SetFileTime(handle, &create, &access, &modified);
	CloseHandle(handle);

	return (ok == TRUE);
}
#endif

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
	qDebug().nospace().noquote() << "Decrypting " << QDir::toNativeSeparators(fi.absoluteFilePath()) << "...";

	QByteArray fileNameBytes = file.toLocal8Bit();
    qint32 fileNameSize = (qint32)fileNameBytes.size();
    client->write((const char*)&fileNameSize, sizeof(fileNameSize));
    
    client->write(fileNameBytes);
	client->waitForBytesWritten();

    QString tmpFileName = file + ".dec_";
    QFile f(tmpFileName);
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);

    qint64 decContentSize = 0;
    blockRead(client, (char*)&decContentSize, sizeof(decContentSize));

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
            break;
        }
    }
    
    f.close();

    // replace file and modify times
#ifdef Q_OS_WIN
	FILETIME create, access, modified;
	getFileTimes(file, create, access, modified);
	setFileTimes(tmpFileName, create, access, modified);
#endif
	QFile::remove(file);
	QFile::rename(tmpFileName, file);
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

    bool ok = server.waitForNewConnection(5000);
    if (!ok)
    {
        qDebug() << "No client connected.";
        return -1;
    }

	QTcpSocket* client = server.nextPendingConnection();
    for (const QString& file : taskFiles)
        doDecTask(client, file);

    qDebug() << taskFiles.size() << "file(s) decrypted.";

    qint32 byeCmd = -1;
    client->write((const char*)&byeCmd, sizeof(byeCmd));
    client->waitForBytesWritten();
    client->close();
    server.close();

    return 0;
}
