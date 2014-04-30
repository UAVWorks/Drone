/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpservercentral.h -- bertrand
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

#ifndef DCPSERVERCENTRAL_H
#define DCPSERVERCENTRAL_H

#include <QtGlobal>
#include <QtSql/QSqlDatabase>
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>

#include <dcp.h>
#include <dcpserver.h>
#include <dcpcommands.h>


class DCPServerCentral : public DCPServer
{
public:
    DCPServerCentral(QUdpSocket *socket, QSqlDatabase db);

    // TODO: make avaliable only to packet handler
    bool    addNewDrone(QHostAddress addr, quint8 port, QString info);
    bool    addNewCommandStation(QHostAddress addr, quint8 port, QString info);
    bool    addNewSession(qint8 iddrone, qint8 idcommand);
    bool    addNewSessionCentralDrone(qint8 iddrone);
    bool    addNewSessionCentralCommand(qint8 idcommand);
    bool    deleteSession(qint8 id);

private:
    QSqlDatabase db;
};

#endif // DCPSERVERCENTRAL_H
