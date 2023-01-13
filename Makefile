objects = drm_tools.o
cc = aarch64-linux-gnu-gcc
drmconfig = `pkg-config --cflags libdrm` `pkg-config --libs libdrm`

drm_tool_app : $(objects)
	$(cc) -o drm_tool_app $(objects) $(drmconfig)

drm_tools.o : drm_tools.c
	$(cc) -c drm_tools.c $(drmconfig)

.PHONY : clean
clean :
	-rm drm_tool_app $(objects)
