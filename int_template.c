#include <stdio.h>
#include <math.h>

#define RECURS_LEVEL_MAX  10
#define N_MAX             10

// pointer to function of one variable
typedef double (*Func1vFp)(double);

// example functions of one variable
double f_poly(double x) { // polynomial a[0] + a[1]x + ... + a[n]x^n
    return 2 * pow(x, 5) - 4 * pow(x, 4) + 3.5 * pow(x, 2) + 1.35 * x - 6.25;
}

double f_rat(double x) {
    return 1 / (pow(x - 0.5, 2) + 0.01);
}

double f_exp(double x) {
    return 2 * x * exp(-1.5 * x) - 1;
}

double f_trig(double x) {
    return x * tan(x);
}

// Quadratures

// rectangle rule, leftpoint
double quad_rect_left(Func1vFp f, double a, double b, int n) {
    double sum = 0;
    double dx = (b - a) / n;
    for (int i = 0; i < n; ++i) sum += f(a + i * dx) * dx;

    return sum;
}

// rectangle rule, rightpoint
double quad_rect_right(Func1vFp f, double a, double b, int n) {
    double sum = 0;
    double dx = (b - a) / n;
    for (int i = 0; i < n; ++i) sum += f(a + (i + 1) * dx) * dx;
    return sum;
}

// rectangle rule, midpoint
double quad_rect_mid(Func1vFp f, double a, double b, int n) {
    double sum = 0;
    double dx = (b - a) / n;
    for (int i = 0; i < n; i++) sum += f((2 * (a + i * dx) + dx) / 2) * dx;
    return sum;
}

// trapezoidal rule
double quad_trap(Func1vFp f, double a, double b, int n) {
    double sum = 0;
    double dx = (b - a) / n;
    for (int i = 0; i < n; i++) sum += (f(a + i * dx) + f(a + (i + 1) * dx)) / 2 * dx;
    return sum;
}

// Simpson's rule
double quad_simpson(Func1vFp f, double a, double b, int n) {
    double sum = 0;
    double dx = (b - a) / n;
    for (int i = 0; i < n; i++)
        sum += (f(a + i * dx) + 4 * f((2 * (a + i * dx) + dx) / 2) + f(a + (i + 1) * dx)) / 6 * dx;
    return sum;
}

// pointer to quadrature function
typedef double (*QuadratureFp)(Func1vFp, double, double, int);

// array of pointers to integrand functions
Func1vFp func_tab[] = {f_poly, f_rat, f_trig, f_exp};

// array of pointers to quadrature functions
QuadratureFp quad_tab[] = {
        quad_rect_left, quad_rect_right, quad_rect_mid, quad_trap, quad_simpson};

// calls 'quad_no' quadrature function for 'fun_no' integrand function
// on interval [a, b] and n subintervals
double quad_select(int fun_no, int quad_no, double a, double b, int n) {
    return quad_tab[quad_no](func_tab[fun_no], a, b, n);
}

// adaptive algorithm
double recurs(Func1vFp f, double a, double b, double S, double delta, QuadratureFp quad, int level) {
    if (level > RECURS_LEVEL_MAX) return NAN;

    double c = (a + b) / 2;

    double S1 = quad(f, a, c, 1);
    double S2 = quad(f, c, b, 1);
    double actual = S1 + S2;

    if (fabs(actual - S) < delta) return actual;
    return recurs(f, a, c, S1, delta / 2, quad, level + 1) + recurs(f, c, b, S2, delta / 2, quad, level + 1);

}

// initialization for adaptive algorithm
double init_recurs(Func1vFp f, double a, double b, double delta, QuadratureFp quad) {
    double S = quad(f, a, b, 1);

    return recurs(f, a, b, S, delta / 2, quad, 1);
}

// double integrals

// pointer to function of two variables
typedef double Func2vFp(double, double);

double func2v_2(double x, double y) {
    return 2 - x * x - y * y * y;
}

// sample functions to define the normal domain

double lower_bound2(double x) {
    return 0.7 * exp(-2 * x * x);
}

double upper_bound2(double x) {
    return sin(10 * x);
}

// rectangle rule (leftpoint) - double integral over rectangular domain
double dbl_integr(Func2vFp f, double x1, double x2, int nx, double y1, double y2, int ny) {
    double sum = 0;
    double dx = (x2 - x1) / nx;
    double dy = (y2 - y1) / ny;
    for (int i = 0; i < nx; ++i) for (int j = 0; j < ny; ++j) sum += f(x1 + i * dx, y1 + j * dy) * dx * dy;
    return sum;
}


