#version 330
#define M_PI 3.1415926535897932384626433832795

in vec4 fragmentColor;
in vec4 vertexWorldPosition;
in vec3 vertexWorldNormal;

out vec4 finalColor;

uniform InputMaterial {
    /*
    vec4 matDiffuse;
    vec4 matSpecular;
    float matShininess;
    vec4 matAmbient;
    */
    float metallic;
    float roughness;
    float specular;

} material;

struct LightProperties {
    //vec4 diffuseColor;
    //vec4 specularColor;
    vec4 color;
};
uniform LightProperties genericLight;

struct PointLight {
    vec4 pointPosition;
};
uniform PointLight pointLight;

uniform vec4 cameraPosition;

uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;

uniform int lightingType;


vec4 pointLightSubroutine(vec4 worldPosition, vec3 worldNormal)
{
/*
    // Normal to the surface
    vec4 N = vec4(normalize(worldNormal), 0.f);
    
    // Direction from the surface to the light
    vec4 L = normalize(pointLight.pointPosition - worldPosition);

    // Direction from the surface to the eye
    vec4 E = normalize(cameraPosition - worldPosition);

    // Direction of maximum highlights (see paper!)
    vec4 H = normalize(L + E);

    // Amount of diffuse reflection
    float d = max(0, dot(N, L));
    vec4 diffuseColor = d * genericLight.diffuseColor * material.matDiffuse;
    
    // Amount of specular reflection
    float s = pow(max(0, dot(N, H)), material.matShininess);
    vec4 specularColor = s * genericLight.specularColor * material.matSpecular;

    return diffuseColor + specularColor;
*/


    ////////////////////////////////////////////////////////// 
    ////////////// calculate the diffuse color, d //////////// 

    // the base color passed to the material
    vec3 c_base = vec3(fragmentColor);

    // the diffuse color of the material
    vec3 c_diff = vec3((1 - material.metallic) * c_base);

    // the final diffuse color (Lambertian diffuse)
    vec3 d = c_diff / M_PI;

    ////////////////////////////////////////////////////////// 
    //////////// calculate the specular color, s //////////// 

    // a = roughness ** 2
    float alpha = pow(material.roughness, 2);

    // the normal vector of a vertex
    vec3 N = vertexWorldNormal;

    // the normalized vector from the vertex to the camera
    vec3 V = vec3(normalize(cameraPosition - vertexWorldPosition));

    // the normalized vector from the vertex to the light
    vec3 L = vec3(normalize(pointLight.pointPosition - worldPosition));

    // H = (L + V) / magnitude(L + V)
    vec3 H = L + V / length(L + V);

    // the specular color of the material
    vec3 c_spec = mix(vec3(0.08f * material.specular), c_base, material.metallic);

    // The following D, k, F, and G are calculated for the equation:
    // s = DFG / 4(N dot L)(N dot V)

    float D = pow(alpha, 2) / ( M_PI * pow((pow(dot(N, H), 2) * (pow(alpha, 2) - 1) + 1 ), 2) );

    float k = pow(material.roughness + 1, 2) / 8;

    float G_of_L = dot(N, L) / ( dot(N, L) * (1 - k) + k );

    float G_of_V = dot(N, V) / ( dot(N, V) * (1 - k) + k );

    float G = G_of_L * G_of_V;

    vec3 F = c_spec + (1 - c_spec) * pow(2, (-5.55473 * dot(V, H) - 6.98316) * dot(V, H));

    // the specular term
    vec3 s = D * F * G / (4 * dot(N, L) * dot(N, V));

    ////////////////////////////////////////////////////////// 
    //////////// calculate the final color, c_brdf //////////// 

    vec3 c_brdf = vec3(genericLight.color) * dot(N, L) * (d + s);


    //return vec4(d, 0.f); // just the diffuse term
    //return vec4(s, 0.f); // just the specular term
    return vec4(c_brdf, 0.f);
}

vec4 globalLightSubroutine(vec4 worldPosition, vec3 worldNormal)
{
    //return material.matAmbient;
    return vec4(0.f, 0.f, 0.f, 0.f);
}

vec4 AttenuateLight(vec4 originalColor, vec4 worldPosition)
{
    float lightDistance = length(pointLight.pointPosition - worldPosition);
    float attenuation = 1.0 / (constantAttenuation + lightDistance * linearAttenuation + lightDistance * lightDistance * quadraticAttenuation);
    return originalColor * attenuation;
}

void main()
{
    vec4 lightingColor = vec4(0);
    if (lightingType == 0) {
        lightingColor = globalLightSubroutine(vertexWorldPosition, vertexWorldNormal);
    } else if (lightingType == 1) {
        lightingColor = pointLightSubroutine(vertexWorldPosition, vertexWorldNormal);
    }
    finalColor = AttenuateLight(lightingColor, vertexWorldPosition) * fragmentColor;
}
