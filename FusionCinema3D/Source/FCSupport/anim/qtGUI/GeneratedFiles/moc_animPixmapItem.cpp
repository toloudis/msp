/****************************************************************************
** Meta object code from reading C++ file 'animPixmapItem.hpp'
**
** Created: Sun Sep 26 19:03:16 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../animPixmapItem.hpp"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'animPixmapItem.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_animPixmapItem[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       2,   64, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      26,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,   15,   15,   15, 0x08,
      57,   15,   15,   15, 0x08,
      70,   15,   15,   15, 0x08,
      90,   15,   15,   15, 0x08,
     109,   15,   15,   15, 0x08,
     126,   15,   15,   15, 0x08,
     156,  145,   15,   15, 0x08,
     174,   15,   15,   15, 0x08,

 // properties: name, type, flags
     198,  192, (QMetaType::QReal << 24) | 0x00095003,
     207,  192, (QMetaType::QReal << 24) | 0x00095003,

       0        // eod
};

static const char qt_meta_stringdata_animPixmapItem[] = {
    "animPixmapItem\0\0rotated()\0rotationFinish()\0"
    "anim_rotate()\0anim_scale()\0"
    "clean_rotate_anim()\0clean_scale_anim()\0"
    "StartAnimScale()\0StartAnimUnScale()\0"
    "i_NewState\0ProcessCheck(int)\0"
    "clean_highlight()\0qreal\0rotation\0scale\0"
};

const QMetaObject animPixmapItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_animPixmapItem,
      qt_meta_data_animPixmapItem, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &animPixmapItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *animPixmapItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *animPixmapItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_animPixmapItem))
        return static_cast<void*>(const_cast< animPixmapItem*>(this));
    if (!strcmp(_clname, "QGraphicsPixmapItem"))
        return static_cast< QGraphicsPixmapItem*>(const_cast< animPixmapItem*>(this));
    return QObject::qt_metacast(_clname);
}

int animPixmapItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rotated(); break;
        case 1: rotationFinish(); break;
        case 2: anim_rotate(); break;
        case 3: anim_scale(); break;
        case 4: clean_rotate_anim(); break;
        case 5: clean_scale_anim(); break;
        case 6: StartAnimScale(); break;
        case 7: StartAnimUnScale(); break;
        case 8: ProcessCheck((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: clean_highlight(); break;
        default: ;
        }
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qreal*>(_v) = getRotate(); break;
        case 1: *reinterpret_cast< qreal*>(_v) = GetScale(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setRotate(*reinterpret_cast< qreal*>(_v)); break;
        case 1: SetScale(*reinterpret_cast< qreal*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void animPixmapItem::rotated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void animPixmapItem::rotationFinish()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
