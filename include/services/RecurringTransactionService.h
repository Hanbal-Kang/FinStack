#pragma once
#include "models/RecurringTransaction.h"
#include <vector>

class RecurringTransactionService {
public:
    bool createRecurring(const RecurringTransaction& r);
    std::vector<RecurringTransaction> getAllByUser(int userId);
    bool setActive(int recurringId, bool active);   // pause/resume
    bool deleteRecurring(int recurringId);


    // Returns the number of transactions actually created.
    int processDueRecurrences(int userId);
};