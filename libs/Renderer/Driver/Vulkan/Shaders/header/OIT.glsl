#ifndef OIT_GLSL
#define OIT_GLSL

/**
 * @brief OIT rendering was STRONGLY inspired
 * by the work of mklefrancois & al. down at https://github.com/nvpro-samples/vk_order_independent_transparency
 * cudos to them for making such an invaluable resource!
 * Here we're using "Loop32" with minor tweaks to allow for HDR rendering.
 * "Loop32" limits synchronization to a single imageAtomicMin, garantees stability between frames and gives similar result to "Interlock"
 */

#define OIT_LAYERS 5

#endif // OIT_GLSL