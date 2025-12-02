#include "QtApp.h"
#include "ui_QtApp.h"
#include "ChartView.h"
#include "CallOut.h"
#include "CsvDataProvider.h"

#include <QtCharts/QChart>
#include <QtCharts/QCandlestickSet>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDateTime>
#include <QApplication>

using namespace QtCharts;

static QLineSeries* makeMAFromSeries(const QCandlestickSeries* k, int N)
{
    auto* ma = new QLineSeries();
    ma->setName(QString("MA%1").arg(N));
    double sum = 0.0; int count = 0;
    for (auto* s : k->sets()) {
        const qreal c = s->close();
        sum += c; ++count;
        if (count >= N) {
            if (count > N) sum -= (*(k->sets().at(count - N - 1))).close();
            ma->append(s->timestamp(), sum / N);
        }
    }
    return ma;
}

//构造函数
QtApp::QtApp(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::QtAppClass)
{
    ui->setupUi(this);

    // 全局字体（中文）
    QFont font("Microsoft YaHei", 10);
    qApp->setFont(font);

    buildChart();

    // 菜单 & 工具栏
    auto* fileMenu = menuBar()->addMenu(QString::fromUtf8(u8"文件"));
    auto* actImport = fileMenu->addAction(QString::fromUtf8(u8"导入CSV"));
    auto* actClear = fileMenu->addAction(QString::fromUtf8(u8"清空"));
    fileMenu->addSeparator();
    auto* actExit = fileMenu->addAction(QString::fromUtf8(u8"退出"));

    auto* viewMenu = menuBar()->addMenu(QString::fromUtf8(u8"视图"));
    auto* actReset = viewMenu->addAction(QString::fromUtf8(u8"重置缩放"));

    auto* toolMenu = menuBar()->addMenu(QString::fromUtf8(u8"工具"));
    auto* actTrend = toolMenu->addAction(QString::fromUtf8(u8"趋势线")); actTrend->setCheckable(true);
    auto* actHLine = toolMenu->addAction(QString::fromUtf8(u8"水平线")); actHLine->setCheckable(true);

    auto* tb = addToolBar(QString::fromUtf8(u8"工具栏"));
    tb->addAction(actImport); tb->addAction(actClear); tb->addSeparator();
    tb->addAction(actReset); tb->addSeparator();
    tb->addAction(actTrend); tb->addAction(actHLine);

    connect(actImport, &QAction::triggered, this, [this](bool) { onImportCsv(); });
    connect(actClear, &QAction::triggered, this, [this](bool) { onClearAll();   });
    connect(actReset, &QAction::triggered, this, [this](bool) { onResetZoom();  });

    // toggled(bool) 我们确实要用，显式接收
    connect(actTrend, &QAction::toggled, this, [this](bool on) { onToggleTrendLine(on); });
    connect(actHLine, &QAction::toggled, this, [this](bool on) { onToggleHLine(on);     });
    connect(actExit, &QAction::triggered, this, [this](bool) { close(); });
}

QtApp::~QtApp()
{
    delete ui;
}

void QtApp::buildChart()
{
    if (m_chart)
    {
        delete m_chart;
        m_chart = nullptr; 
    }

    //创建一个表格，设置标题
    m_chart = new QChart();
    m_chart->setTitle(QString::fromUtf8(u8"K线演示（支持CSV导入、悬停气泡、缩放和平移、画线）"));

    m_k = new QCandlestickSeries();
    m_k->setName(QStringLiteral("K线"));
    m_k->setIncreasingColor(Qt::red);
    m_k->setDecreasingColor(Qt::green);

    m_chart->addSeries(m_k);

    m_ax = new QDateTimeAxis();
    m_ax->setFormat("MM-dd");
    m_ax->setTitleText(QString::fromUtf8(u8"日期"));

    m_ay = new QValueAxis();
    m_ay->setLabelFormat("%.2f");
    m_ay->setTitleText(QString::fromUtf8(u8"价格"));

    //设置坐标轴
    m_chart->addAxis(m_ax, Qt::AlignBottom);
    m_chart->addAxis(m_ay, Qt::AlignLeft);
    
    m_k->attachAxis(m_ax);
    m_k->attachAxis(m_ay);


    if (m_view) 
    {
        m_view->deleteLater();
        m_view = nullptr; 
    }

    m_view = new ChartView(m_chart, this);
    m_view->setReferenceSeries(m_k);
    setCentralWidget(m_view);

    // 悬停显示：使用 CallOut
    m_tip = new CallOut(m_chart);
    connect(m_k,
        qOverload<bool, QtCharts::QCandlestickSet*>(
            &QtCharts::QCandlestickSeries::hovered),
        this,
        [this](bool on, QtCharts::QCandlestickSet* s) {
        if (!s || !on) { if (m_tip) m_tip->hide(); return; }
        const auto t = QDateTime::fromMSecsSinceEpoch((qint64)s->timestamp());
        const QString text = QString::fromUtf8(u8"%1\n开:%2  高:%3  低:%4  收:%5")
            .arg(t.toString("yyyy-MM-dd"))
            .arg(s->open(), 0, 'f', 2)
            .arg(s->high(), 0, 'f', 2)
            .arg(s->low(), 0, 'f', 2)
            .arg(s->close(), 0, 'f', 2);
        const QPointF anchorVal(s->timestamp(), s->high());
        const QPointF anchorPx = m_chart->mapToPosition(anchorVal, m_k);
        m_tip->setAnchor(anchorPx);
        m_tip->setText(text);
        m_tip->updateGeometry();
        m_tip->show();
    });
}

