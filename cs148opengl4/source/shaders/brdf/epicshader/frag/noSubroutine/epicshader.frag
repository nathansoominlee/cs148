#version 330
#define M_PI 3.1415926535897932384626433832795

in vec4 fragmentColor;
in vec4 vertexWorldPosition;
in vec3 vertexWorldNormal;

out vec4 finalColor;

uniform InputMaterial {
    float metallic;
    float roughness;
    float specular;

} material;

struct LightProperties {
    vec4 color;
    vec4 direction;
    vec4 groundColor;
    vec4 skyColor;
    float radius;
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

vec4 lightSubroutine(vec4 worldPosition, vec3 worldNormal)
{
    ////////////////////////////////////////////////////////// 
    /////// calculate values specific to light type //////////

    vec3 L;
    vec3 c_light;

    // the normal vector of a vertex
    vec3 N = vertexWorldNormal;

    if (lightingType == 1) // Point
    {
        // the light position
        vec3 l_pos = vec3(pointLight.pointPosition);

        // the vertex position
        vec3 x_vert = vec3(vertexWorldPosition);

        // THIS WAS REVERSED in assignment3.pdf, see @143 on Piazza for correction
        //L = (x_vert - l_pos) / length(x_vert - l_pos);
        L = (l_pos - x_vert) / length(l_pos - x_vert);

        // the light color
        c_light = vec3(genericLight.color);
    } 
    else if (lightingType == 2) // directional
    {
        L = - vec3(normalize(genericLight.direction));

        // the light color
        c_light = vec3(genericLight.color);
    } 
    else if (lightingType == 3) // hemisphere
    {
        L = N;

        vec3 c_ground = vec3(genericLight.groundColor);

        vec3 c_sky = vec3(genericLight.skyColor);

        c_light = mix( c_ground, c_sky, clamp( dot(N, vec3(0.f, 1.f, 0.f)) * 0.5 + 0.5, 0, 1 ) );
    }

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

    // the normalized vector from the vertex to the camera
    vec3 V = vec3(normalize(cameraPosition - vertexWorldPosition));

    // the normalized vector from the vertex to the light
    //vec3 L = vec3(normalize(pointLight.pointPosition - worldPosition));

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

    //vec3 c_light = vec3(genericLight.color);

    vec3 c_brdf; 

    // No specular on hemisphere
    if (lightingType == 3)  // hemisphere
    {
        c_brdf = c_light * dot(N, L) * d;
    }
    else // point or directional
    {
        c_brdf = c_light * dot(N, L) * (d + s);
    }

    //return vec4(d, 0.f); // just the diffuse term
    //return vec4(s, 0.f); // just the specular term
    return vec4(c_brdf, 0.f);
}

vec4 AttenuateLight(vec4 originalColor, vec4 worldPosition)
{
    // the light distance
    float t = length(pointLight.pointPosition - worldPosition);

    // the light radius <---- tunable parameter, pass-in through LightProperties for now, see @195 on Piazza
    float l_radius = genericLight.radius;

    // the constant attenuation
    float a_constant = constantAttenuation;

    // the linear attenuation
    float a_linear = linearAttenuation;

    // the quadratic attenudation
    float a_quad = quadraticAttenuation;

    // the total attenuation
    float a_total = pow(clamp(1 - pow(t / l_radius, 4), 0.f, 0.f), 2) / (a_constant + a_linear * t + a_quad * pow(t, 2));

    return a_total * originalColor;
}

void main()
{
    vec4 lightingColor = vec4(0);
    lightingColor = lightSubroutine(vertexWorldPosition, vertexWorldNormal);

    /*
    // only attenuate point lights
    if (lightingType == 1) // Point
    {
        finalColor = AttenuateLight(lightingColor, vertexWorldPosition) * fragmentColor;
    }
    else
    {
        // directional and hemisphere lights are unattenuated
        finalColor = lightingColor;
    }
    */
    finalColor = lightingColor; // attenuation is not working
}
