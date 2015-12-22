#include "scene.h"
#include "view.h"
#include "light.h"
#include "glut.h"
#include "Vector3D.h"

using namespace MathTool;

view *globalview;
lightsrc *globalight;
scene *globalscene;

void light();
void display();
void reshape(GLsizei , GLsizei );
void draw_object(mesh *object);
int windowSize[2];
void shadowPolygon(mesh *object);
void shadowPolygon();
void drawScene();
void ambient_light();
void drawShadowScene();
void depthFeild();
float rot_y = 0.0f;
float zoom_distance = 0.0f;
float zoom_unit = 1.0f;
float rot_light = 0.0f;
void keyboard(unsigned char , int, int);

int main(int argc, char** argv)
{
	globalview = new view("Scene2.view");
	globalight = new lightsrc("Scene2.light");

	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("HW3 Buffer");
	globalscene = new scene("Scene2.scene");
	glutDisplayFunc(display);
	//glutDisplayFunc(depthFeild);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	delete(globalview);
	delete(globalight);

}

void light()
{
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	float *position = globalight->lightList[0].light_position;

	for (size_t i = 0; i < globalight->lTotal; i++) {
		// set light property
		glPushMatrix(); 
		glDisable (GL_LIGHTING);
		glTranslatef(position[0], position[1], position[2]);
		glColor3f (1.0f, 1.0f, 0.0f);
		glutSolidSphere(0.5f, 50.0f, 50.0f);
		glPopMatrix();

		glPushMatrix();
		glEnable(GL_LIGHTING);
		glLoadIdentity();
		glRotated(rot_light, 0.0, 1.0, 0.0);
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, globalight->lightList[i].light_position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, globalight->lightList[i].light_diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, globalight->lightList[i].light_specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, globalight->lightList[i].light_ambient);
		glPopMatrix();

	}
	//glutSolidCube(1);
}

void ambient_light()
{
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	float diffuse[3] = {0, 0, 0};
	float specular[3] = {0, 0, 0};
	float *position = globalight->lightList[0].light_position;

	for (size_t i = 0; i < globalight->lTotal; i++) {
		// set light property
		glPushMatrix(); 
		glDisable (GL_LIGHTING);
		glTranslatef(position[0], position[1], position[2]);
		glColor3f (1.0f, 1.0f, 0.0f);
		glutSolidSphere(0.5f, 50.0f, 50.0f);
		glPopMatrix();

		glPushMatrix();
		glEnable(GL_LIGHTING);
		glLoadIdentity();
		glRotated(rot_light, 0.0, 1.0, 0.0);
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, globalight->lightList[i].light_position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, globalight->lightList[i].light_ambient);
		glPopMatrix();
	}
	//glutSolidCube(1);
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		/* ad ��ʾ�����ƶ��� ws ��ʾ�����ƶ���� */
	case 'a':
		rot_y -= 20;
		break;
	case 'd':
		rot_y += 20;
		break;
	case 'w':
		zoom_distance  -= zoom_unit;
		break;
	case 's':
		zoom_distance  += zoom_unit;
		break;
	case 'h':
		globalight->lightList[0].light_position[0] -= 5;
		break;
	case 'j':
		globalight->lightList[0].light_position[0] += 5;
		break;
	case 'k':
		globalight->lightList[0].light_position[2] -= 5;
		break;
	case 'l':
		globalight->lightList[0].light_position[2] += 5;
		break;
	case 'i':
		rot_light -= 20;
		break;
	case 'o':
		rot_light += 20;
		break;
	case 27:     exit(0);
	}
	glutPostRedisplay();
}

void display() {
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// ����color
	glClearDepth(1.0f);										// Depth Buffer (���z buffer) Setup
	glEnable(GL_DEPTH_TEST);								// Enables Depth Testing
	glEnable(GL_STENCIL_TEST);								// ���� stencil ����
	glClearStencil(0);										// ���� stencil Ϊ 0
	glClear(GL_STENCIL_BUFFER_BIT);							// �� stencil ��ֵΪ 0
	glDepthFunc(GL_LEQUAL);									// The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// ˫��������z buffer

	glViewport(globalview->viewport[0], globalview->viewport[1], globalview->viewport[2], globalview->viewport[3]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(globalview->fovy, (GLfloat)windowSize[0]/(GLfloat)windowSize[1], globalview->dnear, globalview->dfar);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	globalview->eye[0], globalview->eye[1], globalview->eye[2],		// eye
		globalview->vat[0], globalview->vat[1], globalview->vat[2],     // center
		globalview->vup[0], globalview->vup[1], globalview->vup[2]);    // up

	// ��������������ı任
	// normalize
	float x = globalview->eye[0] * globalview->eye[0];
	float y = globalview->eye[1] * globalview->eye[1];
	float z = globalview->eye[2] * globalview->eye[2];
	float dist = x + y + z;
	x = sqrt(x / dist);
	y = sqrt(y / dist);
	z = sqrt(z / dist);
	glTranslatef(x * zoom_distance, y * zoom_distance, z * zoom_distance);
	// ������� y ����ת rot_y ��
	glRotatef(rot_y, 0.0, 1.0, 0.0);

	drawShadowScene();
	glutSwapBuffers();
}

