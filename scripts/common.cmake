include(FetchContent)
include(CMakeParseArguments)

function(get_external name url)
    set(prefix EXTERN)
    set(flags DOES_NOT_HAVE_CMAKE)
    set(singleValues INCLUDE_DIRECTORY GIT_TAG)
    set(multipleValues SOURCES LIBS)
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multipleValues}" ${ARGN})

    if(EXTERN_GIT_TAG)
        set(fetchOptions GIT_TAG ${EXTERN_GIT_TAG})
    else()
        set(fetchOptions)
    endif()

    FetchContent_Declare(${name} GIT_REPOSITORY ${url} ${fetchOptions})

    if(${EXTERN_DOES_NOT_HAVE_CMAKE})
        FetchContent_GetProperties(${name})

        if(NOT ${name}_POPULATED)
            FetchContent_Populate(${name})
        endif()

        if(EXTERN_SOURCES)
            list(TRANSFORM EXTERN_SOURCES PREPEND ${${name}_SOURCE_DIR}/)

            add_library(${name} ${EXTERN_SOURCES})
            target_include_directories(${name} SYSTEM PUBLIC ${${name}_SOURCE_DIR})

            if(EXTERN_LIBS)
                target_link_libraries(${name} PRIVATE ${EXTERN_LIBS})
            endif()
        else()
            add_library(${name} INTERFACE)
            target_include_directories(${name} SYSTEM INTERFACE ${${name}_SOURCE_DIR})
        endif()
    else()
        FetchContent_MakeAvailable(${name})
        get_target_property(IID ${name} INTERFACE_INCLUDE_DIRECTORIES)
        set_target_properties(${name} PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${IID}")
    endif()
endfunction()
