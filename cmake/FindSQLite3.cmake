find_path(SQLite3_INCLUDE_DIRS NAMES sqlite3.h HINTS ${CMAKE_SYSTEM_INCLUDE_PATH})
 
find_library(SQLite3_LIBRARIES NAMES sqlite3 "libsqlite3.a" HINTS ${CMAKE_SYSTEM_LIBRARY_PATH})
 
include(FindPackageHandleStandardArgs)
 
find_package_handle_standard_args(SQLite3 DEFAULT_MSG SQLite3_LIBRARIES SQLite3_INCLUDE_DIRS)