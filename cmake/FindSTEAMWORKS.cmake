if (NOT STEAMWORKS_INCLUDE_DIR OR NOT STEAMWORKS_LIBRARIES)
	FIND_PATH(STEAMWORKS_INCLUDE_DIR steam/steam_api.h
		/usr/include
		/usr/local/include
		DOC "Include path for Steamworks"
	)

	if (Windows)
		if (CMAKE_SIZEOF_VOID_P EQUAL 4) 
        FIND_LIBRARY(STEAMWORKS_LIBRARY NAMES steam_api
				PATHS
				$ENV{STEAMWORKS_ROOT}/sdk/redistributable_bin
				DOC "Steamworks library name"
			)
		else ()
			FIND_LIBRARY(STEAMWORKS_LIBRARY NAMES steam_api64
				PATHS
				$ENV{STEAMWORKS_ROOT}/sdk/redistributable_bin/win64
				DOC "Steamworks library name"
			)
			MESSAGE("64 bit steam")
		endif()
	else ()
		FIND_LIBRARY(STEAMWORKS_LIBRARY NAMES steam_api
			PATHS ${LIB_SEARCH_PATHS}
			DOC "Steamworks library name"
		)
	endif ()
	if (STEAMWORKS_LIBRARY)
		set(STEAMWORKS_LIBRARIES ${STEAMWORKS_LIBRARY})
	endif ()
	MARK_AS_ADVANCED(STEAMWORKS_INCLUDE_DIR STEAMWORKS_LIBRARY)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STEAMWORKS DEFAULT_MSG STEAMWORKS_INCLUDE_DIR STEAMWORKS_LIBRARIES)