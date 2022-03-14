#ifndef CONDITION_HPP
#define CONDITION_HPP

#define COND1

#include <valarray>

#define DT 1.0f / 256.0f
#define DIFFUSION 0
#define VISCOSITY 0
#ifdef COND1
#define WIDTH 500
#define HEIGHT 500
#elif defined COND3
#define WIDTH 200
#define HEIGHT 200
#endif

float D(double x, double y, double mx, double my, double s) {
    double dx = x - mx, dy = y - my;
    double r = std::sqrt(dx * dx + dy * dy);
    return static_cast<float> (std::exp(-r / s));
}

float H(int x, int y, int n) {
#ifdef COND1
    switch (n) {
        case 0: //velosity x
            return 250 * (2 * D(x, y, 30, 15, 3) + D(x, y, 30, -15, 3) + -D(x, y, 50, 0, 10));
        case 1: //velosity y
            return 500 * (-D(x, y, 30, 15, 3) + 2 * D(x, y, 30, -15, 3));
            //colors
        case 2:
            return D(x, y, 50, -10, 10) * 3;
        case 3:
            return D(x, y, 50, 10, 10) * 3;
        case 4:
            return D(x, y, 67.32, 0, 10) * 3;
    }
#elif defined COND3
    switch (n) {
        case 0: //velosity x
            return 250 * (2 * D(x, y, 30, 15, 3) + D(x, y, 30, -15, 3) + -D(x, y, 50, 0, 10));
        case 1: //velosity y
            return 250 * (-D(x, y, 30, 15, 3) + 2 * D(x, y, 30, -15, 3));
            //colors
        case 2:
            return D(x, y, 50, -10, 10) * 1;
        case 3:
            return D(x, y, 50, 10, 10) * 1;
        case 4:
            return D(x, y, 67.32, 0, 10) * 1;
    }
#endif
    return 0;
}

float* getF(float *out) {
    for (int i = 0; i < WIDTH; i++) {
        for (int t = 0; t < HEIGHT; t++) {
            int x = i - WIDTH / 2;
            int y = t - HEIGHT / 2;
            out[(t * WIDTH + i) * 2] = H(x, y, 0);
            out[(t * WIDTH + i) * 2 + 1] = H(x, y, 1);
        }
    }
    return out;
}

float* getS(float *out, int n) {
    for (int i = 0; i < WIDTH; i++) {
        for (int t = 0; t < HEIGHT; t++) {
            int x = i - WIDTH / 2;
            int y = t - HEIGHT / 2;
            out[(t * WIDTH + i) * 2] = H(x, y, n * 2 + 2);
            out[(t * WIDTH + i) * 2 + 1] = H(x, y, n * 2 + 3);
        }
    }
    return out;
}

#endif /* CONDITION_HPP */

