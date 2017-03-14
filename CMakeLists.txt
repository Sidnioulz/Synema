CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(synema)

SET(CMAKE_PROJECT_NAME "Synema")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)
INCLUDE(FeatureSummary)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
SET(LS_SHARE_PATH ${CMAKE_INSTALL_PREFIX}/share/synema)
SET(LS_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/synema)
SET(LS_BIN_PATH ${CMAKE_INSTALL_PREFIX}/bin)
FILE(GLOB_RECURSE ls_source "src/*.c")
FILE(GLOB_RECURSE ls_headers "src/*.h")
FILE(GLOB_RECURSE demo_player_source "player/*.c")
FILE(GLOB_RECURSE demo_player_headers "player/*.h")
ADD_DEFINITIONS(-DCMAKE_SHARE_PATH="${LS_SHARE_PATH}")
ADD_DEFINITIONS(-DCMAKE_LIB_PATH="${LS_LIB_PATH}")


OPTION(BUILD_APP "Build the main application" ON)
OPTION(BUILD_DEMO_PLAYER "Build demo log player" ON)
OPTION(AUTOSTART_DEMO_PLAYER "Auto start demo log player" OFF)
OPTION(BUILD_PLUGINS "Build plugins" OFF)
OPTION(BUILD_PLUGIN_BASH_HISTORY "Build Bash History plugin" ON)
OPTION(BUILD_PLUGIN_MOD_SECURITY "Build Mod Security plugin" ON)
OPTION(BUILD_PLUGIN_OSIRIS "Build Osiris plugin" ON)
OPTION(BUILD_PLUGIN_P0F "Build P0F plugin" ON)
OPTION(BUILD_PLUGIN_PIGA "Build PIGA plugin" ON)
OPTION(BUILD_PLUGIN_SELINUX "Build SELinux plugin" ON)
OPTION(BUILD_PLUGIN_SNORT "Build Snort plugin" ON)
OPTION(BUILD_PLUGIN_SNORT_NET "Build Snort Network plugin" OFF)
OPTION(BUILD_PLUGIN_SYSLOG_SSH "Build Syslog SSH" ON)
OPTION(BUILD_PLUGIN_SYSLOG_WGET "Build Syslog Wget" OFF)
OPTION(BUILD_PLUGIN_TEST "Build Test plugin" OFF)
OPTION(BUILD_PLUGIN_CORRELATION "Build Correlation plugin" ON)
## ADD AN OPTION HERE IF YOU WANT TO ADD A PLUGIN ##
OPTION(DEBUG "Debug mode" ON)
OPTION(GENERATE_DBUS "Generate DBus files" OFF)
OPTION(GENERATE_GOBJECT "Generate GObjects from Gob" OFF)
OPTION(GENERATE_MARSHAL "Generate Marshals for GCClosures" OFF)


IF(NOT DEBUG)
	ADD_DEFINITIONS(-DNDEBUG -O4)
ELSE(NOT DEBUG)
	ADD_DEFINITIONS(-g)
ENDIF(NOT DEBUG)


PKG_SEARCH_MODULE(gthread REQUIRED gthread-2.0)
INCLUDE_DIRECTORIES(${gthread_INCLUDE_DIRS})
LINK_DIRECTORIES(${gthread_LIBRARY_DIRS})

PKG_SEARCH_MODULE(gtk2 REQUIRED gtk+-2.0)
INCLUDE_DIRECTORIES(${gtk2_INCLUDE_DIRS})
LINK_DIRECTORIES(${gtk2_LIBRARY_DIRS})

PKG_SEARCH_MODULE(rsvg REQUIRED librsvg-2.0)
INCLUDE_DIRECTORIES(${rsvg_INCLUDE_DIRS})
LINK_DIRECTORIES(${rsvg_LIBRARY_DIRS})

PKG_SEARCH_MODULE(dbusglib REQUIRED dbus-glib-1)
INCLUDE_DIRECTORIES(${dbusglib_INCLUDE_DIRS})
LINK_DIRECTORIES(${dbusglib_LIBRARY_DIRS})

