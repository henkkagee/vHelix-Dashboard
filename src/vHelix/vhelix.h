/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 * Class for routing, relaxation, Python API interfacing and some directory setting.
*/

#ifndef VHELIX_H
#define VHELIX_H
#include <string>
#include <QObject>
#include <QString>
#include <QFile>
#include <QVector>
#include <QVariant>
#include <QDebug>
#include <thread>

#include <iostream>
#include <processio.h>
#include <sstream>
#include <fstream>
#include "boost/lexical_cast.hpp"
#include "atrail.h"
#include "scaffold_free.h"
#include <QtConcurrent>

#pragma push_macro("slots")
#undef slots
#define MS_NO_COREDLL
#ifdef _DEBUG
    #undef _DEBUG
    #include "Python.h"
    #define _DEBUG
#else
    #include "Python.h"
#endif
#pragma pop_macro("slots")

namespace vHelixFunc {
    std::string ExePath();
    bool compareEdges(std::vector<int> i, std::vector<int> j);

    struct plyData {
        std::vector<float> x;
        std::vector<float> y;
        std::vector<float> z;
        std::vector<std::vector<int>> edgelist;
        int vertexnr;
        int facenr;
    };

}

/*class vHelixThread : public QThread 
{
    Q_OBJECT
    void run() override {
        vHelix vh;

    }
public slots:

};*/
// Main class responsible for program logic
class vHelix : public QObject
{
    Q_OBJECT

public:
    vHelix();
    ~vHelix();
    vHelix(QObject* parent);

    const vHelixFunc::plyData getPlyData();
    void setSelection(std::vector<std::string> vec);

private:
    std::string meshdir_;
    std::string mesh_;
    std::string workspace_dir_;
    vHelixFunc::plyData *plydata_;
    std::vector<std::string> fileSelection_;
    Design *design;

    void atrail_(const QVector<QVariant> &args);
    void scaffold_free_(const QVector<QVariant> &args);
    //void physX_relaxation_(const QVector<QVariant> args);
    void export_(const QVector<QVariant> &args);
    void convert_(const std::string& format);
    std::pair<int,int> estimate_base_use_(const QVector<QVariant> &args);
    void openPLY();
    void open3d();

// Qt slots and signals shouldn't pass arguments as references
public slots:
    void readMesh_(QString mesh);
    void action_(QString cmd, QVector<QVariant> arg);

signals:
    void sendToConsole_(QString msg);
    void sendBaseEstimate_(QString msg);
    void update_();
};


#endif // VHELIX_H
