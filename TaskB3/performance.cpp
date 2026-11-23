#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <stdexcept>
#include <optional>

// Virtual function approach
struct VBill {
    virtual ~VBill() = default;
    virtual double total() const = 0;
};

struct VOP : VBill {
    double c, t;
    VOP(double c, double t) : c(c), t(t) {}
    double total() const override { return c + t; }
};

struct VIP : VBill {
    int d; double r, s;
    VIP(int d, double r, double s) : d(d), r(r), s(s) {}
    double total() const override { return d * r + s; }
};

// Switch approach
enum class BillType { OP, IP };
struct SBill {
    BillType type;
    double a, b, c; // flexible fields
    double total() const {
        switch (type) {
            case BillType::OP: return a + b;
            case BillType::IP: return a * b + c;
        }
        return 0;
    }
};

// Return code vs exception for validation
enum class Status { OK, NEGATIVE, TOO_LARGE };

Status validate_rc(double amount, double& result) {
    if (amount < 0) return Status::NEGATIVE;
    if (amount > 1e6) return Status::TOO_LARGE;
    result = amount;
    return Status::OK;
}

void validate_ex(double amount) {
    if (amount < 0) throw std::invalid_argument("negative");
    if (amount > 1e6) throw std::out_of_range("too large");
}

int main() {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(100, 5000);

    // Correctness check with 10 bills
    std::vector<VBill*> v10;
    std::vector<SBill> s10;
    for (int i = 0; i < 5; ++i) {
        double c = dist(rng), t = dist(rng);
        v10.push_back(new VOP(c, t));
        s10.push_back({BillType::OP, c, t, 0});
    }
    for (int i = 0; i < 5; ++i) {
        int d = 1 + (rng() % 5);
        double r = dist(rng), s = dist(rng)*10;
        v10.push_back(new VIP(d, r, s));
        s10.push_back({BillType::IP, (double)d, r, s});
    }

    double sumV = 0, sumS = 0;
    for (auto* b : v10) sumV += b->total();
    for (auto& b : s10) sumS += b.total();
    std::cout << "Correctness (10 bills): Virtual sum=" << sumV << " Switch sum=" << sumS
              << " Match=" << (sumV == sumS ? "YES" : "NO") << "\n";

    for (auto* b : v10) delete b;

    // Timing for virtual vs switch
    auto time_virtual = [](size_t n) {
        std::vector<VBill*> bills;
        bills.reserve(n);
        std::mt19937 r(123);
        std::uniform_real_distribution<double> d(100, 5000);
        for (size_t i = 0; i < n; ++i) {
            if (i % 2 == 0)
                bills.push_back(new VOP(d(r), d(r)));
            else
                bills.push_back(new VIP(1 + r()%5, d(r), d(r)*10));
        }
        auto start = std::chrono::high_resolution_clock::now();
        double sum = 0;
        for (auto* b : bills) sum += b->total();
        auto end = std::chrono::high_resolution_clock::now();
        for (auto* b : bills) delete b;
        return std::chrono::duration<double, std::milli>(end - start).count();
    };

    auto time_switch = [](size_t n) {
        std::vector<SBill> bills;
        bills.reserve(n);
        std::mt19937 r(123);
        std::uniform_real_distribution<double> d(100, 5000);
        for (size_t i = 0; i < n; ++i) {
            if (i % 2 == 0)
                bills.push_back({BillType::OP, d(r), d(r), 0});
            else
                bills.push_back({BillType::IP, (double)(1 + r()%5), d(r), d(r)*10});
        }
        auto start = std::chrono::high_resolution_clock::now();
        double sum = 0;
        for (auto& b : bills) sum += b.total();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    };

    for (size_t n : {1000ull, 1000000ull, 10000000ull}) {
        double tv = time_virtual(n);
        double ts = time_switch(n);
        std::cout << "n=" << n << " Virtual: " << tv << " ms  Switch: " << ts << " ms\n";
    }

    // Exception vs return code
    auto time_rc = [](size_t n, double invalid_ratio) {
        std::mt19937 r(99);
        std::uniform_real_distribution<double> d(0, 1);
        auto start = std::chrono::high_resolution_clock::now();
        size_t ok = 0, err = 0;
        for (size_t i = 0; i < n; ++i) {
            double amt = (d(r) < invalid_ratio) ? -10.0 : 500.0;
            double res;
            Status s = validate_rc(amt, res);
            if (s == Status::OK) ++ok; else ++err;
        }
        auto end = std::chrono::high_resolution_clock::now();
        return std::make_pair(std::chrono::duration<double, std::milli>(end - start).count(), err);
    };

    auto time_ex = [](size_t n, double invalid_ratio) {
        std::mt19937 r(99);
        std::uniform_real_distribution<double> d(0, 1);
        auto start = std::chrono::high_resolution_clock::now();
        size_t ok = 0, err = 0;
        for (size_t i = 0; i < n; ++i) {
            double amt = (d(r) < invalid_ratio) ? -10.0 : 500.0;
            try {
                validate_ex(amt);
                ++ok;
            } catch (...) {
                ++err;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        return std::make_pair(std::chrono::duration<double, std::milli>(end - start).count(), err);
    };

    std::cout << "\nException vs Return Code (1e6 validations):\n";
    for (double ratio : {0.0, 0.01, 0.5}) {
        auto [trc, erc] = time_rc(1000000, ratio);
        auto [tex, eex] = time_ex(1000000, ratio);
        std::cout << "Invalid ratio " << (ratio*100) << "%  RC: " << trc << " ms (err=" << erc
                  << ")  EX: " << tex << " ms (err=" << eex << ")\n";
    }

    return 0;
}