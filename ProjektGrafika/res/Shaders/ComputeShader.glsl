#version 460 core

const float MAX_FLOAT = 3.402823466e+38;


struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Material
{
    vec3 albedo;
    float roughness;
};

struct Sphere 
{
    vec3 position;
    float radius;
    
    Material material;
};

struct Light
{
    vec3 direction;
    vec3 color;
};

layout(rgba32f, binding = 0) uniform writeonly image2D outputImage;
layout(local_size_x = 16, local_size_y = 16) in;
layout(std430, binding = 0) buffer SpheresBuffer {
    Sphere spheres[];
};

uniform vec3 u_CameraPosition;
uniform mat4 u_InverseProjection;
uniform mat4 u_InverseView;
uniform int u_NumOfSpheres;





vec3 RayAt(Ray ray, float t)
{
    return ray.direction * t + ray.origin;
}

float Intersection(Ray ray, Sphere sphere)
{
    vec3 oc = ray.origin - sphere.position;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;

    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) 
        return -1.0;

    float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
    float t2 = (-b + sqrt(discriminant)) / (2.0 * a);
    
    if (t1 > 0.0 && t2 > 0.0)
    {
        return min(t1, t2); 
    }
    else if (t1 > 0.0)
    {
        return t1;
    }
    else if (t2 > 0.0)
    {
        return t2;
    }
    else
    {
        return -1.0;
    }
}


void main()
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    if (pixelCoord.x >= imageSize(outputImage).x || pixelCoord.y >= imageSize(outputImage).y)
		return;

    ivec2 texSize = imageSize(outputImage);
    vec2 fTexSize = vec2(texSize);
    vec2 normalizedCoord = vec2(pixelCoord) / vec2(texSize);

    normalizedCoord = normalizedCoord * 2.0 - 1.0;

    float aspectRatio = float(texSize.x) / float(texSize.y);

    vec4 target = u_InverseProjection * vec4(normalizedCoord.x, normalizedCoord.y, 1, 1);

    Ray ray;
    ray.origin = u_CameraPosition;
    ray.direction = vec3(u_InverseView * vec4(normalize(vec3(target) / target.w), 0));

    Light lightsource = Light(normalize(vec3(-1.0, -1.0, -1.0)), vec3(1.0));


    
    vec4 color = vec4(1.0);
    float hitDistance = MAX_FLOAT;
    Sphere closestSphere;
    bool hit = false;

    for (int i = 0; i < u_NumOfSpheres; i++)
    {
        float t = Intersection(ray, spheres[i]);

        if (t < 0.0)
        {
            continue;
        }

        if (t < hitDistance)
        {
            closestSphere = spheres[i];
            hitDistance = t;
            hit = true;
        }
    }


    if (hit)
    {
        vec3 normal = normalize(RayAt(ray, hitDistance) - closestSphere.position); 
        //normal = normal * 0.5 + 0.5;

        float d = max(dot(normal, -lightsource.direction), 0.0);


        color = vec4(closestSphere.material.albedo * d, 1.0);
    }
    else
    {
        vec3 unit_direction = normalize(ray.direction);
        float a = 0.5 * (unit_direction.y + 1.0);
        color = vec4(mix(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), a), 1.0);

    }

    imageStore(outputImage, pixelCoord, color);
}