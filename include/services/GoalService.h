#pragma once
#include "models/SavingsGoal.h"
#include <vector>

class GoalService {
public:
    bool createGoal(const SavingsGoal& goal);
    std::vector<SavingsGoal> getGoalsForUser(int userId);
    bool deleteGoal(int goalId);
    bool updateGoalProgress(int goalId, double amount);
};