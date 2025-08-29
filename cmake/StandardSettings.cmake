# Compiler warnings
if (MSVC)
    add_compile_options(/W4 /permissive- /Zc:preprocessor)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

# Sanitizers (clang/gcc)
#function(ide_enable_sanitizers tgt)
#  if (IDE_ENABLE_SANITIZERS AND NOT MSVC)
#    target_compile_options(${tgt} PRIVATE -fsanitize=address,undefined)
#    target_link_options(${tgt} PRIVATE -fsanitize=address,undefined)
#  endif()
#endfunction()

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
