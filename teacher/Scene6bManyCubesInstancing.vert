//-------------------------------------------------------------------------------------------------
// INPUTS
//-------------------------------------------------------------------------------------------------

layout(std140, binding = UB_CAMERA) uniform Camera
{
    CameraData data;
} ub_Camera;

layout(location = IN_POSITION) in vec3 in_Position;
layout(location = IN_NORMAL) in vec3 in_Normal;
layout(location = IN_TRANSFORM) in vec3 in_Transform;

//-------------------------------------------------------------------------------------------------
// OUTPUTS
//-------------------------------------------------------------------------------------------------

out Lighting
{
    vec3 eyePosition;
    vec3 eyeNormal;
} out_Lighting;

out Instancing
{
    flat int id;
} out_Instancing;

//-------------------------------------------------------------------------------------------------
// MAIN
//-------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = ub_Camera.data.viewProjMatrix * vec4(in_Position + in_Transform, 1.0f);

    out_Lighting.eyeNormal = mat3(ub_Camera.data.viewMatrix) * in_Normal;
    out_Lighting.eyePosition = vec3(ub_Camera.data.viewMatrix * vec4(in_Position, 1.0f));

    out_Instancing.id = gl_InstanceID;
}
