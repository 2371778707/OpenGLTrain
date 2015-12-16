#include "scene.h"
#include "view.h"
#include "light.h"
#include "glut.h"

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
float rot_y = 0.0f;
float zoom_distance = 0.0f;
float zoom_unit = 1.0f;
void keyboard(unsigned char , int, int);

int main(int argc, char** argv)
{
	globalview = new view("Scene1.view");
	globalight = new lightsrc("Scene1.light");

	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("HW3 Buffer");
	globalscene = new scene("Scene1.scene");
	glutDisplayFunc(display);
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

	for (size_t i = 0; i < globalight->lTotal; i++) {
		// set light property
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, globalight->lightList[i].light_position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, globalight->lightList[i].light_diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, globalight->lightList[i].light_specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, globalight->lightList[i].light_ambient);
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
		glutPostRedisplay();
		break;
	case 'd':
		rot_y += 20;
		glutPostRedisplay();
		break;
	case 'w':
		zoom_distance  -= zoom_unit;
		glutPostRedisplay();
		break;
	case 's':
		zoom_distance  += zoom_unit;
		glutPostRedisplay();
		break;
	case 27:     exit(0);
	}
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
	// ���õƹ�Ч��
	light();
	drawScene();
	/*
	// ���� color buffer and depth buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);	
	// pass 2. front face stencil update
	glCullFace(GL_BACK);						// �е� back face
	shadowPolygon();
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);		// ͨ��z-buff ���ԣ�stencil ֵ+1
	// pass 3. back face stencil update
	glCullFace(GL_FRONT);						// �е� front face	
	shadowPolygon();
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);		// ͨ��z-buff ���ԣ�stencil ֵ-1
	// �ָ�
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE); 
	glStencilMask(~0);							// ���� stencile 
	glDisable(GL_CULL_FACE);
	// pass 4. 
	glStencilFunc(GL_EQUAL, 0, ~0);
	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);
	light();
	drawScene();
	*/
	
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
	// i ��ʾ face �ĸ���
	for(size_t i=0;i < object->fTotal;++i)
	{
		float distVer[3][3];				// shadowPolygon Զ���Դ�Ķ���

		// j = 0 1 2 ��ʾ�������㹹��һ�� triangle
		for (size_t j=0;j<3;++j)
		{
			// ��Դ����xyz ��ȥ��������xyz ���ϳ��� 5 �õ�
			distVer[j][0] =  6 * object->vList[object->faceList[i][j].v].ptr[0] - 5 * lightPos[0];
			distVer[j][1] =  6 * object->vList[object->faceList[i][j].v].ptr[1] - 5 * lightPos[1];
			distVer[j][2] =  6 * object->vList[object->faceList[i][j].v].ptr[2] - 5 * lightPos[1];
		}
		// ���� shadowPolygon
		// ���������ε���������Ϊ 0 1 2
		// ���水��[0 1 dist1 dist0] [1 2 dist2 dist1] [2 3 dist3 dist2] �ķ�ʽ�������
		for (size_t j=0;j<3;++j)
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
}