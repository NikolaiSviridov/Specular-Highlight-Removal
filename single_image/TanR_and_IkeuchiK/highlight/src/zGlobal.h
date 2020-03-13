// zGlobal.h contains general structure
// by robby

#if !defined __zGLOBAL_H__
#define __zGLOBAL_H__

#include <math.h>
#include <assert.h>
#include <vector>
#include <string>
#include <list>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>

#include "image.h"
#include "pnmfile.h"

using namespace std;


template<class T>
class zArray2D {
protected:
    T *p;
    int maxY, maxX;

public:
    T *zGetDataPointer(void) {
        assert(p != 0);
        return p;
    }

    zArray2D() {
        maxY = maxX = 0;
        p = 0;
    }

    zArray2D(int y, int x) {
        p = 0;
        maxX = maxY = 0;
        zAllocate(y, x);
    }

    zArray2D(zArray2D<T> &in) {
        zCopyIn(in);
    }

    ~zArray2D() {
        zDestroy();
    }

    int zAllocate(int inmaxY, int inmaxX) {
        zDestroy();
        assert(inmaxY > 0 && inmaxX > 0 && p == 0);
        maxY = inmaxY;
        maxX = inmaxX;
        p = new T[maxY * maxX];
        assert(p != 0);
        memset(p, 0, maxX * maxY * sizeof(T)); //set the contents to zero
        return 0;
    }

    T &operator()(int y, int x) {
        assert(y >= 0 && y < maxY && x >= 0 && x < maxX);
        assert(p != 0);
        return (*(p + maxX * y + x));
    }

    T *operator[](int y) {
        assert(y >= 0 && y < maxY && p != 0);
        return (p + maxX * y);
    }

    zArray2D<T> &operator=(const zArray2D<T> &a) {
        assert(a.zGetMaxY() > 0 && a.zGetMaxY() > 0 &&
               a.zGetDataPointer() != 0);
        zDestroy();
        assert(zAllocate(a.zGetMaxY(), a.zGetMaxX()) == 0);
        memcpy(p, a.zGetDataPointer(), maxX * maxY * sizeof(T));
        return *this;
    }

    T &zAt(int y, int x) {
        assert(y >= 0 && y < maxY && x >= 0 && x < maxX && p != 0);
        return (*(p + maxX * y + x));
    }

    int zGetMaxY() {
        assert(p != 0);
        return maxY;
    }

    int zGetMaxX() {
        assert(p != 0);
        return maxX;
    }

    int zCopyIn(zArray2D<T> &arr) {
        assert(arr.zGetMaxY() > 0 && arr.zGetMaxY() > 0 &&
               arr.zGetDataPointer() != 0);
        zDestroy();
        assert(zAllocate(arr.zGetMaxY(), arr.zGetMaxX()) == 0);
        memcpy(p, arr.zGetDataPointer(), maxX * maxY * sizeof(T));
        return 0;
    }

    int zCopyOut(zArray2D<T> &arr) {
        assert(p != 0);
        arr.zDestroy();
        assert(arr.zAllocate(maxY, maxX) == 0);
        memcpy(arr.zGetDataPointer(), p, maxX * maxY * sizeof(T));
        return 0;
    }

    int zDestroy(void) {
        maxY = maxX = 0;
        if (p != 0) {
            delete[] p;
            p = 0;
        }
        return 0;
    }

    bool zIsActive() {
        if (p == 0) return false;
        else return true;
    }
};


struct s_rgbi {
    float r, g, b, i;
    unsigned char flag;

    //s_RGB color;
    s_rgbi(float ir, float ig, float ib, float ii = 0, unsigned char iflag = 0) {
        r = ir;
        g = ig;
        b = ib;
        i = ii;
        flag = iflag;
    }

    s_rgbi() {
        r = g = b = i = 0;
        flag = 0;
    };

    bool operator==(const s_rgbi &a) {
        if (a.r == r && a.g == g && a.b == b && a.i == i)
            return true;
        return false;
    };

    s_rgbi &operator=(const s_rgbi &a) {
        r = a.r;
        g = a.g;
        b = a.b;
        i = a.i;
        flag = a.flag;
        return *this;
    };

    int zSetRGB(float ir, float ig, float ib, float ii = 0, unsigned char iflag = 0) {
        r = ir;
        g = ig;
        b = ib;
        i = ii;
        flag = iflag;
        return 0;
    };

    float zTotal() {
        return r + g + b;
    }

    float zMax() {
        float max = r;
        if (g > max) max = g;
        if (b > max) max = b;
        return max;
    }

    float zMin() {
        float min = r;
        if (g < min) min = g;
        if (b < min) min = b;
        return min;
    }

    float zMaxChroma() {
        if (zTotal() != 0) return zMax() / zTotal();
        else return 0;
    }

    float zChroma_r() {
        if (zTotal() != 0) return r / zTotal();
        else return 0;
    }

    float zChroma_g() {
        if (zTotal() != 0) return g / zTotal();
        else return 0;
    }

    float zChroma_b() {
        if (zTotal() != 0) return b / zTotal();
        else return 0;
    }
};

inline int zImage2Array(const image<rgb> *im, zArray2D<s_rgbi> &arr) {
    arr.zAllocate(im->height(), im->width());
    for (int y = 0; y < arr.zGetMaxY(); y++) {
        for (int x = 0; x < arr.zGetMaxX(); x++) {
            arr(y, x).r = im->access[y][x].r;
            arr(y, x).g = im->access[y][x].g;
            arr(y, x).b = im->access[y][x].b;
        }
    }
    return 0;
}

inline int zArray2Image(zArray2D<s_rgbi> &arr, image<rgb> *im) {
    for (int y = 0; y < arr.zGetMaxY(); y++) {
        for (int x = 0; x < arr.zGetMaxX(); x++) {
            im->access[y][x].r = (uchar) arr(y, x).r;
            im->access[y][x].g = (uchar) arr(y, x).g;
            im->access[y][x].b = (uchar) arr(y, x).b;
        }
    }
    return 0;
}

inline int zReadImage(string fname, zArray2D<s_rgbi> &img) {
    image<rgb> *inp = loadPPM(fname.data());
    zImage2Array(inp, img);
    return 0;
}

inline int zWriteImage(zArray2D<s_rgbi> &img, string fname) {
    image<rgb> out(img.zGetMaxX(), img.zGetMaxY());
    zArray2Image(img, &out);
    savePPM(&out, fname.data());

    return 0;
}

#endif
