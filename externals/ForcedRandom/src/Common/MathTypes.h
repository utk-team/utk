#ifndef MATHTYPES_H
#define MATHTYPES_H

struct V2d {
    double x;
    double y;

    V2d() {
        x = 0.0;
        y = 0.0;
    }

    V2d(double _x) {
        x = _x;
        y = _x;
    }

    V2d(double _x, double _y) {
        x = _x;
        y = _y;
    }

    V2d operator*(double _x) const {
        return V2d(x * _x, y * _x);
    }

    V2d operator*(const V2d& v) const {
        return V2d(x * v.x, y * v.y);
    }

    V2d operator/(double _x) const {
        return V2d(x / _x, y / _x);
    }

    V2d operator+(const V2d& v) const {
        return V2d(x + v.x, y + v.y);
    }
};

struct V2i {
    int x;
    int y;

    V2i() {
        x = 0;
        y = 0;
    }

    V2i(int _x) {
        x = _x;
        y = _x;
    }

    V2i(int _x, int _y) {
        x = _x;
        y = _y;
    }

    V2i operator*(int _x) const {
        return V2i(x * _x, y * _x);
    }

    V2i operator*(const V2d& v) const {
        return V2i((int) ((double) x * v.x), (int) ((double) y * v.y));
    }

    V2i operator/(int _x) const {
        return V2i(x / _x, y / _x);
    }

    V2i operator+(const V2d& v) const {
        return V2i((int) ((double) x + v.x), (int) ((double) y + v.y));
    }
};

V2d operator*(double _x, const V2d& v);
V2d operator/(double _x, const V2d& v);
V2i operator*(int _x, const V2i& v);
V2i operator/(int _x, const V2i& v);
V2i ToV2i(const V2d& v);
V2d ToV2d(const V2i& v);

#endif