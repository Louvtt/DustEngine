#version 460 core
out vec4 FragColor;

in vec4 oColor;

float near = 0.1; 
float far  = 2000.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{           
    // if(oColor.a < .5) discard;
      
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    FragColor = vec4(vec3(depth), 1.0);
}