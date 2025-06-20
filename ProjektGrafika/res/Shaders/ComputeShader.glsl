#version 460 core

const float MAX_FLOAT = 3.402823466e+38;
const uint MAX_UINT = 4294967295;
const float EPSILON = 0.001;
const int MAX_OBJECTS = 100;


struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Material
{
    vec4 albedo; //0-15
    float roughness; //16-19
    float metallic; //20-23

    //padding 24-31

    vec4 EmissionColor; //32-47
    float EmissionPower; //48-51
	//alignment 52-64
};

struct Sphere 
{
    vec4 position; //0-15
    float radius; // 16-19
    
    int materialIndex; //20-23
};

struct Light
{
    vec3 direction;
    vec3 color;
};

struct HitInfo
{
    vec3 point;
    vec3 normal;   
    float t;
    float hitDistance;
    int objectIndex;
    int materialIndex;
};

layout(rgba32f, binding = 0) uniform writeonly image2D outputImage;
layout(rgba32f, binding = 1) uniform image2D accumulationImage;
layout(local_size_x = 16, local_size_y = 16) in;
layout(std140, binding = 0) uniform SpheresBuffer {
    Sphere spheres[MAX_OBJECTS];
};
layout(std140, binding = 1) uniform MaterialsBuffer {
    Material materials[MAX_OBJECTS];
};


uniform vec3 u_CameraPosition;
uniform mat4 u_InverseProjection;
uniform mat4 u_InverseView;
uniform uint u_Time;
uniform int u_MAX_BOUNCE;
uniform int u_NumOfSpheres;
uniform bool u_SETTINGS;
uniform int u_FrameIndex;

//Light lightsource = Light(normalize(vec3(-1.0, -1.0, -1.0)), vec3(1.0));


