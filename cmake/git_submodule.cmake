find_package(Git)

function(add_git_submodule dir build)
    # add a Git submodule directory to CMake, assuming the Git submodule directory is a CMake project.
    #
    # Usage: in CMakeLists.txt
    #
    # include(git_submodule.cmake) add_git_submodule(mysubmod_dir)
    if (GIT_FOUND)
        if (NOT EXISTS ${dir}/CMakeLists.txt)
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule init -- ${dir} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    ERROR_QUIET)
        endif ()
    endif ()
    if (build)
        add_subdirectory(${dir})
    endif ()

endfunction(add_git_submodule)
