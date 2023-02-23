objects = drm_tools.o drm_query.o drm_set.o drm_mem.o
cc = aarch64-linux-gnu-gcc
drmconfig = `pkg-config --cflags libdrm` `pkg-config --libs libdrm`

drm_tool_app : $(objects)
	$(cc) -o drm_tool_app $(objects) $(drmconfig)

drm_tools.o : drm_tools.c drm_tools.h 
	$(cc) -c drm_tools.c $(drmconfig)

drm_query.o : drm_query.c drm_query.h
	$(cc) -c drm_query.c $(drmconfig)

drm_set.o : drm_set.c drm_set.h
	$(cc) -c drm_set.c $(drmconfig)

drm_mem.o : drm_mem.c drm_mem.h
	$(cc) -c drm_mem.c $(drmconfig)

.PHONY : clean
clean :
	rm drm_tool_app $(objects)
