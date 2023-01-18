objects = drm_tools.o drm_query.o
cc = aarch64-linux-gnu-gcc
drmconfig = `pkg-config --cflags libdrm` `pkg-config --libs libdrm`

drm_tool_app : $(objects)
	$(cc) -o drm_tool_app $(objects) $(drmconfig)

drm_tools.o : drm_tools.c drm_tools.h 
	$(cc) -c drm_tools.c $(drmconfig)

drm_query.o : drm_query.c drm_query.h
	$(cc) -c drm_query.c $(drmconfig)

.PHONY : clean
clean :
	rm drm_tool_app $(objects)
