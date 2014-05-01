/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpservercentral.cpp -- bertrand
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

#include "dcpservercentral.h"
#include "dcp.h"
#include "dcpcommands.h"

#include <QSqlQuery>
#include <QSqlError>


DCPServerCentral::DCPServerCentral(QUdpSocket *sock, QSqlDatabase db) :
    DCPServer(sock),
    myID(DCP_IDCENTRAL),
    db(db)
{
    this->handler = new DCPPacketHandlerCentralStation(this);
}

DCPServerCentral::remote_t*
DCPServerCentral::addNewDrone(QHostAddress addr, quint16 port, QString info)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSTATIONS) +
                  " (type, ip, port, info) VALUES ('drone', ?, ?, ?)");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    qWarning() << "Sucessfully added new Drone:" << query.lastQuery();
    remote->addr = addr;
    remote->port = port;
    remote->info = info;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSTATIONS) +
                  " WHERE type='drone' AND ip=? AND port=? AND info=?");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    remote->id      = query.value(0).toInt();
    remote->date    = query.value(1).toDateTime();

    return remote;
}

DCPServerCentral::remote_t*
DCPServerCentral::addNewCommandStation(QHostAddress addr, quint16 port,
                                       QString info)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSTATIONS) +
                  " (type, ip, port, info) VALUES ('command', ?, ?, ?)");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    qWarning() << "Sucessfully added new Command Station:" << query.lastQuery();
    remote->addr = addr;
    remote->port = port;
    remote->info = info;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSTATIONS) +
                  " WHERE type='command' AND ip=? AND port=? AND info=?");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    remote->id      = query.value(0).toInt();
    remote->date    = query.value(1).toDateTime();

    return remote;
}

DCPServerCentral::session_t*
DCPServerCentral::addNewSession(qint8 station1, qint8 station2)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSESSIONS) +
                  " (station1, station2) VALUES (?, ?)");
    query.bindValue(0, station1);
    query.bindValue(1, station2);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    qWarning() << "Sucessfully added new Session:" << query.lastQuery();
    session->station1   = station1;
    session->station2   = station2;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSESSIONS) +
                  " WHERE station1=? AND station2=?");
    query.bindValue(0, station1);
    query.bindValue(1, station2);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    session->id      = query.value(0).toInt();
    session->date    = query.value(1).toDateTime();

    return session;
}

bool DCPServerCentral::deleteSession(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSESSIONS) +
                  " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Session :"
               << query.lastQuery();
    return true;
}

bool DCPServerCentral::deleteSessionsForStation(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSESSIONS) +
                  " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Session for station id" << id << " :"
               << query.lastQuery();
    return true;
}

bool DCPServerCentral::deleteStationById(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSTATIONS) + " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Station:" << query.lastQuery();
    return true;
}

DCPServerCentral::session_t*
DCPServerCentral::getSessionFromId(qint8 id)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    // Get session
    QSqlQuery query(this->db);
    query.prepare("SELECT station1, station2, date FROM " +
                  QString(DCP_DBSESSIONS) + " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    session->id         = id;
    session->station1   = query.value(0).toInt();
    session->station2   = query.value(1).toInt();
    session->date       = query.value(2).toDateTime();

    return session;
}

QList<DCPServerCentral::session_t*>
DCPServerCentral::getSessionsForStation(qint8 id)
{
    QList<DCPServerCentral::session_t*> sessions;

    // Get sessions
    QSqlQuery query(this->db);
    query.prepare("SELECT id, station1, station2, date FROM " +
                  QString(DCP_DBSESSIONS) + " WHERE station1=? OR station2=?");
    query.bindValue(0, id);
    query.bindValue(1, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    while(query.next())
    {
        DCPServerCentral::session_t* session = new DCPServerCentral::session_t;
        session->id         = query.value(0).toInt();
        session->station1   = query.value(1).toInt();
        session->station2   = query.value(2).toInt();
        session->date       = query.value(3).toDateTime();
        sessions.append(session);
    }

    return sessions;
}

DCPServerCentral::remote_t**
DCPServerCentral::getStationsFromSessionId(qint8 id)
{
    DCPServerCentral::remote_t** remoteTab = new DCPServerCentral::remote_t*[2];
    remoteTab[0] = new DCPServerCentral::remote_t;
    remoteTab[1] = new DCPServerCentral::remote_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT t1.id, t1.type, t1.ip, t1.port, t1.date, t1.info,"
                  " sess.id, t2.id, t2.type, t2.ip, t2.port, t2.date, t2.info"
                  " FROM (SELECT * FROM sessions WHERE sessions.id=?)"
                  " AS sess"
                  " LEFT JOIN stations t1 ON t1.id=sess.station1"
                  " LEFT JOIN stations t2 ON t2.id=sess.station2");
    query.bindValue(0, id);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    remoteTab[0]->id     = query.value(0).toInt();
    remoteTab[0]->type   = query.value(1).toString();
    remoteTab[0]->addr   = QHostAddress(query.value(2).toString());
    remoteTab[0]->port   = query.value(3).toInt();
    remoteTab[0]->date   = query.value(4).toDateTime();
    remoteTab[0]->info   = query.value(5).toString();

    remoteTab[1]->id     = query.value(7).toInt();
    remoteTab[1]->type   = query.value(8).toString();
    remoteTab[1]->addr   = QHostAddress(query.value(9).toString());
    remoteTab[1]->port   = query.value(10).toInt();
    remoteTab[1]->date   = query.value(11).toDateTime();
    remoteTab[1]->info   = query.value(12).toString();

    return remoteTab;
}

DCPServerCentral::remote_t*
DCPServerCentral::stationIsDrone(qint8 id)
{
    DCPServerCentral::remote_t* remote;

    QSqlQuery query(this->db);
    query.prepare("SELECT type, ip, port, date, info" +
                  " FROM " +  QString(DCP_DBSTATIONS) +
                  " WHERE type='drone' AND id=?");
    query.bindValue(0, id);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    remote->id      = id;
    remote->type    = query.value(0).toString();
    remote->addr    = QHostAddress(query.value(1).toString());
    remote->port    = query.value(2).toString();
    remote->date    = QDateTime(query.value(3).toString());
    remote->info    = query.value(4).toString();

    return remote;
}

DCPServerCentral::remote_t*
DCPServerCentral::stationIsCommand(qint8 id)
{
    DCPServerCentral::remote_t* remote;

    QSqlQuery query(this->db);
    query.prepare("SELECT type, ip, port, date, info" +
                  " FROM " +  QString(DCP_DBSTATIONS) +
                  " WHERE type='command' AND id=?");
    query.bindValue(0, id);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    remote->id      = id;
    remote->type    = query.value(0).toString();
    remote->addr    = QHostAddress(query.value(1).toString());
    remote->port    = query.value(2).toString();
    remote->date    = QDateTime(query.value(3).toString());
    remote->info    = query.value(4).toString();

    return remote;
}

DCPServerCentral::session_t*
DCPServerCentral::sessionIsCentral(qint8 id)
{
    DCPServerCentral::session_t* session;

    QSqlQuery query(this->db);
    query.prepare("SELECT station1, station2, date" +
                  " FROM " +  QString(DCP_DBSESSIONS) +
                  " WHERE id=? AND (station1=0 OR station2=0)");
    query.bindValue(0, id);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    session->id         = id;
    session->station1   = query.value(0).toInt();
    session->station2   = query.value(1).toInt();
    session->date       = QDateTime(query.value(2).toString());

    return session;
}
