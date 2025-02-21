# Adds runtimes files that will be copied next to the target's file
# a_Target : the target name
# a_Files  : a list of relative paths to the resource files
function(AddRuntimeFiles a_Target a_Files)
  set(TargetPath $<TARGET_FILE_DIR:${a_Target}>)
  set(InputFiles ${a_Files} ${ARGN})
  foreach(InputFile ${InputFiles})
    set(InputFilePath ${CMAKE_CURRENT_SOURCE_DIR}/${InputFile})
    set(OutputFilePath ${TargetPath}/${InputFile})
    add_custom_command(TARGET ${a_Target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${InputFilePath} ${OutputFilePath}
      COMMAND ${CMAKE_COMMAND} -E echo "${a_Target} -> ${InputFilePath} -> ${OutputFilePath}")
  endforeach()
endfunction()
