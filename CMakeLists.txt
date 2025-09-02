cmake_minimum_required(VERSION 3.12)

project(HYSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出目录设置（避免污染源码目录）
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")

# 收集源文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c" "src/*.h" "src/*.hpp")

# 构建动态库
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

# 添加库头文件路径
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PROJECT_SOURCE_DIR}/src"
)

# MySQL依赖
if(MSVC)
    set(MYSQL_ROOT "C:/Program Files/MySQL/MySQL Server 5.7")

    target_include_directories(${PROJECT_NAME} PUBLIC "${MYSQL_ROOT}/include")
    target_link_directories(${PROJECT_NAME} PUBLIC "${MYSQL_ROOT}/lib")
    target_link_libraries(${PROJECT_NAME} PUBLIC libmysql.lib)

    # Visual Studio 配置（关键：添加 ARCHIVE 输出）
    set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "all_proj"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}/Debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/Release"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}/RelWithDebInfo"

		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}/Debug"
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/Release"
		ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}/RelWithDebInfo"

		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}/Debug"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/Release"
		LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}/RelWithDebInfo"
    )

endif()

# 添加测试项目
add_subdirectory(Test)


