/****************************************************************************
** Meta object code from reading C++ file 'TestMainWindow.hpp'
**
** Created: Thu Oct 21 16:48:09 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../TestMainWindow.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestMainWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TestMainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      26,   15,   15,   15, 0x08,
      38,   15,   33,   15, 0x08,
      45,   15,   33,   15, 0x08,
      54,   15,   15,   15, 0x08,
      62,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TestMainWindow[] = {
    "TestMainWindow\0\0newFile()\0open()\0bool\0"
    "save()\0saveAs()\0about()\0documentWasModified()\0"
};

const QMetaObject TestMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_TestMainWindow,
      qt_meta_data_TestMainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TestMainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TestMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TestMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestMainWindow))
        return static_cast<void*>(const_cast< TestMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int TestMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newFile(); break;
        case 1: open(); break;
        case 2: { bool _r = save();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = saveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: about(); break;
        case 5: documentWasModified(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
