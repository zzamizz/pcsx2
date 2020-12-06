# Check that people use the good file
if(NOT TOP_CMAKE_WAS_SOURCED)
    message(FATAL_ERROR "
    You did not 'cmake' the good CMakeLists.txt file. Use the one in the top dir.
    It is advice to delete all wrongly generated cmake stuff => CMakeFiles & CMakeCache.txt")
endif()


# plugin name
set(Output LilyPad-0.11.0)

set(lilypadFinalFlags
    -fvisibility=hidden
    -Wall
    -Wno-parentheses
    )
    
# lilypad sources
set(lilypadSources
    DeviceEnumerator.cpp
    InputManager.cpp
    KeyboardQueue.cpp
    LilyPad.cpp
    )

# lilypad headers (I will let people split it between windows/linux, not sure it is useful)
set(lilypadHeaders
    Config.h
    DeviceEnumerator.h
    Windows/Diagnostics.h
    Devices/DirectInput.h
    Devices/DualShock3.h
    Global.h
    Devices/HidDevice.h
    InputManager.h
    KeyboardQueue.h
    Linux/bitmaskros.h
    Linux/ConfigHelper.h
    Devices/JoyEvdev.h
    Devices/LinuxKeyboardMouse.h
    Devices/RawInput.h
    resource.h
    Windows/Tooltips.h
    Devices/usb.h
    Windows/VKey.h
    Devices/WindowsKeyboard.h
    Devices/WindowsMessaging.h
    Devices/WindowsMouse.h
    Windows/WndProcEater.h
    Devices/XInputEnum.h
)

if(Windows)
    LIST(APPEND lilypadSources
    Windows/Diagnostics.cpp
        Devices/DirectInput.cpp
        Devices/DualShock3.cpp
        Global.cpp
        Devices/HidDevice.cpp
        Devices/RawInput.cpp
        Windows/Tooltips.cpp
        Windows/VKey.cpp
        Devices/WindowsKeyboard.cpp
        Devices/WindowsMessaging.cpp
        Devices/WindowsMouse.cpp
        Windows/WndProcEater.cpp
        Devices/XInputEnum.cpp
        )

else()
    LIST(APPEND lilypadSources
        Linux/Config.cpp
        Linux/ConfigHelper.cpp
        Devices/JoyEvdev.cpp
        Devices/LinuxKeyboardMouse.cpp
        Linux/KeyboardQueue.cpp
        )
endif()

if (SDL2_API)
    set(lilypadFinalLibs
        ${SDL2_LIBRARIES}
        )
else()
    set(lilypadFinalLibs
        ${SDL_LIBRARY}
        )
endif()

set(lilypadFinalLibs
    #${lilypadFinalLibs}
    #${GTK2_LIBRARIES}
    #${X11_LIBRARIES}
    )

set(lilypadFinalSources
    ${lilypadSources}
    ${lilypadHeaders}
    ${lilypadLinuxSources}
    ${lilypadLinuxHeaders}
    )

add_pcsx2_plugin(${Output} "${lilypadFinalSources}" "${lilypadFinalLibs}" "${lilypadFinalFlags}")
target_compile_features(${Output} PRIVATE cxx_std_17)