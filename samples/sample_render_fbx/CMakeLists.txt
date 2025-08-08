add_executable(sample_render_fbx
    sample_render_fbx.cpp
)

target_link_libraries(sample_render_fbx
    PRIVATE vivid_core            # 继承引擎功能
)