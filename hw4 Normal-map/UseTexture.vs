attribute vec3 tangent;
attribute vec3 bitangent;

varying vec3 normal, lightDir, eyeDir;
varying vec3 T_var, B_var;

void main()
{	
	// ת�����ӽ����꣬�������м������ӽ�����ϵ�����
	// ������
	normal = gl_NormalMatrix * gl_Normal;

	// ��Դ����������
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);

	// �ӵ㵽��������
	eyeDir = -vVertex;

	// �����������ݵ� fragment shader ��ȥ
	T_var = gl_NormalMatrix * tangent;
	B_var = gl_NormalMatrix * bitangent;

	// ȡ����ͼ����
	// build-in ���� gl_MultiTexCoord0~n�� build-in ��� gl_TexCorrd[0~n]
	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
	gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;
	gl_TexCoord[2].xy = gl_MultiTexCoord2.xy;

	// �õ��������Ժ󣬻���Ҫ�õ���������
	gl_Position = ftransform();	
}