// rectangle rule (midpoint) - double integral over normal domain with respect to the x-axis
double dbl_integr_normal_1(Func2vFp f, double x1, double x2, int nx, double hy,
                           Func1vFp fg, Func1vFp fh) {
    double sum = 0;
    double dx = (x2 - x1) / nx;

    for (int i = 0; i < nx; ++i) {
        double xi = x1 + dx / 2 + i * dx;

        double y1 = fg(xi);
        double y2 = fh(xi);
        int ny = ceil((y2 - y1) / hy);
        double dy = (y2 - y1) / ny;

        for (int j = 0; j < ny; ++j) {
            double yi = y1 + dy / 2 + j * dy;
            sum += f(xi, yi) * dx * dy;
        }
    }
    return sum;

}

double min(double a, double b) {
    if (a >= b)return b;
    else return a;
}

double max(double a, double b) {
    if (a <= b)return b;
    else return a;
}

// rectangle rule (leftpoint) - double integral over multiple normal
// domains with respect to the x-axis
double dbl_integr_normal_n(Func2vFp f, double x1, double x2, int nx, double y1, double y2,
                           int ny, Func1vFp fg, Func1vFp fh) {
    double sum = 0;
    double dx = (x2 - x1) / nx;
    double hy = (y2 - y1) / ny;

    double xi = x1;
    for (int i = 0; i < nx; ++i) {

        double yi1 = max(y1, fg(xi));
        double yi2 = min(y2, fh(xi));

        int nj = ceil((yi2 - yi1) / hy);
        double dy = (yi2 - yi1) / nj;

        double yi = yi1;
        for (int j = 0; j < nj; ++j) {
            sum += f(xi, yi) * dx * dy;
            yi += dy;
        }
        xi += dx;
    }
    return sum;
}

// multiple quadratures

typedef double (*FuncNvFp)(const double *, int);

typedef int (*BoundNvFp)(const double *, int);

// sample function of three variables
double func3v(const double v[], int n) {
    return v[0] - v[1] + 2 * v[2];
}

// sample predicate in 3D
int bound3v(const double v[], int n) { // a cylinder
    return v[0] > 0 && v[0] < 0.5 && v[1] * v[1] + (v[2] - 1) * (v[2] - 1) < 1;
}

// sample function of n variables
double funcNv(const double v[], int n) {
    double fv = 1.;
    for (int i = 1; i < n; ++i) {
        fv += sin(i * v[i]);
    }
    return fv;
}

// sample n-dimensional predicate (n-dim hypersphere)
int boundNv(const double v[], int n) {
    double r = 0.0;
    for (int i = 0; i < n; ++i) r += (v[i] - 1) * (v[i] - 1);
    return r <= 1.;
}

// multiple integrals over a cuboid with predicate (if boundary != NULL)
// rectangular rule (rightpoint)
double trpl_quad_rect(FuncNvFp f, double variable_lim[][2], const int tn[], BoundNvFp boundary) {
    double sum = 0;

    int nx = tn[0];
    double x2 = variable_lim[0][1];
    double x1 = variable_lim[0][0];
    double dx = (x2 - x1) / nx;

    int ny = tn[1];
    double y2 = variable_lim[1][1];
    double y1 = variable_lim[1][0];
    double dy = (y2 - y1) / ny;

    int nz = tn[2];
    double z2 = variable_lim[2][1];
    double z1 = variable_lim[2][0];
    double dz = (z2 - z1) / nz;

    double v[3];

    v[0] = x1 + dx;
    for (int i = 0; i < nx; i++) {
        v[1] = y1 + dy;
        for (int j = 0; j < ny; j++) {
            v[2] = z1 + dz;
            for (int k = 0; k < nz; k++) {
                if (boundary == NULL || boundary(v, 3)) sum += f(v, 3) * dx * dy * dz;
                v[2] += dz;
            }
            v[1] += dy;
        }
        v[0] += dx;
    }
    return sum;
}
/*
7
4
0 1 10
0 1 10
0 1 10
0 1 10
1
*/


