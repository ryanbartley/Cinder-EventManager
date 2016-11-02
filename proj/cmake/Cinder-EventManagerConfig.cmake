if( NOT TARGET Cinder-EventManger )
  get_filename_component( CINDER_EVENT_BLOCK_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE )
  get_filename_component( CINDER_EVENT_INCLUDE_PATH "${CINDER_EVENT_BLOCK_PATH}/src" ABSOLUTE )
	
  if( NOT EXISTS ${CINDER_PATH} )
    get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )
  endif()

  add_library( Cinder-EventManager
    ${CINDER_EVENT_INCLUDE_PATH}/EventManager.cpp 
    ${CINDER_EVENT_INCLUDE_PATH}/EventManagerBase.cpp 
  )

  target_compile_options( Cinder-EventManager PUBLIC "-std=c++11" )

  target_include_directories( Cinder-EventManager PUBLIC "${CINDER_EVENT_INCLUDE_PATH}" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
  target_link_libraries( Cinder-EventManager PRIVATE cinder )
	
endif()



