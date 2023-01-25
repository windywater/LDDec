#pragma once
#include <QTcpSocket>

qint64 blockRead(QTcpSocket* client, char* data, qint64 maxlen);
