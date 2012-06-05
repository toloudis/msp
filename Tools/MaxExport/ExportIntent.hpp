/*****************************************************************************
**  ExportIntent.hpp
****
**	Base of the exporter class which signifies the intention
**	of the export (eg: whether this is a model export, animation export)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MAXEXP_EXPORTINTENT_HPP
#error MAXEXP_EXPORTINTENT_HPP multiply defined!!
#endif
#define MAXEXP_EXPORTINTENT_HPP


#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#include <hash_set>
#include <map>
#include <list>

//forward declarations
class Matrix3;
class mdlNodeInfo;
class INode;
class gfFileBin;
class chBinWriter;
namespace MaxExp
{
	class ExportDoc;
	class BaseExporter;
	namespace MaxObjectType
	{
		enum TypeVal;
	}
}

namespace MaxExp
{

   //========================================================================
	// Wrapper class for progress report logic
	//========================================================================
	//wrapper class for encapsulating
	// the logic of starting, updating and ending
	//progress report communications to
	//GetCOREInterface()
	class ProgressReport
	{
	public:
		ProgressReport( float i_fTotalBudget );
		 virtual ~ProgressReport();
		  virtual void Update( float i_fDelta );
		  //initialized value of a pogresss budget
		  float m_fTotalBudget;
		  //progress so far on this budget
		  float m_fCurrentProgress;
	};


	//========================================================================
	//This is an stl container class
	//we use to store the relevant nodes that need be exported
	//as calculated by ModelExportIntent::ComputeNodesTobeExported
	//========================================================================

	class NodesToBeExported : public stdext::hash_set< INode * >
	{
		//Note: deriving off stl container classes is not usually recommended
		//since they dont have any virtual destructors.
		//But as long as your derived class doesnt  cause
		//any memory leak on deletion, you are fine
	public:
		NodesToBeExported(ExportDoc &exportDoc): 
		  hash_set(),
			  m_pExportDoc(&exportDoc){}
		  NodesToBeExported( const NodesToBeExported & other):
		  hash_set(other),
			  m_pExportDoc(other.m_pExportDoc){}
		  //After  filling up this data structure
		  //just before the export do this
		  bool PreExportValidate();
		  bool IsExported( INode *  i_pCurNode ) const;
		  ExportDoc *m_pExportDoc;
	};

//========================================================================
	// Class for exporting nodes
	//========================================================================
	class ExportIntent
	{
	public:
		ExportIntent( ExportDoc &doc):
			m_pExportDoc(&doc),
			m_pRootNode(NULL),
			m_NodesToBeExported(doc),
			m_nNumNodesSoFarConsideredForExport(0){}
		  virtual ~ExportIntent(){}
		  //export
		  virtual void Do( const std::list<INode *> &suggestedNodes );

		 bool IsExported( INode *  i_pCurNode ) const
		 {
			return ( m_NodesToBeExported.find( i_pCurNode ) != m_NodesToBeExported.end() );
		 }
		 	//
		//For a given model return the  exporter suited
		virtual BaseExporter *GetAppropriateExporter( INode *i_pCurNode );
			
		virtual void ReportProgress(float i_fDelta );

		void ExtractUserDefinedProps_2( INode *i_pCurNode, shared_ptr< mdlNodeInfo >  &io_mdlNodeInfo );
	public:
		static const maMatrix4x4 m_ZaxisUpToYaxisUp;
		static const maMatrix4x4 m_InvZaxisUpToYaxisUp;		
		//pointer to export
		ExportDoc *m_pExportDoc;
	protected:
		//relevant nodes to be exported
		//as found by ComputeNodesToBeExported
		NodesToBeExported m_NodesToBeExported;
		//cached value of rootNode of the scene
		INode * m_pRootNode;
		size_t m_nNumNodesSoFarConsideredForExport;
		shared_ptr< ProgressReport > m_ProgressReport;
	

	protected:

		//A wrapper for governing the
		//life time semantics of opening
		//and closing of the exported file
		struct FileWriterLifeTimeKeeper
		{
			FileWriterLifeTimeKeeper(ExportDoc &exportDoc);
			~FileWriterLifeTimeKeeper();
			void Cleanup();
			ExportDoc *m_pExportDoc;
			gfFileBin *m_pFile;
			chBinWriter *m_pWriter;		
		};


		//Validate the export
		//If not valid dont proceed
		virtual bool PreExportValidation()
		{
			return true;
		};

	
		//Cleanup book keeping
		virtual void CleanUp();

		//Recurse though the heirarchy to collect all the 
		//relavant nodes that need be exported
		virtual size_t ComputeNodesToBeExported( const std::list<INode *> &suggestedNodes );

		//Core Recursive function used by the above function
		//see .cpp
		virtual bool ComputeNodesToBeExportedRec(
			INode *						i_CurNode, 
			const std::list<INode *> &		io_SelectedNodes, 
			stdext::hash_set<INode *> &	io_ForcedNodes);

		//The main function to export the deserving nodes
		virtual void ExportNodes()=0;
		//straighten some book keeping before the 
		//export (eg: for vertex anim intent,  get a list of bone nodes ordered
		//by the depth first traversal of the bone hierarchy)
		virtual void PreExportBookKeeping()=0;		
		//The core recursive workhorse function that 
		//export the nodes
		
		//i_pExportedRootNode:
		//            SceneRoot
		//             /     \
		//            A       B
		//          /  \     /  \
		//         C   D    E   F
		//        / \
		//       G   H
		// If we select the C and B nodes and export
		// the exportedRootNode for the C subhierarchy nodes is C
		// and the exportedRootNode for the B syb hierarchy is B
		virtual void ExportNodesRec( 
			INode *i_CurNode ,  		
			INode *& io_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &io_MdlNode )=0;	
		//process after all the meshes are exported
		virtual void PostProcess( shared_ptr<mdlNodeInfo> &io_RootMdlNode)=0;		//
		//For a given model return the type, typename and the exporter suited
		virtual BaseExporter *GetAppropriateExporter( INode *i_pCurNode, std::string &stypeName, MaxObjectType::TypeVal &objType )=0;
		//extract sgpu specific user data properties
		virtual void ExtractUserDefinedProps( INode *i_pCurNode, shared_ptr< mdlNodeInfo >  &io_mdlNodeInfo );		
		//apply y-axis up correction for the root node
		void ApplyYAxisUpXformCorrection(  shared_ptr<mdlNodeInfo> &io_RootMdlNode );
		//write the frame rate chunk, used by the animation export intents
		void WriteFrameRateChunk( chBinWriter &io_Writer );
		//write the beginframe chunk, used by the animation export intents
		void WriteBeginFrameChunk( chBinWriter &io_Writer, float i_fStartFrame );
		//write the exporter version stamp chunk
		void WriteExporterVersionStamp(chBinWriter &o_Writer);

	};

} //namespace MaxExp
