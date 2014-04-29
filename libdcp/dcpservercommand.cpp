/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpservercommand.cpp -- bertrand
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * */

#include "dcpservercommand.h"
#include "dcpcommands.h"

DCPServerCommand::DCPServerCommand(QUdpSocket *sock) :
    DCPServer(sock),
    droneId(DCP_IDNULL),
    sessIdDrone(DCP_IDNULL),
    sessIdCentralStation(DCP_SESSIDCENTRAL),
    status(Init)
{
    this->handler = new DCPPacketHandlerCommandStationHello(this);
}

void DCPServerCommand::setCentralStationHost(
        QHostAddress addr,
        quint16 port)
{
    this->addrCentralStation = addr;
    this->portCentralStation = port;
}

void DCPServerCommand::setDroneHost(QHostAddress addr, quint16 port)
{
    this->addrDrone = addr;
    this->portDrone = port;
}

void DCPServerCommand::setSessionIdDrone(qint8 sessID)
{
    this->sessIdDrone = sessID;
}

void DCPServerCommand::setSessionIdCentralStation(
        qint8 sessIdCentralStation)
{
    this->sessIdCentralStation = sessIdCentralStation;
}

void DCPServerCommand::setStatus(DCPServerCommandStatus status)
{
    this->statusMutex.lock();
    this->status = status;
    this->statusMutex.unlock();
    emit statusChanged(status);
}

void DCPServerCommand::sayHello(QString description)
{
    if(this->getStatus() != Init) return;
    this->setStatus(SayingHello);

    DCPCommandHelloFromRemote *hello =
            new DCPCommandHelloFromRemote(this->sessIdCentralStation);
    hello->setDescription(description);
    hello->setAddrDst(this->addrCentralStation);
    hello->setPortDst(this->portCentralStation);
    hello->setRemoteType(DCPCommandHelloFromRemote::remoteTypeCommandStation);
    this->time.start();
    hello->setTimestamp(this->time.elapsed());
    this->sendPacket(hello);
}

void DCPServerCommand::connectToDrone(qint8 id)
{
    enum DCPServerCommandStatus status = this->getStatus();
    if(status != NotConnected &&  status != Disconnected) return;

    this->setStatus(Connecting);
    DCPCommandConnectToDrone *conn = new DCPCommandConnectToDrone(
                this->sessIdCentralStation);
    conn->setAddrDst(this->addrCentralStation);
    conn->setPortDst(this->portCentralStation);
    conn->setDroneId(id);
    conn->setTimestamp(this->time.elapsed());
    this->sendPacket(conn);
}

void DCPServerCommand::disconnectFromDrone()
{
    if(this->getStatus() != Connected) return;

    this->setStatus(Disconnecting);
    DCPCommandDisconnect *disc = new DCPCommandDisconnect(
                this->sessIdCentralStation);
    disc->setAddrDst(this->addrCentralStation);
    disc->setPortDst(this->portCentralStation);
    disc->setTimestamp(this->time.elapsed());
    this->sendPacket(disc);
}

void DCPServerCommand::sayByeBye()
{
    enum DCPServerCommandStatus status = this->getStatus();
    if(status != NotConnected &&  status != Disconnected && status != Connected)
        return;

    this->setStatus(Stopping);
    DCPCommandBye *bye = new DCPCommandBye(this->sessIdCentralStation);
    bye->setAddrDst(this->addrCentralStation);
    bye->setPortDst(this->portCentralStation);
    bye->setTimestamp(this->time.elapsed());
    this->sendPacket(bye);
}
