const ivec3 offsets[4] = {
    ivec3(0, 0, 0),
    ivec3(1, 0, 0),
    ivec3(0, 1, 0),
    ivec3(1, 1, 0),
};

layout(location = 0) in vec2 in_UV;

#if CUBE
layout(binding = 0, rg32f) restrict readonly uniform imageCube img_Src;
layout(binding = 1, rg32f) restrict writeonly uniform imageCube img_Dst;
#else
layout(binding = 0, rg32f) restrict readonly uniform image2DArray img_Src;
layout(binding = 1, rg32f) restrict writeonly uniform image2DArray img_Dst;
#endif

void main()
{
#if CUBE
    int layers = 6;
#else
    int layers = imageSize(img_Src).z;
#endif
    ivec2 srcSize  = imageSize(img_Src).xy;
    ivec2 dstSize  = imageSize(img_Dst).xy;
    ivec2 srcCoord = ivec2(gl_FragCoord.xy) * (srcSize / dstSize);
    ivec2 dstCoord = ivec2(gl_FragCoord.xy);
    for (int layer = 0; layer < layers; layer++) {
        float minVal = 100000;
        float maxVal = 0;
        for (int sampleI = 0; sampleI < offsets.length(); sampleI++) {
            ivec3 sampleCoord = ivec3(srcCoord, layer) + offsets[sampleI];
            vec2 inputSample  = imageLoad(img_Src, sampleCoord).xy;
            if (inputSample[0] == -1 || inputSample[1] == -1)
                continue; // avoid sampling empty spaces
            minVal = min(minVal, inputSample[0]);
            maxVal = max(maxVal, inputSample[1]);
        }
        imageStore(img_Dst, ivec3(dstCoord, layer), vec4(minVal, maxVal, 0, 0));
    }
}
