FILE(GLOB SRC *.cpp */*.cpp)

if (PROTOBUF_FOUND)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Network/Protocol.pb.cc ${CMAKE_CURRENT_BINARY_DIR}/Network/Protocol.pb.h
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out  ${CMAKE_CURRENT_BINARY_DIR}/Network --proto_path ${CMAKE_CURRENT_SOURCE_DIR}/Network ${CMAKE_CURRENT_BINARY_DIR}/Network/Protocol.proto
        COMMENT "Running C++ protocol buffer compiler on protocol.proto"
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Network/Protocol.proto
    )
    ADD_CUSTOM_TARGET(pb DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Network/Protocol.pb.cc)
endif()

ADD_EXECUTABLE(stars_client ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/Network/Protocol.pb.cc)
include_directories(.)

TARGET_LINK_LIBRARIES(stars_client ${Boost_LIBRARIES} ${MYSQL_LIBRARIES} ${PROTOBUF_LIBRARIES} ${TINYXML_LIBRARIES} ${LOG4CXX_LIBRARIES} ${SDL_LIBRARY} ${SDLGFX_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${SDLNET_LIBRARY} pthread dl m)

INSTALL(TARGETS stars_client RUNTIME DESTINATION bin)


