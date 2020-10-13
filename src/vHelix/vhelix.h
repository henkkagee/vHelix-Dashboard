/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#ifndef VHELIX_H
#define VHELIX_H
#include <string>
#include <QObject>
#include <QString>
#include <QFile>
#include <QVector>
#include <QVariant>

#include <iostream>
#include <processio.h>

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

    void atrail_();
    void physX_relaxation_(const QVector<QVariant> args);
    void export_(const QVector<QVariant> &args);
    void convert_(const std::string& format);
    int estimate_base_use_(const QVector<QVariant> &args);
    void openPLY();

// Qt slots and signals shouldn't pass arguments as references
public slots:
    void readMesh_(std::string mesh);
    void action_(std::string cmd, QVector<QVariant> arg);

signals:
    void sendToConsole_(std::string msg);
};


#endif // VHELIX_H
