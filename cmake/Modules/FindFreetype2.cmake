if(NOT FREETYPE2_FOUND)
  INCLUDE(FindPkgConfig)
  pkg_check_modules (FREETYPE2_PKG freetype2)
  find_path(FREETYPE2_INCLUDE_DIRS NAMES freetype/freetype.h
    HINTS ${FREETYPE2_PKG_INCLUDE_DIRS}
    PATHS
    /usr/include/freetype2
    /usr/local/include/freetype2
  )

  find_library(FREETYPE2_LIBRARIES NAMES freetype
    HINTS ${FREETYPE2_PKG_LIBRARY_DIRS}
    PATHS
    /usr/lib
    /usr/local/lib
  )

  if(FREETYPE2_INCLUDE_DIRS AND FREETYPE2_LIBRARIES)
    set(FREETYPE2_FOUND TRUE CACHE INTERNAL "freetype2 found")
    message(STATUS "Found freetype2: ${FREETYPE2_INCLUDE_DIR}, ${FREETYPE2_LIBRARIES}")
  else(FREETYPE2_INCLUDE_DIRS AND FREETYPE2_LIBRARIES)
    set(FREETYPE2_FOUND FALSE CACHE INTERNAL "freetype2 found")
    message(STATUS "freetype2 not found.")
  endif(FREETYPE2_INCLUDE_DIRS AND FREETYPE2_LIBRARIES)

  mark_as_advanced(FREETYPE2_INCLUDE_DIRS FREETYPE2_LIBRARIES)

endif(NOT FREETYPE2_FOUND)
