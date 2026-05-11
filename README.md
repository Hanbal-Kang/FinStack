# FinStack

A desktop personal Finance Manager for university students. Built in C++ with Qt 6.

Most finance apps are built for adults with salaries. They assume you have a steady income, recurring bills, investments, retirement goals. They are not built for someone living on a monthly stipend trying to figure out why their money runs out by the 20th.

FinStack is. You add what comes in, you add what goes out, and the app shows you where it went. That's it.

## Features

- **Dashboard** : live balance, monthly income/expense, recent activity
- **Transactions** : deposits and withdrawals with categories (Food, Transport, Rent, Tuition, Entertainment, Other) and search/filter history
- **Budgets** — monthly limits per category with progress bars
- **Savings goals** : set a target, lock money toward it, track progress
- **Recurring transactions** : automate stipends, rent, subscriptions; auto-fires on login
- **Analytics** : pie chart and category breakdown, switchable between week / month / year
- **Reports** : full export to CSV, opens in Excel/Numbers/Sheets
- **Auth** : registration, login, change password, forgot password with one-time recovery codes (offline-friendly, no email required)

All data persists in a local SQLite file. No internet. No telemetry.

## Stack

- C++17
- Qt 6 (Widgets, SQL, Charts)
- SQLite (via Qt SQL)
- CMake

## Architecture

Four layers, strictly separated:

```
UI (Qt Widgets)
   ↓
Services (business logic)
   ↓
Models (plain C++ data classes)
   ↓
Database (SQLite via Qt SQL)
```

UI never touches SQL. Models never touch UI. Services are the only thing that knows about both their domain logic and the database.

Every window inherits from a `BaseWindow` abstract class that provides shared geometry, stylesheet loading, and a two-step `setupUI()` / `applyStyles()` lifecycle. Services follow the same pattern with `BaseService`.

## Build

You'll need Qt 6.5+ with the **Charts** module. Then:

```bash
git clone https://github.com/Hanbal-Kang/FinStack.git
cd FinStack
mkdir build && cd build
cmake ..
cmake --build .
```

Or just open `CMakeLists.txt` in Qt Creator and hit run.

The database file gets created automatically on first launch:

- macOS: `~/Library/Application Support/FinStack/finstack.db`
- Windows: `%APPDATA%\FinStack\finstack.db`
- Linux: `~/.local/share/FinStack/finstack.db`

## Project structure

```
include/         Headers
  models/        User, Transaction, Budget, SavingsGoal, RecurringTransaction
  services/      Auth, Transaction, Budget, Goal, Recurring, Analytics, Report
  ui/            All window classes
  utils/         Validator, Formatter, InsightEngine
  database/      DatabaseManager (singleton, SQLite wrapper)

src/             Implementations, mirrors include/
resources/       Stylesheet (app.qss) and Qt resource file
```

## A note on the auth flow

Since this is an offline app, the usual "we'll email you a reset link" flow doesn't work. Instead, when you register, the app generates a one-time recovery code (`XXXX-XXXX-XXXX-XXXX`) and shows it to you once. Save it somewhere — it's the only way to reset your password if you forget it. The code rotates every time you use it.

This is the same pattern crypto wallets use. It's the most honest way to handle account recovery without a server.

Passwords and recovery codes are SHA-256 hashed before storage. The plaintext never touches the disk.

## Authors

Built as an OOP semester project by:

- **Hanbal Aziz Kang** 
- **Ahmad Nasir** 
- **Abdullah Hussain** 
- **Maryam Waseem**

## License

Educational project. Use it, fork it, learn from it.
