cmake_minimum_required (VERSION 3.6)
project (SteamAchievements)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Poco QUIET)
if(Poco_FOUND)
    set(_DOWNLOAD_POCO OFF)
else()
    set(_DOWNLOAD_POCO ON)
endif()
option(DOWNLOAD_POCO "Downloads Poco from github and builds it" ${_DOWNLOAD_POCO})

if (DOWNLOAD_POCO)
    message(STATUS "Could not find Poco. Download and Build it.")
    find_package(Git REQUIRED)
    include(ExternalProject)

    set(_POCO_CMAKE_ARGS 
    #build poco modules
        "-DENABLE_NET=ON"
        "-DENABLE_XML=ON"
        
        "-DENABLE_CPPUNIT=OFF"
        "-DENABLE_JSON=OFF"
        "-DENABLE_MONGODB=OFF"
        "-DENABLE_PDF=OFF"
        "-DENABLE_UTIL=OFF"
        "-DENABLE_NETSSL=OFF"
        "-DENABLE_NETSSL_WIN=OFF"
        "-DENABLE_CRYPTO=OFF"
        "-DENABLE_DATA=OFF"
        "-DENABLE_DATA_SQLITE=OFF"
        "-DENABLE_DATA_MYSQL=OFF"
        "-DENABLE_DATA_POSTGRESQL=OFF"
        "-DENABLE_DATA_ODBC=OFF"
        "-DENABLE_SEVENZIP=OFF"
        "-DENABLE_ZIP=OFF"
        "-DENABLE_APACHECONNECTOR=OFF"
        "-DENABLE_CPPPARSER=OFF"
        "-DENABLE_POCODOC=OFF"
        "-DENABLE_PAGECOMPILER=OFF"
        "-DENABLE_PAGECOMPILER_FILE2PAGE=OFF"
        "-DENABLE_REDIS=OFF"
        "-DFORCE_OPENSSL=OFF"
        
        #build options
        "-DPOCO_STATIC=ON"
        
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/poco"
    )
    
    ExternalProject_Add(
        poco
        PREFIX ${CMAKE_BINARY_DIR}/poco
        GIT_REPOSITORY https://github.com/pocoproject/poco.git
        CMAKE_ARGS ${_POCO_CMAKE_ARGS}
        LOG_DOWNLOAD ON
    )
    
    set(POCO_INCLUDE_DIR "${CMAKE_BINARY_DIR}/poco/include")
    set(POCO_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/poco/lib")
    set(RUNTIME_SUFFIX "")
    if (MSVC)#todo: generalize
        set(RUNTIME_SUFFIX "md")
        set(CMAKE_DEBUG_POSTFIX "d")
    endif()
    
    #todo: eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeehm, no, try functions? later
    set(POCO_LIBRARY_DEBUG "${POCO_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}PocoFoundation${RUNTIME_SUFFIX}${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(POCO_LIBRARY_RELEASE "${POCO_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}PocoFoundation${RUNTIME_SUFFIX}${CMAKE_RELEASE_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(POCO_XML_LIBRARY_DEBUG "${POCO_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}PocoXML${RUNTIME_SUFFIX}${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(POCO_XML_LIBRARY_RELEASE "${POCO_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}PocoXML${RUNTIME_SUFFIX}${CMAKE_RELEASE_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(POCO_NET_LIBRARY_DEBUG "${POCO_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}PocoNet${RUNTIME_SUFFIX}${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(POCO_NET_LIBRARY_RELEASE "${POCO_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}PocoNet${RUNTIME_SUFFIX}${CMAKE_RELEASE_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    
    set(POCO_LIBRARIES 
        "optimized;${POCO_LIBRARY_RELEASE};optimized;${POCO_XML_LIBRARY_RELEASE};optimized;${POCO_NET_LIBRARY_RELEASE}"
        "debug;${POCO_LIBRARY_DEBUG};debug;${POCO_XML_LIBRARY_DEBUG};debug;${POCO_NET_LIBRARY_DEBUG}"
        )
    
endif()


find_package(STEAMWORKS REQUIRED)

link_directories(${POCO_LIBRARY_DIRS})

set(SRCS
    "AppProxy/AppProxy.cpp")

add_executable(SteamAppProxy ${SRCS})
target_include_directories(SteamAppProxy PUBLIC ${STEAMWORKS_INCLUDE_DIR})
target_link_libraries(SteamAppProxy ${STEAMWORKS_LIBRARIES})

set_target_properties( SteamAppProxy PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set_target_properties( SteamAppProxy PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR} )
set_target_properties( SteamAppProxy PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR} )
    

set(SRCS
 "Gatherer/Gatherer.cpp")

add_executable(GameGatherer ${SRCS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GameGatherer) #requires cmake 3.6
target_include_directories(GameGatherer PUBLIC ${POCO_INCLUDE_DIR} ${STEAMWORKS_INCLUDE_DIR})
target_link_libraries(GameGatherer 
#if (NOT WIN32)
    ${POCO_LIBRARIES} #auto link poco
#endif()
    ${STEAMWORKS_LIBRARIES})
    
if (WIN32)
    target_link_libraries(GameGatherer "Ws2_32.lib" "iphlpapi.lib") #sockets
endif()

if (DOWNLOAD_POCO)
    add_dependencies(GameGatherer poco)
endif()
add_dependencies(GameGatherer SteamAppProxy)
