#include "Common.h"

qint64 blockRead(QTcpSocket* client, char* data, qint64 maxlen)
{
	qint64 ret = client->read(data, maxlen);
	if (ret == 0)
	{
		client->waitForReadyRead();
		ret = client->read(data, maxlen);
	}

	return ret;
}
