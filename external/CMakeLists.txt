
include(FetchContent)

set(wxBUILD_SHARED OFF)

message(STATUS "Fetching wxWidgets...")

FetchContent_Declare(
   wxWidgets
   GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
   GIT_TAG cc331e672a9246301161bf31d0dc4b3899f6c604             # v3.2.4
   #GIT_TAG cc7fefd9f17ced619633a7bef2f379fb8c8d25f2
   GIT_SHALLOW ON
   FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(wxWidgets)

message("==============")
message(${wxWidgets_FOUND})
message("==============")

add_library(external_deps INTERFACE)
if(wxWidgets_FOUND)
   target_link_libraries(external_deps INTERFACE wxWidgets::wxWidgets)
else()
   target_link_libraries(external_deps INTERFACE wxcore wxbase)
endif()   