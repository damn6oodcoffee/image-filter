
add_executable(ImageFilter WIN32
               MainApp.cpp 
               MainApp.hpp 
               MainFrame.cpp 
               MainFrame.hpp 
               BufferedBitmap.cpp
               BufferedBitmap.hpp
               ImageFilter.cpp
               ImageFilter.hpp
               Image.hpp
               ImageWx.hpp
               ImageWx.cpp 
               )

target_link_libraries(ImageFilter PRIVATE external_deps myfftlib)