if(NOT GLFW3_FOUND)
  INCLUDE(FindPkgConfig)
  pkg_check_modules (GLFW3_PKG glfw3)
  find_path(GLFW3_INCLUDE_DIRS NAMES GLFW/glfw3.h
    HINTS ${GLFW3_PKG_INCLUDE_DIRS}
    PATHS
    /usr/include
    /usr/local/include
  )

  find_library(GLFW3_LIBRARIES NAMES glfw3 glfw
    HINTS ${GLFW3_PKG_LIBRARY_DIRS}
    PATHS
    /usr/lib
    /usr/local/lib
  )

  if(GLFW3_INCLUDE_DIRS AND GLFW3_LIBRARIES)
    set(GLFW3_FOUND TRUE CACHE INTERNAL "GLFW3 found")
    message(STATUS "Found GLFW3: ${GLFW3_INCLUDE_DIRS}, ${GLFW3_LIBRARIES}")
  else(GLFW3_INCLUDE_DIRS AND GLFW3_LIBRARIES)
    set(GLFW3_FOUND FALSE CACHE INTERNAL "GLFW3 found")
    message(STATUS "GLFW3 not found.")
  endif(GLFW3_INCLUDE_DIRS AND GLFW3_LIBRARIES)

  mark_as_advanced(GLFW3_INCLUDE_DIRS GLFW3_LIBRARIES)

endif(NOT GLFW3_FOUND)
