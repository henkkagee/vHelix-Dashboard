/****************************************************************************
** Meta object code from reading C++ file 'vhelix.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vHelix/vhelix.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vhelix.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vHelix_t {
    QByteArrayData data[11];
    char stringdata0[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vHelix_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vHelix_t qt_meta_stringdata_vHelix = {
    {
QT_MOC_LITERAL(0, 0, 6), // "vHelix"
QT_MOC_LITERAL(1, 7, 14), // "sendToConsole_"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 11), // "std::string"
QT_MOC_LITERAL(4, 35, 3), // "msg"
QT_MOC_LITERAL(5, 39, 9), // "readMesh_"
QT_MOC_LITERAL(6, 49, 4), // "mesh"
QT_MOC_LITERAL(7, 54, 7), // "action_"
QT_MOC_LITERAL(8, 62, 3), // "cmd"
QT_MOC_LITERAL(9, 66, 17), // "QVector<QVariant>"
QT_MOC_LITERAL(10, 84, 3) // "arg"

    },
    "vHelix\0sendToConsole_\0\0std::string\0"
    "msg\0readMesh_\0mesh\0action_\0cmd\0"
    "QVector<QVariant>\0arg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vHelix[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   32,    2, 0x0a /* Public */,
       7,    2,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 9,    8,   10,

       0        // eod
};

void vHelix::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<vHelix *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendToConsole_((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 1: _t->readMesh_((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 2: _t->action_((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< QVector<QVariant>(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariant> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (vHelix::*)(std::string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&vHelix::sendToConsole_)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject vHelix::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_vHelix.data,
    qt_meta_data_vHelix,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *vHelix::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vHelix::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vHelix.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int vHelix::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void vHelix::sendToConsole_(std::string _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
