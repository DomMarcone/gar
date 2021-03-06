#for simple compilation
cmake_minimum_required(VERSION 3.7)

#working title
project(gar)

set(INCLUDE_DIRECTORY inc)
set(SOURCES_DIRECTORY src)
set(TEST_DIRECTORY test)

find_package( Threads )

include_directories( ${INCLUDE_DIRECTORY} )

set(BUILD_TESTS FALSE)

#gps_types
set(GPS_TYPE_SOURCES
	${INCLUDE_DIRECTORY}/gps_t.h
	${INCLUDE_DIRECTORY}/gps_convert.h
	${SOURCES_DIRECTORY}/gps_convert.c
)

add_library( gps_type ${GPS_TYPE_SOURCES} )


#LocationSolver
set(LOCATION_SOLVER_SOURCES
	${SOURCES_DIRECTORY}/LocationSolverInterface.cpp
	${SOURCES_DIRECTORY}/LocationSolverBST.cpp
)

set(LOCATION_SOLVER_HEADERS
	${INCLUDE_DIRECTORY}/LocationSolverInterface.hpp
	${INCLUDE_DIRECTORY}/LocationSolverBST.hpp
)

add_library(LocationSolver 
	${LOCATION_SOLVER_SOURCES}
	${LOCATION_SOLVER_HEADERS}
)
target_link_libraries(LocationSolver gps_type)
target_link_libraries(LocationSolver ${CMAKE_THREAD_LIBS_INIT})

#DataCollection
set(DATA_COLLECTION_SOURCES
	${SOURCES_DIRECTORY}/RadioData.cpp
	${SOURCES_DIRECTORY}/GpsData.cpp
)

set(DATA_COLLECTION_HEADERS
	${INCLUDE_DIRECTORY}/RadioData.hpp
	${INCLUDE_DIRECTORY}/GpsData.hpp
)

add_library( DataCollection 
	${DATA_COLLECTION_SOURCES}
	${DATA_COLLECTION_HEADERS}
)
target_link_libraries(DataCollection
	gps_type
	LocationSolver
)


#add_executable(triangluate ${TRIANGULATE_SOURCES})
#target_link_libraries(triangluate signal_solver)


if(BUILD_TESTS)
	#Tests
	add_executable(gps_test ${TEST_DIRECTORY}/gps_test.cpp)
	target_link_libraries(gps_test gps_type)


	add_executable(solver_test ${TEST_DIRECTORY}/solver_test.cpp)
	target_link_libraries(solver_test 
		gps_type
		LocationSolver
	)


	add_executable( data_test ${TEST_DIRECTORY}/data_test.cpp)
	target_link_libraries( data_test 
		gps_type
		LocationSolver
		DataCollection
	)
endif(BUILD_TESTS)

#actual software
add_executable( gar ${SOURCES_DIRECTORY}/gar.cpp )
target_link_libraries( gar 
	gps_type
	LocationSolver
	DataCollection
)
