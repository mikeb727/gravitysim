// simple Phong shader
#version 330 core

struct Material {
  vec4 diffuse;
  bool useDiffuseMap;
  sampler2D diffuseMap; // texture
  vec4 specular;
  float shininess;
};

struct DirectionalLight {
  vec3 dir;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

in VS_OUT {
    vec3 fragPos;
    vec4 fragPos_l; // light space transformation
    vec3 normalVec;
    vec2 texCoords;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight dirLight;
uniform sampler2D shadowMap;

out vec4 fragColor;

#define numDirLights 1

#define pcfWidth 3 // to soften shadows; keep this value low! <3
#define shadowBias 0.007

float calcShadow(vec4 pos, DirectionalLight l){
  vec3 projectedPos = ((pos.xyz / pos.w) * 0.5) + 0.5;
  float closestDepth = texture(shadowMap, projectedPos.xy).r;
  float currentDepth = projectedPos.z;
  float bias = max(shadowBias * (1.0 - dot(fs_in.normalVec, -l.dir)), shadowBias); // resolves "shadow acne"
  float shadowed = 0.0;
  // "nearest neighbor" PCF
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  float pcfDivisor = pow((2 * pcfWidth) + 1, 2);
  for (int s = -pcfWidth; s <= pcfWidth; ++s){
    for (int t = -pcfWidth; t <= pcfWidth; ++t){
      float pcfDepth = texture(shadowMap, projectedPos.xy + vec2(s, t) * texelSize).r;
      shadowed += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadowed /= pcfDivisor;
  shadowed = projectedPos.z > 1.0 ? 0.0 : shadowed;
  return shadowed;
}

vec3 computeDirectionalLight(DirectionalLight l, Material m, vec3 normal, vec3 viewDir){
  vec3 lightDir = normalize(-l.dir);
  float diffuseFactor = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);

  vec3 ambient = l.ambient.rgb * m.diffuse.rgb;
  vec3 diffuse = l.diffuse.rgb * diffuseFactor * m.diffuse.rgb * (m.useDiffuseMap ? vec3(texture(m.diffuseMap, fs_in.texCoords)) : vec3(1.0));
  
  vec3 specular = l.specular.rgb * specFactor * m.specular.rgb;
  
  float shadow = calcShadow(fs_in.fragPos_l, l);

  return ambient + (1.0 - shadow*0.8) * (diffuse + specular);
}

void main(){
  vec3 totalLight = vec3(0.0);
  for (int i = 0; i < numDirLights; ++i){
    totalLight += computeDirectionalLight(dirLight, material, normalize(fs_in.normalVec), normalize(viewPos - fs_in.fragPos));
  }
  fragColor = vec4(totalLight, 1.0);
};