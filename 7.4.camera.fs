#version 330 core
out vec4 FragColor;

// in vec2 TexCoord;
in vec3 normal;
in vec3 color;
in vec3 fragPos;

uniform vec3 cameraViewDirection;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float frame;



void main()
{


// Розраховуємо освітлення
    vec3 viewDirection = normalize(cameraPosition - fragPos);
    vec3 lightDirection = normalize(lightPosition - fragPos);
    vec3 ambientLight = vec3(0.2f, 0.2f, 0.2f);
    float diffuseLight = max(dot(lightDirection, normal), 0);
    vec3 reflection = 2 * normal * dot(lightDirection, normal) - lightDirection;
    float specularLight = pow(max(dot(viewDirection, reflection), 0), exp2(10))/10;


// Створюємо код для зміни кольору
    float r = abs(fragPos.x) * (sin(frame)/2 + 0.5f);
    float g = abs(fragPos.y) * (sin(frame)/2 + 0.5f);
    float b = abs(fragPos.z) * (sin(frame)/2 + 0.5f);
//     vec3 Normal = abs(normal/2.f + vec3(0.5f, 0.5f, 0.5f));
    FragColor = vec4((diffuseLight) * vec3(r, g, b) + specularLight*5*vec3(0.2f, 0.0f, 1.0f)  + ambientLight, 1.f);
    //Версія без освітлення
//     FragColor = vec4(r, g, b, 1.f);
}
