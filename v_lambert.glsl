#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;


uniform vec4 lightDir=vec4(0,-1,0,0);

//Atrybuty
layout (location=0) in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout (location=1) in vec4 normal; //wektor normalny w wierzcholku
layout (location=2) in vec2 texCoord; //wspó³rzêdne teksturowania


//zmienne interpolowane
out vec2 i_tc;
out vec4 l1;
out vec4 l2;
out vec4 h1;
out vec4 h2;
out vec4 n;
out vec4 v;
out float distance1;
out float distance2;

void main(void) {
    vec4 lp1 = vec4(-0.889, -2.448, -0.064,1); //wspolrzedna w przestrzeni swiata
    vec4 lp2 = vec4(-31.17, -2.6, 0.072,1);

    l1 = normalize(V*lp1-V*M*vertex);
    l2 = normalize(V*lp2-V*M*vertex);
    
    n = normalize(V*M*normal);
    v = normalize(vec4(0,0,0,1) - V*M*vertex);
    h1 = normalize(l1+v);
    h2 = normalize(l2+v);

    i_tc = texCoord;

    gl_Position=P*V*M*vertex;

    distance1 = length(lp1-M*vertex);
    distance2 = length(lp2-M*vertex);
}
