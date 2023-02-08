#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;



layout (location = 0) out vec4 outColor;

struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
} ubo;

layout(set = 0, binding = 1) uniform highp sampler2D directional_light_shadow;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;


highp vec2 ndcxy_to_uv(highp vec2 ndcxy) { return ndcxy * vec2(0.5, 0.5) + vec2(0.5, 0.5); }

void main() {
  vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 specularLight = vec3(0.0);
  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPosWorld = ubo.invView[3].xyz;
  vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

  for (int i = 0; i < ubo.numLights; i++) {
    PointLight light = ubo.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
    directionToLight = normalize(directionToLight);

    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosAngIncidence;

    // specular lighting
    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
    specularLight += intensity * blinnTerm;
  }

  
  vec4 tmp_color = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);



PointLight light = ubo.pointLights[0];
  highp vec3  L   = normalize(fragPosWorld-light.position.xyz);
  highp float NoL = min(dot(surfaceNormal, L), 1.0);

  if (NoL > 0.0)
  {
        highp float shadow;
        {
            highp vec4 position_clip = ubo.projection * vec4(fragPosWorld,1.0);
            highp vec3 position_ndc  = position_clip.xyz / position_clip.w;

            highp vec2 uv = ndcxy_to_uv(position_ndc.xy);

            highp float closest_depth = texture(directional_light_shadow, uv).r + 0.000075;
            highp float current_depth = position_ndc.z;

            shadow = (closest_depth >= current_depth) ? 1.0f : -1.0f;
        }

        if (shadow < 0.0f)
        {
            outColor = vec4(.0,.0,.0,1.0);
        }
    }
    outColor= tmp_color;
}

