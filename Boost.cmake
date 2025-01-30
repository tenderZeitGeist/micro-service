find_package(Boost REQUIRED COMPONENTS regex url json QUIET)

if(NOT Boost_FOUND)
    message(STATUS "Boost not found on the system, proceeding with FetchContent")

    set(BOOST_VERSION "1.83.0")
    set(BOOST_URL "https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}.tar.gz")
    set(BOOST_URL_MD5 "58db882403e0c16b334760f3c3b76ff8")

    include(FetchContent)

    set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(
        Boost
        URL ${BOOST_URL}
        URL_HASH MD5=${BOOST_URL_MD5}
    )

    FetchContent_MakeAvailable(Boost)

    set(Boost_ROOT ${boost_SOURCE_DIR})
    message(STATUS "Boost Source Directory: ${boost_SOURCE_DIR}")
    message(STATUS "Boost Root: ${Boost_ROOT}")

    set(Boost_INCLUDE_DIR ${Boost_ROOT}/include)
    set(Boost_LIBRARY_DIR ${Boost_ROOT}/lib)

    set(Boost_LIBRARIES
            Boost::beast
            Boost::regex
            Boost::url
            Boost::json
    )

else()
    message(STATUS "Boost found on the system")
endif()
