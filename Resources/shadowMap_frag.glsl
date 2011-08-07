
uniform sampler2DShadow		depthTexture;
uniform sampler2D			aoTexture;
uniform sampler2D			albedoTexture;
uniform vec2				screenSize;

uniform bool ao;
uniform bool shadows;


varying vec3				N, V, L;
varying vec4				q;


void main(void)
{
	vec3 normal = normalize( N );
	vec3 R = -normalize( reflect( L, normal ) );
	
	vec2 uv = vec2( gl_FragCoord.s / screenSize.x, gl_FragCoord.t / screenSize.y );

	vec4 albedo = texture2D(albedoTexture, uv) ;
	vec4 ambient = albedo*gl_FrontLightProduct[0].ambient;
	vec4 diffuse = gl_FrontLightProduct[0].diffuse * max(dot( normal, L), 0.0);
	vec4 specular = gl_FrontLightProduct[0].specular * pow(max(dot(R, V), 0.0), gl_FrontMaterial.shininess);
	vec4 ssao = texture2D( aoTexture, uv );

	if(!ao) ssao = vec4(1.0);
	
	vec3 coord = 0.5 * (q.xyz / q.w + 1.0);

	float shadow = 0.0;
	if(shadows){
		float x,y;
		for (y = -3.5 ; y <=3.5 ; y+=1.0)
			for (x = -3.5 ; x <=3.5 ; x+=1.0)
				shadow += shadow2D(depthTexture, coord + 0.001*vec3(x, y, 0.0) ).r;
		shadow /= 64.0 ;
	}
	else shadow = 1.0;

	gl_FragColor = ( ambient * ssao + 6.5*albedo*(0.05+0.8 * shadow) * diffuse) * ssao + specular * shadow * ssao;
}