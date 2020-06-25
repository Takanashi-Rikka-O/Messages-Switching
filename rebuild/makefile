#	Special all target.
all:prompt

prompt:
	@echo "Please use commands 'make mssw_release' and 'make mssw_debug' to make application for not as same version."

#	Fake target
.PHONY:clean
.PHONY:mssw_release mssw_debug

#	Define work for target mssw_release
mssw_release:
	@make --makefile=./makefile_mssw_release

#	Define work for target mssw_debug
mssw_debug:
	@make --makefile=./makefile_mssw_debug
#	Define work for fake target clean
clean:
	@-rm -f *.o

	
