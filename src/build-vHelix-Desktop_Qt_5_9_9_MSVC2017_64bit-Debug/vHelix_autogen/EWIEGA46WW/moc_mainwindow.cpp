/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[15];
    char stringdata0[232];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 9), // "sendMesh_"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 11), // "std::string"
QT_MOC_LITERAL(4, 34, 3), // "str"
QT_MOC_LITERAL(5, 38, 7), // "action_"
QT_MOC_LITERAL(6, 46, 3), // "arg"
QT_MOC_LITERAL(7, 50, 15), // "vHelixToWindow_"
QT_MOC_LITERAL(8, 66, 3), // "msg"
QT_MOC_LITERAL(9, 70, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(10, 94, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(11, 118, 25), // "on_actionAtrail_triggered"
QT_MOC_LITERAL(12, 144, 24), // "on_actionPhysX_triggered"
QT_MOC_LITERAL(13, 169, 26), // "on_actionConvert_triggered"
QT_MOC_LITERAL(14, 196, 35) // "on_actionExport_selection_tri..."

    },
    "MainWindow\0sendMesh_\0\0std::string\0str\0"
    "action_\0arg\0vHelixToWindow_\0msg\0"
    "on_actionExit_triggered\0on_actionOpen_triggered\0"
    "on_actionAtrail_triggered\0"
    "on_actionPhysX_triggered\0"
    "on_actionConvert_triggered\0"
    "on_actionExport_selection_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       5,    2,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   67,    2, 0x0a /* Public */,
       9,    0,   70,    2, 0x0a /* Public */,
      10,    0,   71,    2, 0x0a /* Public */,
      11,    0,   72,    2, 0x0a /* Public */,
      12,    0,   73,    2, 0x0a /* Public */,
      13,    0,   74,    2, 0x08 /* Private */,
      14,    0,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendMesh_((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 1: _t->action_((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< std::string(*)>(_a[2]))); break;
        case 2: _t->vHelixToWindow_((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 3: _t->on_actionExit_triggered(); break;
        case 4: _t->on_actionOpen_triggered(); break;
        case 5: _t->on_actionAtrail_triggered(); break;
        case 6: _t->on_actionPhysX_triggered(); break;
        case 7: _t->on_actionConvert_triggered(); break;
        case 8: _t->on_actionExport_selection_triggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (MainWindow::*_t)(std::string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::sendMesh_)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(std::string , std::string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::action_)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::sendMesh_(std::string _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::action_(std::string _t1, std::string _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE