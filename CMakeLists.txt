cmake_minimum_required(VERSION 3.12)

project(HYSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 修正：强制设置安装前缀
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)

# 收集源文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c" "src/*.h" "src/*.hpp")

# 构建动态库
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

# --- 核心必要部分 1: 导出符号 ---
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
    EXPORT_MACRO_NAME HYSimulation_API
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h"
)

# 或者针对头文件不多的项目，直接指定公共头文件
set(PUBLIC_HEADERS
    src/Signature.h
	src/DBCache.h
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h" # 包含生成的头
)


# 输出目录设置（避免污染源码目录）
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")


# 添加库头文件路径
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>   # 条件1：仅当【编译本库】时添加
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # 条件1：仅当【编译本库】时添加
        $<INSTALL_INTERFACE:include>                  # 条件2：仅当【被他人使用】时添加
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

# --- 核心必要部分 2 & 3: 安装规则 ---
# 安装库本身 (.dll, .so, .lib, .a)
install(TARGETS ${PROJECT_NAME}
    EXPORT HYSimulationTargets # 核心：导出目标，生成一个名为 HYSimulationTargets.cmake 的导出文件；其他项目能够通过 find_package() 找到并使用您的库的关键
    ARCHIVE DESTINATION lib    # 安装 .lib / .a    安装静态库文件
    LIBRARY DESTINATION lib    # 安装 .so / .dylib  安装共享库文件
    RUNTIME DESTINATION bin    # 安装 .dll     安装运行时文件，Windows: .dll 文件（动态链接库）
    INCLUDES DESTINATION include # 公开头文件路径
)

# 安装公共头文件
install(FILES ${PUBLIC_HEADERS} DESTINATION include)

# --- 核心必要部分 3: 包配置 ---
# 安装目标导出文件（使find_package能找到我们的目标）
install(EXPORT HYSimulationTargets
    FILE HYSimulationConfig.cmake
    DESTINATION lib/cmake/HYSimulation
)

# 添加测试项目
add_subdirectory(Test)


