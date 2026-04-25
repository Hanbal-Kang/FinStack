#pragma once

#include <QString>

class ReportService {
public:
    double getTotalIncome(int userId);
    double getTotalExpense(int userId);
    double getBalance(int userId);
};