add_executable(viper.cli)
set_target_properties(viper.cli PROPERTIES OUTPUT_NAME viper)

target_sources(viper.cli PRIVATE
  main.c)
target_link_libraries(viper.cli viper)
