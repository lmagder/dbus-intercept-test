cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

include(FindPkgConfig)

pkg_check_modules(PKGCONFIG REQUIRED dbus-1 dbus-c++-1)

project(server)
include_directories(${PKGCONFIG_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
link_directories(${PKGCONFIG_LIBRARY_DIRS})
add_definitions(${PKGCONFIG_CFLAGS_OTHER})

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated_interface.h"
                   COMMAND dbusxx-xml2cpp "${CMAKE_CURRENT_SOURCE_DIR}/interface.xml" "--proxy=${CMAKE_CURRENT_BINARY_DIR}/generated_interface.h"
                   DEPENDENCY interface.xml
                   VERBATIM
    )

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated_impl.h"
                   COMMAND dbusxx-xml2cpp "${CMAKE_CURRENT_SOURCE_DIR}/interface.xml" "--adaptor=${CMAKE_CURRENT_BINARY_DIR}/generated_impl.h"
                   MAIN_DEPENDENCY interface.xml
                   VERBATIM
    )

add_executable(server_app "server_main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/generated_impl.h")
add_executable(client_app "client_main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/generated_interface.h")
add_executable(interceptor_app "interceptor_main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/generated_interface.h" "${CMAKE_CURRENT_BINARY_DIR}/generated_impl.h")

target_link_libraries(server_app ${PKGCONFIG_LIBRARIES})
target_link_libraries(client_app ${PKGCONFIG_LIBRARIES})
target_link_libraries(interceptor_app ${PKGCONFIG_LIBRARIES})
