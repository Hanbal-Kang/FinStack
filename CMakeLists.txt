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

    src/ui/LoginWindow.cpp
    src/ui/RegisterWindow.cpp
    src/ui/DashboardWindow.cpp

    src/services/AuthService.cpp
    src/services/TransactionService.cpp

    src/models/User.cpp
    src/models/Transaction.cpp

    src/database/DatabaseManager.cpp

    src/utils/Validator.cpp
    src/utils/Formatter.cpp

    include/ui/LoginWindow.h
    include/ui/RegisterWindow.h
    include/ui/DashboardWindow.h

    include/services/AuthService.h
    include/services/TransactionService.h

    include/models/User.h
    include/models/Transaction.h

    include/database/DatabaseManager.h

    include/utils/Validator.h
    include/utils/Formatter.h

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
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FinStack
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FinStack)
endif()