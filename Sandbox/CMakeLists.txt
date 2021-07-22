project(Sandbox)

set(proj_dir ${PROJECT_SOURCE_DIR}/application)
set(proj_common_dir ${PROJECT_SOURCE_DIR}/common)

include_directories(${proj_dir})
include_directories(${proj_common_dir})

file(GLOB_RECURSE proj_srcs
        ${proj_dir}/*.cpp
        ${proj_common_dir}/*.cpp)

file(GLOB header_files
        ${gloo_dir}/*.h
        ${gloo_dir}/*/*.h
        ${proj_dir}/*.h
        ${proj_dir}/*/*.h
        ${imgui_dir}/*.h
        ${imgui_dir}/*/*.h
        )

set(all_files ${proj_srcs};${external_srcs};${gloo_srcs};${header_files})

foreach (source IN LISTS all_files)
    file(RELATIVE_PATH source_rel ${CMAKE_CURRENT_LIST_DIR} ${source})
    get_filename_component(source_path "${source_rel}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach ()

add_executable(${PROJECT_NAME} ${gloo_srcs} ${external_srcs} ${proj_srcs} ${header_files})

target_link_libraries(${PROJECT_NAME} ${external_libs})
target_compile_options(${PROJECT_NAME} PRIVATE ${cxx_warning_flags})

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif ()

