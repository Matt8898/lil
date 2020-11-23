#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t GpuAddr;
struct LilGpu;

typedef enum LilConnectorType {
    HDMI,
    LVDS,
    DISPLAYPORT,
} LilConnectorType;

typedef struct LilModeInfo {
    uint32_t clock;
    int vactive;
	int vsyncStart;
	int vsyncEnd;
	int vtotal;
	int hactive;
	int hsyncStart;
	int hsyncEnd;
	int htotal;
} LilModeInfo;

/*
 * Note: intel gpus only have one valid (connector, encoder, crtc)
 */

typedef enum LilPlaneType {
    PRIMARY, CURSOR, SPRITE,
} LilPlaneType;

//generic plane
typedef struct LilPlane {
    LilPlaneType type;
    GpuAddr surface_address;
} LilPlane;

struct LilConnector;

typedef struct LilCrtc {
    LilModeInfo current_mode;
    struct LilConnector* connector;

    uint32_t pipe_id;

    uint32_t num_planes;
    LilPlane* planes;

    void (*commit_modeset) (struct LilGpu* gpu, struct LilCrtc* crtc);
} LilCrtc;

typedef struct LilConnectorInfo {
    uint32_t num_modes;
    LilModeInfo* modes;
} LilConnectorInfo;

typedef struct LilMinMax {
    int min, max;
} LilMinMax;

typedef struct LilLimit {
    int dot_limit;
    int slow, fast;
} LilLimit;

typedef struct PllLilLimits {
    LilMinMax dot, vco, n, m, m1, m2, p, p1;
    LilLimit p2;
} PllLilLimits;

typedef struct LilConnector {
    bool (*is_connected) (struct LilGpu* gpu, struct LilConnector* connector);
    LilConnectorInfo (*get_connector_info) (struct LilGpu* gpu, struct LilConnector* connector);
    void (*set_state) (struct LilGpu* gpu, struct LilConnector* connector, uint32_t state);
    uint32_t (*get_state) (struct LilGpu* gpu, struct LilConnector* connector);

    uint32_t id;
    LilConnectorType type;

    PllLilLimits limits;

    LilCrtc* crtc;

    bool on_pch;
} LilConnector;

typedef struct LilGpu {
    uint32_t num_connectors;
    LilConnector* connectors;

    uintptr_t gpio_start;
    uintptr_t mmio_start;
    uintptr_t vram;
} LilGpu;

/*
 * Fills out all the structures relating to the gpu
 *
 * The general modesetting procedure consists of the following:
 * - initialize the gpu
 * - set current_mode in the crtc you want to modeset
 * - allocate memory for the planes that are going to be used
 * - set the surface_address for the planes
 * - call commit_modeset
 */
void lil_init_gpu(LilGpu* ret, void* pci_device);