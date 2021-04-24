#include <QJsonDocument>
#include <QJsonObject>
#include "client.h"
#include "main.h"

namespace MtbNetLib {

DaemonClient::DaemonClient(QObject *parent) : QObject(parent) {
	QObject::connect(&this->m_tKeepAlive, SIGNAL(timeout()), this, SLOT(tKeepAliveTick()));
	QObject::connect(&m_socket, SIGNAL(connected()), this, SLOT(clientConnected()));
	QObject::connect(&m_socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
	QObject::connect(&m_socket, SIGNAL(readyRead()), this, SLOT(clientReadyRead()));
}

void DaemonClient::connect(const QHostAddress &addr, quint16 port, bool keepAlive) {
	this->m_socket.connectToHost(addr, port);
	if (keepAlive)
		this->m_tKeepAlive.start(CLIENT_KEEP_ALIVE_SEND_PERIOD_MS);
}

void DaemonClient::disconnect() {
	this->m_socket.abort();
}

bool DaemonClient::connected() const {
	return this->m_socket.state() == QAbstractSocket::SocketState::ConnectedState;
}

void DaemonClient::clientConnected() {
	log("Connected to server.", LogLevel::Info);
}

void DaemonClient::clientDisconnected() {
	log("Client disconnected", LogLevel::Info);
	this->m_tKeepAlive.stop();
	// client->deleteLater();

	for (size_t i = 0; i < MAX_MODULES; i++) {
		if (modules[i] != nullptr) {
			modules[i]->resetState();
			modules[i]->resetConfig();
		}
	}
}

void DaemonClient::clientReadyRead() {
	if (this->m_socket.canReadLine()) {
		QByteArray data = this->m_socket.readLine();
		if (data.size() > 0) {
			QJsonObject json = QJsonDocument::fromJson(data).object();
			jsonReceived(json);
		}
	}
}

void DaemonClient::send(const QJsonObject &jsonObj) {
	if (!this->connected())
		return; // TODO: throw exception?
	this->m_socket.write(QJsonDocument(jsonObj).toJson(QJsonDocument::Compact));
	this->m_socket.write("\n");
}

void DaemonClient::tKeepAliveTick() {
	if (this->connected())
		this->send({});
}

}; // namespace MtbNetLib
