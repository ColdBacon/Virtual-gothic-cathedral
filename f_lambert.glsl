#version 330

uniform sampler2D tex;
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
in vec2 i_tc;
in vec4 l1;
in vec4 l2;
in vec4 h1;
in vec4 h2;
in vec4 n;
in vec4 v;
in float distance1;
in float distance2;


void main(void) {

	vec4 color=texture(tex,i_tc);
	vec4 ml1 = normalize(l1);
	vec4 ml2 = normalize(l2);
	vec4 mh1 = normalize(h1);
	vec4 mh2 = normalize(h2);
	vec4 mn = normalize(n);

	float nl1 = clamp(dot(mn,ml1),0,1);
	float nl2 = clamp(dot(mn,ml2),0,1);
    float nh1 = pow(clamp(dot(mn,mh1),0,1),10);
	float nh2 = pow(clamp(dot(mn,mh2),0,1),10);

	float attenuation1 = 1.0 / (1 + 0.01 * distance1 + 0.00032 * (distance1 * distance1));
	float attenuation2 = 1.0 / (1 + 0.01 * distance2 + 0.00032 * (distance2 * distance2));

	nh1 *= attenuation1;
	nh2 *= attenuation2;
	nl1 *= attenuation1;
	nl2 *= attenuation2;


	float nl = nl1 + nl2;
	float nh = nh1 + nh2;

	pixelColor=Ka + Kd*vec4(color.rgb * nl,color.a)+Ks*vec4(nh,nh,nh,0);
}
