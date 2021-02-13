//-------------------------------------------------------------------------------------------------
// INPUTS
//-------------------------------------------------------------------------------------------------

layout(std140, binding = UB_CAMERA) uniform Camera
{
    CameraData data;
} ub_Camera;

layout(std140, binding = UB_LIGHT) uniform Light
{
    LightData data;
} ub_Light;

layout(std430, binding = SB_MATERIAL) buffer Material
{
    readonly MaterialData data;
} sb_Material;

layout(location = IN_POSITION) in vec3 in_Position;
layout(location = IN_NORMAL) in vec3 in_Normal;

//-------------------------------------------------------------------------------------------------
// OUTPUTS
//-------------------------------------------------------------------------------------------------

out Lighting
{
    vec3 color;
} out_Lighting;

//-------------------------------------------------------------------------------------------------
// MAIN
//-------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = ub_Camera.data.viewProjMatrix * vec4(in_Position, 1.0f);

    vec3 eyeNormal = mat3(ub_Camera.data.viewMatrix) * in_Normal;
    vec3 eyePosition = vec3(ub_Camera.data.viewMatrix * vec4(in_Position, 1.0f));

    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    const vec3 n = normalize(eyeNormal);
    const vec3 l = normalize(-eyePosition); // light - vert = 0 - vert (eye space)

    float diffuseIntensity = dot(n,l);

    if(diffuseIntensity > 0.0f)
    {
        const MaterialData m = sb_Material.data;

        diffuse = vec3(m.diffuse) * vec3(ub_Light.data.diffuse) * diffuseIntensity;

        const vec3 r = reflect(-l,n);
        const vec3 e = l; // cam - vert = 0 - vert (eye space)
        const float specularIntensity = max(dot(r,e), 0.0f);
        specular = vec3(m.specular) * vec3(ub_Light.data.specular) * pow(specularIntensity, m.specular.w);
    }

    out_Lighting.color = vec3(ub_Light.data.ambient) + diffuse + specular;
}
