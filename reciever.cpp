#include "reciever.h"

#include "../server/protocol.h"
#include "graph.h"

Reciever::Reciever() : udp_socket(this)
{
    ready = false;
}

Reciever::~Reciever()
{
    ready = false;
    udp_socket.close();
}

bool Reciever::setup(QHostAddress addr, quint16 port, Graph *graph)
{
    ready = udp_socket.bind(addr, port);
    if (!ready)
        return false;

    connect(&udp_socket, &QUdpSocket::readyRead,
            this, &Reciever::recieve);

    ready = graph != nullptr;
    return ready;
}

void Reciever::invalidate()
{
    ready = false;
}

void Reciever::run()
{
    dgram dg;
    dg.some_val = 0;

    while(ready)
    {
        //QMutexLocker locker(&mutex);
        if (graph)
            graph->add_point(dg);
    }
}

void Reciever::recieve()
{
    qInfo() << "Hi!";

    dgram dg;
    while (udp_socket.hasPendingDatagrams()) {
        auto readed = udp_socket.readDatagram((char *)&dg, sizeof(dgram));
        // process it
        //if (readed == sizeof(dgram))
        //    graph->add_point(dg);
    }
}