void depthFeild() 
{
	glClearAccum(0.0, 0.0, 0.0, 0.0);
	glClear(GL_ACCUM_BUFFER_BIT);
	
	for (int i = 0; i < 11; i++)
	{
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClearDepth(1.0f);
		glClearStencil(0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDepthFunc(GL_LEQUAL);				// û����һ�л���� bug !!!

		float xOffset = 0.1 * i - 1.0;
		glViewport(globalview->viewport[0], globalview->viewport[1], globalview->viewport[2], globalview->viewport[3]);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(globalview->fovy, (GLfloat)windowSize[0]/(GLfloat)windowSize[1], globalview->dnear, globalview->dfar);
		// viewing and modeling transformation
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(	globalview->eye[0] + xOffset, globalview->eye[1] + xOffset, globalview->eye[2],		// eye
			globalview->vat[0], globalview->vat[1], globalview->vat[2],     // center
			globalview->vup[0], globalview->vup[1], globalview->vup[2]);    // up

		glMatrixMode(GL_MODELVIEW);
		// normalize
		float x = globalview->eye[0] * globalview->eye[0];
		float y = globalview->eye[1] * globalview->eye[1];
		float z = globalview->eye[2] * globalview->eye[2];
		float dist = x + y + z;
		x = sqrt(x / dist);
		y = sqrt(y / dist);
		z = sqrt(z / dist);
		// ��������������ı任
		glTranslatef(x * zoom_distance, y * zoom_distance, z * zoom_distance);
		// ������� y ����ת rot_y ��
		glRotatef(rot_y, 0.0, 1.0, 0.0);

		drawShadowScene();
		glAccum(GL_ACCUM, 1.0/11.0);

	}
	glAccum(GL_RETURN, 1.0);
	glutSwapBuffers();
}

void drawScene() {
	for (int i = 0; i < globalscene->mTotla; i++) {
		glPushMatrix();
		/* �ı���ͼԭ�� */
		glTranslatef(globalscene->mList[i].translate[0], globalscene->mList[i].translate[1], globalscene->mList[i].translate[2]);
		/* ��ת�任 */
		glRotated(globalscene->mList[i].rotate[0], globalscene->mList[i].rotate[1], 
			globalscene->mList[i].rotate[2], globalscene->mList[i].rotate[3]);
		// ����
		glScaled(globalscene->mList[i].scale[0], globalscene->mList[i].scale[1], globalscene->mList[i].scale[2]);
		// ��������
		// pass 1. ֻ���������⣬�õ���� buffer ��ֵ
		draw_object(globalscene->mList[i].obejct);
		glPopMatrix();
		glFlush();
	}
}

void draw_object(mesh *object) {
	int lastMaterial = -1;
	for(size_t i=0;i < object->fTotal;++i)
	{
		// set material property if this face used different material
		if(lastMaterial != object->faceList[i].m)
		{
			lastMaterial = (int)object->faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT  , object->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE  , object->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR , object->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &object->mList[lastMaterial].Ns);
		}

		glBegin(GL_TRIANGLES);
		for (size_t j=0;j<3;++j)
		{
			glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
			glVertex3fv(object->vList[object->faceList[i][j].v].ptr);	
		}
		glEnd();
	}
}

void shadowPolygon() {
	for (int i = 0; i < globalscene->mTotla; i++) {
		glPushMatrix();
		/* �ı���ͼԭ�� */
		glTranslatef(globalscene->mList[i].translate[0], globalscene->mList[i].translate[1], globalscene->mList[i].translate[2]);
		/* ��ת�任 */
		glRotated(globalscene->mList[i].rotate[0], globalscene->mList[i].rotate[1], 
			globalscene->mList[i].rotate[2], globalscene->mList[i].rotate[3]);
		// ����
		glScaled(globalscene->mList[i].scale[0], globalscene->mList[i].scale[1], globalscene->mList[i].scale[2]);
		// ��������� shadow polygon
		shadowPolygon(globalscene->mList[i].obejct);
		glPopMatrix();
	}
}

