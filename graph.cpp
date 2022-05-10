#include "graph.h"

#include <QDebug>
#include <QDateTime>
#include <QPainter>

#define DEFAULT_CONTENT_RECT 0, -100, 0, 200

Graph::Graph(QWidget *parent)
    : QOpenGLWidget(parent),
      update_timer(this),
      content_rect(DEFAULT_CONTENT_RECT)
{
    qInfo() << "Graph init";

    /*
    bool is_ok = reciever.setup(QHostAddress::LocalHost, RECIEVER_PORT, this);
    if (!is_ok)
    {
        qWarning() << "reciever setup failed";
    }
    reciever.start();*/

    setFocusPolicy(Qt::StrongFocus);

    // setup graph policy (mb moved to separate func)
    default_scale = true; right_connected = true;
    scale_f = 1.0f;

    // start Programm Timer
    timer.start();

    // Connect main timer & start
    connect(&update_timer, &QTimer::timeout, this, &Graph::mesh_update);
    start();
}

Graph::~Graph()
{
}

void Graph::initializeGL()
{
    qInfo() << "Graph initializeGL";
    glClearColor( 1.0, 1.0, 1.0, 1.0);
}

void Graph::get_precision(int &value_min, int &value_max, int &pow_degree)
{
    int degree = log10((double)value_max - (double)value_min +1); // -1
    pow_degree = pow(10, degree);
    if (pow_degree <= 0)
        pow_degree = 1;

    value_min = ceil((double)value_min / (double)pow_degree) * pow_degree;
    value_max = floor((double)value_max / (double)pow_degree) * pow_degree;
}

void Graph::paintGL()
{
    //qInfo() << "Graph paintGL";

    glClear(GL_COLOR_BUFFER_BIT);

    // set matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // QREct content_rect adjucted
    glOrtho(visible_rect.left() - 100, visible_rect.right() + 100,
            visible_rect.bottom() + 100,  visible_rect.top() - 100, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.3f, 0.3f, 0.3f);
    glPointSize(20);

    glBegin(GL_POINTS); // GL_POINTS
        for (const auto &p : qAsConst(points)) {
            glVertex2i(p.time, p.val); // p.val
        }
    glEnd();

    glLineWidth(5);
    glBegin(GL_LINES);
        glColor3f(1, 0, 0);

        glVertex2i(timer.elapsed(), content_rect.bottom());
        glVertex2i(timer.elapsed(), content_rect.top());

        glVertex2i(0, 0);
        glVertex2i(timer.elapsed(), 0);

    glEnd();

    glLineWidth(2);
    glColor3f(0, 0, 0);

    glBegin(GL_LINES);
        int left = visible_rect.left(), right = visible_rect.right(), pow_dg_w;
        get_precision(left, right, pow_dg_w);

        for (int i = left; i <= right; i += pow_dg_w)
        {
            glVertex2i(i, content_rect.top());
            glVertex2i(i, content_rect.bottom());
        }

        int top = visible_rect.top(), bottom = visible_rect.bottom(), pow_dg_h;
        get_precision(top, bottom, pow_dg_h);
        for (int i = top; i <= bottom; i += pow_dg_h)
        {
            glVertex2i(content_rect.left(), i);
            glVertex2i(content_rect.right(), i);
        }
    glEnd();
}

void Graph::wheelEvent(QWheelEvent *e)
{
    default_scale = false;
    right_connected = false;

    auto new_scale_f = scale_f +  e->angleDelta().y() / 2400.f;
    if (new_scale_f < 0.5f) {
        new_scale_f = 0.5f;
    }
    if (new_scale_f >= 1.f) {
        new_scale_f = 1.f;
        default_scale = true;
        right_connected = true;
    }

    scale_f = new_scale_f;
    visible_rect.setWidth(content_rect.width()*scale_f);

    update();
}

void Graph::keyPressEvent(QKeyEvent *e)
{
    qInfo() << "keyPressEvent ";
    switch (e->key())
    {
    case Qt::Key_R:
        default_scale = true;
        right_connected = true;
        scale_f= 1.0f;
        break;
    case Qt::Key_Backspace:
        default_scale = true;
        right_connected = true;
        points.clear();
        timer.restart();
        content_rect.setRect(DEFAULT_CONTENT_RECT);
        scale_f= 1.0f;
        break;
    case Qt::Key_Left:
        if (content_rect.left() < visible_rect.left() - 1000)
            visible_rect.translate(-1000, 0);
        else
            visible_rect.moveLeft(content_rect.left());
        default_scale = false;
        right_connected = false;
        break;
    case Qt::Key_Right:
        if (content_rect.right() > visible_rect.right() + 1000) {
            visible_rect.translate(1000, 0);
            right_connected = false;
        } else {
            visible_rect.moveRight(content_rect.right());
            right_connected = true;
        }
        default_scale = false;
        break;
    case Qt::Key_Up:
        if (content_rect.top() < visible_rect.top() - 1000)
            visible_rect.translate(0, -1000);
        else
            visible_rect.moveTop(content_rect.top());
        default_scale = false;
        right_connected = false;
        break;
    case Qt::Key_Down:
        if (content_rect.bottom() > visible_rect.bottom() + 1000)
            visible_rect.translate(0, 1000);
        else
            visible_rect.moveBottom(content_rect.bottom());
        default_scale = false;
        right_connected = false;
        break;
    }
    update();
}

void Graph::start()
{
    update_timer.start(50);
}
void Graph::stop()
{
    update_timer.stop();
}
void Graph::mesh_update()
{
    if (right_connected) {
        visible_rect.moveRight(timer.elapsed());
        content_rect.setRight(timer.elapsed());
    }
    if (default_scale) {
        visible_rect = content_rect;
    }
    update();
}

void Graph::resizeGL(int w, int h)
{
    qInfo() << "Graph resizeGL";


    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, w, h,
            0, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool Graph::add_point(const dgram &d, const int cnt_time)
{
    // create point
    gpoint p;
    p.val = -d.some_val; // invert arrived data
    p.time = timer.elapsed();

    // leveling
    auto it = points.rbegin(); qint64 avg = p.val, cnt = 1;
    while (it != points.rend() && (it->time + cnt_time > p.time))
    {
        avg += it->val; cnt++;
        // next
        it++;
    }
    p.val = avg / cnt;
    // end

    points.append(p);
    // adjust content
    content_rect.setRight(p.time);
    if (p.val > content_rect.bottom())
        content_rect.setBottom(p.val);
    if (p.val < content_rect.top())
        content_rect.setTop(p.val);

    return true;
}
