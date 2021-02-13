//-------------------------------------------------------------------------------------------------
// INPUTS
//-------------------------------------------------------------------------------------------------

layout(std140, binding = UB_CAMERA) uniform Camera
{
    CameraData data;
} ub_Camera;

//scene5: transform ssbo

layout(location = IN_POSITION) in vec3 in_Position;
layout(location = IN_NORMAL) in vec3 in_Normal;

//-------------------------------------------------------------------------------------------------
// OUTPUTS
//-------------------------------------------------------------------------------------------------

out Lighting
{
    vec3 eyePosition;
    vec3 eyeNormal;
} out_Lighting;

//-------------------------------------------------------------------------------------------------
// MAIN
//-------------------------------------------------------------------------------------------------

void main()
{
    //scene5: get transform from ssbo
    mat4 modelMatrix = mat4(vec4(t.row0.x, t.row1.x, t.row2.x, 0.0f),  // col 0
                            vec4(t.row0.y, t.row1.y, t.row2.y, 0.0f),  // col 1
                            vec4(t.row0.z, t.row1.z, t.row2.z, 0.0f),  // col 2
                            vec4(t.row0.w, t.row1.w, t.row2.w, 1.0f)); // col 3
    //scene5: output position
    //scene5: output normal in camera space
    //scene5: output position in camera space
}
