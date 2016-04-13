set(LHG_HEADER_FILES
        src/lineentry.h
        src/outputfile.h
        src/misc.h)

set(LHG_SOURCE_FILES
        src/main.cpp
        src/lineentry.cpp
        src/outputfile.cpp
        src/misc.cpp)

add_executable(LibHeaderGen ${LHG_HEADER_FILES} ${LHG_SOURCE_FILES})