#ifndef _VertexElement__H
#define _VertexElement__H

#include "Core/Prerequisites.h"
#include "GraphicsCommon.h"

namespace RcEngine {
	namespace Render {

		class  _ApiExport VertexElement
		{
		public:
			/// <summary>
			// Constructor, should not be called directly, only needed because of list
			/// </summary>
			VertexElement() {}

			/// <summary>
			// Constructor, should not be called directly, call VertexDeclaration::AddElement
			/// </summary>
			VertexElement(uint32 offset, VertexElementFormat theType, VertexElementUsage semantic, uint16 index = 0);

			/// <summary>
			// Gets the offset into the buffer where this element starts
			/// </summary>
			uint32 GetOffset(void) const { return mOffset; }

			/// <summary>
			// Gets the data format of this element
			/// </summary>
			VertexElementFormat GetType(void) const { return mFormat; }

			/// <summary>
			// Gets the data format of this element
			/// </summary>
			VertexElementUsage GetUsage(void) const  { return mUsage; }

			/// <summary>
			// Gets the index of this element, only applicable for repeating elements
			/// </summary>
			uint16 GetUsageIndex(void) const { return mUsageIndex; }

			/// <summary>
			// Gets the size of this element in bytes
			/// </summary>
			uint32 GetSize(void) const { return GetTypeSize(mFormat); }

			/// <summary>
			// Compare two VertexElement 
			/// </summary>
			inline bool operator == (const VertexElement& rhs) const
			{
				return mFormat == rhs.mFormat &&
					mUsageIndex == rhs.mUsageIndex &&
					mOffset == rhs.mOffset &&
					mUsage == rhs.mUsage;
			}

			inline bool operator != (const VertexElement& rhs) const
			{
				return !(*this == rhs);	
			}


		public:

			/// <summary>
			// Gets the size of this element in bytes
			/// </summary>
			static uint32 GetTypeSize(VertexElementFormat etype);

			/// <summary>
			// Utility method which returns the count of values in a given type
			/// </summary>
			static uint16 GetTypeCount(VertexElementFormat etype);

			static bool IsNormalized(VertexElementFormat etype);

			/** Simple converter function which will turn a single-value type into a
			multi-value type based on a parameter.
			*/
			static VertexElementFormat MultiplyTypeCount(VertexElementFormat baseType, unsigned short count);
			/** Simple converter function which will a type into it's single-value
			equivalent - makes switches on type easier.
			*/
			static VertexElementFormat GetBaseType(VertexElementFormat multiType);


		private:
			/// <summary>
			// The offset in the buffer that this element starts at
			/// </summary>
			uint32 mOffset;

			/// <summary>
			// The format of this vertex element.
			/// </summary>
			VertexElementFormat mFormat;

			/// <summary>
			// The format of this vertex element.
			/// </summary>
			VertexElementUsage mUsage;

			/// <summary>
			// Index of the item, only applicable for some elements like texture coords
			/// </summary>
			uint16 mUsageIndex;
		};


	} // RenderSystem
} // RcEngine


#endif // _VertexElement__H