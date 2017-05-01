cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
set(PROJECT emmi-greeter)
project(${PROJECT})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB SRCS *.cpp *ui *moc)
message(STATUS "SRCS ${SRCS}")

find_package(Qt5Widgets REQUIRED)
#find_package(Qt5X11Extras REQUIRED QUIET)

QT5_ADD_RESOURCES(RSCS emmi-greeter.qrc)


include(FindPkgConfig)
pkg_check_modules(LIGHTDM_QT liblightdm-qt5-3)

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR}
                      ${LIGHTDM_QT_INCLUDE_DIRS}
)

add_executable ( emmi-greeter ${SRCS} ${RSCS})

target_link_libraries ( emmi-greeter Qt5::Widgets ${LIGHTDM_QT_LIBRARIES} )


install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   emmi-greeter.desktop  DESTINATION /usr/share/xgreeters)

if (NOT KEEP_CONFIGS)
    install(FILES emmi-greeter.conf DESTINATION /etc/lightdm)
endif()

