
#set all the lib source files in the folder
FILE(GLOB LIB_SOURCES . *.cpp *.c)

#remove main.cpp from the lib source files list
file(GLOB to_remove main.cpp)
list(REMOVE_ITEM LIB_SOURCES ${to_remove})

#set executable source file
set(APP_SOURCES
  main.cpp
)

# Define the library
add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})

# Set the build version. It will be used in the name of the lib, with corresponding
# symlinks created. SOVERSION could also be specified for api version. 
# set_target_properties(${LIBRARY_NAME} PROPERTIES
#   VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
#   FRAMEWORK FALSE
#   PUBLIC_HEADER "${LIB_HEADERS}" # specify the public headers
# )

# Links the target library against the needed sublibraries (if needed)
if(SUBPROJECT_LIST)
  list(LENGTH SUBPROJECT_LIST LIST_LENGTH)
  if(LIST_LENGTH GREATER "1")
    foreach(SUBPROJECT IN LISTS SUBPROJECT_LIST)
    target_link_libraries(${LIBRARY_NAME} ${SUBPROJECT})
    endforeach()
  else()
    target_link_libraries(${LIBRARY_NAME} ${SUBPROJECT_LIST})
  endif()
endif()
  
add_executable(${APPLICATION_NAME} ${APP_SOURCES})

# Links the target exe (projArunner) against the library (libprojA) and other required libraries
target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME})

# Says how and where to install software
# Targets:
#   * <prefix>/lib/<libraries>
#   * header location after install: <prefix>/include/<project>/*.h
#   * headers can be included by C++ code `#<project>/Bar.hpp>`
install(TARGETS ${LIBRARY_NAME} ${APPLICATION_NAME}
  EXPORT ${PROJECT_NAME}Targets            # for downstream dependencies
  ARCHIVE DESTINATION lib COMPONENT libs   # static lib
  RUNTIME DESTINATION bin COMPONENT libs   # binaries
  LIBRARY DESTINATION lib COMPONENT libs   # shared lib
  FRAMEWORK DESTINATION bin COMPONENT libs # for mac
  PUBLIC_HEADER DESTINATION include/${PROJECT_NAME} COMPONENT devel   # headers for mac (note the different component -> different package)
  INCLUDES DESTINATION include             # headers
)
