cmake_minimum_required(VERSION 2.8)

set(llvmConfig "llvm-config")

execute_process(
	COMMAND ${llvmConfig} --cxxflags
	OUTPUT_VARIABLE llvmCxxFlags
	)

execute_process(
	COMMAND ${llvmConfig} --libs
	OUTPUT_VARIABLE llvmLibs
	)

execute_process(
	COMMAND ${llvmConfig} --ldflags
	OUTPUT_VARIABLE llvmLdFlags
	)

string(STRIP "${llvmCxxFlags}" llvmCxxFlags)
string(REPLACE " " ";" llvmCxxFlags "${llvmCxxFlags}")

string(STRIP "${llvmLibs}" llvmLibs)
string(REPLACE " " ";" llvmLibs "${llvmLibs}")

string(STRIP "${llvmLdFlags}" llvmLdFlags)
string(REPLACE " " ";" llvmLdFlags "${llvmLdFlags}")


set(HostCpuType ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "Host CPU Type: ${HostCpuType}")

set(CC ${CMAKE_C_COMPILER})
set(CXX ${CMAKE_CXX_COMPILER})
set(CXXFLAGS "-std=c++11" "-DDEBUG" "-g" ${llvmCxxFlags})
set(LDFLAGS ${llvmLdFlags} ${llvmLibs} )

#directories
set(RingRoot "${CMAKE_SOURCE_DIR}")
set(SrcRoot "${CMAKE_SOURCE_DIR}/src")
set(BuildRoot "${CMAKE_BINARY_DIR}")
