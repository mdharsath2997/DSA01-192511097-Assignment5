#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Exception hierarchy
class BillingError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ClaimError : public BillingError {
public:
    using BillingError::BillingError;
};

class NegativeChargeError : public BillingError {
public:
    using BillingError::BillingError;
};

// Abstract Bill
class Bill {
protected:
    std::string patient;
public:
    explicit Bill(const std::string& p) : patient(p) {
        if (p.empty()) throw BillingError("missing patient record");
    }
    virtual ~Bill() = default;
    virtual double total() const = 0;
    virtual std::string type() const = 0;
    const std::string& getPatient() const { return patient; }
    void print() const {
        std::cout << type() << " " << patient << ": " << total() << '\n';
    }
};

// Concrete Bills
class OutPatientBill : public Bill {
    double consultation, tests;
public:
    OutPatientBill(const std::string& p, double c, double t)
        : Bill(p), consultation(c), tests(t) {
        if (c < 0 || t < 0) throw NegativeChargeError("negative charges in OP bill");
    }
    double total() const override { return consultation + tests; }
    std::string type() const override { return "OP"; }
};

class InPatientBill : public Bill {
    int days;
    double perDay, procedures, medicines;
public:
    InPatientBill(const std::string& p, int d, double rate, double proc, double med)
        : Bill(p), days(d), perDay(rate), procedures(proc), medicines(med) {
        if (d < 0 || rate < 0 || proc < 0 || med < 0)
            throw NegativeChargeError("negative charges in IP bill");
    }
    double total() const override { return days * perDay + procedures + medicines; }
    std::string type() const override { return "IP"; }
};

class DayCareBill : public Bill {
    double packagePrice;
public:
    DayCareBill(const std::string& p, double price)
        : Bill(p), packagePrice(price) {
        if (price < 0) throw NegativeChargeError("negative package price");
    }
    double total() const override { return packagePrice; }
    std::string type() const override { return "DayCare"; }
};

class EmergencyBill : public Bill {
    double base, hourlyRate;
    int hours;
public:
    EmergencyBill(const std::string& p, double b, double rate, int h)
        : Bill(p), base(b), hourlyRate(rate), hours(h) {
        if (b < 0 || rate < 0 || h < 0)
            throw NegativeChargeError("negative charges in Emergency bill");
    }
    double total() const override { return base + hourlyRate * hours; }
    std::string type() const override { return "Emergency"; }
};

// Abstract Payer
class Payer {
public:
    virtual ~Payer() = default;
    virtual double amountCovered(const Bill& bill) const = 0;
    virtual std::string name() const = 0;
};

// Concrete Payers
class SelfPay : public Payer {
public:
    double amountCovered(const Bill& bill) const override {
        return bill.total(); // patient pays full
    }
    std::string name() const override { return "SelfPay"; }
};

class Insurance : public Payer {
    double coverLimit;
    double coPayPercent; // e.g. 0.2 means 20% co-pay
public:
    Insurance(double limit, double coPay) : coverLimit(limit), coPayPercent(coPay) {
        if (limit < 0 || coPay < 0 || coPay > 1)
            throw BillingError("invalid insurance parameters");
    }
    double amountCovered(const Bill& bill) const override {
        double total = bill.total();
        double covered = total * (1.0 - coPayPercent);
        if (covered > coverLimit) covered = coverLimit;
        if (covered < total && covered < coverLimit) {
            // still ok
        }
        return covered;
    }
    std::string name() const override { return "Insurance"; }
};

class GovernmentScheme : public Payer {
    // fixed package rates - for simplicity, cover a fixed amount or percentage
    double packageRate;
public:
    explicit GovernmentScheme(double rate) : packageRate(rate) {
        if (rate < 0) throw BillingError("invalid scheme rate");
    }
    double amountCovered(const Bill& bill) const override {
        // government covers up to packageRate
        return std::min(bill.total(), packageRate);
    }
    std::string name() const override { return "GovernmentScheme"; }
};

// New payer example for extension
class CorporateTieUp : public Payer {
    double discountPercent;
public:
    explicit CorporateTieUp(double disc) : discountPercent(disc) {
        if (disc < 0 || disc > 1) throw BillingError("invalid corporate discount");
    }
    double amountCovered(const Bill& bill) const override {
        return bill.total() * (1.0 - discountPercent);
    }
    std::string name() const override { return "CorporateTieUp"; }
};

int main() {
    std::vector<std::unique_ptr<Bill>> bills;
    bills.push_back(std::make_unique<OutPatientBill>("Ravi", 500, 1200));
    bills.push_back(std::make_unique<InPatientBill>("Meena", 3, 2500, 30000, 10000));
    bills.push_back(std::make_unique<DayCareBill>("Suresh", 15000));
    bills.push_back(std::make_unique<EmergencyBill>("Anjali", 2000, 500, 4));

    SelfPay self;
    Insurance ins(50000, 0.2);
    GovernmentScheme gov(20000);
    CorporateTieUp corp(0.15);

    std::vector<Payer*> payers = {&self, &ins, &gov, &corp};

    double dayTotal = 0;
    for (const auto& b : bills) {
        b->print();
        dayTotal += b->total();
        std::cout << "  Covered by:\n";
        for (Payer* p : payers) {
            try {
                double covered = p->amountCovered(*b);
                std::cout << "    " << p->name() << ": " << covered << '\n';
            } catch (const std::exception& e) {
                std::cout << "    " << p->name() << " error: " << e.what() << '\n';
            }
        }
    }
    std::cout << "Day Total: " << dayTotal << '\n';

    // Test error conditions
    std::cout << "\n--- Error Tests ---\n";
    try {
        OutPatientBill bad("Test", -100, 200);
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << '\n';
    }

    try {
        double claim = 60000;
        double cover = 50000;
        if (claim > cover) throw ClaimError("claim exceeds cover");
    } catch (const ClaimError& e) {
        std::cout << "ClaimError: " << e.what() << '\n';
    }

    return 0;
}