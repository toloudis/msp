/**********************************************************************

							STUDIOGPU ADDON

 	File:		ExportStudioGPU.cpp

	Purpose:	Implementation of the StudioGPU export class

	Target:		ArchiCAD 12

	Copyright:	Encina Ltd 2009

	Revision history:
	07-05-2009	RW	Created

 **********************************************************************/

#include "ExportStudioGPU.h"

#include "Addon.h"
#include "Body.h"
#include "CommandEvent.h"
#include "CustomProgressWindow.h"
#include "DataFile.h"
#include "Drawing.h"
#include "Element3D.h"
#include "Environment.h"
#include "Face.h"
#include "FileEvent.h"
#include "Hatch.h"
#include "ImageUtility.h"
#include "Leveller.h"
#include "MathFunctions.h"
#include "Matrix3x3.h"
#include "MenuEvent.h"
#include "ParameterList.h"
#include "Plan.h"
#include "Project.h"
#include "SimpleFace.h"
#include "sgpuScene.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuNode.hpp"
#include "StringParameter.h"
#include "StudioGPUResource.h"
#include "UserPath.h"
#include "Vector3.h"
#include "Version.h"

#include "DataBuffer.h"
#include "DataFile.h"

#include "boost/shared_ptr.hpp"

#include <map>

using namespace encina;
using namespace studiogpu;

//#define EXPORT_LOG

namespace {
	
	const Int32 exportStudioGPUVersionNo = 1L;
	
	const double studioGPUnitScale = 100.0;
	
	const char* exportVersionStr = "ArchiCAD Export";
	const char* untitledDocumentStr = "Untitled";
	
	class MaterialLink {
	public:
		MaterialLink(model_index index, const CadString& name, sgpuMaterial mat) : m_name(name), m_material(mat)
		{
			m_index = index;
		}
		
		bool operator== (model_index ref)	{ return m_index == ref; }
		bool operator== (const CadString& ref)	{ return m_name == ref; }

		model_index m_index;
		CadString m_name;
		sgpuMaterial m_material;

	private:
		MaterialLink();
	};

	class MaterialCatalog : public std::list<MaterialLink> {
	public:
		typedef std::list<MaterialLink> base;
		typedef base::iterator iterator;

		iterator find(model_index ref)
		{
			for (iterator i = begin(); i != end(); ++i)
				if (*i == ref)
					return i;
			return end();
		}
	};
	
	DataBufferOut* testLog;
	
