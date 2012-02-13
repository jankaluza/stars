FILE(GLOB SRC *.cpp ../abstract_service/*.cpp)

if (PROTOBUF_FOUND)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service/UnifierProtocol.pb.cc ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service/UnifierProtocol.pb.h
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out  ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service --proto_path ${CMAKE_CURRENT_SOURCE_DIR}/../abstract_service ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service/UnifierProtocol.proto
        COMMENT "Running C++ protocol buffer compiler on UnifierProtocol.proto"
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service/UnifierProtocol.proto
    )
    ADD_CUSTOM_TARGET(pb_unifier4 DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service/UnifierProtocol.pb.cc)
endif()

ADD_EXECUTABLE(stars_persistent_data_service ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/../abstract_service/UnifierProtocol.pb.cc)
include_directories(.)
include_directories(../abstract_service)

TARGET_LINK_LIBRARIES(stars_persistent_data_service ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${MYSQL_LIBRARIES} ${LOG4CXX_LIBRARIES} pthread dl)

INSTALL(TARGETS stars_persistent_data_service RUNTIME DESTINATION bin)
