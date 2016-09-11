cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 NEW)

project(Widgets)
gd_add_extension_includes()

#Defines
###
gd_add_extension_definitions(Widgets)

#The targets
###
include_directories(.)
include_directories(SimplGUI/include)
file(GLOB source_files *)
file(GLOB_RECURSE simplgui_src SimplGUI/src/*)
file(GLOB gui_only_files Dialogs/*)

IF(NO_GUI)
	SET(ide_source_files "${source_files};${simplgui_src}")
ELSE()
	SET(ide_source_files "${source_files};${simplgui_src};${gui_only_files}")
ENDIF()

gd_add_extension_target(Widgets "${ide_source_files}")
gdcpp_add_runtime_extension_target(Widgets_Runtime "${source_files};${simplgui_src}")

#Linker files for the IDE extension
###
gd_extension_link_libraries(Widgets)

#Linker files for the GD C++ Runtime extension
###
gdcpp_runtime_extension_link_libraries(Widgets_Runtime)
