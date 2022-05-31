#include "townlistmodel.h"

TownlistModel::TownlistModel(QObject* parent) : ObjectTreeModel(parent)
{
    QStringList columns;
    columns << "objectName";
    setColumns(std::move(columns));

    if (!createConection()) {
        qDebug() << "Unable to create conection with data base!";
        exit(-1);
    }

    select();
}

bool TownlistModel::createConection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setDatabaseName("townlist");
    db.setUserName("postgres");
    db.setHostName("localhost");
    db.setPassword("c5jktSJW");
    db.setPort(5432);

    if (!db.open()) {
        qDebug() << "Unable to open a data base!";
        return false;
    }

    return true;
}

void TownlistModel::insertData(const QString& source_file, const QString& country_name, const QChar& separator) {
    QFile file(source_file);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot be opened for read: file(" << source_file << ")";
        return;
    }

    QTextStream tin(&file);

    QSqlQuery query;
    QString str_query_country = "INSERT INTO country(name)"
        "VALUES('" + country_name + "');";

    QString str_query_region = "INSERT INTO region(name, country_id)"
        "VALUES('%1', (SELECT id FROM country WHERE name = '" + country_name + "'));";

    QString str_query_distr = "INSERT INTO district(name, region_id)"
        "VALUES('%1', (SELECT id FROM region WHERE name = '%2'));";

    query.exec(str_query_country);

    QString region_name;
    while (!tin.atEnd()) {
        QString str_name = tin.readLine();

        if (str_name[0] == separator) {
            region_name = str_name.remove(separator);
            query.exec(str_query_region.arg(region_name));
        }
        else {
            query.exec(str_query_distr.arg(str_name, region_name));
        }
    }
}

void TownlistModel::select() {
    const QString str_query_table_names{
        "SELECT "
        " table_name "
        "FROM "
        " information_schema.table_constraints "
        "WHERE "
        " constraint_schema = 'public' "
        " AND constraint_type = 'PRIMARY KEY';"
    };
    QSqlQuery query_tables(str_query_table_names);

    while (query_tables.next()) {
        QString table_name = query_tables.value("table_name").toString();
        QSqlQuery query_items("SELECT * FROM " + table_name + ";");

        if (!hasForeignKey(table_name)) {
            while (query_items.next()) {
                QObject* item = new QObject;

                item->setObjectName(query_items.value("name").toString());
                item->setParent(_root_item);
            }
        }
        else {
            QString parent_table_name = referencesTable(table_name);
            while (query_items.next()) {
                QObject* item = new QObject;

                item->setObjectName(query_items.value("name").toString());

                QString str_query_parent_name{
                    "SELECT "
                    " name "
                    "FROM "
                    + parent_table_name +
                    " WHERE "
                    " id = " + query_items.value(parent_table_name + "_id").toString()
                };
                QSqlQuery query_parent_name(str_query_parent_name);
                query_parent_name.first();
                item->setParent(_root_item->findChild<QObject*>(query_parent_name.value("name").toString()));
            }
        }
    }

}

void TownlistModel::update() const {
    const QString str_query_update{
        "UPDATE "
        " country "
        "SET "
        " "
    };
}

bool TownlistModel::hasForeignKey(const QString& table_name) const {
    const QString str_query_keys{
        "SELECT "
        " table_name, "
        " constraint_type "
        "FROM "
        " information_schema.table_constraints "
        "WHERE "
        " constraint_schema = 'public' "
        " AND constraint_type IN('PRIMARY KEY', 'FOREIGN KEY');"
    };
    QSqlQuery query(str_query_keys);

    while (query.next()) {
        if (query.value("table_name").toString() == table_name &&
            query.value("constraint_type").toString() == "FOREIGN KEY") {
            return true;
        }
    }

    return false;
}

QString TownlistModel::referencesTable(const QString& table_name) const {
    const QString str_query_references_table{
        "SELECT "
        " tc.table_name, "
        " ccu.table_name AS references_table  "
        "FROM "
        " information_schema.table_constraints tc "
        " LEFT JOIN information_schema.referential_constraints rc "
        " ON tc.constraint_catalog = rc.constraint_catalog "
        " AND tc.constraint_schema = rc.constraint_schema "
        " AND tc.constraint_name = rc.constraint_name "
        " LEFT JOIN information_schema.constraint_column_usage ccu "
        " ON rc.unique_constraint_catalog = ccu.constraint_catalog "
        " AND rc.unique_constraint_schema = ccu.constraint_schema "
        " AND rc.unique_constraint_name = ccu.constraint_name  "
        "WHERE "
        " tc.constraint_catalog = 'townlist' "
        " AND tc.constraint_type = 'FOREIGN KEY';"
    };
    QSqlQuery query(str_query_references_table);

    while (query.next()) {
        if (query.value("table_name").toString() == table_name) {
            return query.value("references_table").toString();
        }
    }

    return QString();
}