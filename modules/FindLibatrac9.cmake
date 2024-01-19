set(_LIBATRAC9_SEARCH
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  /usr
  /usr/local
)

foreach(search ${_LIBATRAC9_SEARCH})

find_path(LIBATRAC9_INCLUDE_DIR libatrac9/libatrac9.h
  HINTS
    ENV LIBATRAC9DIR
  PATH_SUFFIXES include
)

find_library(LIBATRAC9_LIBRARY
  NAMES atrac9 libatrac9
  HINTS
    ENV LIBATRAC9DIR
  PATH_SUFFIXES libx32 lib64 lib libs64 libs
)

endforeach()

unset(_LIBATRAC9_SEARCH)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libatrac9 DEFAULT_MSG LIBATRAC9_LIBRARY LIBATRAC9_INCLUDE_DIR)

mark_as_advanced(LIBATRAC9_LIBRARY LIBATRAC9_INCLUDE_DIR)