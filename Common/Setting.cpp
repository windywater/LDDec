#include "Setting.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

Setting::Setting()
{
	m_port = 34500;
	m_fakerProcName = "notepad.exe";
}

Setting::~Setting()
{

}

void Setting::load()
{
	QFile f(QCoreApplication::applicationDirPath() + "/config.json");
	f.open(QIODevice::ReadOnly);
	QByteArray contents = f.readAll();
	f.close();

	QJsonDocument doc = QJsonDocument::fromJson(contents);
	QJsonObject rootObj = doc.object();
	m_port = (quint16)rootObj["port"].toInt();
	m_fakerProcName = rootObj["faker"].toString();
}

quint16 Setting::port()
{
	return m_port;
}

QString Setting::fakerProcesssName()
{
	return m_fakerProcName;
}

/*static*/ Setting* Setting::instance()
{
	static Setting inst;
	return &inst;
}