#pragma once
#include <QtWidgets/QMainWindow>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QScopedPointer>


QT_BEGIN_NAMESPACE
namespace Ui { class QtAppClass; }
QT_END_NAMESPACE

class ChartView;
class CallOut;
namespace QtCharts { class QChart; }

class QtApp : public QMainWindow
{
    Q_OBJECT

public:
    QtApp(QWidget* parent = nullptr);
    ~QtApp();

private slots:
    void onImportCsv(bool checked = false);
    void onClearAll(bool checked = false);
    void onResetZoom(bool checked = false);
    void onToggleTrendLine(bool checked);
    void onToggleHLine(bool checked);

private:
    void buildChart(); //构建表格
    void rebuildMA();  //均线

private:
    Ui::QtAppClass* ui;

    QtCharts::QChart* m_chart = nullptr;
    ChartView* m_view = nullptr;

    QtCharts::QCandlestickSeries* m_k = nullptr;
    QtCharts::QLineSeries* m_ma5 = nullptr;
    QtCharts::QLineSeries* m_ma20 = nullptr;

    QtCharts::QDateTimeAxis* m_ax = nullptr;
    QtCharts::QValueAxis* m_ay = nullptr;

    CallOut* m_tip = nullptr; // 悬浮气泡

    int m_maN1 = 5;
    int m_maN2 = 20;
};