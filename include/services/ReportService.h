#pragma once

#include <QString>

class ReportService {
public:
    double getTotalIncome(int userId);
    double getTotalExpense(int userId);
    double getBalance(int userId);

    // Writes all of the user's transactions to a CSV file at `filePath`.
    // Returns true on success.
    bool exportTransactionsCSV(int userId, const QString& filePath);
};