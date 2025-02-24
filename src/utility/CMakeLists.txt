aux_source_directory(. DIR_LIB_SRCS)
add_library(utility STATIC ${DIR_LIB_SRCS})

set_target_properties(utility PROPERTIES CXX_STANDARD 20)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(utility PRIVATE ws2_32)
endif()

if (${CANDY_STATIC_SPDLOG})
    target_link_libraries(utility PRIVATE spdlog::spdlog)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DEPS REQUIRED spdlog)
    add_definitions(${DEPS_CFLAGS})
    include_directories(${DEPS_INCLUDEDIR})
    target_link_libraries(utility PRIVATE ${DEPS_LIBRARIES})
endif()

if (${CANDY_STATIC_POCO})
    target_link_libraries(utility PRIVATE Poco::Foundation)
    target_link_libraries(utility PRIVATE Poco::Net)
else()
    find_package(Poco REQUIRED Net)
    target_link_libraries(utility PRIVATE Poco::Foundation)
endif()
