/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 *  Qt3D view for 3D graphics.
 *
 *  Currently old entities are apparently not completely unloaded when loading a new model, resulting in even poorer performance.
*/

#include "graphicsview.h"
#include <QObject>
#include <QObjectList>
#include <math.h>

GraphicsView::GraphicsView(QWidget *parent)
    : QWidget(parent)
{
    view_ = new Qt3DExtras::Qt3DWindow();
    view_ -> defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    input_ = new Qt3DInput::QInputAspect;
    view_ -> registerAspect(input_);

    rootEntity_ = new Qt3DCore::QEntity();

    // enable depth testing for all renderstates. This is at least a temporary fix for Qt3D 5.9.9 where depth testing is broken.
    Qt3DRender::QMaterial *  rootMaterial   = new Qt3DRender::QMaterial(rootEntity_);
            Qt3DRender::QEffect *effect = rootMaterial->effect();
            if (effect)
            {
                for (Qt3DRender::QTechnique* currentTechnique : effect->techniques())
                {
                    for (Qt3DRender::QRenderPass * currentPass : currentTechnique->renderPasses())
                    {
                        for (Qt3DRender::QRenderState * currentState : currentPass->renderStates())
                        {
                            if (dynamic_cast<Qt3DRender::QNoDepthMask *>(currentState))
                            {
                                currentPass->removeRenderState(currentState);
                                Qt3DRender::QDepthTest*  depthTest  = new Qt3DRender::QDepthTest;
                                depthTest ->setDepthFunction(Qt3DRender::QDepthTest::Less);
                                currentPass->addRenderState(depthTest);
                                break;
                            }
                        }
                    }
                }
            }
            rootMaterial  ->setEffect(effect);
            rootEntity_->addComponent(rootMaterial);

    // camera
    cameraEntity_ = view_ -> camera();
    std::vector<double> vec {45.0f, 1.0f/1.0f, 0.1f, 1000.0f};
    setCamera(vec, QVector3D(0, 0, 20.0f), QVector3D(0, 1, 0), QVector3D(0, 0, 0));
    lightEntity_ = new Qt3DCore::QEntity(rootEntity_);
    light_ = new Qt3DRender::QPointLight(lightEntity_);
    light_->setColor("white");
    light_->setIntensity(1);
    //light_->setLinearAttenuation(2.f);
    //light_->setQuadraticAttenuation(2.f);
    lightEntity_->addComponent(light_);
    lightTransform_ = new Qt3DCore::QTransform(lightEntity_);
    lightTransform_->setTranslation(cameraEntity_->position());
    lightEntity_->addComponent(lightTransform_);
    camController_ = new Qt3DExtras::QOrbitCameraController(rootEntity_);
    camController_->setCamera(cameraEntity_);

    Qt3DRender::QRenderSettings *renderSettings_ = view_ -> renderSettings();
    renderSettings_ ->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    rootEntity_->addComponent(renderSettings_);

    view_ -> setRootEntity(rootEntity_);
    container = createWindowContainer(view_, this);
    lay_ = new QVBoxLayout(this);
    lay_->addWidget(container);
    screenSize_ = view_ -> screen()->size();
    container -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GraphicsView::~GraphicsView()
{
    delete lay_;
    delete camController_;
    delete light_;
    delete lightEntity_;
    delete rootEntity_;
    delete view_;
}

void GraphicsView::addSphere(const QVector3D &translation, const QQuaternion &rotation, const QColor &color, const float &radius)
{
    // Sphere shape data
    Qt3DCore::QEntity *m_sphereEntity = new Qt3DCore::QEntity();
    m_sphereEntity ->setEnabled(true);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(4);
    sphereMesh->setSlices(4);
    sphereMesh->setRadius(radius);

    // Sphere mesh transform
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.rotate(rotation);
    matrix.rotate(90, QVector3D(1.0f, 0.0f, 0.0f));
    sphereTransform->setMatrix(matrix);
    sphereTransform->setTranslation(translation);

    Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(color);
    sphereMaterial->setAmbient(color);

    // Sphere
    m_sphereEntity->addComponent(sphereMesh);
    m_sphereEntity->addComponent(sphereMaterial);
    m_sphereEntity->addComponent(sphereTransform);
    m_sphereEntity->setParent(rootEntity_);
    m_sphereEntity->setObjectName(QString("sphere"));
    entities.push_back(m_sphereEntity);
}

void GraphicsView::addCylinder(const QVector3D &translation, const QQuaternion &rotation, const QColor &color)
{
    // Sphere shape data
    Qt3DCore::QEntity *m_sphereEntity = new Qt3DCore::QEntity();
    m_sphereEntity ->setEnabled(true);
    Qt3DExtras::QCylinderMesh *mesh = new Qt3DExtras::QCylinderMesh();
    mesh->setRings(6);
    mesh->setSlices(6);
    mesh->setRadius(0.5f);
    mesh->setLength(15.0f);

    // Sphere mesh transform
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.rotate(rotation);
    matrix.rotate(90, QVector3D(1.0f, 0.0f, 0.0f));
    transform->setMatrix(matrix);
    transform->setTranslation(translation);

    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(color);
    material->setAmbient(color);

    // enable depth testing for all renderstates. This is at least a temporary fix for Qt3D 5.9.9 where depth testing is missing.
    Qt3DRender::QEffect *effect = material->effect();
    if (effect)
    {
        for (Qt3DRender::QTechnique* currentTechnique : effect->techniques())
        {
            for (Qt3DRender::QRenderPass * currentPass : currentTechnique->renderPasses())
            {
                for (Qt3DRender::QRenderState * currentState : currentPass->renderStates())
                {
                    if (dynamic_cast<Qt3DRender::QNoDepthMask *>(currentState))
                    {
                        currentPass->removeRenderState(currentState);
                        Qt3DRender::QDepthTest*  depthTest  = new Qt3DRender::QDepthTest;
                        depthTest ->setDepthFunction(Qt3DRender::QDepthTest::Less);
                        currentPass->addRenderState(depthTest);
                        break;
                    }
                }
            }
        }
    }

    // Sphere
    QTransform qtrans;
    m_sphereEntity->addComponent(mesh);
    material -> setEffect(effect);
    m_sphereEntity->addComponent(material);
    m_sphereEntity->addComponent(transform);
    m_sphereEntity->setParent(rootEntity_);
    m_sphereEntity->setObjectName(QString("cylinder"));
    entities.push_back(m_sphereEntity);
}

void GraphicsView::setCamera(const std::vector<double> &perspective,
                             const QVector3D &position, const QVector3D &upvector,
                             const QVector3D &viewcenter)
{
    cameraEntity_->lens()->setPerspectiveProjection(perspective[0], perspective[1],
                                            perspective[2], perspective[3]);
    cameraEntity_->setPosition(position);
    cameraEntity_->setUpVector(upvector);
    cameraEntity_->setViewCenter(viewcenter);
}

void GraphicsView::drawHelices(std::vector<Model::Helix> helices)
{
    // Clear previous entities

    for (unsigned long long i = 0; i < entities.size(); i++) {
        entities[i]->setParent((Qt3DCore::QEntity*) nullptr);
        //entities[i]->setEnabled(false);
        delete (entities[i]);
    }
    entities.clear();
    std::vector<QColor> colors {QRgb(0x53886D), QRgb(0xa69900), QRgb(0xAC166F), QRgb(0x7FFFD4), QRgb(0xFF7F50), QRgb(0xd3FFCE), QRgb(0xBADA55)};
    unsigned int col = 0;
    int first = 0;
    unsigned int cnt = 0;
    for (auto h : helices) {
        // each coordinate must be cast to float in order to construct the object properly
        //addCylinder(QVector3D((float)h.position_.x(), (float)h.position_.y(), (float)h.position_.z()),
        //         h.orientation_, colors[col]);
        col++;
        if (col >= colors.size()) {
            col = 0;
        }

        for (int i = 0; i < h.bases_; i++) {
            addSphere(h.Fbases_[i].getPos(), QQuaternion(), colors[col], 0.2f);
            addSphere(h.Bbases_[i].getPos(), QQuaternion(), colors[col], 0.2f);
            // test connection visibility
            //drawLine(h.Fbases[i].position, h.Bbases[i].position, QColor(QRgb(0x000000)), rootEntity_);
        }
        if (h.Fbases_[h.bases_ - 1].getForward() != nullptr) {
            first = 1;
            //drawLine(h.Fbases[h.bases_ - 1].position, h.Fbases[h.bases_ - 1].getForward()->position, QColor(QRgb(0x000000)), rootEntity_);
            drawLine(h.position_, h.Fbases_[h.bases_ - 1].getForward()->getParent()->position_, QColor(QRgb(0x000000)), rootEntity_);
        }
        cnt++;
    }
}

void GraphicsView::drawStrands(std::vector<Model::Strand> strands)
{
    // Clear previous entities

    for (unsigned long long i = 0; i < entities.size(); i++) {
        entities[i]->setParent((Qt3DCore::QEntity*) nullptr);
        entities[i]->setEnabled(false);
        entities[i]->~QEntity();
    }
    entities.clear();
    std::vector<QColor> colours {QRgb(0xC19068), QRgb(0xff9933)};
    QColor scaffold_colour = QRgb(0x385B83);
    QColor colour;
    unsigned long long int num = 0;
    unsigned int col = 0;
    for (auto &s : strands) {
        if (s.scaffold_) {
            colour = scaffold_colour;
        }
        else {
            colour = colours[col];
        }
        for (unsigned long long int i = 0; i < s.bases_.size(); i++) {
            if (s.scaffold_) {
                if (i == 0) {
                    addSphere(s.bases_[i]->getPos(), QQuaternion(), QRgb(0x00ff00), 0.5f);
                }
                else if (i == s.bases_.size() - 1) {
                    addSphere(s.bases_[i]->getPos(), QQuaternion(), QRgb(0xff0000), 0.5f);
                }
            }
            addSphere(s.bases_[i]->getPos(), QQuaternion(), colour, 0.2f);
            if (s.bases_[i]->getOpposite() != nullptr) {
                drawLine(s.bases_[i]->getPos(), s.bases_[i]->getOpposite()->getPos(), QRgb(0x000000), rootEntity_);
            }
            /*if (s.bases_[i]->getForward() != nullptr) {
                drawLine(s.bases_[i]->getPos(), s.bases_[i]->getForward()->getPos(), QRgb(0xFF0000), rootEntity_);
            }
            if (s.bases_[i]->getBackward() != nullptr) {
                drawLine(s.bases_[i]->getPos(), s.bases_[i]->getBackward()->getPos(), QRgb(0xFF0000), rootEntity_);
            }*/

        }
        num++;
        col++;
        if (col > colours.size()-1) {
            col = 0;
        }
    }
}

void GraphicsView::drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity)
{

    auto *geometry = new Qt3DRender::QGeometry(_rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();

    // new constructor from Qnode
    // https://doc.qt.io/qt-5/qt3drender-qbuffer.html#QBuffer
    auto *buf = new Qt3DRender::QBuffer();
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    // new constructor from Qnode
    // https://doc.qt.io/qt-5/qt3drender-qbuffer.html#QBuffer
    auto *indexBuffer = new Qt3DRender::QBuffer();
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(_rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(_rootEntity);
    lineEntity->addComponent(line);
    lineEntity->setObjectName("line");
    lineEntity->addComponent(material);
    entities.push_back(lineEntity);

}

void GraphicsView::clear() {
    const QObjectList& children = rootEntity_->children();
    for (auto &object : children) {
        if (object->objectName() == "sphere" || object->objectName() == "cylinder" || object->objectName() == "line") {
            object->setParent(nullptr);
        }
        //delete object;
    }
}