PKG_SEARCH_MODULE(libglib2.0 REQUIRED glib-2.0)
INCLUDE_DIRECTORIES(${libglib2.0_INCLUDE_DIRS})
LINK_DIRECTORIES(${libglib2.0_LIBRARY_DIRS})

PKG_SEARCH_MODULE(libcairo2 REQUIRED cairo)
INCLUDE_DIRECTORIES(${libcairo2_INCLUDE_DIRS})
LINK_DIRECTORIES(${libcairo2_LIBRARY_DIRS})

CHECK_LIBRARY_EXISTS(dl dlopen /usr/include HAVE_LIBDL)

CHECK_LIBRARY_EXISTS(tar tar_open /usr/include HAVE_LIBTAR)
IF(NOT HAVE_LIBTAR)
    MESSAGE(FATAL_ERROR "libtar-dev missing !")
ENDIF(NOT HAVE_LIBTAR)

CHECK_INCLUDE_FILES(pcre.h HAVE_PCRE)
# not working ???
#IF(NOT HAVE_PCRE)
#    MESSAGE(FATAL_ERROR "libpcre3-dev missing ! pcre.h couldn't be found")
#ENDIF(NOT HAVE_PCRE)


IF(GENERATE_DBUS)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/player/dbus-methods.h
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-methods.h
		COMMAND dbus-binding-tool --mode=glib-server --prefix=object ../data/log-player-demo.xml > ${CMAKE_CURRENT_SOURCE_DIR}/player/dbus-methods.h
		COMMAND dbus-binding-tool --mode=glib-client ../data/log-player-demo.xml > ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-methods.h
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/log-player-demo.xml
	)
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus.h
		PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-methods.h)
	LIST(APPEND ls_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-methods.h)

	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/player/server.c
		PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/player/dbus-methods.h)
	LIST(APPEND demo_player_headers ${CMAKE_CURRENT_SOURCE_DIR}/player/dbus-methods.h)

ENDIF(GENERATE_DBUS)


IF(GENERATE_GOBJECT)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/frame-signals.c
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/frame-signals.h
		COMMAND gob2 -w --no-lines ${CMAKE_CURRENT_SOURCE_DIR}/data/frame-signals.gob && mv frame-signals.? ${CMAKE_CURRENT_SOURCE_DIR}/src/
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/frame-signals.gob
	)
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/src/frame-common.h ${CMAKE_CURRENT_SOURCE_DIR}/src/frame-private.c
		PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/frame-signals.c)
	LIST(APPEND ls_source ${CMAKE_CURRENT_SOURCE_DIR}/src/frame-signals.c)
	LIST(APPEND ls_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/frame-signals.h)
ENDIF(GENERATE_GOBJECT)


IF(GENERATE_MARSHAL)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.c
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.h
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.c
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.h
		COMMAND echo VOID:UINT64 | glib-genmarshal --header > ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.h
		COMMAND echo VOID:UINT64 | glib-genmarshal --body > ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.c
		COMMAND echo VOID:UINT64 | glib-genmarshal --header > ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.h
		COMMAND echo VOID:UINT64 | glib-genmarshal --body > ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.c
	)
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus.c
		PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.c)
	LIST(APPEND ls_source ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.c)
	LIST(APPEND ls_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/log-player-dbus-marshal.h)
	
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/player/server.c
		PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.c)
	LIST(APPEND demo_player_source ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.c)
	LIST(APPEND demo_player_headers ${CMAKE_CURRENT_SOURCE_DIR}/player/log-player-marshal.h)
ENDIF(GENERATE_MARSHAL)


LIST(REMOVE_DUPLICATES ls_source)
LIST(REMOVE_DUPLICATES ls_headers)
LIST(REMOVE_DUPLICATES demo_player_source)
LIST(REMOVE_DUPLICATES demo_player_headers)


