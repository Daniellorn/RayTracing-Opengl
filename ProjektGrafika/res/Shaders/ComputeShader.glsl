#version 460 core

layout(rgba32f, binding = 0) uniform writeonly image2D outputImage;

layout(local_size_x = 16, local_size_y = 16) in;
void main()
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    if (pixelCoord.x >= imageSize(outputImage).x || pixelCoord.y >= imageSize(outputImage).y)
		return;

    ivec2 texSize = imageSize(outputImage);
    vec2 fTexSize = vec2(texSize);
    vec2 normalizedCoord = vec2(pixelCoord) / vec2(texSize);
    
    vec4 color = vec4(normalizedCoord, 0.0, 1.0);
    imageStore(outputImage, pixelCoord, color);
}