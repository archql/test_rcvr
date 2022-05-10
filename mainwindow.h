#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QComboBox>
#include <QMap>
#include <QUdpSocket>

#include "graph.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent ( QResizeEvent * e ) override;

private:
    Ui::MainWindow *ui;
    Graph *graph;
    QVBoxLayout *layout;
    QComboBox *combo_filter;

    QUdpSocket udp_socket;

    void recieve();

    // QMap with filters ptrs
    const QMap<QString, int> combo_elements = { { "default", 0 },
                                                { "10 ms averaging", 10 },
                                                { "50 ms averaging", 50 },
                                                { "100 ms averaging", 100 },
                                                { "500 ms averaging", 500 },
                                                { "1000 ms averaging", 1000 },
                                                { "5000 ms averaging", 5000 }};
};

#endif // MAINWINDOW_H
