add_library(viper)

target_sources(viper PRIVATE
	code.c
	core.c
	viper.c
	rt/file.c
	rt/mem.c
	rt/state.c)

