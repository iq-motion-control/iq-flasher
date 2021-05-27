# Define Library variable in order not to repeat ourselves.
set(LIBRARY_NAME Schmi)

# This is a make target, so you can do a "make projArunner"
set(APPLICATION_NAME Schmi_runner)

# Set the name of your project here
project("Schmi")
# set(CMAKE_BUILD_WITH_INSTALL_RPATH 1)

# Set a list of needed subprojects
# set(SUBPROJECT_LIST MySubProject)

# Set the version number of your project here (format is MAJOR.MINOR.PATCHLEVEL - e.g. 1.0.0)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# Mac needed variables (adapt according to your needs)
# set(CMAKE_MACOSX_RPATH ON)
# set(CMAKE_SKIP_BUILD_RPATH FALSE)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE

#---------------------------------------------------------------------------------------------------

# CMAKE PACKAGING (for other CMake projects to use this one easily)
if(MULTIPLE_PROJECTS)
  include(PackageConfigurator)
  include(Common)
endif()

# Find subproject (if needed) if we are compiling only this project otherwise reuse the targets
if(SUBPROJECT_LIST)
  list(LENGTH SUBPROJECT_LIST LIST_LENGTH)
  if(LIST_LENGTH GREATER "1")
    foreach(SUBPROJECT IN LISTS SUBPROJECT_LIST)
      add_dependent_subproject(${SUBPROJECT})  
    endforeach()
  else()
    add_dependent_subproject(${SUBPROJECT_LIST})
  endif()
endif()

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake) # main (top) cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir
set(CMAKE_CXX_STANDARD 14)

# CMake useful variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# This sets the include directory for the reference project. This is the -I flag in gcc.
# All the includes should be in this variable To add new directories to the include path,
# just add them in this list (after a space), or add an include_directories statement in a
# subdirectory's CMakeLists.
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

enable_testing()
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)


# --------------------- SCHMI SPECIFIC -------------------------

file(GLOB MY_MISC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/misc/*.txt") 
file(COPY ${MY_MISC_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/misc/)

file(GLOB MY_BIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/binaries/*.bin") 
file(COPY ${MY_BIN_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/binaries/)

file(GLOB MY_BIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/test_files/*") 
file(COPY ${MY_BIN_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test_files/)