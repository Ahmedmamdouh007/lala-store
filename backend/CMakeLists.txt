cmake_minimum_required(VERSION 3.15)
project(LalaStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite3 (file-based DB, no server)
find_package(SQLite3 QUIET)
if(NOT SQLite3_FOUND)
    # Try pkg-config on Unix
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SQLite3 QUIET sqlite3)
        if(SQLite3_FOUND)
            set(SQLite3_LIBRARIES ${SQLite3_LIBRARIES})
            set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIRS})
        endif()
    endif()
endif()
if(NOT SQLite3_FOUND)
    # Manual paths for Windows (vcpkg, MSYS2 MinGW, or system)
    find_library(SQLite3_LIBRARY NAMES sqlite3 sqlite3.lib libsqlite3.a PATHS
        "C:/msys64/mingw64/lib" "C:/Program Files/sqlite" "C:/sqlite" "$ENV{VCPKG_ROOT}/installed/x64-windows/lib")
    find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h PATHS
        "C:/msys64/mingw64/include" "C:/Program Files/sqlite" "C:/sqlite" "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
    if(SQLite3_LIBRARY AND SQLite3_INCLUDE_DIR)
        set(SQLite3_FOUND TRUE)
        set(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
        set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
        message(STATUS "Using SQLite3 from: ${SQLite3_INCLUDE_DIR}")
    endif()
endif()
if(NOT SQLite3_FOUND)
    message(FATAL_ERROR "SQLite3 not found. Install sqlite3 development package (e.g. vcpkg install sqlite3, or system libsqlite3-dev).")
endif()

include_directories(${SQLite3_INCLUDE_DIRS})

# ASIO library (required by Crow)
if(EXISTS "${CMAKE_SOURCE_DIR}/asio/asio/include")
    include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/asio/include")
    include_directories(${CMAKE_SOURCE_DIR}/asio/include)
endif()

# Crow framework
if(EXISTS "${CMAKE_SOURCE_DIR}/crow/include")
    include_directories(${CMAKE_SOURCE_DIR}/crow/include)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/../crow/include")
    include_directories(${CMAKE_SOURCE_DIR}/../crow/include)
endif()

# nlohmann/json
if(EXISTS "${CMAKE_SOURCE_DIR}/include/nlohmann/json.hpp")
    include_directories(${CMAKE_SOURCE_DIR}/include)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/../include/nlohmann/json.hpp")
    include_directories(${CMAKE_SOURCE_DIR}/../include)
endif()

# Find CURL for Stripe
find_package(CURL QUIET)
if(NOT CURL_FOUND)
    find_library(CURL_LIBRARY NAMES curl curl.dylib PATHS /usr/lib /usr/local/lib /opt/homebrew/lib)
    find_path(CURL_INCLUDE_DIR NAMES curl/curl.h PATHS /usr/include /usr/local/include /opt/homebrew/include)
    if(CURL_LIBRARY)
        set(CURL_FOUND TRUE)
        set(CURL_LIBRARIES ${CURL_LIBRARY})
        if(CURL_INCLUDE_DIR)
            include_directories(${CURL_INCLUDE_DIR})
        endif()
    endif()
else()
    include_directories(${CURL_INCLUDE_DIRS})
endif()

# Source files
set(SOURCES
    main.cpp
    db/connection.cpp
    utils/stripe_client.cpp
    utils/vulnerable_helper.cpp
    routes/home_routes.cpp
    routes/product_routes.cpp
    routes/cart_routes.cpp
    routes/order_routes.cpp
    routes/stripe_routes.cpp
)

# Executable
add_executable(lala_store ${SOURCES})

# Link libraries
target_link_libraries(lala_store
    ${SQLite3_LIBRARIES}
    pthread
)
if(WIN32)
    # Windows sockets (required by ASIO/Crow on MinGW and MSVC)
    target_link_libraries(lala_store ws2_32 mswsock)
endif()
if(CURL_FOUND)
    target_link_libraries(lala_store ${CURL_LIBRARIES})
endif()

# Copy config files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/config/db_config.json
     DESTINATION ${CMAKE_BINARY_DIR}/config)
file(COPY ${CMAKE_SOURCE_DIR}/config/stripe_config.json
     DESTINATION ${CMAKE_BINARY_DIR}/config)
