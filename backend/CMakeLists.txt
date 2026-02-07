cmake_minimum_required(VERSION 3.15)
project(LalaStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find PostgreSQL
# Try to find PostgreSQL, if not found, set paths manually
find_package(PostgreSQL QUIET)

if(NOT PostgreSQL_FOUND)
    # Manual PostgreSQL paths for macOS Homebrew
    set(PostgreSQL_ROOT "/usr/local/opt/postgresql@15")
    if(EXISTS "${PostgreSQL_ROOT}")
        set(PostgreSQL_INCLUDE_DIR "${PostgreSQL_ROOT}/include")
        set(PostgreSQL_LIBRARY "${PostgreSQL_ROOT}/lib/libpq.dylib")
        message(STATUS "Using PostgreSQL from: ${PostgreSQL_ROOT}")
    else()
        # Try postgresql@14
        set(PostgreSQL_ROOT "/usr/local/opt/postgresql@14")
        if(EXISTS "${PostgreSQL_ROOT}")
            set(PostgreSQL_INCLUDE_DIR "${PostgreSQL_ROOT}/include")
            set(PostgreSQL_LIBRARY "${PostgreSQL_ROOT}/lib/libpq.dylib")
            message(STATUS "Using PostgreSQL from: ${PostgreSQL_ROOT}")
        else()
            message(FATAL_ERROR "PostgreSQL not found. Please install PostgreSQL.")
        endif()
    endif()
else()
    message(STATUS "PostgreSQL found via find_package")
endif()

# Include directories
include_directories(${PostgreSQL_INCLUDE_DIR})

# ASIO library (required by Crow)
if(EXISTS "${CMAKE_SOURCE_DIR}/asio/asio/include")
    include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/asio/include")
    include_directories(${CMAKE_SOURCE_DIR}/asio/include)
endif()

# Crow framework - adjust path if needed
# Option 1: If Crow is in backend/crow/include/
if(EXISTS "${CMAKE_SOURCE_DIR}/crow/include")
    include_directories(${CMAKE_SOURCE_DIR}/crow/include)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/../crow/include")
    include_directories(${CMAKE_SOURCE_DIR}/../crow/include)
endif()

# nlohmann/json - adjust path if needed
# Option 1: If json.hpp is in backend/include/
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
    routes/home_routes.cpp
    routes/product_routes.cpp
    routes/cart_routes.cpp
    routes/order_routes.cpp
    routes/stripe_routes.cpp
)

# Executable
add_executable(lala_store ${SOURCES})

# Link libraries
if(PostgreSQL_FOUND)
    target_link_libraries(lala_store 
        ${PostgreSQL_LIBRARIES}
        pthread
    )
else()
    target_link_libraries(lala_store 
        ${PostgreSQL_LIBRARY}
        pthread
    )
endif()
if(CURL_FOUND)
    target_link_libraries(lala_store ${CURL_LIBRARIES})
endif()

# Copy config files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/config/db_config.json 
     DESTINATION ${CMAKE_BINARY_DIR}/config)
file(COPY ${CMAKE_SOURCE_DIR}/config/stripe_config.json 
     DESTINATION ${CMAKE_BINARY_DIR}/config)
