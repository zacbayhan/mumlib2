# FindSpeexDSP
#
#  Copyright (c) 2008 Joshua L. Blocher <verbalshadow at gmail dot com>
#  Copyright (c) 2011 Ralf Habacker, <ralf dot habacker at freenet dot de>
#  Copyright (c) 2012 Dmitry Baryshnikov <polimax at mail dot ru>
#  Copyright (c) 2020 Mikhail Paulyshka <pavlyshko-m at yandex dot by>
#
# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# includes
include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

# additional paths
set(_SpeexDSP_SEARCHES)
if(SpeexDSP_ROOT)
  set(_SpeexDSP_SEARCH_ROOT PATHS ${SpeexDSP_ROOT} NO_DEFAULT_PATH)
  list(APPEND _SpeexDSP_SEARCHES SpeexDSP_ROOT)
endif()

# find headers
find_path(SpeexDSP_INCLUDE_DIR
    NAMES
        "speex/speexdsp_config_types.h"
    HINTS
        ${_SpeexDSP_SEARCHES}
    PATH_SUFFIXES
        include
) 
mark_as_advanced(SpeexDSP_INCLUDE_DIR)


# find library
if(NOT SpeexDSP_LIBRARY)
    find_library(
        SpeexDSP_LIBRARY_RELEASE 
        NAMES 
            speexdsp
        HINTS
            ${_SpeexDSP_SEARCHES}
        PATH_SUFFIXES 
            ../../lib
            lib
    )
    
    find_library(
        SpeexDSP_LIBRARY_DEBUG
        NAMES 
            speexdsp
        HINTS
            ${_SpeexDSP_SEARCHES}
        PATH_SUFFIXES 
            debug/lib
            lib
    )


    select_library_configurations(SpeexDSP)
endif()


#handle args
find_package_handle_standard_args(
    SpeexDSP
    REQUIRED_VARS 
        SpeexDSP_LIBRARY 
        SpeexDSP_INCLUDE_DIR
)

if(SpeexDSP_FOUND)
    #set include dirs
    set(SpeexDSP_INCLUDE_DIRS ${SpeexDSP_INCLUDE_DIR})
    
    #set libraries
    if(NOT SpeexDSP_LIBRARIES)
      set(SpeexDSP_LIBRARIES ${SpeexDSP_LIBRARY})
    endif()

    #create import target
    if(NOT TARGET SpeexDSP::SpeexDSP)
      add_library(SpeexDSP::SpeexDSP UNKNOWN IMPORTED)
      set_target_properties(SpeexDSP::SpeexDSP PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SpeexDSP_INCLUDE_DIRS}")

      if(SpeexDSP_LIBRARY_RELEASE)
        set_property(TARGET SpeexDSP::SpeexDSP APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(SpeexDSP::SpeexDSP PROPERTIES IMPORTED_LOCATION_RELEASE "${SpeexDSP_LIBRARY_RELEASE}")
      endif()

      if(SpeexDSP_LIBRARY_DEBUG)
        set_property(TARGET SpeexDSP::SpeexDSP APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(SpeexDSP::SpeexDSP PROPERTIES IMPORTED_LOCATION_DEBUG "${SpeexDSP_LIBRARY_DEBUG}")
      endif()

      if(NOT SpeexDSP_LIBRARY_RELEASE AND NOT SpeexDSP_LIBRARY_DEBUG)
        set_property(TARGET SpeexDSP::SpeexDSP APPEND PROPERTY IMPORTED_LOCATION "${SpeexDSP_LIBRARY}")
      endif()
    endif()
endif()
