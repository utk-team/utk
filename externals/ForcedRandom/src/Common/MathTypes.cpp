#include "MathTypes.h"

V2d operator*(double _x, const V2d& v) {
    return v * _x;
}

V2d operator/(double _x, const V2d& v) {
    return v / _x;
}

V2i operator*(int _x, const V2i& v) {
    return v * _x;
}

V2i operator/(int _x, const V2i& v) {
    return v / _x;
}

V2i ToV2i(const V2d& v) {
    return V2i((int) v.x, (int) v.y);
}

V2d ToV2d(const V2i& v) {
    return V2d((double) v.x, (double) v.y);
}