// multiple integrals over a n-dim hypercuboid with predicate (if boundary != NULL)
// rectangular rule (midpoint)
void recur_quad_rect_mid(double *p_sum, FuncNvFp f, int variable_no, double t_variable[],
                         double variable_lim[][2], const int tn[], int level, BoundNvFp boundary, double t_d[]) {

    int n = tn[level];
    double lim1 = variable_lim[level][0];
    double lim2 = variable_lim[level][1];
    t_d[level] = (lim2 - lim1) / n;
    double d = t_d[level];

    t_variable[level] = lim1 + d / 2;


    if (variable_no == 0) {
        for (int i = 0; i < n; i++) {
            if (boundary == NULL || boundary(t_variable, level + 1)) {
                double res = f(t_variable, level + 1);
                for (int j = 0; j < level + 1; ++j) {
                    res *= t_d[j];
                }
                *p_sum += res;
            }
            t_variable[level] += d;
        }

    } else {
        for (int i = 0; i < n; i++) {
            recur_quad_rect_mid(p_sum, f, variable_no - 1, t_variable, variable_lim, tn, level + 1, boundary, t_d);
            t_variable[level] += d;
        }
    }
}

int main(void) {
    int to_do, n, nx, ny, integrand_fun_no, method_no, flag;
    int no_funcs = sizeof(func_tab) / sizeof(Func1vFp);
    int no_quads = sizeof(quad_tab) / sizeof(QuadratureFp);
    double a, b, x1, x2, y1, y2, hy, sum, delta;
    double t_variable[N_MAX], variable_lim[N_MAX][2], t_d[N_MAX];
    int tn[N_MAX];

    scanf("%d", &to_do);
    switch (to_do) {
        case 1: // loop over quadratures and integrands
            scanf("%lf %lf %d", &a, &b, &n);
            for (int q = 0; q < no_quads; ++q) {
                for (int f = 0; f < no_funcs; ++f) {
                    printf("%.5f ", quad_select(f, q, a, b, n));
                }
                printf("\n");
            }
            break;
        case 2: // adaptive algorithm
            scanf("%d %d", &integrand_fun_no, &method_no);
            scanf("%lf %lf %lf", &a, &b, &delta);
            printf("%.5f\n", init_recurs(func_tab[integrand_fun_no], a, b, delta, quad_tab[method_no]));
            break;
        case 3: // double integral over a rectangle
            scanf("%lf %lf %d", &x1, &x2, &nx);
            scanf("%lf %lf %d", &y1, &y2, &ny);
            printf("%.5f\n", dbl_integr(func2v_2, x1, x2, nx, y1, y2, ny));
            break;
        case 4: // double integral over normal domain
            scanf("%lf %lf %d", &x1, &x2, &nx);
            scanf("%lf", &hy);
            printf("%.5f\n", dbl_integr_normal_1(func2v_2, x1, x2, nx, hy, lower_bound2, upper_bound2));
            break;
        case 5: // double integral over multiple normal domains
            scanf("%lf %lf %d", &x1, &x2, &nx);
            scanf("%lf %lf %d", &y1, &y2, &ny);
            printf("%.5f\n", dbl_integr_normal_n(func2v_2, x1, x2, nx, y1, y2, ny, lower_bound2, upper_bound2));
            break;
        case 6: // triple integral over a cuboid
            scanf("%lf %lf %d", &variable_lim[0][0], &variable_lim[0][1], tn);
            scanf("%lf %lf %d", &variable_lim[1][0], &variable_lim[1][1], tn + 1);
            scanf("%lf %lf %d", &variable_lim[2][0], &variable_lim[2][1], tn + 2);
            scanf("%d", &flag);
            printf("%.5f\n", trpl_quad_rect(func3v, variable_lim, tn, flag ? bound3v : NULL));
            break;
        case 7: // multiple integral over hyper-cuboid
            scanf("%d", &n);
            if (n > N_MAX) break;
            for (int i = 0; i < n; ++i) {
                scanf("%lf %lf %d", &variable_lim[i][0], &variable_lim[i][1], tn + i);
            }
            scanf("%d", &flag);
            sum = 0.;
            recur_quad_rect_mid(&sum, funcNv, n - 1, t_variable, variable_lim, tn, 0, flag ? boundNv : NULL, t_d);
            printf("%.5f\n", sum);
            break;
        default:
            printf("Nothing to do for %d\n", to_do);
            break;
    }
    return 0;
}

