#pragma once

#include "objecttreemodel.h"
#include <QtSql>
#include <QFile>

class TownlistModel : public ObjectTreeModel {
    Q_OBJECT

private:
    void select();
    bool    hasForeignKey(const QString& table_name) const;
    QString referencesTable(const QString& table_name) const;

public:
    TownlistModel(QObject* parent = nullptr);

    bool createConection();
    void insertData(const QString& source_file, const QString& country_name,
        const QChar& separator = QChar('!'));
public slots:
    void update() const;
};

