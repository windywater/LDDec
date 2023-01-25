#pragma once
#include <QString>

class Setting
{
private:
	Setting();
	~Setting();
	Setting(const Setting&) = delete;
	Setting& operator=(const Setting&) = delete;

public:
	void load();
	quint16 port();
	QString fakerProcesssName();

	static Setting* instance();

private:
	quint16 m_port;
	QString m_fakerProcName;
};

