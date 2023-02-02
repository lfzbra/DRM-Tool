#ifndef __DRM_QUERY_H__
#define __DRM_QUERY_H__

#include "drm_tools.h"
// #include <drm_mode.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


struct device {
	int fd;

	struct resources *resources;

	struct {
		unsigned int width;
		unsigned int height;

		unsigned int fb_id;
		struct bo *bo;
		struct bo *cursor_bo;
	} mode;
};

struct drm_format_modifier_blob {
#define FORMAT_BLOB_CURRENT 1
	/* Version of this blob format */
	__u32 version;

	/* Flags */
	__u32 flags;

	/* Number of fourcc formats supported */
	__u32 count_formats;

	/* Where in this blob the formats exist (in bytes) */
	__u32 formats_offset;

	/* Number of drm_format_modifiers */
	__u32 count_modifiers;

	/* Where in this blob the modifiers exist (in bytes) */
	__u32 modifiers_offset;

	/* __u32 formats[] */
	/* struct drm_format_modifier modifiers[] */
};

struct drm_format_modifier {
	/* Bitmask of formats in get_plane format list this info applies to. The
	 * offset allows a sliding window of which 64 formats (bits).
	 *
	 * Some examples:
	 * In today's world with < 65 formats, and formats 0, and 2 are
	 * supported
	 * 0x0000000000000005
	 *		  ^-offset = 0, formats = 5
	 *
	 * If the number formats grew to 128, and formats 98-102 are
	 * supported with the modifier:
	 *
	 * 0x0000007c00000000 0000000000000000
	 *		  ^
	 *		  |__offset = 64, formats = 0x7c00000000
	 *
	 */
	__u64 formats;
	__u32 offset;
	__u32 pad;

	/* The modifier that applies to the >get_plane format list bitmask. */
	__u64 modifier;
};



int mod_open(const char *device, const char *module);
void dump_framebuffers(struct device *dev);
void dump_crtcs(struct device *dev);
void dump_connectors(struct device *dev);
void dump_encoders(struct device *dev);
void dump_planes(struct device *dev);
struct resources *get_resources(struct device *dev);
void free_resources(struct resources *res);

#endif // !__DRM_QUERY_H__
