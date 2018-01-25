set(HEADER_FILES Matrix.hpp Rand.hpp)
set(SOURCE_FILES CorrectnessTests.cpp)

include_directories(../eigen3)
add_executable(CorrectnessTests ${HEADER_FILES} CorrectnessTests.cpp)

if(WIN32 AND MSVC) # Check if we are using the Visual Studio compiler
	set_target_properties(CorrectnessTests PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()
