#include "MeshObject.h"
#pragma unmanaged
#include <Eigen/Sparse>
#pragma managed
#include <map>
#include <vector>

using namespace Eigen;

struct OpenMesh::VertexHandle const OpenMesh::PolyConnectivity::InvalidVertexHandle;

#pragma region MyMesh

MyMesh::MyMesh()
{
	//default property
	request_vertex_normals(); //add vertex_normals property
	request_vertex_status(); //add vertex status properties
	request_face_status(); //add face status properties
	request_edge_status(); //add edge status properties
}

MyMesh::~MyMesh()
{

}

int MyMesh::FindVertex(MyMesh::Point goalPoint)
{
	//return point of index
	int idx = -1;
	for (MyMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		MyMesh::Point p = point(*v_it);
		if (goalPoint == p)
		{
			idx = v_it->idx();
			break;
		}
	}

	return idx;
}

void MyMesh::ClearMesh()
{
	if (!faces_empty())
	{
		for (MyMesh::FaceIter f_it = faces_begin(); f_it != faces_end(); ++f_it)
		{
			delete_face(*f_it, true);
		}

		garbage_collection(); //tidy the mesh from bin
	}
}

#pragma endregion

#pragma region GLMesh
GLMesh::GLMesh()
{
	//inherit all property from MyMesh, no need to edit
}

GLMesh::~GLMesh()
{

}

bool GLMesh::Init(std::string fileName)
{
	//both functions defined below
	if (LoadModel(fileName))
	{
		LoadToShader();
		return true;
	}
	return false;
}

