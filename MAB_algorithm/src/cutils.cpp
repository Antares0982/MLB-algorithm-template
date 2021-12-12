#ifndef CUTILS_CPP
#define CUTILS_CPP

#include "cutils.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace mabCutils {
    double catonialpha(const double &v, const int &itercount, const int &_size) {
        double lg4t = 4.0 * std::log(double(itercount));
        return std::sqrt(lg4t / (double(_size) * (v + v * lg4t / (double(_size) - lg4t))));
    }

    double psi(const double &x) {
        if (x < 0) return -psi(-x);
        if (x > 1) return std::log(2 * x - 1) / 4 + 5.0 / 6;
        return x - x * x * x / 6;
    }

    double dpsi(const double &x) {
        if (x < 0) return dpsi(-x);
        if (x > 1) return 1.0 / (4 * x - 2);
        return 1.0 - x * x / 2;
    }

    double sumpsi(const double &v, const int &itercount, const double &guess, mabarraycpp &arr) {
        double ans = 0.0;
        auto a_d = catonialpha(v, itercount, arr.size());
        for (int i = 0; i < arr.size(); ++i) ans += psi(a_d * (arr[i] - guess));
        return ans;
    }

    double dsumpsi(const double &v, const int &itercount, const double &guess, mabarraycpp &arr) {
        double ans = 0.0;
        auto a_d = catonialpha(v, itercount, arr.size());
        for (int i = 0; i < arr.size(); ++i) ans += dpsi(a_d * (arr[i] - guess));
        return -a_d * ans;
    }

    double nt_iter(const double &v, const int &itercount, const double &guess, mabarraycpp &arr, const double &fguess) {
        return guess - fguess / dsumpsi(v, itercount, guess, arr);
    }

    double findmedian(std::vector<double> &vec) {
        int mind = vec.size() / 2;
        std::nth_element(vec.begin(), vec.begin() + mind, vec.end());
        if (vec.size() & 1) return vec[mind];
        auto it = std::max_element(vec.begin(), vec.begin() + mind);
        return ((*it) + vec[mind]) / 2;
    }

    // mean sestimator
    std::pair<double, int> getcatoni(const double &v, const int &itercount, double &guess, mabarraycpp &arr, const double &tol) {
        auto a = sumpsi(v, itercount, guess, arr);
        int nt_itercount = 0;
        auto a_d = catonialpha(v, itercount, arr.size());
        auto realtol = tol * a_d * a_d;
        while ((a > realtol || a < -realtol) && nt_itercount < 50) {
            guess = nt_iter(v, itercount, guess, arr, a);
            a = sumpsi(v, itercount, guess, arr);
            ++nt_itercount;
        }
        return {guess, nt_itercount};
    }

    double truncatedMean(const double &u, const double &ve, const int &itercount, mabarraycpp &arr) {
        double ee = u / (2 * std::log(itercount));
        double vinv = 1 / (ve + 1);
        auto bd = [&](const double &x) {
            return std::pow(ee * x, vinv);
        };
        double ans = 0.0;
        for (int i = 0; i < arr.size();) {
            const double &v = arr[i];
            if (std::abs(v) <= bd(++i)) ans += v;
        }
        return ans / arr.size();
    }

    double meadianMean(const double &v, const double &ve, const int &itercount, mabarraycpp &arr) {
        double ee = v / (2 * std::log(itercount));
        double vinv = 1 / (ve + 1);
        auto bd = [&](const int &x) {
            return std::pow(ee * x, vinv);
        };
        int k = int(std::floor(std::min(1 + 16 * std::log(itercount), double(arr.size()) / 2)));
        if (k < 1) k = 1;
        int N = int(std::floor(double(arr.size()) / k));
        std::vector<double> tmp(k, 0.0);
        for (int i = 0; i < arr.size(); ++i) {
            int b = i / N;
            if (b == k) break;
            double &v = arr[i];
            if (std::abs(v) <= bd((i % N) + 1)) tmp[b] += v;
        }
        return findmedian(tmp) / N;
    }

    // distns utils
    double heavytail_pdf(const double &alpha, const double &beta, const double &coef, const double &maxMomentOrder, double x) {
        x = alpha * x + beta;
        if (x < 2) return 0.0;
        double log_sq = std::log(x);
        log_sq *= log_sq;
        return alpha * coef / (std::pow(x, maxMomentOrder + 1) * log_sq);
    }
} // namespace mabCutils

#endif // CUTILS_CPP