	/*--------------------------------------------------------------------
		Write a specified body to a StudioGPU file
		
	 	body: The body to write
	 	bodyIndex: A unique body index
	 	materialIndex: The material index for the body
	 	elementNode: The element node
	 
	 	return: The body mesh
	  --------------------------------------------------------------------*/
	sgpuMesh writeBody(const Body& body, model_index& bodyIndex, model_index materialIndex, const Matrix3x3& transform, sgpuNode& elementNode)
	{
			//Create a new mesh
		sgpuMesh bodyMesh = elementNode.CreateTriangleMesh();
#ifdef EXPORT_LOG
		*testLog << "CreateTriangleMesh\t" << (CadString("Body ") + ToString(bodyIndex).value()).data() << "\n";
#endif
		bodyMesh.SetMeshName((CadString("Body ") + ToString(bodyIndex++).value()).data());
			//Calculate the number of faces and vertices associated with the specified material
		int meshIndex = 0, totalFaces = 0, totalVertices = 0;
		for (model_index n = body.faceSize(); n--; ) {
			Face face = body.face(n);
			if (face.getMaterialIndex() == materialIndex) {
				totalFaces += face.facetSize();
				for (model_index j = 0; j < face.facetSize(); ++j)
					totalVertices += face.facet(j).vertexSize();
			}
		}
		bodyMesh.SetNumVertices(totalVertices);
#ifdef EXPORT_LOG
		*testLog << "SetNumVertices:\t" << (Int32) totalVertices << "\n";
#endif
			//Export the mesh vertices and normals
		for (model_index n = body.faceSize(); n--; ) {
			Face face = body.face(n);
			if (face.getMaterialIndex() == materialIndex) {
				for (model_index j = 0; j < face.facetSize(); ++j) {
					SimpleFace facet = face.facet(j);
					for (model_index i = 0; i < facet.vertexSize(); ++i, ++meshIndex) {
							//Write the vertex coordinates
						CadPoint vertex = body.vertex(facet.vertex(i));
						bodyMesh.SetPosition(meshIndex, studioGPUnitScale * vertex.x(), studioGPUnitScale * vertex.z(), studioGPUnitScale * -vertex.y());
#ifdef EXPORT_LOG
						*testLog << "SetPosition:\t" << (Int32) meshIndex << "\t" << studioGPUnitScale * vertex.x() << "\t" <<
								studioGPUnitScale * vertex.z() << "\t" << studioGPUnitScale * -vertex.y() << "\n";
#endif
							//Write the vertex texture coordinates
						CadPoint coord(face.textureCoordinates(vertex));
						coord *= transform;
						bodyMesh.SetTexCoord(meshIndex, coord.x(), coord.y());
#ifdef EXPORT_LOG
						*testLog << "SetTexCoord:\t" << (Int32) meshIndex << "\t" << coord.x() << "\t" << coord.y() << "\n";
#endif
							//Write the vertex normal
						Vector3 normal(body.vector(facet.vector(i)));
						bodyMesh.SetNormal(meshIndex, normal[0], normal[2], -normal[1]);
#ifdef EXPORT_LOG
						*testLog << "SetNormal:\t" << (Int32) meshIndex << "\t" << normal[0] << "\t" << normal[2] << "\t" << -normal[1] << "\n";
#endif
					}
				}
			}
		}
		bodyMesh.SetNumIndices(3 * (totalVertices - (2 * totalFaces)));
#ifdef EXPORT_LOG
		*testLog << "SetNumIndices:\t" << (Int32) (3 * (totalVertices - (2 * totalFaces))) << "\n";
#endif
			//Export the mesh indices
		meshIndex = 0;
		model_index facetOrigin = 0;
		for (model_index n = body.faceSize(); n--; ) {
			Face face = body.face(n);
			if (face.getMaterialIndex() == materialIndex) {
				for (model_index j = 0; j < face.facetSize(); ++j) {
					SimpleFace facet = face.facet(j);
					for (model_index i = 2; i < facet.vertexSize(); ++i) {
#ifdef EXPORT_LOG
						*testLog << "SetIndex:\t" << (Int32) meshIndex << "\t" << facetOrigin << "\n";
#endif
						bodyMesh.SetIndex(meshIndex++, facetOrigin);
#ifdef EXPORT_LOG
						*testLog << "SetIndex:\t" << (Int32) meshIndex << "\t" << facetOrigin + i - 1 << "\n";
#endif
						bodyMesh.SetIndex(meshIndex++, facetOrigin + i - 1);
#ifdef EXPORT_LOG
						*testLog << "SetIndex:\t" << (Int32) meshIndex << "\t" << facetOrigin + i << "\n";
#endif
						bodyMesh.SetIndex(meshIndex++, facetOrigin + i);
					}
					facetOrigin += facet.vertexSize();
				}
			}
		}
		return bodyMesh;
	} //writeBody
	
	
	/*--------------------------------------------------------------------
		Write the material for a specified body to a StudioGPU file
		
	 	material: The material to write
	 	index: The material index in the 3D model
	 	bodyMesh: The mesh to append the face to
	 	catalog: The catalog of exported materials
		relativePath: A relative path to the textures directory
	 	textureFolder: The texture directory
	  --------------------------------------------------------------------*/
	void writeMaterial(const Material& material, model_index index, sgpuMesh& bodyMesh, sgpuScene& scene,
					   MaterialCatalog& catalog, const CadString& relativePath, Directory& textureFolder)
	{
		MaterialCatalog::iterator thisMaterial = catalog.find(index);
		if (thisMaterial == catalog.end()) {
				//First check whether the name is unique and append a number as required
			short matching = 0;
			CadString materialName(material.getName());
				//We need a material name that can also be a legal file name
			replaceEnclosing(materialName);
			replaceAllOf(materialName, "?[]/\\=+<>:;\",*.", "_");
			for (MaterialCatalog::iterator i = catalog.begin(); i != catalog.end(); ++i)
				if (*i == materialName)
					++matching;
			if (matching > 0)
				materialName += (CadString("-sgpu") + ToString(matching).value());
			sgpuMaterial bodyMaterial = scene.CreateMaterial(materialName.data());
#ifdef EXPORT_LOG
			*testLog << "CreateMaterial\t" << materialName.data() << "\n";
#endif
			bodyMaterial.SetDiffuseColor(1, 1, 1);
			Image* image = material.getTextureImage();
			if (image != 0) {
#ifdef EXPORT_LOG
				*testLog << "SetDiffuseColor:\t" << 1L << "\t" << 1L << "\t" << 1L << "\t" << "\n";
#endif
				ResultCode err = noErr;
				CadString textureName(materialName);
				DataFile* textureFile = DataFile::create(textureFolder, textureName, DataFile::readWrite, err, "jpg", '    ', 'JPEG');
				textureName += ".jpg";
				if ((textureFile != 0) && image->writeTo(*textureFile, "image/jpeg", 24)) {
					bodyMaterial.SetDiffuseTexture((relativePath + textureName).data());
#ifdef EXPORT_LOG
					*testLog << "SetDiffuseTexture:\t" << (relativePath + textureName).data() << "\n";
#endif
					delete textureFile;
				}
				delete image;
			} else {
				CadColour surfaceColour(material.getSurfaceColour());
				bodyMaterial.SetDiffuseColor(surfaceColour.red(), surfaceColour.green(), surfaceColour.blue());
#ifdef EXPORT_LOG
				*testLog << "SetDiffuseColor:\t" << surfaceColour.red() << "\t" << surfaceColour.green() << "\t" << surfaceColour.blue() << "\t" << "\n";
#endif
			}
			CadColour specularColour(material.getSurfaceColour());
			bodyMaterial.SetSpecularColor(specularColour.red(), specularColour.green(), specularColour.blue());
#ifdef EXPORT_LOG
			*testLog << "SetSpecularColor:\t" << specularColour.red() << "\t" << specularColour.green() << "\t" << specularColour.blue() << "\t" << "\n";
#endif
			bodyMaterial.SetShininess((999 * material.getShininess()) + 1);
#ifdef EXPORT_LOG
			*testLog << "SetShininess:\t" << (999 * material.getShininess()) + 1 << "\n";
#endif
			bodyMaterial.SetOpacity(1 - material.getTransparency());
#ifdef EXPORT_LOG
			*testLog << "SetOpacity:\t" << 1 - material.getTransparency() << "\n";
#endif
			catalog.push_back(MaterialLink(index, materialName, bodyMaterial));
			bodyMesh.AssignMaterial(bodyMaterial);
		} else
			bodyMesh.AssignMaterial(thisMaterial->m_material);
	} //writeMaterial
	
	
	/*--------------------------------------------------------------------
		Write a specified element to a StudioGPU file
		
	 	element: The element to write
	 	bodyIndex: A unique body index
	 	scene: The exported scene
	 	catalog: The catalog of exported materials
	 	relativePath: A relative path to the directory
	 	textureFolder: The texture directory
	  --------------------------------------------------------------------*/
	void writeElement(const Element3D& element, model_index& bodyIndex, sgpuScene& scene,
					  MaterialCatalog& catalog, const CadString& relativePath, Directory& textureFolder)
	{
		for (model_index b = element.bodySize(); b--; ) {
				//First divide the body faces by material - each body must be a single material
			Body body = element.body(b);
			typedef std::map<model_index, Material> MaterialMap;
			std::map<model_index, Material> uniqueBody;
			for (model_index f = body.faceSize(); f--; ) {
				Face face = body.face(f);
				if (uniqueBody.find(face.getMaterialIndex()) == uniqueBody.end())
					uniqueBody[face.getMaterialIndex()] = face.getMaterial();
			}
				//Then write the body for each unique material
			model_index elementIndex = 0;
			for (MaterialMap::iterator i = uniqueBody.begin(); i != uniqueBody.end(); ++i, ++elementIndex) {
				sgpuNode elementNode = scene.GetRootNode().AddChildNode();
				CadString elementID = (ToString(element.getLink().getUniqueID()).value() + "-" + ToString(elementIndex).value());
				elementNode.SetNodeName(elementID.data());
#ifdef EXPORT_LOG
				*testLog << "AddChildNode\t" << elementID << "\n";
#endif
					//Create the texture coordinate transformation matrix
				Matrix3x3 transform(Matrix3x3::createScale(1 / i->second.getTextureScaleX(), 1 / i->second.getTextureScaleY(), 1));
				transform *= Matrix3x3::createZRotate(i->second.getTextureRotation());
				sgpuMesh bodyMesh = writeBody(body, bodyIndex, i->first, transform, elementNode);
				writeMaterial(i->second, i->first, bodyMesh, scene, catalog, relativePath, textureFolder);
			}
		}
	} //writeElement
	
	
	/*--------------------------------------------------------------------
		Get the directory for all linked textures
	 
		dest: The destination file for the exported model
	 	fileName: The final export filename (may differ when a scratch file is written)
	 	relativePath: A relative path to the directory

		return: A pointer to the textures directory, or 0 on failure
	  --------------------------------------------------------------------*/
	boost::shared_ptr<Directory> getTextureDirectory(DataFile& dest, const CadString& fileName, CadString& relativePath)
	{
		Directory* textureFolder = 0;
		Directory* parent = dest.getParent();
		if (parent != 0) {
			relativePath = fileName;
			CadString::size_type extPos = relativePath.rfind(".");
			if (extPos != CadString::npos)
				relativePath = relativePath.substr(0, extPos);
			if (!relativePath.empty()) {
				relativePath += " textures";
				textureFolder = new Directory(*parent, relativePath);
				ResultCode err = noErr;
				if (!textureFolder->exists()) {
					delete textureFolder;
					textureFolder = Directory::create(*parent, relativePath, err);
					if ((err != noErr) || (textureFolder == 0) || !textureFolder->exists()) {
						delete textureFolder;
						textureFolder = 0;
					}
				}
				relativePath = CadString(".") + File::pathDelimiter + relativePath + File::pathDelimiter;
			}
		}
		return boost::shared_ptr<Directory>(textureFolder);
	} //getTextureDirectory