void GLMesh::Render()
{
	glBindVertexArray(MeshVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshEBO);
	glDrawElements(GL_TRIANGLES, mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


bool GLMesh::LoadModel(std::string fileName)
{
	OpenMesh::IO::Options ropt;
	if (OpenMesh::IO::read_mesh(mesh, fileName, ropt))
	{
		if (!ropt.check(OpenMesh::IO::Options::VertexNormal) && mesh.has_vertex_normals())
		{
			mesh.request_face_normals();
			mesh.update_normals();
			mesh.release_face_normals();
		}

		return true;
	}

	return false;
}

void GLMesh::LoadToShader()
{
	std::vector<MyMesh::Point> vertices;
	std::vector<MyMesh::Normal> normals;
	std::vector<unsigned int> indices;
	//allocate space for faster vector insert
	vertices.reserve(mesh.n_vertices()); 
	normals.reserve(mesh.n_vertices());
	indices.reserve(mesh.n_faces() * 3); //vertex is less than nfaces*3 due to repeated vertices
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		//collect the vertices and normals
		vertices.push_back(mesh.point(*v_it));
		normals.push_back(mesh.normal(*v_it));
	}
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		//collect vertex indices, for rendering using drawelements
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			indices.push_back(fv_it->idx());
		}
	}
	//generate vao
	glGenVertexArrays(1, &MeshVAO);
	glBindVertexArray(MeshVAO);
	//store vertices to vbo
	glGenBuffers(1, &MeshVBOVertex);
	glBindBuffer(GL_ARRAY_BUFFER, MeshVBOVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
	glEnableVertexAttribArray(0);
	//store normal to vbo
	glGenBuffers(1, &MeshVBONormal);
	glBindBuffer(GL_ARRAY_BUFFER, MeshVBONormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Normal) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
	glEnableVertexAttribArray(1);
	//indices for draw elements
	glGenBuffers(1, &MeshEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);


	glGenBuffers(1, &MeshVBOTexCoord);

	//unbind, for now tex coord doesn't stored, because we store it later during the parameterization
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GLMesh::LoadTexCoordToShader()
{
	//check wehether texcoords property is available
	if (mesh.has_vertex_texcoords2D())
	{
		std::vector<MyMesh::TexCoord2D> texCoords;
		//bool firstTime = false;
		for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
		{
			//store texcoords data
			MyMesh::TexCoord2D texCoord = mesh.texcoord2D(*v_it);
			/*if (texCoord.data()[0] >= 0 && texCoord.data()[0] <= 1 && texCoord.data()[1] >= 0 && texCoord.data()[1] <= 1) {
				std::cout << texCoord.data()[0] << " " << texCoord.data()[1] << std::endl;
				
			}*/

			//for debugging purposes
			if (texCoord.data()[0] != -10 || texCoord.data()[1] != -10) {
				if (texCoord.data()[0] < 0 || texCoord.data()[0] > 1 || texCoord.data()[1] < 0 || texCoord.data()[1] > 1) {
					std::cout << "abnormalities: " << texCoord.data()[0] << " " << texCoord.data()[1] << "\n";
				}
			}

			texCoords.push_back(texCoord);

			/*if (sizeof(MyMesh::TexCoord2D) == sizeof(texCoords[0])) {
				std::cout << "same!\n";
			}*/
			/*if (sizeof(MyMesh::TexCoord2D) == sizeof(GL_FLOAT) * 2) {
				std::cout << "Same\n";
			}
			else {
				std::cout << "Not same\n";
			}*/
		}
		//texCoords.push_back(0.5, 0.5);
		glBindVertexArray(MeshVAO);
		//store texcoord to vbo
		glBindBuffer(GL_ARRAY_BUFFER, MeshVBOTexCoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); //representation in vec2
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

#pragma endregion

#pragma region MeshObject

float getRadianBetween2Lines(MyMesh::Point anglePoint, MyMesh::Point end1, MyMesh::Point end2) {
	glm::vec3 vertexPoint = glm::vec3(anglePoint[0], anglePoint[1], anglePoint[2]);
	glm::vec3 vEnd1 = glm::vec3(end1[0], end1[1], end1[2]);
	glm::vec3 vEnd2 = glm::vec3(end2[0], end2[1], end2[2]);

	//find angle using the triangle cosine rule
	glm::vec3 a = vertexPoint - vEnd1;
	glm::vec3 b = vertexPoint - vEnd2;
	glm::vec3 c = vEnd1 - vEnd2;
	//c^2 = a^2 + b^2 - 2 ab cos alpha
	float aLength = glm::length(a);
	float bLength = glm::length(b);
	float cLength = glm::length(c);
	//goal: find alpha
	//alpha = arccos((a^2 + b^2 - c^2)/2ab)
	float alpha = glm::acos((pow(aLength, 2) + pow(bLength, 2) - pow(cLength, 2)) / (2.0 * aLength * bLength));
	return alpha;
}

MeshObject::MeshObject()
{
	curWeightMode = MEAN;
	//curWeightMode = HARMONIC;
	//curWeightMode = UNIFORM;
	curBorderMode = QUAD;
	//curBorderMode = DISK;

}

MeshObject::~MeshObject()
{
}

bool MeshObject::Init(std::string fileName)
{
	//selectedFace = add face feature
	selectedFace.clear();
	objName = fileName;
	universalTextureLoaded = false;
	bool success = model.Init(fileName);
	if (success) {
		currentlySelectedComponent = -1;
		univBgColor = glm::vec4(1.0);
		model.mesh.request_vertex_texcoords2D();
		model.mesh.add_property(uniOrigheWeight, "uniOrigheWeight");
		model.mesh.add_property(cotOrigheWeight, "cotOrigheWeight");
		model.mesh.add_property(tanOrigheWeight, "tanOrigheWeight");
		model.mesh.add_property(faceCategorization, "faceCategorization");
		for (MyMesh::FIter ft = model.mesh.faces_begin(); ft != model.mesh.faces_end(); ft++) {
			model.mesh.property(faceCategorization, *ft) = -1;
		}
		submodel.mesh.add_property(toModelV, "toModelV"); //bridging sub model vertex to main model
		submodel.mesh.add_property(toModelHe, "toModelHe");
		submodel.mesh.request_vertex_texcoords2D();
		submodel.mesh.add_property(halfEdgeWeight, "halfEdgeWeight");
		submodel.mesh.add_property(checkedBorder, "checkedBorder");
		//textureNumberingIndex.clear();
		std::cout << "Loading universal face mapping\n";
		for (int i = 0; i < BODYPARTSAMOUNT; i++) {
			bodyPartsLoader(i);
			glGenVertexArrays(1, &submeshesVAO[i]);
			glGenBuffers(1, &submeshesVBO[i]);
			glGenBuffers(1, &submeshesNBO[i]);
			glGenBuffers(1, &submeshesTBO[i]);

			loadParameterize(i, true, UNIFORM);
			loadParameterize(i, true, HARMONIC);
			loadParameterize(i, true, MEAN);
			loadParameterize(i, false, UNIFORM);
			loadParameterize(i, false, HARMONIC);
			loadParameterize(i, false, MEAN);
		}
		std::cout << "Loading universal face mapping finished\n";
		//count weights beforehad
		std::cout << "Counting weights of He\n";
		for (MyMesh::EdgeIter e_it = model.mesh.edges_begin(); e_it != model.mesh.edges_end(); ++e_it) {
			//count the halfedge weight
			MyMesh::HalfedgeHandle curHe = model.mesh.halfedge_handle(*e_it, 0);
			MyMesh::VertexHandle v0 = model.mesh.from_vertex_handle(curHe);
			MyMesh::VertexHandle v1 = model.mesh.to_vertex_handle(curHe);
			//std::cout << "b\n";
			MyMesh::Point p0 = model.mesh.point(v0);
			MyMesh::Point p1 = model.mesh.point(v1);
			//v2 will be the last vertex of the face that include the curHe halfedge, 
			MyMesh::HalfedgeHandle tempHe = model.mesh.next_halfedge_handle(curHe);
			MyMesh::VertexHandle v2 = model.mesh.from_vertex_handle(tempHe);
			if (v2 == v0 || v2 == v1) {
				v2 = model.mesh.to_vertex_handle(tempHe);
			}
			MyMesh::Point p2 = model.mesh.point(v2);
			//while v3 will be the last vertex of the face that include opposide curHe halfedge
			MyMesh::HalfedgeHandle opposingHe = model.mesh.opposite_halfedge_handle(curHe);
			tempHe = model.mesh.next_halfedge_handle(opposingHe);
			MyMesh::VertexHandle v3 = model.mesh.from_vertex_handle(tempHe);
			if (v3 == v0 || v3 == v1) {
				v3 = model.mesh.to_vertex_handle(tempHe);
			}
			MyMesh::Point p3 = model.mesh.point(v3);
			//std::cout << "c\n";
			//count mean tangent weight
			//use the function of counting the angle
			//angle0 = for the edge containing curHe, start on v0
			//angle1 = for the edge containing opposingHe, start on v0
			//angle2 = for the edge containing curHe, start on v1
			//angle3 = for the edge containing opposingHe, start on v1
			float angle0 = getRadianBetween2Lines(p0, p1, p2);
			float angle1 = getRadianBetween2Lines(p0, p1, p3);
			float angle2 = getRadianBetween2Lines(p1, p0, p2);
			float angle3 = getRadianBetween2Lines(p1, p0, p3);
			//storing He weight method:weight with central angle on v0, will be on He that start on v0, and the same otherwise
			float heLength = glm::distance(glm::vec3(p0[0], p0[1], p0[2]), glm::vec3(p1[0], p1[1], p1[2]));
			float weightCurHe = (glm::tan(angle0 / 2) + glm::tan(angle1 / 2)) / heLength;
			float weightOpposingHe = (glm::tan(angle2 / 2) + glm::tan(angle3 / 2)) / heLength;
			model.mesh.property(tanOrigheWeight, curHe) = weightCurHe;
			model.mesh.property(tanOrigheWeight, opposingHe) = weightOpposingHe;
			//std::cout << "d\n";
			//harmonic cotangent
			//count harmonic implementation of the halfedge weight
			//note that in this implementation both the weight of the halfedge is the same
			//use the function of counting the angle
			//angle0 = for the edge containing curHe
			//angle1 = for the edge containing opposingHe
			angle0 = getRadianBetween2Lines(p2, p0, p1);
			angle1 = getRadianBetween2Lines(p3, p0, p1);
			//storing He weight method:weight with central angle on v0, will be on He that start on v0, and the same otherwise
			weightCurHe = (1.0 / glm::tan(angle0) + 1.0 / glm::tan(angle1));
			weightOpposingHe = weightCurHe;
			model.mesh.property(cotOrigheWeight, curHe) = weightCurHe;
			model.mesh.property(cotOrigheWeight, opposingHe) = weightOpposingHe;
			//std::cout << "e\n";
			//Uniform
			model.mesh.property(uniOrigheWeight, curHe) = 1;
			model.mesh.property(uniOrigheWeight, opposingHe) = 1;
			//std::cout << "f\n";
		}
		std::cout << "Counting weights of He finished\n";

		//load the VAO,VBO,NBO
		for (int i = 0; i < BODYPARTSAMOUNT; i++) {
			LoadVAOVBONBOBodyParts(i);
			LoadTBOBodyParts(i, DISK, MEAN);
		}
	}
	return success;
}

void MeshObject::Render()
{
	//render everything
	model.Render();
	/*glBindVertexArray(model.vao);
	glDrawElements(GL_TRIANGLES, model.mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);*/
}

void MeshObject::RenderSelectedFace()
{
	//render just the selected face
	//follows the implementation from 姚老師 example program
	if (selectedFace.size() > 0)
	{
		std::vector<unsigned int*> offsets(selectedFace.size());
		int i = 0;
		for (auto iter = selectedFace.begin(); iter != selectedFace.end(); ++iter, i++)
		{
			offsets[i] = (GLuint*)(*iter * 3 * sizeof(GLuint));
		}
		//create selectedFace.size() number of elements with value 3 each
		std::vector<int> count(selectedFace.size(), 3);

		glBindVertexArray(model.MeshVAO);
		//render multiple sets of primitives by specifying indices of array data elements
		//first, specify the modes
		//count points to the  *array* of the elements counts. 
		//type specifies value in indices
		//Specifies a pointer to the location where the indices are stored. 
		//Specifies the size of the count and indices arrays. 
		glMultiDrawElements(GL_TRIANGLES, &count[0], GL_UNSIGNED_INT, (const GLvoid**)&offsets[0], selectedFace.size());
		glBindVertexArray(0);
	}
}

void MeshObject::RenderSelectedFaceCC() {
	//render just the selected face
	//follows the implementation from 姚老師 example program
	if (choosenFaceForUniversal.size() > 0)
	{
		std::vector<unsigned int*> offsets(choosenFaceForUniversal.size());
		int i = 0;
		for (auto iter = choosenFaceForUniversal.begin(); iter != choosenFaceForUniversal.end(); ++iter, i++)
		{
			offsets[i] = (GLuint*)(*iter * 3 * sizeof(GLuint));
		}
		//create selectedFace.size() number of elements with value 3 each
		std::vector<int> count(choosenFaceForUniversal.size(), 3);

		glBindVertexArray(model.MeshVAO);
		//render multiple sets of primitives by specifying indices of array data elements
		//first, specify the modes
		//count points to the  *array* of the elements counts. 
		//type specifies value in indices
		//Specifies a pointer to the location where the indices are stored. 
		//Specifies the size of the count and indices arrays. 
		glMultiDrawElements(GL_TRIANGLES, &count[0], GL_UNSIGNED_INT, (const GLvoid**)&offsets[0], choosenFaceForUniversal.size());
		glBindVertexArray(0);
	}
}

bool MeshObject::AddSelectedFace(unsigned int faceID)
{
	//add face id (already minus by 1)
	if (selectedFace.find(faceID) == selectedFace.end() &&	faceID >= 0 && faceID < model.mesh.n_faces())
	{
		selectedFace.insert(faceID);
		return true;
	}
	return false;
}

// match row number to vertex handle ID
unsigned int getMatrixRowNumber(std::vector<unsigned int> rowMap_vhID_Map, int vhID) {
	for (int i = 0; i < rowMap_vhID_Map.size(); i++) {
		if (rowMap_vhID_Map[i] == vhID) {
			return i;
		}
	}
	//should not reach here, throw error as warning
	std::cout << "Row number couldn't be found\n";
	exit(EXIT_FAILURE);
}

void MeshObject::customParameterization(GLuint index, float uvRotateAngle, float scaler, float offSetX, float offSetY) {
	//sort selected face for back mapping
	//std::sort(selectedFace.begin(), selectedFace.end());
	// CopySelectFaceGL();
	// find boundary halfedge first
	//std::cout << "a1\n";
	CopySelectFaceFromFile(index);
	//std::cout << "a2\n";
	std::vector<GLuint> tempVec;
	for (auto iter = submeshesFaces[index].begin(); iter != submeshesFaces[index].end(); iter++) {
		tempVec.push_back(*iter);
	}
	MyMesh::HalfedgeHandle heh;
	bool firstNotFound = true;
	//std::cout << "a3\n";
	for (MyMesh::EdgeIter e_it = submeshes[index].mesh.edges_begin(); e_it != submeshes[index].mesh.edges_end(); ++e_it) {
		if (submeshes[index].mesh.is_boundary(*e_it)) {
			//std::cout << "a3.2\n";
			// we found the half edge boundary, only evaluate it when it is first found
			if (firstNotFound) {
				// init our half edge handle
				// get first half edge and check whether it is a boundary or not
				firstHeh = submeshes[index].mesh.halfedge_handle(*e_it, 0);
				//go to opposite side of he that is boundary
				if (!submeshes[index].mesh.is_boundary(firstHeh)) {
					firstHeh = submeshes[index].mesh.halfedge_handle(*e_it, 1);
				}
				submeshes[index].mesh.property(checkedBorderSubmeshes[index], firstHeh) = true;
				//traverse and mark each halfedge as boundary
				MyMesh::HalfedgeHandle he = firstHeh;
				//std::cout << "---c---\n";
				//std::cout << "a3.3\n";
				while ((he = submeshes[index].mesh.next_halfedge_handle(he)) != firstHeh) {
					submeshes[index].mesh.property(checkedBorderSubmeshes[index], he) = true;
				}
				//std::cout << "a3.4\n";
				//std::cout << "---d---\n";
				firstNotFound = false;
			}
			else {
				//std::cout << "a3.5\n";
				MyMesh::HalfedgeHandle borderHeh = submeshes[index].mesh.halfedge_handle(*e_it, 0);
				if (!submeshes[index].mesh.is_boundary(borderHeh)) {
					borderHeh = submeshes[index].mesh.halfedge_handle(*e_it, 1);
				}

				//error detection, checking whether the Heh has been traversed before, if false, then stop parameterization, error
				if (!submeshes[index].mesh.property(checkedBorderSubmeshes[index], borderHeh)) {
					std::cout << "Stopping parameterization! Donut-shaped, filename: " << meshNames[index] << "\n";
					return;
				}
			}
		}
	}
	//std::cout << "a4\n";
	if (firstNotFound) {
		std::cout << "Stopping parameterization! Couldn't find border. filename: " << meshNames[index] << "\n";
		return;
	}
	//reset the texture mapping on original mesh
	/*if (!model.mesh.has_vertex_texcoords2D())
	{
		model.mesh.request_vertex_texcoords2D();
	}*/
	/*for (MyMesh::VertexIter vIter = model.mesh.vertices_begin(); vIter != model.mesh.vertices_end(); ++vIter)
	{
		model.mesh.set_texcoord2D(*vIter, MyMesh::TexCoord2D(-10, -10));
	}*/

	//std::cout << "--- f ---\n";
	// if we have any selected face
	if (tempVec.size() > 0) {
		// we will run the same loop 2 times
		// first time -> add up the total length of the boundary vertices, and set those vertex handle to booundary
		// second time -> calculate the UV of each boundary vertex handler

		float totalLength3DSpace = 0;

		// init our heh
		heh = firstHeh;
		std::vector<double> cummulativeLength; //for total sum of all HeH from it until that point
		do {
			MyMesh::VertexHandle vh1 = submeshes[index].mesh.from_vertex_handle(heh);
			MyMesh::VertexHandle vh2 = submeshes[index].mesh.to_vertex_handle(heh);

			// get the point of 2 vertices
			MyMesh::Point p1 = submeshes[index].mesh.point(vh1);
			MyMesh::Point p2 = submeshes[index].mesh.point(vh2);

			// get the distance between this 2 vertices
			totalLength3DSpace += glm::distance(glm::vec3(p1[0], p1[1], p1[2]), glm::vec3(p2[0], p2[1], p2[2]));
			cummulativeLength.push_back(totalLength3DSpace);
			// goto next half edge, since we are sure our halfedge is at the boundary, it is sure to traverse the perimeter
			heh = submeshes[index].mesh.next_halfedge_handle(heh);
		} while (heh != firstHeh);

		// now we have the total length

		if (curBorderMode == QUAD) {
			// yet we set our uv space to be (0, 0), (0, 1), (1, 1), (1, 0)
			// so the total length of uv space should be 4
			float totalLengthUVSpace = 4;

			// re-init our heh
			heh = firstHeh;
			// init our first texture point based on the formula of UVRotateAngle, from 姚老師‘s code
			float rotateRadiant = (225 + uvRotateAngle) * M_PI / 180.0;
			// this variable track the distance for each vertex runned through
			// without rotate, essentially we can choose to start from zero, but since we have rotate, might interpolate between 0-4, to decide placement on which edge
			float currentDis = 0;
			MyMesh::TexCoord2D currentTexCoord(0, 0);
			float lengthInterpolate = std::sqrt(2) / 2.0; //multiplied with rotateRadiant to decide start position
			currentTexCoord[0] = lengthInterpolate * cos(rotateRadiant) + 0.5;
			currentTexCoord[1] = lengthInterpolate * sin(rotateRadiant) + 0.5;

			//but remember coordinate should be at border, and in range of 0 to 1 in uv space
			//handle range of currentTexCoord[0] first
			if (currentTexCoord[0] > 1) {
				currentTexCoord[0] = 1;
				currentTexCoord[1] = tan(rotateRadiant) / 2 + 0.5;
			}
			else if (currentTexCoord[0] < 0) {
				currentTexCoord[0] = 0;
				currentTexCoord[1] = 0.5 - tan(rotateRadiant) / 2;
			}

			//now handle currentTexCoord[1] in similar manner
			if (currentTexCoord[1] > 1) {
				currentTexCoord[1] = 1;
				currentTexCoord[0] = tan(M_PI_2 - rotateRadiant) / 2 + 0.5;
			}
			else if (currentTexCoord[1] < 0) {
				currentTexCoord[1] = 0;
				currentTexCoord[0] = 0.5 - tan(M_PI_2 - rotateRadiant) / 2;
			}
			//put current coordinate both submodel and model
			/*if (!model.mesh.has_vertex_texcoords2D())
			{
				model.mesh.request_vertex_texcoords2D();
			}*/
			if (!submodel.mesh.has_vertex_texcoords2D()) {
				submodel.mesh.request_vertex_texcoords2D();
			}
			submeshes[index].mesh.set_texcoord2D(submeshes[index].mesh.from_vertex_handle(heh), currentTexCoord);
			//MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
			//model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
			//based on uv rotate, which is done in range 0 - 360, we interpolate the start distance
			// mapping order: down - right - up - left
			float initDis = 0;
			if (uvRotateAngle <= 90)
			{
				initDis = currentTexCoord.length();
			}
			else if (uvRotateAngle <= 180)
			{
				initDis = 1 + currentTexCoord[1];
			}
			else if (uvRotateAngle <= 270)
			{
				initDis = 3 - currentTexCoord[0];
			}
			else
			{
				initDis = 4 - currentTexCoord[1];
			}
			unsigned int tempIndex = 0;
			currentDis = cummulativeLength[tempIndex] / totalLength3DSpace * 4.0 + initDis;
			//traverse the other vhs
			while ((heh = submeshes[index].mesh.next_halfedge_handle(heh)) != firstHeh) {
				currentDis = fmod(currentDis, 4);

				if (currentDis <= 1) {
					//down
					currentTexCoord[0] = currentDis;
					currentTexCoord[1] = 0;
				}
				else if (currentDis <= 2) {
					//right
					currentTexCoord[0] = 1;
					currentTexCoord[1] = currentDis - 1;
				}
				else if (currentDis <= 3) {
					//up
					currentTexCoord[0] = 3 - currentDis;
					currentTexCoord[1] = 1;
				}
				else {
					//left
					currentTexCoord[0] = 0;
					currentTexCoord[1] = 4 - currentDis;
				}
				currentDis = cummulativeLength[++tempIndex] / totalLength3DSpace * 4.0 + initDis;
				submeshes[index].mesh.set_texcoord2D(submeshes[index].mesh.from_vertex_handle(heh), currentTexCoord);
				//MyMesh::VertexHandle subToModelVTranslation = submeshes[index].mesh.property(toModelSubmeshes[index], submodel.mesh.from_vertex_handle(heh));
				//model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
			}
		}
		else {
			//disk border
			heh = firstHeh;

			//first point starts from uvRotateAngle
			double startAngle = fmod(uvRotateAngle, 360) * 2 * M_PI;
			MyMesh::TexCoord2D currentTexCoord((std::cos(startAngle) + 1.0) / 2, (std::sin(startAngle) + 1.0) / 2);
			submeshes[index].mesh.set_texcoord2D(submeshes[index].mesh.from_vertex_handle(heh), currentTexCoord);
			//MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
			//model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
			int tempIndex = 0;
			while ((heh = submeshes[index].mesh.next_halfedge_handle(heh)) != firstHeh) {
				//count angle from Start angle
				//interpolate between 0 - 2pi
				double angleFromStart = 2 * M_PI * cummulativeLength[++tempIndex] / totalLength3DSpace;
				double totalAngle = fmod(startAngle + angleFromStart, 2 * M_PI);
				currentTexCoord[0] = (std::cos(totalAngle) + 1.0) / 2;
				currentTexCoord[1] = (std::sin(totalAngle) + 1.0) / 2;
				submeshes[index].mesh.set_texcoord2D(submeshes[index].mesh.from_vertex_handle(heh), currentTexCoord);
				//MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
				//model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
			}
		}

	}
	else {
		return;
	}
	//std::cout << "a5\n";
	std::cout << "Halfway!\n";
	// find the middle points
	std::vector<unsigned int> rowMapToVHID;
	// solve linear equation
	// we should run through 2 loops (vertex handle iter)
	// first loop is to initialize the mapping between MatIDMapToVHID (vertex handle iter)
	for (MyMesh::VertexIter v_it = submeshes[index].mesh.vertices_begin(); v_it != submeshes[index].mesh.vertices_end(); ++v_it) {
		// we only fill the vertex that isn't a boundary
		if (!submeshes[index].mesh.is_boundary(*v_it)) {
			rowMapToVHID.push_back(v_it->idx());
		}
	}
	//std::cout << "a6\n";
	if (rowMapToVHID.size() > 0) {
		// initialize the matrix
		int m = rowMapToVHID.size();
		SparseMatrix<double> A(m, m);
		VectorXd BX(m);
		VectorXd BY(m);

		// reset all matrix to 0
		A.setZero();
		BX.setZero();
		BY.setZero();
		// second loop is to fill up the matrix
		for (MyMesh::VertexIter v_it = submeshes[index].mesh.vertices_begin(); v_it != submeshes[index].mesh.vertices_end(); ++v_it) {
			// if vertex is not boundary vertex
			if (!submeshes[index].mesh.is_boundary(*v_it)) {

				int row = getMatrixRowNumber(rowMapToVHID, v_it->idx());
				double totalWeight = 0;

				// we use one ring to find the vertex around v_it vertex
				//put if outside to prevent repeated if
				if (curWeightMode == MEAN) {
					for (MyMesh::VertexVertexIter v_around = submeshes[index].mesh.vv_iter(*v_it); v_around.is_valid(); v_around++) {
						//get weight, based on our storing method of halfedge, we find he where center the from_vHandle is v_it
						MyMesh::HalfedgeHandle heHandle = submeshes[index].mesh.find_halfedge(*v_it, *v_around);
						MyMesh::HalfedgeHandle originalHeHandle = submeshes[index].mesh.property(toModelHeSubmeshes[index], heHandle);
						double curWeight = model.mesh.property(tanOrigheWeight, originalHeHandle);
						totalWeight += curWeight;
						//check whether v_around is boundary, to decide where to put the v_around
						if (submeshes[index].mesh.is_boundary(*v_around)) {
							//put it on the Bx or By
							MyMesh::TexCoord2D curTex = submeshes[index].mesh.texcoord2D(*v_around);
							BX[row] += curWeight * curTex[0];
							BY[row] += curWeight * curTex[1];
						}
						else {
							//put it on the A
							int column = getMatrixRowNumber(rowMapToVHID, v_around->idx());

							//put on A[row, column] with negative weight
							A.insert(row, column) = -curWeight;
						}
					}
				}
				else if (curWeightMode == HARMONIC) {
					for (MyMesh::VertexVertexIter v_around = submeshes[index].mesh.vv_iter(*v_it); v_around.is_valid(); v_around++) {
						//get weight, based on our storing method of halfedge, we find he where center the from_vHandle is v_it
						MyMesh::HalfedgeHandle heHandle = submeshes[index].mesh.find_halfedge(*v_it, *v_around);
						MyMesh::HalfedgeHandle originalHeHandle = submeshes[index].mesh.property(toModelHeSubmeshes[index], heHandle);
						double curWeight = model.mesh.property(cotOrigheWeight, originalHeHandle);
						totalWeight += curWeight;
						//check whether v_around is boundary, to decide where to put the v_around
						if (submeshes[index].mesh.is_boundary(*v_around)) {
							//put it on the Bx or By
							MyMesh::TexCoord2D curTex = submeshes[index].mesh.texcoord2D(*v_around);
							BX[row] += curWeight * curTex[0];
							BY[row] += curWeight * curTex[1];
						}
						else {
							//put it on the A
							int column = getMatrixRowNumber(rowMapToVHID, v_around->idx());

							//put on A[row, column] with negative weight
							A.insert(row, column) = -curWeight;
						}
					}
				}
				else {
					for (MyMesh::VertexVertexIter v_around = submeshes[index].mesh.vv_iter(*v_it); v_around.is_valid(); v_around++) {
						//get weight, based on our storing method of halfedge, we find he where center the from_vHandle is v_it
						MyMesh::HalfedgeHandle heHandle = submeshes[index].mesh.find_halfedge(*v_it, *v_around);
						MyMesh::HalfedgeHandle originalHeHandle = submeshes[index].mesh.property(toModelHeSubmeshes[index], heHandle);
						double curWeight = model.mesh.property(uniOrigheWeight, originalHeHandle);
						totalWeight += curWeight;
						//check whether v_around is boundary, to decide where to put the v_around
						if (submeshes[index].mesh.is_boundary(*v_around)) {
							//put it on the Bx or By
							MyMesh::TexCoord2D curTex = submeshes[index].mesh.texcoord2D(*v_around);
							BX[row] += curWeight * curTex[0];
							BY[row] += curWeight * curTex[1];
						}
						else {
							//put it on the A
							int column = getMatrixRowNumber(rowMapToVHID, v_around->idx());

							//put on A[row, column] with negative weight
							A.insert(row, column) = -curWeight;
						}
					}
				}

				// lastly we put the total weight of the current row weight
				A.insert(row, row) = totalWeight;
			}
		}

		//for debug only
		/*double tBX = BX[0];
		double tBY = BY[0];
		double tA = A.coeffRef(0, 0);
		std::cout << tBX << " " << tBY << " " << tA << std::endl;*/


		// solve the linear equation with eigen
		A.makeCompressed();
		SparseQR<SparseMatrix<double>, COLAMDOrdering<int>> linearSolverX;
		//SimplicialCholesky<SparseMatrix<double>> linearSolverX;
		linearSolverX.compute(A);
		VectorXd X = linearSolverX.solve(BX);
		// AX = (BX)
		// X = (A^(-1))(BX)
		// AY = (BY)
		// Y = (A^(-1))(BY)
		SparseQR<SparseMatrix<double>, COLAMDOrdering<int>> linearSolverY;
		//SimplicialCholesky<SparseMatrix<double>> linearSolverY;
		linearSolverY.compute(A);
		VectorXd Y = linearSolverY.solve(BY);
		std::cout << "Eigen Finish!\n";

		// place all X to the correct uv
		for (int i = 0; i < rowMapToVHID.size(); i++) {
			int vhID = rowMapToVHID[i];
			MyMesh::VertexHandle vh = submeshes[index].mesh.vertex_handle(vhID);
			submeshes[index].mesh.set_texcoord2D(vh, MyMesh::TexCoord2D(X[i], Y[i]));
		}
	}
	//std::cout << "a7\n";
	//map back the tex coord from the submodel to the original mesh
	//all the unset tex coord has been set to -1 -1 previously during the beginning of the function
	//int index = 0;
	//for (MyMesh::FaceIter f_it = submodel.mesh.faces_begin(); f_it != submodel.mesh.faces_end(); ++f_it)
	//{
	//	//std::cout << "a\n";
	//	MyMesh::FaceHandle realFace = model.mesh.face_handle(tempVec[index++]);
	//	MyMesh::FaceHandle subFace = *f_it;
	//	//std::cout << "b\n";
	//	MyMesh::FaceVertexIter real_fv_iter = model.mesh.fv_iter(realFace);
	//	//std::cout << "c\n";
	//	for (MyMesh::FaceVertexIter sub_fv_iter = submodel.mesh.fv_iter(subFace); real_fv_iter.is_valid() && sub_fv_iter.is_valid(); ++real_fv_iter, ++sub_fv_iter)
	//	{
	//		MyMesh::TexCoord2D subTexCoord = submodel.mesh.texcoord2D(*sub_fv_iter);

	//		//take care of scaling
	//		glm::vec4 tempVec((subTexCoord[0] - 0.5) * scaler + 0.5, (subTexCoord[1] - 0.5) * scaler + 0.5, 0, 1);
	//		subTexCoord[0] = tempVec.x;
	//		subTexCoord[1] = tempVec.y;
	//		//submodel.mesh.set_texcoord2D(*sub_fv_iter, subTexCoord); //DANGEROUS, DO NOT OPEN
	//		model.mesh.set_texcoord2D(*real_fv_iter, subTexCoord);
	//	}
	//	//std::cout << "d\n";
	//}

	//update the data to VBO
	//model.LoadTexCoordToShader();

	//For the multiDrawElements in render parameterized, update the face index and also the total vertices need to be drawn
	//fvIDsPtr.clear();
	//elemCount.clear();
	//fvIDsPtr.reserve(tempVec.size());
	//elemCount.reserve(tempVec.size());
	//for (int i = 0; i < tempVec.size(); ++i)
	//{
	//	fvIDsPtr.push_back((GLuint*)(tempVec[i] * 3 * sizeof(GLuint))); //multiply by 3 since 1 selectedFace = 3 vertices. So to point to first vertices, *3
	//	elemCount.push_back(3);
	//}
	//std::cout << "a8\n";
	std::cout << "Body parts: " << meshNames[index] << " parameterization done!\n";
}

void MeshObject::Parameterization(float uvRotateAngle, float scaler, float offSetX, float offSetY)
{
	//sort selected face for back mapping
	//std::sort(selectedFace.begin(), selectedFace.end());
	CopySelectFaceGL();
	// find boundary halfedge first

	std::vector<GLuint> tempVec;
	//std::cout << "faces" << "\n";
	for (auto iter = selectedFace.begin(); iter != selectedFace.end(); iter++) {
		//std::cout << *iter << "\n";
		tempVec.push_back(*iter);
	}
	//std::cout << "facesFinish"  << "\n";
	MyMesh::HalfedgeHandle heh;
	bool firstNotFound = true;
	// loop through all halfedge to check is boundary
	//for (MyMesh::EdgeIter e_it = submodel.mesh.edges_begin(); e_it != submodel.mesh.edges_end(); ++e_it) {
	//	if (!submodel.mesh.is_boundary(*e_it)) {
	//		//count the halfedge weight
	//		MyMesh::HalfedgeHandle curHe = submodel.mesh.halfedge_handle(*e_it, 0);
	//		MyMesh::VertexHandle v0 = submodel.mesh.from_vertex_handle(curHe);
	//		MyMesh::VertexHandle v1 = submodel.mesh.to_vertex_handle(curHe);
	//		MyMesh::Point p0 = submodel.mesh.point(v0);
	//		MyMesh::Point p1 = submodel.mesh.point(v1);
	//		//v2 will be the last vertex of the face that include the curHe halfedge, 
	//		MyMesh::HalfedgeHandle tempHe = submodel.mesh.next_halfedge_handle(curHe);
	//		MyMesh::VertexHandle v2 = submodel.mesh.from_vertex_handle(tempHe);
	//		if (v2 == v0 || v2 == v1) {
	//			v2 = submodel.mesh.to_vertex_handle(tempHe);
	//		}
	//		MyMesh::Point p2 = submodel.mesh.point(v2);
	//		//while v3 will be the last vertex of the face that include opposide curHe halfedge
	//		MyMesh::HalfedgeHandle opposingHe = submodel.mesh.opposite_halfedge_handle(curHe);
	//		tempHe = submodel.mesh.next_halfedge_handle(opposingHe);
	//		MyMesh::VertexHandle v3 = submodel.mesh.from_vertex_handle(tempHe);
	//		if (v3 == v0 || v3 == v1) {
	//			v3 = submodel.mesh.to_vertex_handle(tempHe);
	//		}
	//		MyMesh::Point p3 = submodel.mesh.point(v3);
	//		if (curWeightMode == MEAN) {
	//			//use the function of counting the angle
	//			//angle0 = for the edge containing curHe, start on v0
	//			//angle1 = for the edge containing opposingHe, start on v0
	//			//angle2 = for the edge containing curHe, start on v1
	//			//angle3 = for the edge containing opposingHe, start on v1
	//			float angle0 = getRadianBetween2Lines(p0, p1, p2);
	//			float angle1 = getRadianBetween2Lines(p0, p1, p3);
	//			float angle2 = getRadianBetween2Lines(p1, p0, p2);
	//			float angle3 = getRadianBetween2Lines(p1, p0, p3);
	//			//storing He weight method:weight with central angle on v0, will be on He that start on v0, and the same otherwise
	//			float heLength = glm::distance(glm::vec3(p0[0], p0[1], p0[2]), glm::vec3(p1[0], p1[1], p1[2]));
	//			float weightCurHe = (glm::tan(angle0 / 2) + glm::tan(angle1 / 2)) / heLength;
	//			float weightOpposingHe = (glm::tan(angle2 / 2) + glm::tan(angle3 / 2)) / heLength;
	//			submodel.mesh.property(halfEdgeWeight, curHe) = weightCurHe;
	//			submodel.mesh.property(halfEdgeWeight, opposingHe) = weightOpposingHe;
	//		}
	//		else if (curWeightMode == HARMONIC) {
	//			//count harmonic implementation of the halfedge weight
	//			//note that in this implementation both the weight of the halfedge is the same
	//			//use the function of counting the angle
	//			//angle0 = for the edge containing curHe
	//			//angle1 = for the edge containing opposingHe
	//			float angle0 = getRadianBetween2Lines(p2, p0, p1);
	//			float angle1 = getRadianBetween2Lines(p3, p0, p1);
	//			//storing He weight method:weight with central angle on v0, will be on He that start on v0, and the same otherwise
	//			float weightCurHe = (1.0/glm::tan(angle0) + 1.0/glm::tan(angle1));
	//			float weightOpposingHe = weightCurHe;
	//			submodel.mesh.property(halfEdgeWeight, curHe) = weightCurHe;
	//			submodel.mesh.property(halfEdgeWeight, opposingHe) = weightOpposingHe;
	//		}
	//		else {
	//			//uniform
	//			submodel.mesh.property(halfEdgeWeight, curHe) = 1;
	//			submodel.mesh.property(halfEdgeWeight, opposingHe) = 1;
	//		}
	//	}
	//	// we found the half edge boundary, only evaluate it when it is first found
	//	else{
	//		if (firstNotFound) {
	//			// init our half edge handle
	//			// get first half edge and check whether it is a boundary or not
	//			firstHeh = submodel.mesh.halfedge_handle(*e_it, 0);
	//			//go to opposite side of he that is boundary
	//			if (!submodel.mesh.is_boundary(firstHeh)) {
	//				firstHeh = submodel.mesh.halfedge_handle(*e_it, 1);
	//			}
	//			submodel.mesh.property(checkedBorder, firstHeh) = true;
	//			//traverse and mark each halfedge as boundary
	//			MyMesh::HalfedgeHandle he = firstHeh;
	//			//std::cout << "---c---\n";
	//			while ((he = submodel.mesh.next_halfedge_handle(he)) != firstHeh) {
	//				submodel.mesh.property(checkedBorder, he) = true;
	//			}
	//			//std::cout << "---d---\n";
	//			firstNotFound = false;
	//		}else{
	//			MyMesh::HalfedgeHandle borderHeh = submodel.mesh.halfedge_handle(*e_it, 0);
	//			if (!submodel.mesh.is_boundary(borderHeh)) {
	//				borderHeh = submodel.mesh.halfedge_handle(*e_it, 1);
	//			}
	//			
	//			//error detection, checking whether the Heh has been traversed before, if false, then stop parameterization, error
	//			if (!submodel.mesh.property(checkedBorder, borderHeh)) {
	//				std::cout << "Stopping parameterization! Donut-shaped\n";
	//				return;
	//			}
	//		}
	//	}
	//}

	for (MyMesh::EdgeIter e_it = submodel.mesh.edges_begin(); e_it != submodel.mesh.edges_end(); ++e_it) {
		if (submodel.mesh.is_boundary(*e_it)) {
			// we found the half edge boundary, only evaluate it when it is first found
			//std::cout << "Found one\n";
			if (firstNotFound) {
				//std::cout << "Found one\n";
				// init our half edge handle
				// get first half edge and check whether it is a boundary or not
				firstHeh = submodel.mesh.halfedge_handle(*e_it, 0);
				//go to opposite side of he that is boundary
				if (!submodel.mesh.is_boundary(firstHeh)) {
					//std::cout << "Entering2\n";
					firstHeh = submodel.mesh.opposite_halfedge_handle(firstHeh);
					//firstHeh = submodel.mesh.halfedge_handle(*e_it, 1);
				}
				submodel.mesh.property(checkedBorder, firstHeh) = true;
				//std::cout << "Looping start " << submodel.mesh.property(checkedBorder, firstHeh) << "\n";
				//traverse and mark each halfedge as boundary
				MyMesh::HalfedgeHandle he = firstHeh;
				he = submodel.mesh.next_halfedge_handle(he);
				//std::cout << "---c---\n";
				while ((he) != firstHeh) {
					submodel.mesh.property(checkedBorder, he) = true;
					he = submodel.mesh.next_halfedge_handle(he);
					//std::cout << "Repeatng1\n";
					//std::cout << "While looping " << submodel.mesh.property(checkedBorder, he) << "\n";
				}
				//std::cout << "---d---\n";
				firstNotFound = false;
			}
			else {
				MyMesh::HalfedgeHandle borderHeh = submodel.mesh.halfedge_handle(*e_it, 0);
				if (!submodel.mesh.is_boundary(borderHeh)) {
					//std::cout << "Entering 3\n";
					borderHeh= submodel.mesh.opposite_halfedge_handle(borderHeh);
					//borderHeh = submodel.mesh.halfedge_handle(*e_it, 1);
				}

				////error detection, checking whether the Heh has been traversed before, if false, then stop parameterization, error
				if (!submodel.mesh.property(checkedBorder, borderHeh)) {
					std::cout << "===================\n";
					std::cout << "Stopping parameterization! Donut-shaped\n";
					std::cout << "===================\n";
					//std::cout << "After stopping " << submodel.mesh.property(checkedBorder, borderHeh) << "\n";
					return;
				}
			}
		}
	}

	if (firstNotFound) {
		std::cout << "Stopping parameterization! Couldn't find border!\n";
		return;
	}
	//reset the texture mapping on original mesh
	if (!model.mesh.has_vertex_texcoords2D())
	{
		model.mesh.request_vertex_texcoords2D();
	}
	for (MyMesh::VertexIter vIter = model.mesh.vertices_begin(); vIter != model.mesh.vertices_end(); ++vIter)
	{
		model.mesh.set_texcoord2D(*vIter, MyMesh::TexCoord2D(-10, -10));
	}

	//std::cout << "--- f ---\n";
	// if we have any selected face
	if (tempVec.size() > 0) {
		// we will run the same loop 2 times
		// first time -> add up the total length of the boundary vertices, and set those vertex handle to booundary
		// second time -> calculate the UV of each boundary vertex handler

		float totalLength3DSpace = 0;

		// init our heh
		heh = firstHeh;
		std::vector<double> cummulativeLength; //for total sum of all HeH from it until that point
		do {
			MyMesh::VertexHandle vh1 = submodel.mesh.from_vertex_handle(heh);
			MyMesh::VertexHandle vh2 = submodel.mesh.to_vertex_handle(heh);

			// get the point of 2 vertices
			MyMesh::Point p1 = submodel.mesh.point(vh1);
			MyMesh::Point p2 = submodel.mesh.point(vh2);

			// get the distance between this 2 vertices
			totalLength3DSpace  += glm::distance(glm::vec3(p1[0], p1[1], p1[2]), glm::vec3(p2[0], p2[1], p2[2]));
			cummulativeLength.push_back(totalLength3DSpace);
			// goto next half edge, since we are sure our halfedge is at the boundary, it is sure to traverse the perimeter
			heh = submodel.mesh.next_halfedge_handle(heh);
		} while (heh != firstHeh);

		// now we have the total length
		 
		if (curBorderMode == QUAD) {
			// yet we set our uv space to be (0, 0), (0, 1), (1, 1), (1, 0)
			// so the total length of uv space should be 4
			float totalLengthUVSpace = 4;

			// re-init our heh
			heh = firstHeh;
			// init our first texture point based on the formula of UVRotateAngle, from 姚老師‘s code
			float rotateRadiant = (225 + uvRotateAngle) * M_PI / 180.0;
			// this variable track the distance for each vertex runned through
			// without rotate, essentially we can choose to start from zero, but since we have rotate, might interpolate between 0-4, to decide placement on which edge
			float currentDis = 0;
			MyMesh::TexCoord2D currentTexCoord(0, 0);
			float lengthInterpolate = std::sqrt(2) / 2.0; //multiplied with rotateRadiant to decide start position
			currentTexCoord[0] = lengthInterpolate * cos(rotateRadiant) + 0.5;
			currentTexCoord[1] = lengthInterpolate * sin(rotateRadiant) + 0.5;

			//but remember coordinate should be at border, and in range of 0 to 1 in uv space
			//handle range of currentTexCoord[0] first
			if (currentTexCoord[0] > 1) {
				currentTexCoord[0] = 1;
				currentTexCoord[1] = tan(rotateRadiant) / 2 + 0.5;
			}
			else if (currentTexCoord[0] < 0) {
				currentTexCoord[0] = 0;
				currentTexCoord[1] = 0.5 - tan(rotateRadiant) / 2;
			}

			//now handle currentTexCoord[1] in similar manner
			if (currentTexCoord[1] > 1) {
				currentTexCoord[1] = 1;
				currentTexCoord[0] = tan(M_PI_2 - rotateRadiant) / 2 + 0.5;
			}
			else if (currentTexCoord[1] < 0) {
				currentTexCoord[1] = 0;
				currentTexCoord[0] = 0.5 - tan(M_PI_2 - rotateRadiant) / 2;
			}
			//put current coordinate both submodel and model
			if (!model.mesh.has_vertex_texcoords2D())
			{
				model.mesh.request_vertex_texcoords2D();
			}
			if (!submodel.mesh.has_vertex_texcoords2D()) {
				submodel.mesh.request_vertex_texcoords2D();
			}
			submodel.mesh.set_texcoord2D(submodel.mesh.from_vertex_handle(heh), currentTexCoord);
			MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
			model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
			//based on uv rotate, which is done in range 0 - 360, we interpolate the start distance
			// mapping order: down - right - up - left
			float initDis = 0;
			if (uvRotateAngle <= 90)
			{
				initDis = currentTexCoord.length();
			}
			else if (uvRotateAngle <= 180)
			{
				initDis = 1 + currentTexCoord[1];
			}
			else if (uvRotateAngle <= 270)
			{
				initDis = 3 - currentTexCoord[0];
			}
			else
			{
				initDis = 4 - currentTexCoord[1];
			}
			unsigned int tempIndex = 0;
			currentDis = cummulativeLength[tempIndex] / totalLength3DSpace * 4.0 + initDis;
			//traverse the other vhs
			heh = submodel.mesh.next_halfedge_handle(heh);
			while (heh != firstHeh) {
				currentDis = fmod(currentDis, 4);

				if (currentDis <= 1) {
					//down
					currentTexCoord[0] = currentDis;
					currentTexCoord[1] = 0;
				}
				else if (currentDis <= 2) {
					//right
					currentTexCoord[0] = 1;
					currentTexCoord[1] = currentDis - 1;
				}
				else if (currentDis <= 3) {
					//up
					currentTexCoord[0] = 3 - currentDis;
					currentTexCoord[1] = 1;
				}
				else {
					//left
					currentTexCoord[0] = 0;
					currentTexCoord[1] = 4 - currentDis;
				}
				currentDis = cummulativeLength[++tempIndex] / totalLength3DSpace * 4.0 + initDis;
				submodel.mesh.set_texcoord2D(submodel.mesh.from_vertex_handle(heh), currentTexCoord);
				MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
				model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
				heh = submodel.mesh.next_halfedge_handle(heh);
			}
		}
		else {
			//disk border
			heh = firstHeh;

			//first point starts from uvRotateAngle
			double startAngle = fmod(uvRotateAngle,360)*2* M_PI;
			MyMesh::TexCoord2D currentTexCoord((std::cos(startAngle) + 1.0)/2, (std::sin(startAngle) + 1.0) / 2);
			submodel.mesh.set_texcoord2D(submodel.mesh.from_vertex_handle(heh), currentTexCoord);
			MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
			model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
			int tempIndex = 0;
			heh = submodel.mesh.next_halfedge_handle(heh);
			while (heh != firstHeh) {
				//count angle from Start angle
				//interpolate between 0 - 2pi
				double angleFromStart = 2 * M_PI * cummulativeLength[++tempIndex] / totalLength3DSpace;
				double totalAngle = fmod(startAngle + angleFromStart, 2 * M_PI);
				currentTexCoord[0] = (std::cos(totalAngle) + 1.0) / 2;
				currentTexCoord[1] = (std::sin(totalAngle) + 1.0) / 2;
				submodel.mesh.set_texcoord2D(submodel.mesh.from_vertex_handle(heh), currentTexCoord);
				MyMesh::VertexHandle subToModelVTranslation = submodel.mesh.property(toModelV, submodel.mesh.from_vertex_handle(heh));
				model.mesh.set_texcoord2D(subToModelVTranslation, currentTexCoord);
				heh = submodel.mesh.next_halfedge_handle(heh);
			}
		}
		
	}else {
		return;
	}

	// find the middle points
	std::vector<unsigned int> rowMapToVHID;
	// solve linear equation
	// we should run through 2 loops (vertex handle iter)
	// first loop is to initialize the mapping between MatIDMapToVHID (vertex handle iter)
	for (MyMesh::VertexIter v_it = submodel.mesh.vertices_begin(); v_it != submodel.mesh.vertices_end(); ++v_it) {
		// we only fill the vertex that isn't a boundary
		if (!submodel.mesh.is_boundary(*v_it)) {
			rowMapToVHID.push_back(v_it->idx());
		}
	}

	if (rowMapToVHID.size() > 0) {
		// initialize the matrix
		int m = rowMapToVHID.size();
		SparseMatrix<double> A(m, m);
		VectorXd BX(m);
		VectorXd BY(m);

		// reset all matrix to 0
		A.setZero();
		BX.setZero();
		BY.setZero(); 
		// second loop is to fill up the matrix
		for (MyMesh::VertexIter v_it = submodel.mesh.vertices_begin(); v_it != submodel.mesh.vertices_end(); ++v_it) {
			// if vertex is not boundary vertex
			if (!submodel.mesh.is_boundary(*v_it)) {
				
				int row = getMatrixRowNumber(rowMapToVHID, v_it->idx());
				double totalWeight = 0;

				// we use one ring to find the vertex around v_it vertex
				//put if outside to prevent repeated if
				if (curWeightMode == MEAN) {
					for (MyMesh::VertexVertexIter v_around = submodel.mesh.vv_iter(*v_it); v_around.is_valid(); v_around++) {
						//get weight, based on our storing method of halfedge, we find he where center the from_vHandle is v_it
						MyMesh::HalfedgeHandle heHandle = submodel.mesh.find_halfedge(*v_it, *v_around);
						MyMesh::HalfedgeHandle originalHeHandle = submodel.mesh.property(toModelHe, heHandle);
						double curWeight = model.mesh.property(tanOrigheWeight, originalHeHandle);
						totalWeight += curWeight;
						//check whether v_around is boundary, to decide where to put the v_around
						if (submodel.mesh.is_boundary(*v_around)) {
							//put it on the Bx or By
							MyMesh::TexCoord2D curTex = submodel.mesh.texcoord2D(*v_around);
							BX[row] += curWeight * curTex[0];
							BY[row] += curWeight * curTex[1];
						}
						else {
							//put it on the A
							int column = getMatrixRowNumber(rowMapToVHID, v_around->idx());

							//put on A[row, column] with negative weight
							A.insert(row, column) = -curWeight;
						}
					}
				}
				else if (curWeightMode == HARMONIC) {
					for (MyMesh::VertexVertexIter v_around = submodel.mesh.vv_iter(*v_it); v_around.is_valid(); v_around++) {
						//get weight, based on our storing method of halfedge, we find he where center the from_vHandle is v_it
						MyMesh::HalfedgeHandle heHandle = submodel.mesh.find_halfedge(*v_it, *v_around);
						MyMesh::HalfedgeHandle originalHeHandle = submodel.mesh.property(toModelHe, heHandle);
						double curWeight = model.mesh.property(cotOrigheWeight, originalHeHandle);
						totalWeight += curWeight;
						//check whether v_around is boundary, to decide where to put the v_around
						if (submodel.mesh.is_boundary(*v_around)) {
							//put it on the Bx or By
							MyMesh::TexCoord2D curTex = submodel.mesh.texcoord2D(*v_around);
							BX[row] += curWeight * curTex[0];
							BY[row] += curWeight * curTex[1];
						}
						else {
							//put it on the A
							int column = getMatrixRowNumber(rowMapToVHID, v_around->idx());

							//put on A[row, column] with negative weight
							A.insert(row, column) = -curWeight;
						}
					}
				}
				else {
					for (MyMesh::VertexVertexIter v_around = submodel.mesh.vv_iter(*v_it); v_around.is_valid(); v_around++) {
						//get weight, based on our storing method of halfedge, we find he where center the from_vHandle is v_it
						MyMesh::HalfedgeHandle heHandle = submodel.mesh.find_halfedge(*v_it, *v_around);
						MyMesh::HalfedgeHandle originalHeHandle = submodel.mesh.property(toModelHe, heHandle);
						double curWeight = model.mesh.property(uniOrigheWeight, originalHeHandle);
						totalWeight += curWeight;
						//check whether v_around is boundary, to decide where to put the v_around
						if (submodel.mesh.is_boundary(*v_around)) {
							//put it on the Bx or By
							MyMesh::TexCoord2D curTex = submodel.mesh.texcoord2D(*v_around);
							BX[row] += curWeight * curTex[0];
							BY[row] += curWeight * curTex[1];
						}
						else {
							//put it on the A
							int column = getMatrixRowNumber(rowMapToVHID, v_around->idx());

							//put on A[row, column] with negative weight
							A.insert(row, column) = -curWeight;
						}
					}
				}
				
				// lastly we put the total weight of the current row weight
				A.insert(row, row) = totalWeight;
			}
		}
		
		//for debug only
		/*double tBX = BX[0];
		double tBY = BY[0];
		double tA = A.coeffRef(0, 0);
		std::cout << tBX << " " << tBY << " " << tA << std::endl;*/


		// solve the linear equation with eigen
		A.makeCompressed();
		SparseQR<SparseMatrix<double>, COLAMDOrdering<int>> linearSolverX;
		//SimplicialCholesky<SparseMatrix<double>> linearSolverX;
		linearSolverX.compute(A);
		VectorXd X = linearSolverX.solve(BX);
		// AX = (BX)
		// X = (A^(-1))(BX)
		// AY = (BY)
		// Y = (A^(-1))(BY)
		SparseQR<SparseMatrix<double>, COLAMDOrdering<int>> linearSolverY;
		//SimplicialCholesky<SparseMatrix<double>> linearSolverY;
		linearSolverY.compute(A);
		VectorXd Y = linearSolverY.solve(BY);

		
		// place all X to the correct uv
		for (int i = 0; i < rowMapToVHID.size(); i++) {
			int vhID = rowMapToVHID[i];
			MyMesh::VertexHandle vh = submodel.mesh.vertex_handle(vhID);
			submodel.mesh.set_texcoord2D(vh, MyMesh::TexCoord2D(X[i], Y[i]));
		}
	}

	//map back the tex coord from the submodel to the original mesh
	//all the unset tex coord has been set to -1 -1 previously during the beginning of the function
	int index = 0;
	for (MyMesh::FaceIter f_it = submodel.mesh.faces_begin(); f_it != submodel.mesh.faces_end(); ++f_it)
	{
		//std::cout << "a\n";
		MyMesh::FaceHandle realFace = model.mesh.face_handle(tempVec[index++]);
		MyMesh::FaceHandle subFace = *f_it;
		//std::cout << "b\n";
		MyMesh::FaceVertexIter real_fv_iter = model.mesh.fv_iter(realFace);
		//std::cout << "c\n";
		for (MyMesh::FaceVertexIter sub_fv_iter = submodel.mesh.fv_iter(subFace); real_fv_iter.is_valid() && sub_fv_iter.is_valid(); ++real_fv_iter, ++sub_fv_iter)
		{
			MyMesh::TexCoord2D subTexCoord = submodel.mesh.texcoord2D(*sub_fv_iter);

			//take care of scaling
			glm::vec4 tempVec((subTexCoord[0]-0.5)*scaler + 0.5, (subTexCoord[1]-0.5)*scaler+0.5, 0, 1);
			subTexCoord[0] = tempVec.x;
			subTexCoord[1] = tempVec.y;
			//submodel.mesh.set_texcoord2D(*sub_fv_iter, subTexCoord); //DANGEROUS, DO NOT OPEN
			model.mesh.set_texcoord2D(*real_fv_iter, subTexCoord);
		}
		//std::cout << "d\n";
	}

	//update the data to VBO
	model.LoadTexCoordToShader();

	//For the multiDrawElements in render parameterized, update the face index and also the total vertices need to be drawn
	fvIDsPtr.clear(); 
	elemCount.clear();
	fvIDsPtr.reserve(tempVec.size());
	elemCount.reserve(tempVec.size());
	for (int i = 0; i < tempVec.size(); ++i)
	{
		fvIDsPtr.push_back((GLuint*)(tempVec[i] * 3 * sizeof(GLuint))); //multiply by 3 since 1 selectedFace = 3 vertices. So to point to first vertices, *3
		elemCount.push_back(3);
	}


	// debug
	OpenMesh::IO::Options wopt;
	wopt += OpenMesh::IO::Options::VertexTexCoord;
	wopt += OpenMesh::IO::Options::VertexNormal;

	if (!OpenMesh::IO::write_mesh(model.mesh, "../Proj3Assets/debug.obj", wopt))
	{
		printf("Write Mesh Error\n");
	}
}

void MeshObject::RenderParameterized()
{

	if (model.mesh.has_vertex_texcoords2D() && elemCount.size() > 0) {
		//std::cout << "Rendering!\n";
		glBindVertexArray(model.MeshVAO);
		glMultiDrawElements(GL_TRIANGLES, &elemCount[0], GL_UNSIGNED_INT, (const GLvoid**)&fvIDsPtr[0], elemCount.size());
		glBindVertexArray(0);
	}
}
#pragma endregion