#version 450


layout(early_fragment_tests) in;

layout(location = 0) out float out_depth;

void main()
{
    out_depth = gl_FragCoord.z;
}