/****************************************************************************
** Meta object code from reading C++ file 'MedSquare.h'
**
** Created: Mon Oct 1 13:38:18 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MedSquare.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MedSquare.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MedSquare[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      46,   11,   10,   10, 0x0a,
     102,   94,   10,   10, 0x0a,
     127,  118,   10,   10, 0x0a,
     148,   10,   10,   10, 0x08,
     158,   10,   10,   10, 0x08,
     169,   10,   10,   10, 0x08,
     180,   10,   10,   10, 0x08,
     191,   10,   10,   10, 0x08,
     204,   10,   10,   10, 0x08,
     218,   10,   10,   10, 0x08,
     233,   10,   10,   10, 0x08,
     245,   10,   10,   10, 0x08,
     296,  263,   10,   10, 0x08,
     341,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MedSquare[] = {
    "MedSquare\0\0caller,eventId,clientData,callData\0"
    "updateProgressBar(vtkObject*,ulong,void*,void*)\0"
    "iClosed\0imageClose(int)\0iCurrent\0"
    "setCurrentImage(int)\0fileNew()\0"
    "fileOpen()\0fileSave()\0fileExit()\0"
    "viewZoomIn()\0viewZoomOut()\0viewResetAll()\0"
    "helpAbout()\0fileImportDicom()\0"
    "selected,seriesName,sliceSpacing\0"
    "importDicomFiles(QStringList,QString,double)\0"
    "useOrthogonalViewer()\0"
};

const QMetaObject MedSquare::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MedSquare,
      qt_meta_data_MedSquare, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MedSquare::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MedSquare::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MedSquare::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MedSquare))
        return static_cast<void*>(const_cast< MedSquare*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MedSquare::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateProgressBar((*reinterpret_cast< vtkObject*(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2])),(*reinterpret_cast< void*(*)>(_a[3])),(*reinterpret_cast< void*(*)>(_a[4]))); break;
        case 1: fileClose((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setCurrentImage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: fileNew(); break;
        case 4: fileOpen(); break;
        case 5: fileSave(); break;
        case 6: fileExit(); break;
        case 7: viewZoomIn(); break;
        case 8: viewZoomOut(); break;
        case 9: viewResetAll(); break;
        case 10: helpAbout(); break;
        case 11: fileImportDicom(); break;
        case 12: importDicomFiles((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const double(*)>(_a[3]))); break;
        case 13: useOrthogonalViewer(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