	/*--------------------------------------------------------------------
		Write a StudioGPU file to the specified file
		
		dest: The destination file
	
		return: True if the export was successfully completed
	  --------------------------------------------------------------------*/
	bool doExport(DataFile& dest, const CadString& fileName)
	{
		CadString relativePath;
		boost::shared_ptr<Directory> textureFolder = getTextureDirectory(dest, fileName, relativePath);
		if (textureFolder == 0)
			return false;
		DataFile::AutoPtr projectFile = addon->getProject()->getFile();
		CadString projectName((projectFile.get() == 0) ? untitledDocumentStr : projectFile->getName());
		MaterialCatalog catalog;
#ifdef EXPORT_LOG
		DataFile* logFile = DataFile::create(Location("E:\\StudioGPU Export log.txt"), DataFile::readWrite);
		if (logFile == 0)
			return false;
		logFile->setSize(0);
		testLog = new DataBufferOut(logFile);
#endif
		CustomProgressWindow progress(progressDialog, app.getResString(titleString, sgpuProgressTitleStr), 1);
		Face::setFacetLoaded(true);
		sgpuScene scene;
		scene.GetRootNode().SetNodeName(projectName.data());
#ifdef EXPORT_LOG
		*testLog << "Scene\t" << projectName.data() << "\n";
#endif
		Model* model = addon->getProject()->getModel();
		progress.startNext(app.getResString(titleString, exportElementStr), model->size3D());
		model_index bodyIndex = 0;
		for (Model::const_iterator3D i = model->begin3D(); i != model->end3D(); ++i, ++progress)
			writeElement(**i, bodyIndex, scene, catalog, relativePath, *textureFolder);
		Face::setFacetLoaded(false);
#ifdef EXPORT_LOG
		delete testLog;
		testLog = 0;
		delete logFile;
#endif
		bool result = scene.WriteScene(dest.getPathName().data(), exportVersionStr);
		if (textureFolder->fileCount() == 0)
			textureFolder->erase();
		return result;
	} //doExport
	
}


