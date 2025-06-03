#version 460 core

layout(rgba32f, binding = 0) uniform writeonly image2D outputImage;
layout(local_size_x = 16, local_size_y = 16) in;

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
uniform vec3 cameraFront;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Sphere 
{
    vec3 position;
    vec3 albedo;
    float radius;
};

struct Light
{
    vec3 direction;
    vec3 color;
};

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

    normalizedCoord.x *= aspectRatio;

    Ray ray;
    ray.origin = cameraPosition;
    float fov = radians(45.0);

    float px = normalizedCoord.x * tan(fov / 2.0);
    float py = normalizedCoord.y * tan(fov / 2.0);

    ray.direction = normalize(cameraFront + px * cameraRight + py * cameraUp);


    Sphere sphere = Sphere(vec3(0.0, 0.0, -1.0), vec3(1.0, 0.0, 1.0), 1.5);

    Light lightsource = Light(normalize(vec3(-1.0, -1.0, -1.0)), vec3(1.0));


    
    vec4 color = vec4(1.0);

    float t = Intersection(ray, sphere);

    if (t > 0.0)
    {
        vec3 normal = normalize(RayAt(ray, t) - sphere.position); 
        //normal = normal * 0.5 + 0.5;

        float d = max(dot(normal, -lightsource.direction), 0.0);


        color = vec4(sphere.albedo * d, 1.0);
    }
    else
    {
        vec3 unit_direction = normalize(ray.direction);
        float a = 0.5 * (unit_direction.y + 1.0);
        color = vec4(mix(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), a), 1.0);

    }

    imageStore(outputImage, pixelCoord, color);
}