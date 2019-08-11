/*
 * 2017-01-13: Created by Abdalla Ahmed
 */

#ifndef SELF_SIMILAR_H
#define SELF_SIMILAR_H
#include <algorithm>
#include <fstream>
#include <cmath>
#include <vector>

namespace art
{

struct Point {
    double x, y;
};

typedef std::vector<Point> Points;
typedef std::vector<unsigned> TList;                                            // List of indexes

struct Matrix {                                                                // Postscript-style matrix, but only with scale and translate
    double scale, tx, ty;
    Matrix concat(const Matrix &transformation) const {
        Matrix result;
        result.scale = transformation.scale * scale;
        result.tx = transformation.tx * scale + tx;
        result.ty = transformation.ty * scale + ty;
        return result;
    };
};

inline Point transform(const Point &p, const Matrix &m) {
    double X = m.scale * p.x + m.tx;
    double Y = m.scale * p.y + m.ty;
    return {X, Y};
};

/********************** Global Variables *******/
int idCount, tileCount;
unsigned ONE;
TList idList;
int scale;
int nChildren;
unsigned mask;
unsigned shift;
const Matrix IDENTITY = {1, 0, 0};
Matrix *M;
/***********************************************/

struct Tile {
    Point p;
    TList children;                                                             // ids of sub-tiles
    TList order;
    std::fstream& read(std::fstream &file) {
        file >> p.x >> p.y;
        children.resize(nChildren);
        for (int i = 0; i < nChildren; i++) {
            file >> children[i];
        }
        order.resize(nChildren);
        for (int i = 0; i < nChildren; i++) {
            file >> order[i];
        }
        return file;
    };
};

std::vector<Tile> tiles;

void loadTileSet(std::fstream &file) {
    file >> scale >> idCount;
    file >> ONE;
    idList.resize(ONE);
    for (int i = 0; i < ONE; i++) file >> idList[i];
    nChildren = scale * scale;
    mask = nChildren - 1;
    shift = round(log2(nChildren));
    tileCount = idCount * idCount;
    tiles.resize(tileCount);
    for (int id = 0; id < tiles.size(); id++) {
        tiles[id].read(file);
        if (file.eof()) {
            fprintf(stderr, "Failed to load tiles\n");
            exit(1);
        }
    }
    // Populate the matrix:
    M = new Matrix[nChildren];
    double step = 1.0 / scale;
    for (int Y = 0; Y < scale; Y++) {
        for (int X = 0; X < scale; X++) {
            M[Y * scale + X] = {step, step * X, step * Y};
        }
    }
    fprintf(stderr, "Loaded %d tiles\n", (int)tiles.size());
}

}

#endif  // #define SELF_SIMILAR_H
