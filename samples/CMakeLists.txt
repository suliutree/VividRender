# 遍历 samples/* 子目录，逐个 add_subdirectory
file(GLOB SAMPLE_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach(dir ${SAMPLE_DIRS})
    if (EXISTS "${dir}/CMakeLists.txt")
        add_subdirectory(${dir})
    endif()
endforeach()
# 