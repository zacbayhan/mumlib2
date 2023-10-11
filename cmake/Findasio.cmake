find_path( asio_INCLUDE asio.hpp HINTS "/usr/include" "/usr/local/include" "/opt/local/include" "${PROJECT_SOURCE_DIR}/dependency/asio/include" )

if ( asio_INCLUDE )
    set( ASIO_FOUND TRUE )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE=YES" )

    message( STATUS "Found ASIO include at: ${asio_INCLUDE}" )
else ( )
    message( FATAL_ERROR "Failed to locate ASIO dependency." )
endif ( )
