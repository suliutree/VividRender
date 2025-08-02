add_executable(sample_render_obj
    sample_render_obj.cpp
)

target_link_libraries(sample_render_obj
    PRIVATE vivid_core            # 继承引擎功能
)

# copy_runtime_assets(sample_render_obj)