/*--------------------------------------------------------------------
	Constructor
  --------------------------------------------------------------------*/
ExportStudioGPU::ExportStudioGPU() :
		File3DSubscriber(saveAsStudioGPU, '    ', 'GXB ', CadString("gxb"), studioGPUExportMenu, SaveAs3DSupported)
{
} //ExportStudioGPU::ExportStudioGPU


/*--------------------------------------------------------------------
	Respond to a subscribed Event
	
	event: The subscribed Event
	
	return: True if the event is solely handled by this subscriber
  --------------------------------------------------------------------*/
bool ExportStudioGPU::handleEvent(const File3DEvent& event)
{
	addon->reset();
	DataFile* file = event.getFile();
	Drawing* drawing = addon->getActiveDrawing();
	Model* model = addon->getProject()->getModel();
	if ((drawing == 0) || (model == 0) || (file == 0))
		return true;
	model->setViewpoint(event.getSight());
	doExport(*file, event.getName());
	addon->reset();
	return true;
} //ExportStudioGPU::handleEvent


/*--------------------------------------------------------------------
	Constructor
  --------------------------------------------------------------------*/
StudioGPUService::StudioGPUService() :
		CommandSubscriber(exportStudioGPUEvent, exportStudioGPUVersionNo)
{
} //StudioGPUService::StudioGPUService


/*--------------------------------------------------------------------
	Respond to a subscribed Event
	
	event: The subscribed Event
	
	return: True if the event is solely handled by this subscriber
  --------------------------------------------------------------------*/
bool StudioGPUService::handleEvent(const CommandEvent& event)
{
	addon->reset();
	bool result = false;
	const ParameterList* params = dynamic_cast<const ParameterList*>(&event);
	if ((params != 0) && (params->size() > 0)) {
		const StringParameter* nameParam = dynamic_cast<const StringParameter*>(params->getParameter("studiogpu_export_path"));
		if (nameParam != 0) {
			CadString pathName(nameParam->getValue());
			Drawing* drawing = addon->getActiveDrawing();
			if ((drawing != 0) && !pathName.empty()) {
				DataFile exportFile(pathName);
				result = doExport(exportFile, exportFile.getName());
			}
		}
	}
	Int32* message = reinterpret_cast<Int32*>(event.getResult());
	if (message != 0)
		*message = (result) ? 1 : 0;
	return true;
} //StudioGPUService::handleEvent
