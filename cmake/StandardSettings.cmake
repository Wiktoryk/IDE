# Compiler warnings
if (MSVC)
    add_compile_options(/W4 /permissive- /Zc:preprocessor)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

# Sanitizers (clang/gcc)
if (IDE_ENABLE_SANITIZERS AND NOT MSVC)
    add_link_options(-fsanitize=address,undefined)
    add_compile_options(-fsanitize=address,undefined)
endif()

# Link Time Optimization
include(CheckIPOSupported)
if (IDE_ENABLE_LTO)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if (ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()

# Qt meta features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
