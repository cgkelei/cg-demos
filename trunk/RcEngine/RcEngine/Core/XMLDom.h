#ifndef XMLDom_h__
#define XMLDom_h__

#include <Core/Prerequisites.h>

namespace RcEngine{
namespace Core{

class XMLNode;
typedef std::shared_ptr<XMLNode> XMLNodePtr;

class XMLAttribute;
typedef std::shared_ptr<XMLAttribute> XMLAttributePtr;

enum XMLNodeType
{
	// A document node. Name and value are empty.
	XML_Node_Document,
		
	// An element node. Name contains element name. Value contains text of first data node.
	XML_Node_Element,
		
	// A data node. Name is empty. Value contains data text.
	XML_Node_Data,
		
	// A CDATA node. Name is empty. Value contains data text.
	XML_Node_CData,
		
	// A comment node. Name is empty. Value contains comment text.
	XML_Node_Comment,
		
	//A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
	XML_Node_Declaration,
		
	// A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
	XML_Node_Doctype,
		
	// A PI node. Name contains target. Value contains instructions.
	XML_Node_PI,	
};
	
/************************************************************************/
/* Class XMLDocument represents a root of the DOM hierarchy.                                                                     */
/************************************************************************/

class _ApiExport XMLDocument
{
public:
	XMLDocument();

	XMLNodePtr Parse(const std::string& xmlName);
	void Print(std::ostream& os);

	XMLNodePtr AllocateNode(XMLNodeType type, const std::string& name);
	XMLAttributePtr AllocateAttributeInt(const std::string& name, int32 value);
	XMLAttributePtr AllocateAttributeUInt(const std::string& name, uint32 value);
	XMLAttributePtr AllocateAttributeFloat(const std::string& name, float value);
	XMLAttributePtr AllocateAttributeString(const std::string& name, const std::string& value);

	void RootNode( const XMLNodePtr& newNode );

private:
	std::shared_ptr<rapidxml::xml_document<> > mDocument;
	XMLNodePtr mRoot;
	std::vector<char> mXMLSrc; // must read xml file in memory
};

class _ApiExport XMLNode
{
	friend class XMLDocument;

public:
	explicit XMLNode( rapidxml::xml_node<>* node);
	XMLNode( rapidxml::xml_document<>* doc, XMLNodeType type, const std::string& name );

	const std::string& NodeName() const;
	XMLNodeType   NodeType() const;

	XMLNodePtr GetParent();
		
	XMLAttributePtr FirstAttribute( const std::string& name );
	XMLAttributePtr LastAttribute( const std::string& name );
	XMLAttributePtr FirstAttribute( );
	XMLAttributePtr LastAttribute( );
		
	XMLAttributePtr Attribute( const std::string& name );
	int32  AttributeInt( const std::string& name , int32 defaultVar);
	uint32 AttributeUInt( const std::string& name , uint32 defaultVar);
	float  AttributeFloat( const std::string& name , float defaultVar);
	std::string AttributeString( const std::string& name , std::string defaultVar);

	XMLNodePtr FirstNode( const std::string& name );
	XMLNodePtr LastNode( const std::string& name );
	XMLNodePtr FirstNode();
	XMLNodePtr LastNode();
		
	XMLNodePtr PrevSibling( const std::string& name );
	XMLNodePtr NextSibling( const std::string& name );
	XMLNodePtr PrevSibling();
	XMLNodePtr NextSibling();

	void InsertNode( const XMLNodePtr& where, const XMLNodePtr& child);
	void AppendNode(const XMLNodePtr& child);

	void AppendAttribute( const XMLAttributePtr& attribute );
	void InsertAttribute( const XMLAttributePtr& where, const XMLAttributePtr& attribute);

	void RemoveFirstNode();
	void RemoveLastNode();
	void RemoveNode( const XMLNodePtr& where );
	void RemoveAllNodes();

	void RemoveFirstAttribute();
	void RemoveLastAttribute();
	void RemoveAttribute( const XMLAttributePtr& where );
	void RemoveAllAttributes();

	uint32 ValueUInt()   const;
	int32  ValueInt()    const;
	float  ValueFlaot()  const;
	std::string ValueString() const;

private:
	rapidxml::xml_node<>* mNode;
	std::string mName;
		
	std::vector<XMLNodePtr> mChildren;
	std::vector<XMLAttributePtr> mAttributes;

public:
	static XMLNodePtr NullObejct;
};

class _ApiExport XMLAttribute
{
	friend class XMLDocument;
	friend class XMLNode;

public:
	explicit XMLAttribute( rapidxml::xml_attribute<>* attr );
	XMLAttribute( rapidxml::xml_document<>* doc, const std::string& name, const std::string& value );

	const std::string& Name() const;
			
	XMLAttributePtr PrevAttribute( const std::string& name ) const;
	XMLAttributePtr NextAttribute( const std::string& name ) const;
	XMLAttributePtr PrevAttribute( ) const;
	XMLAttributePtr NextAttribute( ) const;

	uint32 ValueUInt()   const;
	int32  ValueInt()    const;
	float  ValueFloat()  const;
	std::string ValueString() const;

private:
	rapidxml::xml_attribute<>* mAttribute;
	std::string mName;
	std::string mValue;
	
public:
	static XMLAttributePtr NullObejct;

};

} // Namespace Core
} // Namespace RcEngine

#endif // XMLDom_h__