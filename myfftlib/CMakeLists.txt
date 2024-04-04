
add_library(myfftlib STATIC
            FFT.cpp 
            FFT.hpp)
        
target_include_directories(myfftlib INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
