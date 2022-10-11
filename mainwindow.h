#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QtCharts>
#include <QLabel>
#include <vector>
#include <unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::unordered_map<double, double> graph_points;

    QScatterSeries* scatterSeries;
    QLineSeries* interpolateSeries;
    QLineSeries* fitSeries;

    QChart* chart = nullptr;
    QPair<double, double> xLimits;
    QPair<double, double> yLimits;
    QChartView* chartView = nullptr;

    void interpolate();

    Ui::MainWindow *ui;

public slots:
    void add_new_point();
    void remove_point();
};
#endif // MAINWINDOW_H
