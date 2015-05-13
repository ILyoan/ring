set(target ringBin)
set(outDir ${BuildRoot}/bin)

macro(buildDriver driverName)
	set(outTarget target${driverName})
	set(outBin ${outDir}/${driverName})
	set(src ${SrcRoot}/driver/${driverName}.cpp)

	add_custom_command(
		OUTPUT ${outBin}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${outDir}
		COMMAND ${CXX} ${CXXFLAGS} -o ${outBin} ${src} ${outLibs} ${LDFLAGS}
			    && touch ${outBin}
		DEPENDS ${src} ${outLibs}
		)

	add_custom_target(
		${outTarget}
		DEPENDS ${outBin} ${outLibs}
		)
	add_dependencies(${outTarget} ${outLibTargets})

	set(outBinTargets ${outBinTargets} ${outTarget})

endmacro(buildDriver)

buildDriver(ringci)
buildDriver(ringc)

add_custom_target(ring ALL)
add_dependencies(ring ${outBinTargets})