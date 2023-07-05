pybind11_add_module(pyutk
    pyUTK.cpp
)

target_include_directories(pyutk PRIVATE "${UTK_INCLUDE_DIR}")
target_link_libraries     (pyutk PRIVATE pyutkSamplers pyutkMetrics pybind11::module spdlog::spdlog)