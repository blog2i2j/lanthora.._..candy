aux_source_directory(. DIR_LIB_SRCS)
add_library(tun STATIC ${DIR_LIB_SRCS})

set_target_properties(tun PROPERTIES CXX_STANDARD 23)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(tun PRIVATE iphlpapi)
    target_link_libraries(tun PRIVATE ws2_32)

    set(WINTUN_VERSION 0.14.1)
    set(WINTUN_ZIP wintun-${WINTUN_VERSION}.zip)
    set(WINTUN_URL https://www.wintun.net/builds/${WINTUN_ZIP})

    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${WINTUN_ZIP})
        file(DOWNLOAD ${WINTUN_URL} ${CMAKE_CURRENT_BINARY_DIR}/${WINTUN_ZIP} STATUS DOWNLOAD_STATUS)
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

        if(${STATUS_CODE} EQUAL 0)
            message(STATUS "wintun download success")
        else()
            message(FATAL_ERROR "wintun download failed: ${ERROR_MESSAGE}")
        endif()
    else()
        message(STATUS "use wintun cache")
    endif()

    file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/${WINTUN_ZIP})

    include_directories(${CMAKE_CURRENT_BINARY_DIR}/wintun/include)
endif()

if (${CANDY_STATIC_SPDLOG})
    target_link_libraries(tun PRIVATE spdlog::spdlog)
else()
    target_link_libraries(tun PRIVATE ${SPDLOG_LIBRARIES})
endif()

if (${CANDY_STATIC_OPENSSL})
    target_link_libraries(tun PRIVATE ${OPENSSL_LIB_CRYPTO} ${OPENSSL_LIB_SSL})
endif()

target_link_libraries(tun PRIVATE Poco::Foundation Poco::Net Poco::NetSSL)

target_link_libraries(tun PRIVATE utils)
