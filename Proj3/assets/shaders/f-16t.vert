#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec4 lightDirWorldSpace;

uniform float angle;
uniform float sidex;
uniform float sidey;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;
out vec2 fragTexCoord;
out vec3 fragPObj;
out vec3 fragNObj;

void main() {
  float sinAngle = sin(angle);
  float cosAngle = cos(angle);

  vec3 P = (viewMatrix * modelMatrix * vec4( inPosition.x * cosAngle*0.55 + inPosition.y * sinAngle*0.55 + sidex, 
                      inPosition.y * cosAngle*0.55 - inPosition.x * sinAngle*0.55+ sidey,
                      -inPosition.z , 
                      1.0
                    )).xyz;
  
  vec3 N = normalMatrix * inNormal;
  vec3 L = -(viewMatrix * lightDirWorldSpace).xyz;

  fragL = L;
  fragV = -P;
  fragN = N;
  fragTexCoord = vec2(-inTexCoord.x,inTexCoord.y);
  fragPObj = vec3( inPosition.x * cosAngle*0.55 + inPosition.y * sinAngle*0.55 + sidex, 
                      inPosition.y * cosAngle*0.55 - inPosition.x * sinAngle*0.55+ sidey,
                      -inPosition.z);
  fragNObj = vec3(-inNormal.x , -inNormal.y , -inNormal.z);

  gl_Position = projMatrix * vec4(P, 1.0);
}


