//-------------------------------------------------------------------------------------------------
// INPUTS
//-------------------------------------------------------------------------------------------------

layout(std140, binding = UB_CAMERA) uniform Camera
{
    CameraData data;
} ub_Camera;

//scene2: light ubo

//scene2: material ssbo

layout(location = IN_POSITION) in vec3 in_Position;
//scene2: normal attrib

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

    //scene2: output lighting result
}
