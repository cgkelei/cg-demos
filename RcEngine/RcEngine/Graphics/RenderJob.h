#ifndef RenderJob_h__
#define RenderJob_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"

namespace RcEngine
{
	namespace Render
	{
		class _ApiExport RenderJob
		{		
		public:
			enum StreamType
			{
				ST_Geometry,
				ST_Instance
			};

			struct StreamUnit
			{
				shared_ptr<GraphicsBuffer> Stream;
				shared_ptr<VertexDeclaration> VertexDecl;

				uint32_t VertexSize;
				uint32_t Frequency;
				StreamType Type;
			};

			typedef std::vector<StreamUnit> StreamSlots;

			void AddVertexStream(const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& decl, uint32_t fre =1, StreamType type = ST_Geometry );


		public:
			RenderJob();

			PrimitiveType PrimitiveType;

			shared_ptr<Material> Material;
			StreamSlots VertexStreams;



			bool UseIndex;
			shared_ptr<GraphicsBuffer> IndexBuffer;
			IndexBufferType IndexType;
			
			uint32_t IndexCount;
			uint32_t VertexCount;
			uint32_t PrimitiveCount;

			int32_t  BaseVertexLocation;
			uint32_t StartVertexLocation;
			uint32_t StartIndexLocation;


			uint32_t StartInstanceLocation;
		};


		class _ApiExport RenderJobCollection
		{
		public:
			RenderJobCollection();
			~RenderJobCollection();

			void AddJob(RenderJob* job)								{ mRenderJobs.push_back(job); }
			void Clear()											{ mRenderJobs.clear(); }		
			RenderJob* GetRenderJob(size_t i) const					{ return mRenderJobs[i]; }
			const vector<RenderJob*>& GetRenderJobs() const 		{ return mRenderJobs; }

		private:
			vector<RenderJob*> mRenderJobs;
		};
	}
}


#endif // RenderJob_h__

