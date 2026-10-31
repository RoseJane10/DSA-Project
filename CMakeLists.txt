
cmake_minimum_required(VERSION 3.16)

project(beautrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find PostgreSQL
find_path(PQ_INCLUDE_DIR libpq-fe.h
    PATHS C:/msys64/msys2/ucrt64/include
)

find_library(PQ_LIBRARY pq
    PATHS C:/msys64/msys2/ucrt64/lib
)

if(NOT PQ_INCLUDE_DIR OR NOT PQ_LIBRARY)
    message(FATAL_ERROR "PostgreSQL libpq not found!")
endif()

# Create application
add_executable(beautrix
    main.cpp
    src/Database.cpp
    src/Item.cpp
    src/InventoryRepository.cpp
    src/DSAComponents.cpp
    src/ShoppingCart.cpp
    src/BrandShowcase.cpp
    src/InventoryUndoStack.cpp
    src/AuthSystem.cpp
    src/OrderQueue.cpp
)

# Include directories
target_include_directories(beautrix PRIVATE
    include
    ${PQ_INCLUDE_DIR}
)

# Link PostgreSQL
target_link_libraries(beautrix PRIVATE
    ${PQ_LIBRARY}
)
