FILE(GLOB SRC *.cpp */*.cpp ../../client/Widgets/*.cpp ../../client/Base/Surface.cpp ../../client/Base/FontManager.cpp ../../client/Base/Timer.cpp)

ADD_EXECUTABLE(stars_dialog_editor ${SRC})
include_directories(.)
include_directories(../../client)

TARGET_LINK_LIBRARIES(stars_dialog_editor ${SDL_LIBRARY} ${TINYXML_LIBRARIES} ${SDLGFX_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${SDLNET_LIBRARY} pthread dl m)

INSTALL(TARGETS stars_dialog_editor RUNTIME DESTINATION bin)


