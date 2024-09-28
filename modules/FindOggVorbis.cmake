set(_OGGVORBIS_SEARCH
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

foreach(search ${_OGGVORBIS_SEARCH})

find_path(OGG_INCLUDE_DIR ogg/ogg.h
  HINTS
    ENV OGGDIR
  PATH_SUFFIXES include
)

find_path(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h
  HINTS
    ENV VORBISDIR
  PATH_SUFFIXES include

)

find_library(OGG_LIBRARY
  NAMES ogg
  HINTS
    ENV OGGDIR
  PATH_SUFFIXES libx32 lib64 lib libs64 libs
)

find_library(VORBIS_LIBRARY
  NAMES vorbis
  HINTS
    ENV VORBISDIR
  PATH_SUFFIXES libx32 lib64 lib libs64 libs
)

find_library(VORBISFILE_LIBRARY
  NAMES vorbisfile
  HINTS
    ENV VORBISDIR
  PATH_SUFFIXES libx32 lib64 lib libs64 libs
)

endforeach()

unset(_OGGVORBIS_SEARCH)

set(OGGVORBIS_LIBRARIES
  ${VORBISFILE_LIBRARY}
  ${VORBIS_LIBRARY}
  ${OGG_LIBRARY}
)

set(OGGVORBIS_INCLUDE_DIRS
  ${OGG_INCLUDE_DIR}
  ${VORBIS_INCLUDE_DIR}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OggVorbis DEFAULT_MSG OGGVORBIS_LIBRARIES OGGVORBIS_INCLUDE_DIRS)

mark_as_advanced(OGGVORBIS_LIBRARIES OGGVORBIS_INCLUDE_DIRS)