void QtApp::rebuildMA()
{
    if (m_ma5) 
    { 
      m_chart->removeSeries(m_ma5);
      delete m_ma5; 
      m_ma5 = nullptr; 
    }
    if (m_ma20) { m_chart->removeSeries(m_ma20); delete m_ma20; m_ma20 = nullptr; }

    m_ma5 = makeMAFromSeries(m_k, m_maN1);
    m_ma20 = makeMAFromSeries(m_k, m_maN2);
    m_ma5->setColor(QColor("red"));

    m_chart->addSeries(m_ma5);
    m_chart->addSeries(m_ma20);

    for (auto* s : { (QAbstractSeries*)m_ma5, (QAbstractSeries*)m_ma20 }) 
    {
        s->attachAxis(m_ax);
        s->attachAxis(m_ay);
    }
}

void QtApp::onImportCsv(bool)
{
    const QString path = QFileDialog::getOpenFileName(this, QString::fromUtf8(u8"选择CSV文件"), QString(), QString::fromUtf8(u8"CSV (*.csv)"));
    if (path.isEmpty()) return;

    QString err; 
    auto rows = CsvDataProvider::load(path, &err);
    if (!err.isEmpty())
    {
        statusBar()->showMessage(err, 5000);
        return; 
    }
    // 清空现有 K 数据
    m_k->clear();
    for (const auto& r : rows)
    {
        auto* set = new QCandlestickSet(r.t.toMSecsSinceEpoch());
        set->setOpen(r.o); 
        set->setHigh(r.h); 
        set->setLow(r.l); 
        set->setClose(r.c);
        m_k->append(set);
    }

    // 调整 X 轴范围
    if (!rows.isEmpty())
    {
        // 2) 计算时间和价格范围（并防护极端情况）
        QDateTime tmin = rows.front().t;
        QDateTime tmax = rows.back().t;
        if (!tmin.isValid() || !tmax.isValid() || tmin >= tmax) {
            // 兜底：单点或无效时间 -> 给一个最小跨度
            const QDateTime now = QDateTime::currentDateTime();
            tmin = now.addDays(-1);
            tmax = now;
        }

        double ymin = std::numeric_limits<double>::max();
        double ymax = -std::numeric_limits<double>::max();
        for (const auto& r : rows) 
        {
            ymin = std::min(ymin, r.l);
            ymax = std::max(ymax, r.h);
        }
        if (ymin > ymax) { ymin = 0; ymax = 1; } // 防护
        double pad = (ymax - ymin) * 0.05;
        if (pad <= 0) pad = std::max(1.0, ymax * 0.02);

        // 3) **Remove + re-add axes**，并 attach（确保轴是 chart 管理的最新实例）
        // （有时候在 series 变动/重建时，重新 add/attach 更稳健）
        if (m_chart->axes().contains(m_ax))
        {
            m_chart->removeAxis(m_ax);
        }
        if (m_chart->axes().contains(m_ay)) 
        {
            m_chart->removeAxis(m_ay);
        }

        m_chart->addAxis(m_ax, Qt::AlignBottom);
        m_chart->addAxis(m_ay, Qt::AlignLeft);
        m_k->attachAxis(m_ax);
        m_k->attachAxis(m_ay);

        // 4) 显式设置范围（必须用 QDateTime 对象）
        m_ax->setRange(tmin, tmax);
        m_ay->setRange(ymin - pad, ymax + pad);
    }

    rebuildMA();
}

void QtApp::onClearAll(bool)
{
    m_k->clear();
    // 删除 MA 线（QChart::removeSeries 不会 delete 对象）
    if (m_ma5) { m_chart->removeSeries(m_ma5);  m_ma5->deleteLater();  m_ma5 = nullptr; }
    if (m_ma20) { m_chart->removeSeries(m_ma20); m_ma20->deleteLater(); m_ma20 = nullptr; }

    //todo  清空坐标轴。

}

void QtApp::onResetZoom(bool)
{
    m_chart->zoomReset();
}

void QtApp::onToggleTrendLine(bool checked)
{
    if (!m_view)
    {
        return;
    }
    auto m = m_view->drawMode();
    if (m == ChartView::DrawMode::TrendLine) m_view->setDrawMode(ChartView::DrawMode::None);
    else m_view->setDrawMode(checked ? ChartView::DrawMode::TrendLine
        : ChartView::DrawMode::None);
}

void QtApp::onToggleHLine(bool checked)
{
    if (!m_view) return;
    auto m = m_view->drawMode();
    if (m == ChartView::DrawMode::HLine) m_view->setDrawMode(ChartView::DrawMode::None);
    else m_view->setDrawMode(checked ? ChartView::DrawMode::HLine
        : ChartView::DrawMode::None);
}