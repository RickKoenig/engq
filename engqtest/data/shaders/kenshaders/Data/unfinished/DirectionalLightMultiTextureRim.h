const char DirectionalLightMultiTextureRimVertex[] =

	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	// Input

	"attribute vec4 Position;\n"
	"attribute vec4 Normal;\n"
	"attribute vec2 UV0;\n"
	"attribute vec2 UV1;\n"

	// Output



	"varying mediump vec4 g_Color;\n"
	"varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec4 g_Normal;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"

	// Matrices

	"uniform mat4 WorldViewProjection;\n"
	"uniform mat4 WorldInverseTranspose;\n"
	"uniform mat4 WorldViewInverseTranspose;\n"
	"uniform mat4 WorldView;\n"
	"uniform mat4 View;\n"

	"uniform vec3 LampDirection;\n"
	"uniform vec3 LampColor;\n"
	"uniform vec3 Diffuse;\n"
	"uniform vec3 Ambient;\n"
	"uniform vec3 Specular;\n"
	"uniform vec3 Emissive;\n"
	"uniform float Alpha;\n"
	"uniform float Shininess;\n"
	"uniform vec3 GlobalAmbient;\n"
	

	"void main() {\n"
	"    gl_Position = Position * WorldViewProjection;\n"
	"    g_UV0 = UV0;\n"
	"    g_UV1 = UV1;\n"


	"    vec3 P = Position.xyz;\n"
	"    vec3 N = (WorldInverseTranspose * Normal).xyz;\n"

	"    N = normalize(N);\n"

	"    vec3 ambient = Ambient * GlobalAmbient;\n"
	
	"    vec3 L = normalize(-LampDirection);\n"
	"    float diffuseLight = max(dot(N, L), 0.0);\n"
	"    vec3 diffuse = Diffuse * LampColor * diffuseLight;\n"

	"    vec3 V = normalize(View[2].xyz);\n"
	"    vec3 H = normalize(L + V);\n"
	"    float specularLight = pow(max(dot(N, H), 0.0), Shininess);\n"
	"    vec3 specular = Specular * LampColor * specularLight;\n"
	"    g_Color = vec4(Emissive + diffuse + specular + ambient, Alpha);\n"
	"    g_Normal = WorldViewInverseTranspose * Normal;\n"
	"    g_ObjPos = (WorldView * Position).xyz;\n"

	"}\n"
;


const char DirectionalLightMultiTextureRimFrag[] =
	
	"#version 110\n"

	"#ifndef ignore_precision\nprecision mediump float;\n#endif\n"

	"varying mediump vec4 g_Color;\n"
	"varying mediump vec4 g_Normal;\n"
    "varying mediump vec3 g_ObjPos;\n"
	"varying mediump vec2 g_UV0;\n"
	"varying mediump vec2 g_UV1;\n"
	"uniform vec3 RimColor;\n"
	"uniform float RimMaximum;\n"

	"uniform sampler2D tex1;\n"
	"uniform sampler2D tex2;\n"
	
	"void main() {\n"
	"    vec4 col = g_Color * texture2D(tex1, g_UV0) * texture2D(tex2, g_UV1);\n"
	
	"    vec3 viewVec = normalize(-g_ObjPos);\n"
	"    float viewDot = 1.0 - dot(viewVec, normalize(g_Normal.xyz));\n"
	"    float finalRimFactor = smoothstep(1.0 - RimMaximum, 1.0, viewDot);\n"
	"    gl_FragColor = vec4((RimColor * finalRimFactor), 0.0) + col;\n"
	
	"}\n"
;
