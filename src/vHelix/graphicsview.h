/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>

#include <QGuiApplication>
#include <QVBoxLayout>

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/qcameralens.h>
#include <QtGui/QScreen>
#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QRenderSettings>
#include <QViewport>
#include <QCameraSelector>
#include <QClearBuffers>
#include <QRenderStateSet>
#include <QCullFace>
#include <QDepthTest>
#include <QRenderPass>
#include <QTechnique>
#include <QNoDepthMask>
#include <QGoochMaterial>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <QWidget>
#include <Qt3DCore/QNode>

#include <QMAtrix4x4>

#include <vector>
#include <iostream>
#include <string>
#include "model.h"

// graphical representation of helix
namespace Graphics {


// temporary storage
class Helix
{
public:
    Helix(const QVector3D position, const QVector4D orientation, int bases);
private:
    Qt3DExtras::QCylinderMesh *cyl;
    Qt3DExtras::QSphereMesh *sphere;
    std::vector<int> bases;
    int length;
    QVector3D position;
    QVector4D orientation;
};
}

class GraphicsView : public QWidget
{
    Q_OBJECT
    QWidget *container;

public:
    explicit GraphicsView(QWidget *parent = nullptr);
    ~GraphicsView();

    void addSphere(const QVector3D &translation, const QQuaternion &rotation, const QColor &color);
    void addCylinder(const QVector3D &translation, const QQuaternion &rotation, const QColor &color);
    void drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity);

    void drawHelices(std::vector<Model::Helix> helices);
    void drawStrands(std::vector<Model::Strand> strands);

    void clear();

protected:
    void setCamera(const std::vector<double> &perspective,
                   const QVector3D &position, const QVector3D &upvector,
                   const QVector3D &viewcenter);

private:

    Qt3DExtras::Qt3DWindow *view_;

    QSize screenSize_;
    Qt3DInput::QInputAspect *input_;
    Qt3DCore::QEntity *rootEntity_;

    // camera
    Qt3DRender::QCamera *cameraEntity_;
    Qt3DCore::QEntity *lightEntity_;
    Qt3DRender::QPointLight *light_;
    Qt3DCore::QTransform *lightTransform_;
    Qt3DExtras::QOrbitCameraController *camController_;

    // simple mesh for testing
    QVBoxLayout *lay_;

    std::vector<Graphics::Helix> helices;
    std::vector<Qt3DCore::QEntity*> entities;
};

#endif // GRAPHICSVIEW_H
