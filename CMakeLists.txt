cmake_minimum_required(VERSION 3.16)
project(FinStack VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Charts)

# ── Include Directories ───────────────────────────
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ── All Source Files ──────────────────────────────
set(PROJECT_SOURCES
    src/main.cpp

    # App controller (navigation hub)
    src/AppController.cpp
    include/AppController.h

    # UI layer
    src/ui/BaseWindow.cpp
    src/ui/LoginWindow.cpp
    src/ui/RegisterWindow.cpp
    src/ui/DashboardWindow.cpp
    src/ui/TransactionWindow.cpp
    src/ui/AnalyticsWindow.cpp
    src/ui/BudgetWindow.cpp
    src/ui/SavingsGoalWindow.cpp
    src/ui/ProfileWindow.cpp

    # Services
    src/services/BaseService.cpp
    src/services/AuthService.cpp
    src/services/TransactionService.cpp
    src/services/BudgetService.cpp
    src/services/AnalyticsService.cpp
    src/services/ReportService.cpp

    # Models
    src/models/User.cpp
    src/models/Transaction.cpp
    src/models/Budget.cpp
    src/models/SavingsGoal.cpp
    src/models/RecurringTransaction.cpp

    # Database
    src/database/DatabaseManager.cpp

    # Utils
    src/utils/Validator.cpp
    src/utils/Formatter.cpp
    src/utils/InsightEngine.cpp

    # Headers
    include/ui/BaseWindow.h
    include/ui/LoginWindow.h
    include/ui/RegisterWindow.h
    include/ui/DashboardWindow.h
    include/ui/TransactionWindow.h
    include/ui/AnalyticsWindow.h
    include/ui/BudgetWindow.h
    include/ui/SavingsGoalWindow.h
    include/ui/ProfileWindow.h

    include/services/BaseService.h
    include/services/AuthService.h
    include/services/TransactionService.h
    include/services/BudgetService.h
    include/services/AnalyticsService.h
    include/services/ReportService.h

    include/models/User.h
    include/models/Transaction.h
    include/models/Budget.h
    include/models/SavingsGoal.h
    include/models/RecurringTransaction.h

    include/database/DatabaseManager.h

    include/utils/Validator.h
    include/utils/Formatter.h
    include/utils/InsightEngine.h

    resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FinStack
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(FinStack SHARED ${PROJECT_SOURCES})
    else()
        add_executable(FinStack ${PROJECT_SOURCES})
    endif()
endif()

# ── Link Libraries ────────────────────────────────
target_link_libraries(FinStack PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Charts
)

set_target_properties(FinStack PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(APPLE)
    set_target_properties(FinStack PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

include(GNUInstallDirs)
install(TARGETS FinStack
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FinStack)
endif()