#include "CsvDataProvider.h"
#include <QFile>
#include <QTextStream>

QVector<OhlcRow> CsvDataProvider::load(const QString& path, QString* err)
{
    QVector<OhlcRow> rows;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (err) *err = QString::fromLatin1("无法打开文件: %1").arg(path);
        return rows;
    }
    QTextStream ts(&f);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    ts.setCodec("UTF-8");
#endif
    if (!ts.atEnd()) ts.readLine(); // 跳过表头
    while (!ts.atEnd()) {
        const QString line = ts.readLine().trimmed();
        if (line.isEmpty()) 
            continue;
        const auto cols = line.split(',');
        if (cols.size() < 6) 
            continue;
        OhlcRow r;
        r.t = QDateTime::fromString(cols[0], "yyyy/MM/dd");
        if (!r.t.isValid())
            r.t = QDateTime::fromString(cols[0], "yyyy/M/d");
        if (!r.t.isValid())
            r.t = QDateTime::fromString(cols[0], "yyyy-MM-dd");
        if (!r.t.isValid())
            r.t = QDateTime::fromString(cols[0], "yyyy-MM-dd HH:mm");

        r.o = cols[1].toDouble();
        r.h = cols[2].toDouble();
        r.l = cols[3].toDouble();
        r.c = cols[4].toDouble();
        r.v = cols[5].toLongLong();
        rows.push_back(r);
    }
    return rows;
}