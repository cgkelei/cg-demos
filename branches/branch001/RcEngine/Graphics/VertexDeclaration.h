#pragma once
#ifndef _VertexDeclaration__H
#define _VertexDeclaration__H

//  [8/16/2011 hustruan]
#include "Core/Prerequisites.h"
#include "Graphics/VertexElement.h"

namespace RcEngine {
	namespace Render {

#define Max_Vertex_Streams 8

		typedef std::vector<VertexElement> VertexElementList;

		struct _ApiExport VertexDeclarationDesc
		{
			unsigned int               Offset;
			VertexElementFormat        Format;
			VertexElementUsage         Usage;
			unsigned int               UsageIndex;
			
			
		};

		class  _ApiExport VertexDeclaration
		{
			static bool VertexElementLess(const VertexElement& e1, const VertexElement& e2);

		public:
			VertexDeclaration();
			virtual ~VertexDeclaration();

			/// <summary>
			// Get the number of elements in the declaration
			/// </summary>
			uint32 GetElementCount(void) { return static_cast<uint32>(mElementList.size()); }

			/// <summary>
			// Gets read-only access to the list of vertex elements.
			/// </summary>
			const VertexElementList& GetElements(void) const;

			/// <summary>
			// Get a single element.
			/// </summary>
			const VertexElement& GetElement(uint32 index)	{ return mElementList.at(index); }


			/// <summary>
			// Gets the vertex size defined by this declaration for a given source. 
			/// </summary>
			virtual uint32 GetVertexSize() const;

			/// <summary>
			// Finds a VertexElement with the given semantic, and index if there is more than
			// one element with the same semantic.
			/// <remark>
			// If the element is not found, this method returns null.
			/// </remark>
			/// </summary>
			virtual const VertexElement* FindElementBySemantic(VertexElementUsage sem, uint16 index = 0);

			/// <summary>
			// Adds a new VertexElement to this declaration.
			/// <remark>
			// This method adds a single element (positions, normals etc) to the end of the
			// vertex declaration.
			/// </remark>
			/// </summary>
			virtual const VertexElement& AddElement( uint32 offset, VertexElementFormat theType,
				VertexElementUsage semantic, uint16 index = 0);

			/// <summary>
			// Inserts a new VertexElement at a given position in this declaration.
			/// <remark>
			//  This method adds a single element (positions, normals etc) at a given position in this
			//  vertex declaration.
			/// </remark>
			/// </summary>
			virtual const VertexElement& InsertElement(uint32 atPosition,  uint32 offset, VertexElementFormat theType,
				VertexElementUsage semantic, uint16 index = 0);

			/// <summary>
			// Remove the element at the given index from this declaration. 
			/// </summary>
			virtual void RemoveElement(uint32 elemIndex);

			/// <summary>
			// Remove the element with the given semantic and usage index.
			/// <remark>
			//  In this case 'index' means the usage index for repeating elements such
			//  as texture coordinates. For other elements this will always be 0 and does
			//  not refer to the index in the vector.
			/// </remark>
			/// </summary>
			virtual void RemoveElement(VertexElementUsage semantic, uint16 index = 0);

			/// <summary>
			// Remove all elements. 
			/// </summary>
			virtual void RemoveAllElements(void);

			/// <summary>
			/// Sorts the elements in this list to be compatible with the maximum
			/// number of rendering APIs / graphics cards.
			/// </summary>
			void Sort(void);

			/// <summary>
			// Compare two VertexDeclaration
			/// </summary>
			inline bool operator == (const VertexDeclaration& rhs) const
			{
				if (mElementList.size() != rhs.mElementList.size())
					return false;

				VertexElementList::const_iterator i, iend, rhsi, rhsiend;
				iend = mElementList.end();
				rhsiend = rhs.mElementList.end();
				rhsi = rhs.mElementList.begin();
				for (i = mElementList.begin(); i != iend && rhsi != rhsiend; ++i, ++rhsi)
				{
					if ( *i != *rhsi )
						return false;
				}

				return true;
			}

			inline bool operator != (const VertexDeclaration& rhs) const
			{
				return !(*this == rhs);
			}


		protected:

			/// <summary>
			// Defines the list of vertex elements that makes up this declaration
			/// </summary>
			VertexElementList mElementList;

		};


	} // RenderSystem
} // RcEngine


#endif // _VertexDeclaration__H
