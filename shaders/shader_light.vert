#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform vec3 color;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	vColor = vec4(color.x,color.y,color.z, 1.0f);
	
	TexCoord = tex;
	//para tomar las Transformaciones geométricas y la transpuesta es para que las escalas se inviertan en caso de escalas no uniformes en las normales
	Normal = mat3(transpose(inverse(model))) * norm;
	
	FragPos = (model * vec4(pos, 1.0)).xyz; //para obtener un vec3 sólo posición con respecto al origen en orden xyz: Swizzling 
	//yzx para swizzling
}