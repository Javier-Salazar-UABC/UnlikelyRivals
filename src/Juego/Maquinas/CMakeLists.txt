project(CimaEngine3v)

#todos los archivos agregados
file(GLOB_RECURSE gl_cpp RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(CimaEngine3v PRIVATE ${gl_cpp})

#agregar el directorio donde esta los fsm de las naves
add_subdirectory(Naves)
#hacer ^ lo mismo con los folders de enemigos y bosses más adelante
add_subdirectory(Bosses)
add_subdirectory(Balas)