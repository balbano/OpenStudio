#ifndef PRJDATA_H
#define PRJDATA_H

#include "PrjObjects.hpp"
#include "PrjReader.hpp"

namespace openstudio {
namespace contam {
namespace prj {

class Wall
{
public:
    enum Type{General, Horizontal, Vertical};

    Wall(Icon *start, Icon *end, Type type, Zone *left=NULL, Zone *right=NULL, Wall *parent=NULL)
        : start(start), end(end), type(type), left(left), right(right), parent(parent){}
    int length();
    void reorient();
    void getZones(int nx, int ny, int *grid, Zone *zones);
    void getZones(int nx, int ny, int *grid, QList<Zone*>zones);

    Icon *start;
    Icon *end;
    Type type;
    Zone *left;
    Zone *right;
    Wall *parent;
};

class Loop
{
public:
    //Loop(Zone *zone, Wall *wall, QList<Wall*> walls);
    Loop(Zone *zone, QList<Wall*> *onleft);
    ~Loop();
    bool rectangular();
    double area();

    Zone *zone;
    QList<Wall*> walls;
    bool valid;
};

class ZoneGeometry
{
public:
    //ZoneGeometry(Zone *zone):zone(zone){valid=false;}
    ZoneGeometry(Zone *zone, QList<Wall*> walls);
    ~ZoneGeometry();
    double area();

    Zone *zone;
    QList<Loop*> loops;
    bool valid;

private:
    void buildLoops(QList<Wall*> walls);
};

class Data
{
public:
    Data(QString filename, bool process=true);
    ~Data();
    QString summary();
    bool write(QString filename);

    bool valid;
    RunControl rc;
    QList<int> contaminants;
    QList<Species*> species;
    QList<Level*> levels;
    QString dsch;
    QString wsch;
    QString wpf;
    QString kinr;
    QString flte;
    QString filt;
    QString cse;
    QString afe;
    QString dfe;
    QString selmt;
    QString ctrl;
    QList<Ahs*> ahs;
    QList<Zone*> zones;
    QString zoneic;
    QList<Path*> paths;
    QString jct;
    QString jctic;
    QString dct;
    QString css;
    QString osch;
    QString pexp;
    QString note;

    QList<Wall*> walls;
    QList<ZoneGeometry*> geometry;
    QList<int*>grids;

};

void error(QString mesg, const char *file, int line);
QList<Wall*> findWalls(int w, int h, int *skpd, int nicons, Icon *icons);
void fillFromPoint(int value, int x, int y, int nx, int ny, int *map);
QList<Wall*> zoneWalls(Zone *zone, QList<Wall *> walls);

template <class T> QList<T*> readSection(Reader *input)
{
    QList<T*> list;
    T *object;
    int n = input->readInt();
    for(int i=0;i<n;i++)
    {
        object = new T();
        list << object;
        object->read(input);
    }
    input->read999();
    return list;
}

}
}
}

#endif // PRJDATA_H
