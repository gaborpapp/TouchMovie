uniform sampler2DRect tex;

uniform vec2 size;

void main()
{
	vec2 uv = gl_TexCoord[0].st;
	vec2 cuv = vec2( uv.s / 2., uv.t );
	vec2 auv = vec2( size.x / 2. + cuv.s, uv.t );
	vec4 color = texture2DRect( tex, cuv );
	color.a = texture2DRect( tex, auv ).r;
	gl_FragColor = color;
}

