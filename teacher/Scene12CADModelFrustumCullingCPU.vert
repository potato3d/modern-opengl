//-------------------------------------------------------------------------------------------------
// INPUTS
//-------------------------------------------------------------------------------------------------

layout(std140, binding = UB_CAMERA) uniform Camera
{
    CameraData data;
} ub_Camera;

layout(std430, binding = SB_TRANSFORM) buffer Transform
{
    readonly TransformData data[];
} sb_Transform;

layout(location = IN_POSITION) in vec3 in_Position;
layout(location = IN_NORMAL) in vec3 in_Normal;
layout(location = IN_DRAWID) in int in_DrawID;

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
    const TransformData t = sb_Transform.data[in_DrawID];
    mat4 modelMatrix = mat4(vec4(t.row0.x, t.row1.x, t.row2.x, 0.0f),  // col 0
                            vec4(t.row0.y, t.row1.y, t.row2.y, 0.0f),  // col 1
                            vec4(t.row0.z, t.row1.z, t.row2.z, 0.0f),  // col 2
                            vec4(t.row0.w, t.row1.w, t.row2.w, 1.0f)); // col 3
    gl_Position = ub_Camera.data.viewProjMatrix * modelMatrix * vec4(in_Position, 1.0f);

    out_Lighting.eyeNormal = mat3(ub_Camera.data.viewMatrix) * mat3(transpose(inverse(modelMatrix))) * in_Normal;
    out_Lighting.eyePosition = vec3(ub_Camera.data.viewMatrix * modelMatrix * vec4(in_Position, 1.0f));

    out_Instancing.id = in_DrawID;
}
