#include "StaticModelDrawable.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/ShaderMgr.h"
#include "../Manager/TextureMgr.h"

#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Renderer
{
	StaticModelDrawable::StaticModelDrawable(unsigned int shaderIndex, const std::string &path)
	{
		m_shaderIndex = shaderIndex;
		loadModel(path);
	}

	StaticModelDrawable::~StaticModelDrawable()
	{
	}

	void StaticModelDrawable::render(Camera3D::ptr camera, Light::ptr sunLight,
		Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		// render the model.
		if (!m_visiable) return;
		if (shader == nullptr)
		{
			shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
			shader->bind();
		}
		if(sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("albedoMap", 0);
		shader->setInt("normalMap", 1);
		shader->setInt("roughMap", 2);
		shader->setInt("metallicMap", 3);
		shader->setFloat("nearPlane", camera->getNear());
		shader->setFloat("farPlane", camera->getFar());
		// depth map.
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		if (depthMap != nullptr)
		{
			shader->setInt("depthMap", 4);
			depthMap->bind(4);
		}
		if (lightCamera != nullptr)
			shader->setMat4("lightSpaceMatrix",
				lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		else
			shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
		// object matrix.
		shader->setBool("instance", false);
		shader->setBool("receiveShadow", m_receiveShadow);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		shader->setMat3("normalMatrix", m_transformation.getNormalMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void StaticModelDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		if (!m_visiable || !m_produceShadow)
			return;
		shader->bind();
		shader->setBool("instance", false);
		shader->setMat4("lightSpaceMatrix",
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void StaticModelDrawable::loadModel(const std::string &path)
	{
		// load the model file.
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate 
			| aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		m_directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}

	void StaticModelDrawable::processNode(aiNode *node, const aiScene *scene)
	{
		// process all meshes.
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			unsigned int meshIndex;
			PBRMaterial pbrMat;
			processMesh(mesh, scene, meshIndex, pbrMat);
			this->addMesh(meshIndex);
			this->addPbrTexture(pbrMat);
		}
		// process children' nodes.
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void StaticModelDrawable::processMesh(aiMesh * mesh, const aiScene * scene,
		unsigned int & meshIndex, PBRMaterial &pbrMat)
	{
		// process mesh.
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		for (unsigned int x = 0; x < mesh->mNumVertices;++ x)
		{
			Vertex vertex;
			// position.
			vertex.position = glm::vec3(
				mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z);
			// normal.
			vertex.normal = glm::vec3(
				mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z);
			// texcoord.
			if (mesh->mTextureCoords[0])
				vertex.texcoord = glm::vec2(
					mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y);
			else
				vertex.texcoord = glm::vec2(0.0f, 0.0f);
			// color.
			vertex.color = vertex.normal;
			// tangent.
			vertex.tangent = glm::vec3(
				mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z);
			// bitangent.
			vertex.bitangent = glm::vec3(
				mesh->mBitangents[x].x, mesh->mBitangents[x].y, mesh->mBitangents[x].z);

			vertices.push_back(vertex);
		}

		for (unsigned int x = 0; x < mesh->mNumFaces;++ x)
		{
			aiFace face = mesh->mFaces[x];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int y = 0; y < face.mNumIndices;++ y)
				indices.push_back(face.mIndices[y]);
		}

		Mesh *target = new Mesh(vertices, indices);
		meshIndex = MeshMgr::getSingleton()->loadMesh(target);

		// process pbr material.
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiString nameStr;

			// abledo texture.
			material->GetTexture(aiTextureType_DIFFUSE, 0, &nameStr);
			std::string name(nameStr.C_Str());
			if (!name.empty())
				pbrMat.m_albedoTexIndex = textureMgr->loadTexture2D(name, m_directory + "/" + name);

			// normal texture.
			material->GetTexture(aiTextureType_HEIGHT, 0, &nameStr);
			name = nameStr.C_Str();
			if (!name.empty()) 
				pbrMat.m_normalTexIndex = textureMgr->loadTexture2D(name, m_directory + "/" + name);

			// roughness texture.
			material->GetTexture(aiTextureType_SHININESS, 0, &nameStr);
			name = nameStr.C_Str();
			if (!name.empty())
				pbrMat.m_roughTexIndex = textureMgr->loadTexture2D(name, m_directory + "/" + name);

			// metallic texture.
			material->GetTexture(aiTextureType_AMBIENT, 0, &nameStr);
			name = nameStr.C_Str();
			if (!name.empty())
				pbrMat.m_metallicIndex = textureMgr->loadTexture2D(name, m_directory + "/" + name);
		}
	}

}