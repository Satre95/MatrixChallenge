set(HEADER_FILES Matrix.hpp)
set(SOURCE_FILES CorrectnessTests.cpp)

link_libraries(glm)
add_executable(CorrectnessTests CorrectnessTests.cpp)

if(WIN32 AND MSVC) # Check if we are using the Visual Studio compiler
	set_target_properties(CorrectnessTests PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()