void shadowPolygon(mesh *object) {
	int lastMaterial = -1;
	// ֻ��һյ�ƣ�����ѡȡ ligthList[0]
	float *lightPos = globalight->lightList[0].light_position;
	// i ��ʾ face �ĸ����� j ��ʾ���㣬 k ��ʾ����� xyz ����
	for(size_t i=0;i < object->fTotal;++i)
	{
		float distVer[3][3];				// shadowPolygon Զ���Դ�Ķ���

		// j = 0 1 2 ��ʾ�������㹹��һ�� triangle
		for (size_t j=0;j<3;++j)
		{
			// ��Դ����xyz ��ȥ��������xyz ���ϳ��� 5 �õ�
			distVer[j][0] =  6 * object->vList[object->faceList[i][j].v].ptr[0] - 5 * lightPos[0];
			distVer[j][1] =  6 * object->vList[object->faceList[i][j].v].ptr[1] - 5 * lightPos[1];
			distVer[j][2] =  6 * object->vList[object->faceList[i][j].v].ptr[2] - 5 * lightPos[2];
		}

		// face �� a b c �������㰴��ʱ�뷽�򹹳ɣ��������������棬���Ǳ�����
		float abDirect[3];
		float acDirect[3];
		float aToLight[3];
		for (int k = 0; k < 3; k++)
		{
			abDirect[k] = object->vList[object->faceList[i][1].v].ptr[k] - object->vList[object->faceList[i][0].v].ptr[k];
			acDirect[k] = object->vList[object->faceList[i][2].v].ptr[k] - object->vList[object->faceList[i][1].v].ptr[k];
			aToLight[k] = lightPos[k] - object->vList[object->faceList[i][0].v].ptr[k];
		}
		Vector3D abDireV(abDirect);
		Vector3D acDireV(acDirect);
		Vector3D aToLightV(aToLight);
		//int isFaceLight = aToLightV.Dot(abDireV.Cross(abDirect));
		// ʹ�� face ������һ��ķ������ڻ��ϸõ㵽��Դ������
		Vector3D aNormal(object->nList[object->faceList[i][0].n].ptr);
		int isFaceLight = aToLightV.Dot(aNormal);

		// ���� shadowPolygon
		// ���������ε���������Ϊ 0 1 2
		// ���水��[0 1 dist1 dist0] [1 2 dist2 dist1] [2 3 dist3 dist2] �ķ�ʽ�������
		if (isFaceLight >= 0) {
			for (size_t j=0; j<3; ++j)
			{
				// GL_POLYGON �������ѭ���ڲ���
				glBegin(GL_POLYGON);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
				glVertex3fv(distVer[j]);
				glVertex3fv(distVer[(j + 1)%3]);
				glVertex3fv(object->vList[object->faceList[i][(j + 1)%3].v].ptr);
				glEnd();
			}
		}
		else {  // isFaceLight < 0 ������
			for (size_t j=0; j<3; ++j)
			{
				// GL_POLYGON �������ѭ���ڲ���
				glBegin(GL_POLYGON);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
				glVertex3fv(object->vList[object->faceList[i][(j + 1)%3].v].ptr);
				glVertex3fv(distVer[(j + 1)%3]);
				glVertex3fv(distVer[j]);
				glEnd();
			}
		}
	}
}

void drawShadowScene() 
{
	// ���õƹ�Ч��
	ambient_light();
	drawScene();

	// ���� color buffer and depth buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);	
	// pass 2. front face stencil update
	glCullFace(GL_BACK);						// �е� back face

	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);		// ͨ��z-buff ���ԣ�stencil ֵ+1
	shadowPolygon();
	// pass 3. back face stencil update
	glCullFace(GL_FRONT);						// �е� front face	

	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);		// ͨ��z-buff ���ԣ�stencil ֵ-1
	shadowPolygon();
	// �ָ�
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE); 
	glStencilMask(~0);							// ���� stencile 
	glDisable(GL_CULL_FACE);
	// pass 4. 
	// ģ�����Ϊ������0ͨ���� ��Ȳ���Ϊ�����ͨ������ɫ���Ϊֱ���ۼ�
	glStencilFunc(GL_EQUAL, 0, ~0);
	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);
	light();
	drawScene();
	//glDisable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);
}