uint PCG_Hash(uint hash)
{
    uint state = hash * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float RandomFloat(inout uint seed)
{
    seed = PCG_Hash(seed);
    return float(seed) / MAX_UINT;
}

float RandomRange(inout uint seed, float min, float max)
{
    return mix(min, max, RandomFloat(seed));
}

vec3 RandomUnitVec(inout uint seed)
{
    return normalize(vec3(
        RandomFloat(seed) * 2.0 - 1.0,
        RandomFloat(seed) * 2.0 - 1.0,
        RandomFloat(seed) * 2.0 - 1.0));
}

vec3 RandomVec3OnUnitSphere(inout uint seed, vec3 normal)
{
    vec3 randomvec = RandomUnitVec(seed);

    if (dot(randomvec, normal) > 0.0)
    {
        return randomvec;
    }
    else
    {
        return -randomvec;
    }
}

vec3 RandomVec3(inout uint seed, float min, float max)
{
    return vec3(
        RandomRange(seed, min, max),
        RandomRange(seed, min, max),
        RandomRange(seed, min, max)
    );
}

vec3 GetEmission(Material material)
{
    vec4 result = material.EmissionColor * material.EmissionPower;

    return vec3(result.x, result.y, result.z);
}


vec3 RayAt(Ray ray, float t)
{
    return ray.direction * t + ray.origin;
}

float Intersection(Ray ray, Sphere sphere)
{
    vec3 oc = ray.origin - vec3(sphere.position.x, sphere.position.y, sphere.position.z);

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

HitInfo Miss()
{
    HitInfo hitInfo;
    hitInfo.hitDistance = -1.0;

    return hitInfo;
}

HitInfo TraceRay(Ray ray)
{
    HitInfo hitInfo;
    hitInfo.hitDistance = MAX_FLOAT;
    hitInfo.objectIndex = -1;

    for (int i = 0; i < u_NumOfSpheres; i++)
    {
        float t = Intersection(ray, spheres[i]);

        if (t < 0.0)
        {
            continue;
        }

        if (t < hitInfo.hitDistance)
        {
            hitInfo.objectIndex = i;
            hitInfo.hitDistance = t;
            hitInfo.t = t;
            hitInfo.point = RayAt(ray, t);
        }
    }

    if (hitInfo.objectIndex < 0)
    {
        HitInfo miss = Miss();
        return miss;
    }

    return  hitInfo;
}

bool InShadow(HitInfo hitInfo, Light lightSource)
{
    Ray shadowRay;
    shadowRay.origin = hitInfo.point + hitInfo.normal * EPSILON;
    shadowRay.direction = -lightSource.direction;

    for (int i = 0; i < u_NumOfSpheres; i++)
    {
        float t = Intersection(shadowRay, spheres[i]);

        if (t < 0.0)
        {
            continue;
        }

        if (t > 0.0)
        {
            return true;
        }
    }

    return false;
}


vec3 BounceRay(Ray ray, inout uint seed)
{
    vec3 light = vec3(0.0);
    vec3 contribution = vec3(1.0);

    for (int i = 0; i < u_MAX_BOUNCE; i++)
    {
        HitInfo hitInfo = TraceRay(ray);

        if (hitInfo.hitDistance < 0.0)
        {
            vec3 unit_direction = normalize(ray.direction);
            float a = 0.5 * (unit_direction.y + 1.0);
            vec3 skyColor = vec3(mix(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), a));
            //light += skyColor * contribution;
            break;
        }
    
    
        Sphere closestSphere = spheres[hitInfo.objectIndex];
        Material closestSphereMaterial = materials[closestSphere.materialIndex];

        vec3 normal = normalize(hitInfo.point - vec3(closestSphere.position.x, closestSphere.position.y, closestSphere.position.z)); 
        
        hitInfo.normal = normal;

        light += GetEmission(closestSphereMaterial) * contribution;

        vec3 albedo = vec3(closestSphereMaterial.albedo.x, closestSphereMaterial.albedo.y, closestSphereMaterial.albedo.z);
        contribution *= albedo;


        ray.origin = hitInfo.point + hitInfo.normal * EPSILON;
        
        ray.direction = normalize(hitInfo.normal + RandomUnitVec(seed)); //diffuse
        
        //ray.direction = reflect(ray.direction, 
        //hitInfo.normal + closestSphereMaterial.roughness * RandomVec3(seed, -0.5, 0.5)); //pseudospecular
        //ray.direction  - 2 * dot(ray.direction, normal) * normal;

    }

    return light;
}


void main()
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    if (pixelCoord.x >= imageSize(outputImage).x || pixelCoord.y >= imageSize(outputImage).y)
		return;
    
    uint seed = pixelCoord.x + pixelCoord.y * imageSize(outputImage).x;
    seed *= u_FrameIndex;


    vec4 accumulatedColor = imageLoad(accumulationImage, pixelCoord);

    ivec2 texSize = imageSize(outputImage);
    vec2 fTexSize = vec2(texSize);
    vec2 normalizedCoord = vec2(pixelCoord) / vec2(texSize);

    normalizedCoord = normalizedCoord * 2.0 - 1.0;

    float aspectRatio = float(texSize.x) / float(texSize.y);

    vec4 target = u_InverseProjection * vec4(normalizedCoord.x, normalizedCoord.y, 1, 1);

    Ray ray;
    ray.origin = u_CameraPosition;
    ray.direction = vec3(u_InverseView * vec4(normalize(vec3(target) / target.w), 0));

    if (u_SETTINGS)
    {
        vec3 color = BounceRay(ray, seed);

        vec3 blended = mix(accumulatedColor.rgb, color, 1.0 / float(u_FrameIndex));
        imageStore(accumulationImage, pixelCoord, vec4(blended, 1.0));
        imageStore(outputImage, pixelCoord, vec4(blended, 1.0));

    }
    else
    {
        vec3 color = BounceRay(ray, seed);
        imageStore(outputImage, pixelCoord, vec4(color, 1.0));
    }
}