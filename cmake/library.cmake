macro(prepareLib libname)
	set(lib lib${libname})
	set(${lib}Dir ${SrcRoot}/${libname})
	file(GLOB_RECURSE ${lib}Srcs
		${${lib}Dir}/*.cpp
		)
	file(GLOB_RECURSE ${lib}Headers
		${${lib}Dir}/*.h
		)
endmacro(prepareLib)

macro(buildLib libname)
	set(lib lib${libname})
	set(outDir ${BuildRoot}/lib)
	set(outLib ${outDir}/${lib}.so)
	set(srcs ${${lib}Srcs})
	set(headers ${${lib}Headers})

	foreach(src ${srcs})
		get_filename_component(cext ${src} EXT)
		get_filename_component(cpath ${src} PATH)
		get_filename_component(cname ${src} NAME_WE)

		set(obj ${outDir}/${cname}.o)

		add_custom_command(
			OUTPUT ${obj}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${outDir}
			COMMAND ${CXX} ${CXXFLAGS} -fPIC -c ${src} -o ${obj}
			        && touch ${obj}
			DEPENDS ${src} ${headers}
			)
		add_custom_target(
			${cname}
			DEPENDS ${obj}
			)

		set(targets ${targets} ${cname})
		set(outObjs ${outObjs} ${obj})
	endforeach(src)

	add_custom_command(
		OUTPUT ${outLib}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${outDir}
		COMMAND ${CXX} ${CXXFLAGS} -shared -o ${outLib} ${outObjs} ${LDFLAGS}
			    && touch ${outLib}
		DEPENDS ${outObjs}
		)

	add_custom_target(
		${lib}
		DEPENDS ${outLib} ${targets}
		)

	set(outLibs ${outLibs} ${outLib})
	set(outLibTargets ${outLibTargets} ${lib})
endmacro(buildLib)


prepareLib(ringrt)
buildLib(ringrt)

prepareLib(ringc)
buildLib(ringc)

#prepareLib(ringi)
#buildLib(ringi)


add_dependencies(libringc ringrt)
#add_dependencies(libringi libringc)

add_custom_target(liball ALL)
add_dependencies(liball ${outLibTargets})
