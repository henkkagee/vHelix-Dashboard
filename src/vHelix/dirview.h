/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <QDir>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QModelIndexList>
#include <QItemSelectionModel>
#include <QFileSystemModel>

class DirView : public QTreeView
{
    Q_OBJECT

public:
    DirView(QWidget *parent = nullptr);
    ~DirView();
    void onItemClick();
    const QFileSystemModel* getModel();

private:
    QFileSystemModel *model;
};


#endif // DIRVIEW_H
