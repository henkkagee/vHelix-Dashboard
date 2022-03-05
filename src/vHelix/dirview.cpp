/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#include "dirview.h"
#include <iostream>

DirView::DirView(QWidget *parent)
    : QTreeView(parent)
{
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath() + "/../workspace");
    setModel(model);
    const QModelIndex rootIndex = model->index(QDir::currentPath() + "/../workspace");
    setRootIndex(rootIndex);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setAnimated(false);
    setIndentation(20);
    setSortingEnabled(true);
    //setColumnWidth(0, width() / 3);
    setColumnWidth(0, width()*2);
    setColumnWidth(1, width());
    setColumnWidth(2, width());
    setColumnWidth(3, width()*2);
    show();
}

DirView::~DirView()
{
    delete model;
}

void DirView::onItemClick()
{
    /*selectionModel()->selectedIndexes();
    model->filePath(->selectedIndexes);
    */
}

void DirView::update_() {
    std::cout << "DirView updating\n";
    model->setRootPath("");
    model->setRootPath("/../workspace");
    show();
}

const QFileSystemModel* DirView::getModel()
{
    return model;
}
