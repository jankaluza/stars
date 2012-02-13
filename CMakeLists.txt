cmake_minimum_required(VERSION 2.6)
project(stars)

set(CMAKE_MODULE_PATH "cmake_modules")

set(sqlite3_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(sqlite3)

set(mysql_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(mysql)

set(Boost_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(Boost COMPONENTS date_time system filesystem program_options regex signals REQUIRED)

set(tinyxml_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(tinyxml)

set(log4cxx_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(log4cxx)

set(SDL_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SDL)

set(SDL_imageDIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SDL_image)

set(SDL_ttfDIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SDL_ttf)

set(SDL_gfxDIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SDL_gfx)

set(SDL_netDIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SDL_net)

set(Protobuf_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(Protobuf REQUIRED)

# ADD_DEFINITIONS(${SWIFTEN_CFLAGS})

SET(STARS_VERSION 1.0)
include_directories(include)

message("  Supported features")
message("-----------------------")
if (MYSQL_FOUND)
	ADD_DEFINITIONS(-DWITH_MYSQL)
	include_directories(${MYSQL_INCLUDE_DIR})
	message("MySQL             : yes")
else (MYSQL_FOUND)
	set(MYSQL_LIBRARIES "")
	message("MySQL             : no (install mysql-devel)")
endif (MYSQL_FOUND)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-ggdb)
	ADD_DEFINITIONS(-DDEBUG)
	ADD_DEFINITIONS(-Wall)
	ADD_DEFINITIONS(-W)
	ADD_DEFINITIONS(-Wcast-align)
	ADD_DEFINITIONS(-Wextra -Wno-sign-compare -Wno-unused-parameter)
	ADD_DEFINITIONS(-Winit-self)
	ADD_DEFINITIONS(-Wmissing-declarations)
	ADD_DEFINITIONS(-Wpointer-arith)
#	ADD_DEFINITIONS(-Wreorder)
#	ADD_DEFINITIONS(-Woverloaded-virtual)
#	ADD_DEFINITIONS(-Wsign-promo)
	ADD_DEFINITIONS(-Wundef -Wunused)
	message("Debug             : yes")
else(CMAKE_BUILD_TYPE MATCHES Debug)
	message("Debug             : no (run \"cmake . -DCMAKE_BUILD_TYPE=Debug\")")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

if (SDL_FOUND)
	include_directories(SDL_INCLUDE_DIR)
	include_directories(SDLIMAGE_INCLUDE_DIR)
	include_directories(SDLGFX_INCLUDE_DIR)
	message("Client            : yes")
else (SDL_FOUND)
	message("Client            : no (install libSDL)")
endif (SDL_FOUND)

if (TINYXML_FOUND)
	ADD_DEFINITIONS(-DWITH_TINYXML)
	include_directories(TINYXML_INCLUDE_DIR)
	message("XML support       : yes")
else (TINYXML_FOUND)
	set(TINYXML_LIBRARIES "")
	message("XML support       : no (install tinyxml-devel)")
endif (TINYXML_FOUND)

if (LOG4CXX_FOUND)
	ADD_DEFINITIONS(-DWITH_LOG4CXX)
	include_directories(${LOG4CXX_INCLUDE_DIR})
	message("Logging to file   : yes")
else()
	set(LOG4CXX_LIBRARIES "")
	message("Logging to file   : no (install log4cxx-devel)")
endif()


SET(HAMLOG_VERSION 1.0)
SET(PROJECT_VERSION 1.0)

include_directories(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(client)
ADD_SUBDIRECTORY(server)
ADD_SUBDIRECTORY(tools)

message("----------------------")
