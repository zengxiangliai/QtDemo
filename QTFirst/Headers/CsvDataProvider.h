#pragma once
#include <QVector>
#include <QDateTime>
#include <QString>

struct OhlcRow {
    QDateTime t; double o = 0, h = 0, l = 0, c = 0; long long v = 0;
};

class CsvDataProvider {
public:
    static QVector<OhlcRow> load(const QString& path, QString* err = nullptr);
};