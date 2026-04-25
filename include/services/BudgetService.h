#pragma once

#include "models/Budget.h"
#include <vector>

class BudgetService {
public:
    bool createBudget(const Budget& budget);
    std::vector<Budget> getUserBudgets(int userId);
    bool updateBudget(int id, double newLimit);
    bool deleteBudget(int id);
};