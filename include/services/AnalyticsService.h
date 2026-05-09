#pragma once

class AnalyticsService {
public:
    double getTotalIncome(int userId);
    double getTotalExpense(int userId);
    double getBalance(int userId);
   double getCategoryExpense(int userId, const QString& category);
};