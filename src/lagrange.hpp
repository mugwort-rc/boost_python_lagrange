#include <cassert>
#include <cmath>
#include <vector>

#include <boost/multiprecision/cpp_dec_float.hpp>


// https://stackoverflow.com/a/24519913
template <typename T>
std::vector<T> full_convolve(const std::vector<T> &f, const std::vector<T> &g) {
    const int nf = f.size();
    const int ng = g.size();
    const int n = nf + ng - 1;
    std::vector<T> ret(n, T());
    for (int i = 0; i < n; ++i) {
        const int jmin = (i >= ng - 1)? i - (ng - 1) : 0;
        const int jmax = (i <  nf - 1)? i            : nf - 1;
        for(int j = jmin; j <= jmax; ++j) {
            ret[i] += (f[j] * g[i - j]);
        }
    }
    return ret;
}


template <typename T>
class Lagrange {
public:
    Lagrange(const std::vector<T> &x, const std::vector<T> &w) :
        coefficients()
    {
        assert(x.size() == w.size());
        std::vector<T> p;
        p.push_back(0.0);
        for (std::size_t j = 0; j < x.size(); ++j) {
            std::vector<T> pt;
            pt.push_back(w[j]);
            for (std::size_t k = 0; k < x.size(); ++k) {
                if (k == j) {
                    continue;
                }
                T fac = x[j] - x[k];
                std::vector<T> poly;
                poly.push_back(1.0 / fac);
                poly.push_back(-x[k] / fac);
                pt = full_convolve(pt, poly);
            }
            p = add(p, pt);
        }
        coefficients = p;
    }

    T operator ()(const T &x) const {
        T result = 0.0;
        int d = coefficients.size() - 1;
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            //result += std::pow(x, d) * coefficients[i];
            result += boost::multiprecision::pow(x, d) * coefficients[i];
            --d;
        }
        return result;
    }

    std::vector<T> add(const std::vector<T> &a, const std::vector<T> &b) const {
        int diff = a.size() - b.size();
        if ( diff == 0 ) {
            std::vector<T> result;
            for (std::size_t i = 0; i < a.size(); ++i) {
                result.push_back(a[i] + b[i]);
            }
            return result;
        } else if ( diff > 0 ) {
            std::vector<T> new_b(diff, 0);
            new_b.reserve(diff + b.size());
            new_b.insert(new_b.end(), b.begin(), b.end());
            return add(a, new_b);
        } else {
            std::vector<T> new_a(-diff, 0);
            new_a.reserve(-diff + a.size());
            new_a.insert(new_a.end(), a.begin(), a.end());
            return add(new_a, b);
        }
    }

    std::vector<T> coefficients;


    static T fast(T x, const std::vector<T> &xs, const std::vector<T> &ws) {
        assert(xs.size() == ws.size());
        T result = 0.0;
        for (std::size_t j = 0; j < xs.size(); ++j) {
            T n = 1.0, d = 1.0;
            for (std::size_t k = 0; k < xs.size(); ++k) {
                if ( j == k ) {
                    continue;
                }
                n *= x     - xs[k];
                d *= xs[j] - xs[k];
            }
            result += ws[j] * (n / d);
        }
        return result;
    }

};

