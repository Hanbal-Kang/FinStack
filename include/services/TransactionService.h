#pragma once

#include "models/Transaction.h"
#include <vector>

class TransactionService {
public:
    bool addTransaction(const Transaction& t);
    std::vector<Transaction> getAllByUser(int userId);
    std::vector<Transaction> getByType(int userId, Transaction::Type type);
    bool deleteTransaction(int id);
};