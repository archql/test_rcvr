#ifndef RECIEVER_H
#define RECIEVER_H

#include <QThread>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMutex>

class Graph;

class Reciever : public QThread
{
public:
    explicit Reciever();
    ~Reciever();

    bool setup(QHostAddress addr, quint16 port, Graph *graph);

    void invalidate();

protected:
    void run();
private:
    QUdpSocket udp_socket;
    Graph *graph;
    QMutex mutex;

    bool ready;

    void recieve();
};

#endif // RECIEVER_H
