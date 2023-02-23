#ifndef __DRM_SET_H__
#define __DRM_SET_H__

#include "drm_tools.h"
#include "drm_query.h"

int show_rail(struct drmtool_device *dev);
int show_column(struct drmtool_device *device);

#endif // !__DRM_SET_H__