IF(BUILD_PLUGINS)
	SET(BUILD_PLUGIN_BASH_HISTORY ON)
	SET(BUILD_PLUGIN_MOD_SECURITY ON)
	SET(BUILD_PLUGIN_OSIRIS ON)
	SET(BUILD_PLUGIN_P0F ON)
	SET(BUILD_PLUGIN_PIGA ON)
	SET(BUILD_PLUGIN_SELINUX ON)
	SET(BUILD_PLUGIN_SNORT ON)
	SET(BUILD_PLUGIN_SNORT_NET ON)
	SET(BUILD_PLUGIN_SYSLOG_SSH ON)
	SET(BUILD_PLUGIN_SYSLOG_WGET ON)
	SET(BUILD_PLUGIN_TEST ON)
	SET(BUILD_PLUGIN_CORRELATION ON)
	## TURN YOUR NEW PLUGIN OPTION TO ON HERE ##
ENDIF(BUILD_PLUGINS)



IF(BUILD_PLUGIN_BASH_HISTORY)
	FILE(GLOB_RECURSE bash_history_source "plugins/bash_history/*.c")
	FILE(GLOB_RECURSE bash_history_headers "plugins/bash_history/*.h")

	ADD_LIBRARY(bash_history MODULE ${bash_history_source} ${bash_history_headers})
	TARGET_LINK_LIBRARIES(bash_history ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(bash_history ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(bash_history ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(bash_history "dl")
	SET_TARGET_PROPERTIES(bash_history PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS bash_history DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/bash_history/data/" DESTINATION ${LS_SHARE_PATH}/plugins/bash_history/)
ENDIF(BUILD_PLUGIN_BASH_HISTORY)



IF(BUILD_PLUGIN_MOD_SECURITY)
	FILE(GLOB_RECURSE mod_security_source "plugins/mod_security/*.c")
	FILE(GLOB_RECURSE mod_security_headers "plugins/mod_security/*.h")

	ADD_LIBRARY(mod_security MODULE ${mod_security_source} ${mod_security_headers})
	TARGET_LINK_LIBRARIES(mod_security ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(mod_security ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(mod_security ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(mod_security "dl")
	SET_TARGET_PROPERTIES(mod_security PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS mod_security DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/mod_security/data/" DESTINATION ${LS_SHARE_PATH}/plugins/mod_security/)
ENDIF(BUILD_PLUGIN_MOD_SECURITY)



IF(BUILD_PLUGIN_OSIRIS)
	FILE(GLOB_RECURSE osiris_source "plugins/osiris/*.c")
	FILE(GLOB_RECURSE osiris_headers "plugins/osiris/*.h")

	ADD_LIBRARY(osiris MODULE ${osiris_source} ${osiris_headers})
	TARGET_LINK_LIBRARIES(osiris ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(osiris ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(osiris ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(osiris "dl")
	SET_TARGET_PROPERTIES(osiris PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS osiris DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/osiris/data/" DESTINATION ${LS_SHARE_PATH}/plugins/osiris/)
ENDIF(BUILD_PLUGIN_OSIRIS)



IF(BUILD_PLUGIN_P0F)
	FILE(GLOB_RECURSE p0f_source "plugins/p0f/*.c")
	FILE(GLOB_RECURSE p0f_headers "plugins/p0f/*.h")

	ADD_LIBRARY(p0f MODULE ${p0f_source} ${p0f_headers})
	TARGET_LINK_LIBRARIES(p0f ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(p0f ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(p0f ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(p0f "dl")
	SET_TARGET_PROPERTIES(p0f PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS p0f DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/p0f/data/" DESTINATION ${LS_SHARE_PATH}/plugins/p0f/)
ENDIF(BUILD_PLUGIN_P0F)



IF(BUILD_PLUGIN_PIGA)
	FILE(GLOB_RECURSE piga_source "plugins/piga/*.c")
	FILE(GLOB_RECURSE piga_headers "plugins/piga/*.h")

	ADD_LIBRARY(piga MODULE ${piga_source} ${piga_headers})
	TARGET_LINK_LIBRARIES(piga ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(piga ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(piga ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(piga "dl")
	SET_TARGET_PROPERTIES(piga PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS piga DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/piga/data/" DESTINATION ${LS_SHARE_PATH}/plugins/piga/)
ENDIF(BUILD_PLUGIN_PIGA)



IF(BUILD_PLUGIN_SELINUX)
	FILE(GLOB_RECURSE selinux_source "plugins/selinux/*.c")
	FILE(GLOB_RECURSE selinux_headers "plugins/selinux/*.h")

	ADD_LIBRARY(selinux MODULE ${selinux_source} ${selinux_headers})
	TARGET_LINK_LIBRARIES(selinux ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(selinux ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(selinux ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(selinux "dl")
	SET_TARGET_PROPERTIES(selinux PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS selinux DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/selinux/data/" DESTINATION ${LS_SHARE_PATH}/plugins/selinux/)
ENDIF(BUILD_PLUGIN_SELINUX)



IF(BUILD_PLUGIN_SNORT)
	FILE(GLOB_RECURSE snort_source "plugins/snort/*.c")
	FILE(GLOB_RECURSE snort_headers "plugins/snort/*.h")

	ADD_LIBRARY(snort MODULE ${snort_source} ${snort_headers})
	TARGET_LINK_LIBRARIES(snort ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(snort ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(snort ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(snort "dl")
	SET_TARGET_PROPERTIES(snort PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS snort DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/snort/data/" DESTINATION ${LS_SHARE_PATH}/plugins/snort/)
ENDIF(BUILD_PLUGIN_SNORT)



IF(BUILD_PLUGIN_SNORT_NET)
	FILE(GLOB_RECURSE snort_net_source "plugins/snort_net/*.c")
	FILE(GLOB_RECURSE snort_net_headers "plugins/snort_net/*.h")

	ADD_LIBRARY(snort_net MODULE ${snort_net_source} ${snort_net_headers})
	TARGET_LINK_LIBRARIES(snort_net ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(snort_net ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(snort_net ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(snort_net "dl")
	SET_TARGET_PROPERTIES(snort_net PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS snort_net DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/snort_net/data/" DESTINATION ${LS_SHARE_PATH}/plugins/snort_net/)
ENDIF(BUILD_PLUGIN_SNORT_NET)



IF(BUILD_PLUGIN_SYSLOG_SSH)
	FILE(GLOB_RECURSE syslog_ssh_source "plugins/syslog_ssh/*.c")
	FILE(GLOB_RECURSE syslog_ssh_headers "plugins/syslog_ssh/*.h")

	ADD_LIBRARY(syslog_ssh MODULE ${syslog_ssh_source} ${syslog_ssh_headers})
	TARGET_LINK_LIBRARIES(syslog_ssh ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(syslog_ssh ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(syslog_ssh ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(syslog_ssh "dl")
	SET_TARGET_PROPERTIES(syslog_ssh PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS syslog_ssh DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/syslog_ssh/data/" DESTINATION ${LS_SHARE_PATH}/plugins/syslog_ssh/)
ENDIF(BUILD_PLUGIN_SYSLOG_SSH)



IF(BUILD_PLUGIN_SYSLOG_WGET)
	FILE(GLOB_RECURSE syslog_wget_source "plugins/syslog_wget/*.c")
	FILE(GLOB_RECURSE syslog_wget_headers "plugins/syslog_wget/*.h")

	ADD_LIBRARY(syslog_wget MODULE ${syslog_wget_source} ${syslog_wget_headers})
	TARGET_LINK_LIBRARIES(syslog_wget ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(syslog_wget ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(syslog_wget ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(syslog_wget "dl")
	SET_TARGET_PROPERTIES(syslog_wget PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS syslog_wget DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/syslog_wget/data/" DESTINATION ${LS_SHARE_PATH}/plugins/syslog_wget/)
ENDIF(BUILD_PLUGIN_SYSLOG_WGET)



IF(BUILD_PLUGIN_TEST)
	FILE(GLOB_RECURSE test_source "plugins/test/*.c")
	FILE(GLOB_RECURSE test_headers "plugins/test/*.h")

	ADD_LIBRARY(test MODULE ${test_source} ${test_headers})
	TARGET_LINK_LIBRARIES(test ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(test ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(test ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(test "dl")
	SET_TARGET_PROPERTIES(test PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS test DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/test/data/" DESTINATION ${LS_SHARE_PATH}/plugins/test/)
ENDIF(BUILD_PLUGIN_TEST)



IF(BUILD_PLUGIN_CORRELATION)
	FILE(GLOB_RECURSE correlation_source "plugins/correlation/*.c")
	FILE(GLOB_RECURSE correlation_headers "plugins/correlation/*.h")

	ADD_LIBRARY(correlation MODULE ${correlation_source} ${correlation_headers})
	TARGET_LINK_LIBRARIES(correlation ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(correlation ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(correlation ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(correlation "dl")
	SET_TARGET_PROPERTIES(correlation PROPERTIES
		COMPILE_FLAGS "-Wall"
		PREFIX ""
		SUFFIX ".so")
	INSTALL(TARGETS correlation DESTINATION ${LS_LIB_PATH})
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/correlation/data/" DESTINATION ${LS_SHARE_PATH}/plugins/correlation/)
ENDIF(BUILD_PLUGIN_CORRELATION)


## COPY AND IF BLOCK HERE AND ADAPT IT TO YOUR NEW PLUGIN ##



IF(BUILD_APP)
	ADD_EXECUTABLE(synema ${ls_source} ${ls_headers})
	TARGET_LINK_LIBRARIES(synema ${gthread_LIBRARIES})
	TARGET_LINK_LIBRARIES(synema ${gtk2_LIBRARIES})
	TARGET_LINK_LIBRARIES(synema ${rsvg_LIBRARIES})
	TARGET_LINK_LIBRARIES(synema ${dbusglib_LIBRARIES})
	TARGET_LINK_LIBRARIES(synema "dl")
	TARGET_LINK_LIBRARIES(synema "tar")
	SET_TARGET_PROPERTIES(synema PROPERTIES
		COMPILE_FLAGS "-Wall -rdynamic")
	
	INSTALL(TARGETS synema DESTINATION ${LS_BIN_PATH})
	ADD_DEFINITIONS(-DCMAKE_BINARY="${LS_BIN_PATH}/synema")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/icons/synema-menu-button.svg" DESTINATION "share/icons/hicolor/scalable/actions/")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/icons/synema-window.svg" DESTINATION "share/icons/hicolor/scalable/apps/")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/frame.ui" DESTINATION ${LS_SHARE_PATH})
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/window.ui" DESTINATION ${LS_SHARE_PATH})
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/synema.conf" DESTINATION ${LS_SHARE_PATH})
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/machine.conf" DESTINATION ${LS_SHARE_PATH})
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/data/synema.1.gz" DESTINATION "share/man/man1/")
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/html/" DESTINATION "${LS_SHARE_PATH}/doc")
ENDIF(BUILD_APP)



IF(BUILD_DEMO_PLAYER)
	ADD_EXECUTABLE(synema-log-player-demo ${demo_player_source} ${demo_player_headers})
	TARGET_LINK_LIBRARIES(synema-log-player-demo ${dbusglib_LIBRARIES})
	SET_TARGET_PROPERTIES(synema-log-player-demo PROPERTIES
		COMPILE_FLAGS "-Wall -g")

	INSTALL(TARGETS synema-log-player-demo DESTINATION ${LS_BIN_PATH})
ENDIF(BUILD_DEMO_PLAYER)



IF(AUTOSTART_DEMO_PLAYER)
	INCLUDE(${CMAKE_SOURCE_DIR}/cmake/modules/PkgConfigGetVar.cmake)
	dbus_add_activation_service(data/fr.ensi_bourges.synema.log_player.service.in)
ENDIF(AUTOSTART_DEMO_PLAYER)



MESSAGE("Application:")
MESSAGE("    Build Application : ${BUILD_APP} (-DBUILD_APP=ON/OFF, default ON)")
MESSAGE("    Build Demo Player : ${BUILD_DEMO_PLAYER} (-DBUILD_DEMO_PLAYER=ON/OFF, default ON)")
MESSAGE("    Auto Start Player : ${AUTOSTART_DEMO_PLAYER} (-DAUTOSTART_DEMO_PLAYER=ON/OFF, default ON)")
MESSAGE("    Debug Mode        : ${DEBUG} (-DDEBUG=ON/OFF, default ON)")
MESSAGE("    Install Prefix    : '${CMAKE_INSTALL_PREFIX}' (-DCMAKE_INSTALL_PREFIX=/your/prefix to modify the prefix to /your/prefix)")
MESSAGE("")

MESSAGE("Plugins: (-DBUILD_PLUGINS=ON to build all plugins)")
MESSAGE("    Bash History      : ${BUILD_PLUGIN_BASH_HISTORY} (-DBUILD_PLUGIN_BASH_HISTORY=ON/OFF, default OFF)")
MESSAGE("    Mod_security      : ${BUILD_PLUGIN_MOD_SECURITY} (-DBUILD_PLUGIN_MOD_SECURITY=ON/OFF, default OFF)")
MESSAGE("    Osiris            : ${BUILD_PLUGIN_OSIRIS} (-DBUILD_PLUGIN_OSIRIS=ON/OFF, default OFF)")
MESSAGE("    P0F               : ${BUILD_PLUGIN_P0F} (-DBUILD_PLUGIN_P0F=ON/OFF, default OFF)")
MESSAGE("    PIGA-IDS          : ${BUILD_PLUGIN_PIGA} (-DBUILD_PLUGIN_PIGA=ON/OFF, default OFF)")
MESSAGE("    SELinux           : ${BUILD_PLUGIN_SELINUX} (-DBUILD_PLUGIN_SELINUX=ON/OFF, default OFF)")
MESSAGE("    Snort             : ${BUILD_PLUGIN_SNORT} (-DBUILD_PLUGIN_SNORT=ON/OFF, default OFF)")
MESSAGE("    Snort (network)   : ${BUILD_PLUGIN_SNORT_NET} (-DBUILD_PLUGIN_SNORT_NET=ON/OFF, default OFF)")
MESSAGE("    Syslog (SSH)      : ${BUILD_PLUGIN_SYSLOG_SSH} (-DBUILD_PLUGIN_SYSLOG_SSH=ON/OFF, default OFF)")
MESSAGE("    Syslog (Wget)     : ${BUILD_PLUGIN_SYSLOG_WGET} (-DBUILD_PLUGIN_SYSLOG_WGET=ON/OFF, default OFF)")
MESSAGE("    Test plugin       : ${BUILD_PLUGIN_TEST} (-DBUILD_PLUGIN_TEST=ON/OFF, default OFF)")
MESSAGE("    Correlation       : ${BUILD_PLUGIN_CORRELATION} (-DBUILD_PLUGIN_CORRELATION=ON/OFF, default OFF)")
## ADD A MESSAGE LINE HERE FOR YOUR NEW PLUGIN ##
MESSAGE("")

MESSAGE("Code Generation (requires extra deps, recommended):")
MESSAGE("    GObject           : ${GENERATE_GOBJECT} (-DGENERATE_GOBJECT=ON/OFF, default OFF)")
MESSAGE("    DBus bindings     : ${GENERATE_DBUS} (-DGENERATE_DBUS=ON/OFF, default OFF)")
MESSAGE("    Player marshals   : ${GENERATE_MARSHAL} (-DGENERATE_MARSHAL=ON/OFF, default OFF)")
MESSAGE("")


MESSAGE("Run make to build the application (make install to install it).")
