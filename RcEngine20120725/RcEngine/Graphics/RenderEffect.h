#pragma once

#include "Core/Prerequisites.h"

namespace RcEngine {
	namespace Render {

		//////////////////////////////////////////////////////////////////////////
		class _ApiExport RenderEffect
		{
		public:
			typedef std::vector<RenderTechnique*> TechniqueList;
			typedef std::vector<RenderParameter*> ParameterList;
			typedef std::vector<std::pair<String, std::vector<uint32_t> > > ConstBuffers;
			typedef std::vector<std::pair<String, String> > Macros;


		public:
			RenderEffect(const std::string& effectFile, const std::string& effectName);
			virtual ~RenderEffect(void);

			void Load(const String& effectFile);

			bool IsValid() const;

			TechniqueList&	 GetTechniques();
			RenderTechnique* GetTechniqueByName(const std::string& techName);

			ParameterList&	 GetParameters();
			RenderParameter* GetParameterByName(const std::string& paraName);
			RenderParameter* GetParameterBySemantic(const std::string& semanticName);

			virtual bool LoadEffectFromFile(const std::string& effectFile) = 0;

		protected:
			virtual void Destroy() = 0;

		protected:

			TechniqueList   mTechniques;
			ParameterList   mParameters;
			ConstBuffers    mConstBuffers;
			Macros			mMacros;

			bool            mIsValid;
			std::string     mEffectName;
			std::string     m_EffectFile;
		};



		class _ApiExport RenderTechnique
		{
		public:
			typedef std::vector<RenderPass*> PassList;

		public:
			RenderTechnique();
			virtual ~RenderTechnique();


			const std::string& GetTechniqueName() const;
			bool IsValid() const;
			PassList GetPasses();
			RenderPass* GetPassByName( const std::string& passName);

		protected:

			PassList			mPasses;
			std::string			mTechniqueName;
			bool                mIsValid;
		};

		class _ApiExport RenderPass
		{
		public:
			RenderPass();
			virtual ~RenderPass();


			const std::string& GetPassName() const;
			bool IsValid() const;

			virtual bool BeginPass() = 0;
			virtual void EndPass() = 0;

		protected:
			std::string			mPassName;
			bool                mIsValid;
		};


	} // RenderSystem
} // RcEngine
