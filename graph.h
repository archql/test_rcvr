#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QVector>
#include <QRect>
#include <QElapsedTimer>

#include <QWheelEvent>
#include <QKeyEvent>

#include <QTimer>

#include <gl/GLU.h>
#include <gl/GL.h>

#include "reciever.h"
#include "../server/protocol.h"

class Graph : public QOpenGLWidget
{
public:
    Graph(QWidget *parent = nullptr);
    ~Graph();

    bool add_point(const dgram &d, const int cnt_time = 0); // cnt_time -- amount of time to cnt average
protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

    void start();
    void stop();
    void mesh_update();

private:
    QElapsedTimer timer;
    QTimer update_timer;

    QVector<gpoint> points;
    QRect visible_rect;
    QRect content_rect;
    GLfloat scale_f;
    bool default_scale, right_connected;

    void get_precision(int &value_min, int &value_max, int &pow_degree);
};

#endif // GRAPH_H
