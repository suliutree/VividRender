add_executable(sample_texture_triangle
    sample_texture_triangle.cpp
)

target_link_libraries(sample_texture_triangle
    PRIVATE vivid_core            # 继承引擎功能
)

copy_runtime_assets(sample_texture_triangle)