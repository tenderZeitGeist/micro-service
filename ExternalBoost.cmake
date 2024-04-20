include(ExternalProject)

set(BOOST_VERSION "1.83.0")
set(BOOST_URL "https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}.tar.gz")
set(BOOST_URL_MD5 "58db882403e0c16b334760f3c3b76ff8")

set(BOOST_INCLUDE_LIBRARIES system url json)
set(BOOST_ENABLE_CMAKE On)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    Boost
    URL ${BOOST_URL}
    URL_HASH MD5=${BOOST_URL_MD5}
)

FetchContent_MakeAvailable(Boost)
set(Boost_ROOT ${boost_SOURCE_DIR})
