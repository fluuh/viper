target_sources(viper PRIVATE
	vp/code.c
	vp/core.c
	vp/viper.c
	vp/rt/file.c
	vp/rt/mem.c
	vp/rt/state.c)

target_sources(viper.cli PRIVATE
	cli/main.c)
