# libqutim project for cmake
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( libqutim )

# Search for source and headers in source directory (non-recursive)
FILE( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
FILE( GLOB HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
FILE( GLOB SRC_GAME "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp" )
FILE( GLOB HDR_GAME "${CMAKE_CURRENT_SOURCE_DIR}/game/*.h" )

LIST( APPEND SRC ${SRC_GAME} )
LIST( APPEND HDR ${HDR_GAME} )

# Add K8JSON
option( SYSTEM_K8JSON "Use system k8json" off )
add_definitions( -DK8JSON_INCLUDE_GENERATOR -DK8JSON_INCLUDE_COMPLEX_GENERATOR )
if( SYSTEM_K8JSON )
	find_package( PkgConfig )
	pkg_check_modules( K8JSON REQUIRED k8json )
	add_definitions( -DK8JSON_LIB_DLL )
	include_directories( ${K8JSON_INCLUDE_DIRS} )
	list( APPEND EXTRA_LIBS ${K8JSON_LIBRARIES} )
else()
	list( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/k8json/k8json.cpp" )
	include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty" )
endif()

# Add qtdwm code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm.h" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/qtdwm_p.h" )

# Add SlidingStackedWidget
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/slidingstackedwidget.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/slidingstackedwidget.h" )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/fingerswipegesture.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/slidingstackedwidget/fingerswipegesture.h" )

# Add flowlayout code
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/flowlayout/flowlayout.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/flowlayout/flowlayout.h" )

# Add itemdelegate
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/itemdelegate/itemdelegate.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/itemdelegate/itemdelegate.h" )

# Add avatarfilter
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatarfilter.cpp" )
LIST( APPEND SRC "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatariconengine.cpp" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatarfilter.h" )
LIST( APPEND HDR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/avatarfilter/avatariconengine_p.h" )

# MinGW special options
# Not sure if those applicable for linux
if(MINGW)
    ADD_DEFINITIONS( -DUNICODE -DQT_LARGEFILE_SUPPORT -DLIBQUTIM_LIBRARY -DQT_THREAD_SUPPORT )
    LIST( APPEND COMPILE_FLAGS "-mthreads" )
endif(MINGW)

FIND_PACKAGE( Qt4 REQUIRED )

option(WITH_DOXYGEN "" ON)

if(WITH_DOXYGEN)
    message("-- Checking for Doxygen...")
    find_package(Doxygen)
endif()

# Include QT4
INCLUDE( ${QT_USE_FILE} )

# Add include directories
INCLUDE_DIRECTORIES(
	.
	..
	../3rdparty/k8json/
	../3rdparty/jdns/
	../3rdparty/qtdwm/
	../3rdparty/qticonloader/
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook
)

# Generate moc files
QUTIM_WRAP_CPP( MOC_SRC ${HDR})
# Add JDNS using separate cmake project
#ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/jdns/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/jdns/" )
# Add Aero theme integration for Windows Vista and later
# ADD_SUBDIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/qtdwm/" "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/qtdwm/")

# This project will generate library

SET(LIBQUTIM_TYPE "SHARED")
IF(LIBQUTIM_BUILD_TYPE STREQUAL "STATIC")
	SET(LIBQUTIM_TYPE "STATIC")
	MESSAGE(STATUS "Libqutim will be linked staticaly")
ENDIF()

ADD_LIBRARY(libqutim ${LIBQUTIM_TYPE} ${SRC} ${MOC_SRC} ${HDR} ${SRC_MM})

set_target_properties(libqutim PROPERTIES
	PREFIX ""
	VERSION ${CMAKE_QUTIM_VERSION_STRING}
	SOVERSION ${CMAKE_QUTIM_VERSION_MAJOR}
	DEFINE_SYMBOL LIBQUTIM_LIBRARY
)

#if( CMAKE_COMPILER_IS_GNUCXX )
#	set_target_properties( libqutim PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}" )
#endif( CMAKE_COMPILER_IS_GNUCXX )

file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/qutim" )
file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook" )
configure_file( version.h.cmake "${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook/libqutim_version.h" )
file( GLOB_RECURSE QUTIM_INCLUDE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
foreach( header ${QUTIM_INCLUDE_HEADERS} )
	if( NOT ${header} MATCHES ".*_p\\.h" )
#		if( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/qutim/${header}" )
#			file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/qutim/${header}"
#		"#include \"${CMAKE_CURRENT_SOURCE_DIR}/${header}\"
#	"
#			)
#		endif( NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/qutim/${header}" )
		LIST(APPEND QUTIM_INSTALL_HEADERS ${header})
    endif( NOT ${header} MATCHES ".*_p\\.h" )
endforeach( header )

list( APPEND QUTIM_INSTALL_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/include/qutim_hook/libqutim_version.h" )
set( QUTIM_LIBRARY libqutim CACHE INTERNAL "" )
set( QUTIM_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include" CACHE INTERNAL "" )
QUTIM_GENERATE_INCLUDES("qutim" "${QUTIM_INSTALL_HEADERS}")
QUTIM_GENERATE_INCLUDES("qutim_hook" "${CMAKE_CURRENT_SOURCE_DIR}/libqutim_global.h")

IF(APPLE)
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(CORE_SERVICES_LIBRARY CoreServices )
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	MARK_AS_ADVANCED (CARBON_LIBRARY
		CORE_SERVICES_LIBRARY
		COCOA_LIBRARY
	)
	LIST(APPEND EXTRA_LIBS ${CARBON_LIBRARY} ${APP_SERVICES_LIBRARY} ${COCOA_LIBRARY})
ENDIF (APPLE)

IF(WIN32)
	LIST(APPEND EXTRA_LIBS ws2_32)
ENDIF()


# Link with Qt
qutim_target_link_libraries( libqutim ${QT_LIBRARIES} ${EXTRA_LIBS})

install( TARGETS libqutim 
	RUNTIME DESTINATION bin
		COMPONENT Core
		LIBRARY DESTINATION lib${LIB_SUFFIX}
		COMPONENT Core
		ARCHIVE DESTINATION lib${LIB_SUFFIX}
		COMPONENT CoreDevel
	BUNDLE DESTINATION library
)

INSTALL(FILES ${QUTIM_INSTALL_HEADERS}
	DESTINATION include/qutim
		COMPONENT CoreDevel
)

if(DOXYGEN_FOUND)
    add_subdirectory(doc)
    set(DOC_TARGET "doc")
    configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    configure_file(doc/DoxygenLayout.xml.in ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml @ONLY)
    add_custom_target(${DOC_TARGET} ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMENT "Building documentation"
        SOURCE doc/Doxyfile.in doc/DoxygenLayout.xml)

	install( DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}/doc/html
		${CMAKE_CURRENT_BINARY_DIR}/doc/qch
		DESTINATION share/qutim/doc
		COMPONENT CoreDevel
	)

    add_dependencies(libqutim ${DOC_TARGET})
endif()

