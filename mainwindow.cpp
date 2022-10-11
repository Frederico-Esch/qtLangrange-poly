#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , xLimits({-5, 5})
    , yLimits({-5, 5})
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    chart = new QChart();
    scatterSeries = new QScatterSeries(chart);
    interpolateSeries = new QLineSeries(chart);
    fitSeries = new QLineSeries(chart);

    scatterSeries->setName("Points");
    interpolateSeries->setName("Interpolation");
    fitSeries->setName("Fitted Line");

    //scatter customize
    QBrush bbrush(Qt::black);
    QBrush tbrush(Qt::transparent);
    QPen pen(bbrush, 2);
    scatterSeries->setPen(pen);
    scatterSeries->setBrush(tbrush);
    scatterSeries->setMarkerSize(4);

    //interpolate customize
    interpolateSeries->setColor(Qt::blue);

    chart->addSeries(scatterSeries);
    chart->addSeries(interpolateSeries);
    chart->addSeries(fitSeries);
    //chart->legend()->hide();
    chart->createDefaultAxes();
    for(auto& ax : chart->axes(Qt::Horizontal)){
        ax->setRange(xLimits.first, xLimits.second);
    }
    for(auto& ax : chart->axes(Qt::Vertical)){
        ax->setRange(yLimits.first, yLimits.second);
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->plot_frame->layout()->addWidget(chartView);
}

void MainWindow::interpolate(){
    auto L = [&](const QList<QPointF>& ps, size_t tx) -> std::function<double(double)> {
        auto f = [&](double x) -> double{
            double result = 1;
            for(auto& p : ps){
                if(p.x() != ps[tx].x()) {
                result *= x-p.x();
                result /= ps[tx].x() - p.x();
                }
            }
            return result;
        };
        return f;
    };

    auto poly = [&](double x){
        double result = 0;
        size_t index = 0;
        for(auto& p : scatterSeries->points()){
            result += p.y()*L(scatterSeries->points(), index)(x);
            index++;
        }
        return result;
    };

    interpolateSeries->clear();
    interpolateSeries->setParent(nullptr);
    double stepSize = (xLimits.second - xLimits.first)/1000;
    for(double x = xLimits.first; x < xLimits.second; x += stepSize){
        interpolateSeries->append(x, poly(x));
        //qDebug(std::to_string(x).c_str());
    }
    interpolateSeries->setParent(chart);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete scatterSeries;
    delete chart;
}

void MainWindow::add_new_point(){
    std::pair<double, double> point = {ui->x_value->value(), ui->y_value->value()};

    if(graph_points.find(point.first) != graph_points.end()) {
        scatterSeries->remove(point.first, graph_points[point.first]);
        graph_points.erase(point.first);
    }
    graph_points[point.first] = point.second;

    scatterSeries->append(point.first, point.second);

    if(point.first < xLimits.first) xLimits.first = point.first -2;
    else if(point.first > xLimits.second) xLimits.second = point.first +2;

    if(point.second < yLimits.first) yLimits.first = point.second -2;
    else if(point.second > yLimits.second) yLimits.second = point.second +2;

    for(auto& ax : chart->axes(Qt::Horizontal)){
        ax->setRange(xLimits.first, xLimits.second);
    }
    for(auto& ax : chart->axes(Qt::Vertical)){
        ax->setRange(yLimits.first, yLimits.second);
    }

    if(graph_points.size() > 1) interpolate();
    chartView->repaint();
}

void MainWindow::remove_point(){
    std::pair<double, double> point = {ui->x_value->value(), ui->y_value->value()};
    if(graph_points.find(point.first) != graph_points.end()) {
        scatterSeries->remove(point.first, graph_points[point.first]);
        graph_points.erase(point.first);

        if(graph_points.size() > 1){
            double max = scatterSeries->points()[0].x();
            double min = scatterSeries->points()[0].x();

            for(auto& p : scatterSeries->points()){
                if(p.x() > max) max = p.x();
                if(p.x() < min) min = p.x();
            }

            xLimits.first = min-2; xLimits.second = max+2;

            for(auto& ax : chart->axes(Qt::Horizontal)){
                ax->setRange(xLimits.first, xLimits.second);
            }
            max = scatterSeries->points()[0].y();
            min = scatterSeries->points()[0].y();

            for(auto& p : scatterSeries->points()){
                if(p.y() > max) max = p.y();
                if(p.y() < min) min = p.y();
            }

            yLimits.first = min-2; yLimits.second = max+2;

            for(auto& ax : chart->axes(Qt::Vertical)){
                ax->setRange(yLimits.first, yLimits.second);
            }
        }
    }
    chartView->repaint();
}
