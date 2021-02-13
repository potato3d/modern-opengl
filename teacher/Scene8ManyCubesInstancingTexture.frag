//-------------------------------------------------------------------------------------------------
// INPUTS
//-------------------------------------------------------------------------------------------------

layout(std140, binding = UB_LIGHT) uniform Light
{
    LightData data;
} ub_Light;

layout(std430, binding = SB_MATERIAL) buffer Material
{
    readonly MaterialData data[];
} sb_Material;

layout(std430, binding = SB_TEXTURE) buffer Texture
{
    readonly uvec2 data[];
} sb_Texture;

in Lighting
{
    vec3 eyePosition;
    vec3 eyeNormal;
    vec2 texCoord;
} in_Lighting;

in Instancing
{
    flat int id;
} in_Instancing;

//-------------------------------------------------------------------------------------------------
// OUTPUTS
//-------------------------------------------------------------------------------------------------

layout(location = OUT_COLOR) out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
// MAIN
//-------------------------------------------------------------------------------------------------

// this code assumes light is at camera position (0,0,0 in eye space)
void main()
{
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    const vec3 n = normalize(in_Lighting.eyeNormal);
    const vec3 l = normalize(-in_Lighting.eyePosition); // light - vert = 0 - vert (eye space)

    float diffuseIntensity = dot(n,l);

    if(diffuseIntensity > 0.0f)
    {
        sampler2D sampler = sampler2D(sb_Texture.data[in_Instancing.id]);
        vec3 texColor = texture(sampler, in_Lighting.texCoord).rgb;

        const MaterialData m = sb_Material.data[in_Instancing.id];

        diffuse = vec3(m.diffuse) * texColor * vec3(ub_Light.data.diffuse) * diffuseIntensity;

        const vec3 r = reflect(-l,n);
        const vec3 e = l; // cam - vert = 0 - vert (eye space)
        const float specularIntensity = max(dot(r,e), 0.0f);
        specular = vec3(m.specular) * vec3(ub_Light.data.specular) * pow(specularIntensity, m.specular.w);
    }

    out_Color = vec4(vec3(ub_Light.data.ambient) + diffuse + specular, 1.0f);
};
