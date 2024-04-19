include(ExternalProject)

set(BOOST_VERSION "1.83.0")
set(BOOST_GIT_REPOSITORY "https://github.com/boostorg/boost.git")

ExternalProject_Add(Boost
    GIT_REPOSITORY ${BOOST_GIT_REPOSITORY}
    GIT_TAG ${BOOST_VERSION_TAG}
    PREFIX "${CMAKE_BINARY_DIR}/_deps"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/boost-src"
    BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/boost-build"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/_deps/boost-install"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ./bootstrap.sh --prefix=<INSTALL_DIR> --with-libraries=beast,json
                  && ./b2 --prefix=<INSTALL_DIR> link=static threading=multi runtime-link=shared --with-beast --with-json install
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

set(Boost_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/boost-install/include" CACHE PATH "Boost include directory")
set(Boost_LIBRARY_DIR "${CMAKE_BINARY_DIR}/_deps/boost-install/lib" CACHE PATH "Boost library directory")

message(STATUS "Boost include directory set to: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost library directory set to: ${Boost_LIBRARY_DIR}")
