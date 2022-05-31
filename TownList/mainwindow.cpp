#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // model
    model = new TownlistModel(this);
    model->setObjectName("model");

    proxy_model = new QSortFilterProxyModel(this);
    proxy_model->setSourceModel(model);

    proxy_model->sort(0);

    // ui
    ui.setupUi(this);

    // view
    ui.view_country->setModel(proxy_model);
    ui.view_region->setModel(proxy_model);
    ui.view_district->setModel(proxy_model);

    // indexes
    QObject::connect(ui.view_country->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
        ui.view_region, SLOT(setRootIndex(const QModelIndex&))
    );
    QObject::connect(ui.view_country->selectionModel(), &QItemSelectionModel::currentChanged,
        ui.view_region, [=]() {
            ui.view_region->setCurrentIndex(proxy_model->index(0, 0, ui.view_region->rootIndex())); }
    );
    QObject::connect(ui.view_region->selectionModel(), &QItemSelectionModel::currentChanged,
        ui.view_district, [=](const QModelIndex& root) {
            root.isValid() ? ui.view_district->setRootIndex(root) :
                ui.view_district->setRootIndex(ui.view_region->rootIndex()); }
    );
    QObject::connect(ui.view_region->selectionModel(), &QItemSelectionModel::currentChanged,
        ui.view_district, [=]() {
            ui.view_district->setCurrentIndex(proxy_model->index(0, 0, ui.view_district->rootIndex())); }
    );

    // line edits
    QObject::connect(ui.view_country->selectionModel(), &QItemSelectionModel::currentChanged,
        ui.edit_country, [=](const QModelIndex& index) {
            ui.edit_country->setText(index.data().toString()); }
    );
    QObject::connect(ui.view_region->selectionModel(), &QItemSelectionModel::currentChanged,
        ui.edit_region, [=](const QModelIndex& index) {
            ui.edit_region->setText(index.data().toString()); }
    );
    QObject::connect(ui.view_district->selectionModel(), &QItemSelectionModel::currentChanged,
        ui.edit_district, [=](const QModelIndex& index) {
            ui.edit_district->setText(index.data().toString()); }
    );
}


void MainWindow::on_btn_add_country_clicked() {
    QModelIndex parent = proxy_model->mapToSource(ui.view_country->rootIndex());

    QObject* item = new QObject;
    item->setObjectName("");
    model->add(item, parent);

    QModelIndex current_index = proxy_model->index(0, 0, proxy_model->mapFromSource(parent));
    ui.view_country->setCurrentIndex(current_index);
    ui.view_country->edit(current_index);
}

void MainWindow::on_btn_add_region_clicked() {
    QModelIndex parent = proxy_model->mapToSource(ui.view_region->rootIndex());

    QObject* item = new QObject;
    item->setObjectName("");
    model->add(item, parent);

    QModelIndex current_index = proxy_model->index(0, 0, proxy_model->mapFromSource(parent));
    ui.view_region->setCurrentIndex(current_index);
    ui.view_region->edit(current_index);
}

void MainWindow::on_btn_add_district_clicked() {
    QModelIndex parent = proxy_model->mapToSource(ui.view_district->rootIndex());

    QObject* item = new QObject;
    item->setObjectName("");
    model->add(item, parent);

    QModelIndex current_index = proxy_model->index(0, 0, proxy_model->mapFromSource(parent));
    ui.view_district->setCurrentIndex(current_index);
    ui.view_district->edit(current_index);
}

void MainWindow::on_model_dataChanged(const QModelIndex& top_left, const QModelIndex& bottom_right,
    const QList<int>& roles) {
    Q_UNUSED(bottom_right)
        Q_UNUSED(roles)

        QString object_name = top_left.data().toString();
    if (object_name == "") {
        model->remove(top_left);
    }
}

void MainWindow::on_btn_del_country_clicked() {
    model->remove(proxy_model->mapToSource(ui.view_country->currentIndex()));
}

void MainWindow::on_btn_del_region_clicked() {
    model->remove(proxy_model->mapToSource(ui.view_region->currentIndex()));
}

void MainWindow::on_btn_del_district_clicked() {
    model->remove(proxy_model->mapToSource(ui.view_district->currentIndex()));
}

void MainWindow::on_btn_copy_country_clicked() {
    QApplication::clipboard()->setText(ui.edit_country->text());
}

void MainWindow::on_btn_copy_region_clicked() {
    QApplication::clipboard()->setText(ui.edit_region->text());
}

void MainWindow::on_btn_copy_district_clicked() {
    QApplication::clipboard()->setText(ui.edit_district->text());
}

void MainWindow::on_btn_save_country_clicked() {
    model->update();
}