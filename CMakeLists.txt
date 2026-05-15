cmake_minimum_required(VERSION 3.20)
project(TODOS VERSION 0.1.0 LANGUAGES C CXX)

find_package(SQLite3 REQUIRED)

add_executable(TODOS src/main.cpp)

if(SQLite3_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SQLite3_LIBRARIES})
else()
    message(FATAL_ERROR "Không tìm thấy SQLite3.")
endif()