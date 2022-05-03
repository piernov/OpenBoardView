PROJECT(SQlite)
cmake_minimum_required(VERSION 2.8.12)

# See https://github.com/android-ndk/ndk/issues/332
if(ANDROID)
	add_definitions(-D_FILE_OFFSET_BITS=32)
endif()

add_library(SQLite3 STATIC sqlite3.c)
target_include_directories(SQLite3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
if(NOT WIN32 AND NOT ANDROID)
	target_link_libraries(SQLite3 pthread)
#install(TARGETS sqlite3 DESTINATION ${INSTALL_ARCHIVE_DIR}) # No need to install a static lib
endif()

add_library(SQLite::SQLite3 ALIAS SQLite3)
set(SQLite3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
set(SQLite3_LIBRARIES "sqlite3" PARENT_SCOPE)
