# Check that people use the good file
if(NOT TOP_CMAKE_WAS_SOURCED)
    message(FATAL_ERROR "
    You did not 'cmake' the good CMakeLists.txt file. Use the one in the top dir.
    It is advice to delete all wrongly generated cmake stuff => CMakeFiles & CMakeCache.txt")
endif()

# plugin name
set(Output twopad)

set(twopadFinalFlags
    -fvisibility=hidden
    -Wall
    -Wno-parentheses
    )

# twopad linux sources
set(twopadLinuxSources
linux/keyboard_x11.cpp)

# twopad linux headers
set(twopadLinuxHeaders
linux/keyboard_x11.h)

# twopad sources
set(twopadSources
    twopad.cpp
    poll.cpp
    ps2_pad.cpp)

# twopad headers
set(twopadHeaders
    twopad.h
    poll.h
    ps2_pad.h)

if(Windows)
    # to be added.
else()
set(twopadSources
    ${twopadSources}
    ${twopadLinuxSources})

set(twopadHeaders
    ${twopadHeaders}
    ${twopadLinuxHeaders})
endif()

set(twopadFinalLibs ${SDL2_LIBRARIES})
add_definitions(-DSDL_BUILD)

set(twopadFinalLibs
    ${twopadFinalLibs}
    ${GTK2_LIBRARIES}
    ${X11_LIBRARIES}
)

set(twopadFinalSources
    ${twopadSources}
    ${twopadHeaders}
)

include_directories(
    ${CMAKE_BINARY_DIR}/plugins/twopad/
)

#set(RESOURCE_FILES res/game_controller_db.txt)

#add_custom_glib_res("resources" "onepad-res.xml" "onepad_res" ${RESOURCE_FILES})


if(BUILTIN_PAD)
    add_pcsx2_lib(${Output} "${twopadFinalSources}" "${twopadFinalLibs}" "${twopadFinalFlags}")
else()
    add_pcsx2_plugin(${Output} "${twopadFinalSources}" "${twopadFinalLibs}" "${twopadFinalFlags}")
endif()