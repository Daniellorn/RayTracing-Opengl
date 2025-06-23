#version 460 core

const float MAX_FLOAT = 3.402823466e+38;
const uint MAX_UINT = 4294967295;
const float EPSILON = 0.001;
const int MAX_OBJECTS = 100;
const float PI = 3.141592;
const float invPI = 1 / PI;


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

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float reach;
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
uniform int u_MAX_SAMPLES;
uniform int u_NumOfSpheres;
uniform bool u_SETTINGS;
uniform int u_FrameIndex;

DirectionalLight DLight = DirectionalLight(normalize(vec3(-1.0, -1.0, -1.0)), vec3(0.9, 0.2, 0.3), 1.0);
PointLight PLight = PointLight(vec3(0.0, 15.0, 0.0), vec3(1.0), 80.0, 50.0);


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

vec3 RandomVec3OnUnitHemiSphere(inout uint seed, vec3 normal)
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

vec3 DirectionalLightContribution(DirectionalLight Light, HitInfo info, Material mat)
{
    vec3 L = -Light.direction;
    vec3 N = info.normal;
    float NdotL = max(dot(N,L), 0.0);

    vec3 radiance = Light.color * Light.intensity * NdotL;

    return radiance * vec3(mat.albedo.xyz);
}

vec3 PointLightContribution(PointLight Light, HitInfo info, Material mat)
{
    vec3 L = Light.position - info.point;
    vec3 N = info.normal;
    float Distance = length(L);

    if (Distance >= Light.reach)
        return vec3(0.0);

    float NdotL = max(dot(N,normalize(L)), 0.0);

    float attenuation = 1.0 / (Distance * Distance);
    vec3 radiance = Light.color * Light.intensity * NdotL * attenuation;

    return radiance * vec3(mat.albedo.xyz);
}

vec3 GetEmission(Material material)
{
    vec4 result = material.EmissionColor * material.EmissionPower;

    return vec3(result.x, result.y, result.z);
}

float D(vec3 normal, vec3 H, float roughness) //DistributionFunctionGXX
{
    float roughenss2 = pow(roughness, 2.0);
    float NdotH = max(dot(normal, H), 0.0);
    float NdotH2 = pow(NdotH, 2.0);

    float denom = PI * (NdotH2 * (roughenss2 - 1) + 1);
    denom = max(denom, 0.0001);

    return roughenss2 / pow(denom, 2.0);
}

float Gshlick(vec3 normal, vec3 V, float roughenss)
{
    float NdotV = max(dot(normal,V), 0.0);
    float k = roughenss / 2.0;

    return NdotV / (NdotV * (1 - k) + k);
}

float G(float roughenss, vec3 normal, vec3 V, vec3 lightdir)
{
    return Gshlick(normal, V, roughenss) * Gshlick(normal, lightdir, roughenss);
}

vec3 FresnelSchlick(vec3 albedo, vec3 V, vec3 H)
{
    return albedo + (1 - albedo) * pow(1 - max(dot(V, H), 0.0), 5.0);
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

HitInfo CheckIntersection(Ray ray)
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

bool InShadow(vec3 origin, vec3 directionToLight, float maxDistance)
{
    Ray shadowRay;
    shadowRay.origin = origin + directionToLight * EPSILON;
    shadowRay.direction = directionToLight;

    for (int i = 0; i < u_NumOfSpheres; i++)
    {
        float t = Intersection(shadowRay, spheres[i]);

        if (t > 0.0 && t < maxDistance)
        {
            Material mat = materials[spheres[i].materialIndex];

            if (mat.EmissionPower > 0.0)
                continue;

            return true;
        }
    }

    return false;
}


vec3 TraceRay(Ray ray, inout uint seed)
{
    vec3 light = vec3(0.0);
    vec3 contribution = vec3(1.0);

    for (int i = 0; i < u_MAX_BOUNCE; i++)
    {
        HitInfo hitInfo = CheckIntersection(ray);

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

        vec3 normal = normalize(hitInfo.point - vec3(closestSphere.position.xyz)); 
        
        hitInfo.normal = normal;

        light += GetEmission(closestSphereMaterial) * contribution;

        vec3 lightDir = PLight.position - hitInfo.point;
        float lightDist = length(lightDir);

        //if (!InShadow(hitInfo.point, normalize(lightDir), lightDist))
        //{
        //    light += PointLightContribution(PLight, hitInfo, closestSphereMaterial) * contribution;
        //}

        vec3 albedo = vec3(closestSphereMaterial.albedo.xyz);
        contribution *= albedo; //* invPI;


        ray.origin = hitInfo.point + hitInfo.normal * EPSILON;
        ray.direction = normalize(RandomVec3OnUnitHemiSphere(seed, hitInfo.normal)) ; //diffuse

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

    vec3 totalColor = vec3(0.0);

    for (int i = 0; i < u_MAX_SAMPLES; i++)
    {
        vec3 jitteredDirection = ray.direction;
        jitteredDirection += RandomVec3(seed, -0.001, 0.001);
        
        Ray jitteredRay = Ray(ray.origin, jitteredDirection);

        totalColor += TraceRay(jitteredRay, seed);
    }

    vec3 avgColor = totalColor / float(u_MAX_SAMPLES);

    if (u_SETTINGS)
    {

        vec3 blended = mix(accumulatedColor.rgb, avgColor, 1.0 / float(u_FrameIndex));
        imageStore(accumulationImage, pixelCoord, vec4(blended, 1.0));
        imageStore(outputImage, pixelCoord, vec4(blended, 1.0));

    }
    else
    {
        //vec3 color = TraceRay(ray, seed);
        imageStore(outputImage, pixelCoord, vec4(avgColor, 1.0));
    }

}