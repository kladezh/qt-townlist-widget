#pragma once

#include "ui_mainwindow.h"

#include <QtWidgets/QWidget>
#include <QSortFilterProxyModel>
#include <QClipboard>

#include "townlistmodel.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::MainWindowClass ui;

    TownlistModel* model;
    QSortFilterProxyModel* proxy_model;

private slots:
    void on_btn_add_country_clicked();
    void on_btn_add_region_clicked();
    void on_btn_add_district_clicked();
    void on_model_dataChanged(const QModelIndex& top_left, const QModelIndex& bottom_right,
        const QList<int>& roles);
    void on_btn_del_country_clicked();
    void on_btn_del_region_clicked();
    void on_btn_del_district_clicked();
    void on_btn_copy_country_clicked();
    void on_btn_copy_region_clicked();
    void on_btn_copy_district_clicked();
    void on_btn_save_country_clicked();
};

// "^" + text + ".*|(?<=-|\\s)" + text   - filter regexp