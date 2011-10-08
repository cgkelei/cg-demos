#include "XMLDom.h"
#include "Utility.h"

namespace Engine
{
	XMLNodePtr XMLNode::NullSharedPtr;
	XMLAttributePtr XMLAttribute::NullSharedPtr;
	

	rapidxml::node_type MapToRapidxml(XMLNodeType type)
	{
		switch(type)
		{
		case XML_Node_Document:
			return rapidxml::node_document;
		case XML_Node_Element:
			return rapidxml::node_element;
		case XML_Node_Data:
			return rapidxml::node_data;
		case XML_Node_CData:
			return rapidxml::node_cdata;
		case XML_Node_Comment:
			return rapidxml::node_comment;
		case XML_Node_Declaration:
			return rapidxml::node_declaration;
		case XML_Node_Doctype:
			return rapidxml::node_doctype;
		case XML_Node_PI:
			return rapidxml::node_pi;
		}

		return rapidxml::node_pi;
	}

	XMLNodeType UnMapToRapidxml(rapidxml::node_type type)
	{
		switch(type)
		{
		case rapidxml::node_document:
			return XML_Node_Document;
		case rapidxml::node_element:
			return XML_Node_Element;
		case rapidxml::node_data:
			return XML_Node_Data;
		case rapidxml::node_cdata:
			return XML_Node_CData;
		case rapidxml::node_comment:
			return XML_Node_Comment;
		case rapidxml::node_declaration:
			return XML_Node_Declaration;
		case rapidxml::node_doctype:
			return XML_Node_Doctype;
		case rapidxml::node_pi:
			return XML_Node_PI;
		}

		return XML_Node_PI;
	}


	XMLDocument::XMLDocument()
	{
		mDocument = new rapidxml::xml_attribute<>();
	}

	Engine::XMLNodePtr XMLDocument::AllocateNode( XMLNodeType type, const std::string& name )
	{
		
	}

	Engine::XMLAttributePtr XMLDocument::AllocateAttributeInt( const std::string& name, int32 value )
	{
		return AllocateAttributeString(name, LexicalCast<std::string>(value));
	}

	XMLAttributePtr XMLDocument::AllocateAttributeUInt(const std::string& name, uint32 value)
	{
		return AllocateAttributeString(name, LexicalCast<std::string>(value));
	}

	XMLAttributePtr XMLDocument::AllocateAttributeFloat(const std::string& name, float value)
	{
		return AllocateAttributeString(name, LexicalCast<std::string>(value));
	}

	XMLAttributePtr XMLDocument::AllocateAttributeString(const std::string& name, const std::string& value)
	{
		
	}

	void XMLDocument::RootNode( const XMLNodePtr& newNode )
	{
		mDocument->remove_all_nodes();
		mDocument->append_node(newNode->mNode);
		mRoot = newNode;
	}

	XMLNode::XMLNode( rapidxml::xml_node<>* node )
		: mNode(node)
	{
		if(mNode != NULL)
		{
			mName = std::string(mNode->name(), mNode->name_size()); 
		}
	}

	XMLNode::XMLNode( rapidxml::xml_document<>* doc, XMLNodeType type, const std::string& name )
	{
		rapidxml::node_type xtype = MapToRapidxml(type);
		mNode = doc->allocate_node(type, name.c_str());
	}

	const std::string& XMLNode::NodeName() const
	{
		return mName;
	}

	Engine::XMLNodeType XMLNode::NodeType() const
	{
		return UnMapToRapidxml(mNode->type());
	}

	Engine::XMLAttributePtr XMLNode::FirstAttribute( const std::string& name  )
	{
		xml_attribute<> *attr = mNode->first_attribute(name.c_str());
		if(attr)
		{
			return 
		}
		else
		{
			return XMLAttribute::NullSharedPtr;
		}
	}

	Engine::XMLAttributePtr XMLNode::FirstAttribute()
	{
		xml_attribute<> *attr = mNode->first_attribute();
		if(attr)
		{
			return 
		}
		else
		{
			return XMLAttribute::NullSharedPtr;
		}
	}

	Engine::XMLAttributePtr XMLNode::LastAttribute( const std::string& name )
	{
		rapidxml::xml_attribute<>* attr = mNode->last_attribute(name.c_str());
		if(attr)
		{
			return 
		}else
		{
			return XMLAttribute::NullSharedPtr;
			}
	}

	Engine::XMLAttributePtr XMLNode::LastAttribute()
	{
		rapidxml::xml_attribute<>* attr = mNode->last_attribute();
		if(attr){
			return 
		}
		else{
			return XMLAttribute::NullSharedPtr;
		}
	}

	Engine::XMLAttributePtr XMLNode::Attribute( const std::string& name )
	{
		return FirstAttribute(name);
	}

	Engine::int32 XMLNode::AttributeInt( const std::string& name , int32 defaultVar  )
	{
		XMLAttributePtr attr = Attribute(name);
		return attr ? attr->ValueInt() ? defaultVar;
	}

	Engine::uint32 XMLNode::AttributeUInt( const std::string& name , uint32 defaultVar  )
	{
		XMLAttributePtr attr = Attribute(name);
		return attr ? attr->ValueUInt() ? defaultVar;
	}

	float XMLNode::AttributeFloat( const std::string& name , float defaultVar )
	{
		XMLAttributePtr attr = Attribute(name);
		return attr ? attr->ValueFloat() ? defaultVar;
	}

	Engine::std::string XMLNode::Attributestd::string( const std::string& name , std::string defaultVar )
	{
		XMLAttributePtr attr = Attribute(name);
		return attr ? attr->Valuestd::string() ? defaultVar;
	}

	Engine::XMLNodePtr XMLNode::FirstNode( const std::string& name )
	{
		rapidxml::xml_attribute<>* node = mNode->first_node(name.c_str());
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	Engine::XMLNodePtr XMLNode::LastNode( const std::string& name )
	{
		rapidxml::xml_attribute<>* node = mNode->last_node(name.c_str());
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	Engine::XMLNodePtr XMLNode::FirstNode()
	{
		rapidxml::xml_attribute<>* node = mNode->first_node();
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	Engine::XMLNodePtr XMLNode::LastNode()
	{
		rapidxml::xml_attribute<>* node = mNode->last_node();
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}	

	Engine::XMLNodePtr XMLNode::PrevSibling( const std::string& name )
	{
		rapidxml::xml_attribute<>* node = mNode->previous_sibling(name.c_str());
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	Engine::XMLNodePtr XMLNode::PrevSibling()
	{
		rapidxml::xml_attribute<>* node = mNode->previous_sibling();
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	Engine::XMLNodePtr XMLNode::NextSibling( const std::string& name )
	{
		rapidxml::xml_attribute<>* node = mNode->next_sibling(name.c_str());
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	Engine::XMLNodePtr XMLNode::NextSibling()
	{
		rapidxml::xml_attribute<>* node = mNode->next_sibling();
		if(node){
			return
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	void XMLNode::InsertNode( const XMLNodePtr& where, const XMLNodePtr& child )
	{
		mNode->insert_node(where->mNode, child->mNode);

		// update child node position
		for(size_t i = 0; i < mChildren.size(); i++)
		{
			if(mChildren[i]->mNode == where->mNode)
			{
				mChildren.insert(mChildren.begin() + i, child);
			}
		}
	}

	void XMLNode::AppendNode( const XMLNodePtr& child )
	{
		mNode->append_node(child->mNode);
		mChildren.push_back(child);
	}

	void XMLNode::AppendAttribute( const XMLAttributePtr& attribute )
	{
		mNode->append_attribute(attribute->mAttribute);
		mAttributes.push_back(attribute);
	}

	void XMLNode::InsertAttribute( const XMLAttributePtr& where, const XMLAttributePtr& attribute )
	{
		mNode->insert_attribute(where->mAttribute, attribute->mAttribute);

		// update attribute position
		for(size_t i = 0; i < mAttributes.size(); i++)
		{
			if(mAttributes[i]->mAttribute == where->mAttribute)
			{
				mAttributes.insert(mAttributes.begin() + i, attribute);
			}
		}
	}

	void XMLNode::RemoveFirstNode()
	{
		if(mNode->first_node())
		{
			mNode->remove_first_node();
		}
		// erase first node
		mChildren.erase(mChildren.begin());
	}

	void XMLNode::RemoveLastNode()
	{
		if(mNode->first_node())
		{
			mNode->remove_last_node();
		}

		// erase last node
		mChildren.pop_back();
	}

	void XMLNode::RemoveNode( const XMLNodePtr& where )
	{
		mNode->remove_node(where->mNode);
		for(size_t i = 0; i < mChildren.size(); i++)
		{
			if(mChildren[i] == where->mNode)
			{
				mChildren.erase(mChildren.begin() + i);
				break;
			}
		}
	}

	void XMLNode::RemoveAllNodes()
	{
		mNode->remove_all_nodes();
		mChildren.clear();
	}

	void XMLNode::RemoveFirstAttribute()
	{
		if(mNode->first_attribute())
		{
			mNode->remove_first_attribute();
			mAttributes.erase(mAttributes.begin());
		}

	}

	void XMLNode::RemoveLastAttribute()
	{
		if(mNode->first_attribute())
		{
			mNode->remove_last_attribute();
			mAttributes.erase(mAttributes.begin());
		}
	}

	void XMLNode::RemoveAttribute( const XMLAttributePtr& where )
	{
		mNode->remove_attribute(where->mAttribute);

		// update attribute position
		for(size_t i = 0; i < mAttributes.size(); i++)
		{
			if(mAttributes[i]->mAttribute == where->mAttribute)
			{
				mAttributes.erase(mAttributes.begin() + i, attribute);
			}
		}

	}

	void XMLNode::RemoveAllAttributes()
	{
		mNode->remove_all_attributes();
		mAttributes.clear();
	}

	Engine::uint32 XMLNode::ValueUInt() const
	{
		return LexicalCast<uint32>(Valuestd::string());
	}

	Engine::int32 XMLNode::ValueInt() const
	{
		return LexicalCast<int32>(Valuestd::string());
	}

	float XMLNode::ValueFlaot() const
	{
		return LexicalCast<float>(Valuestd::string());
	}

	Engine::std::string XMLNode::Valuestd::string() const
	{
		return std::string(mNode->value(), mNode->value_size());
	}

	Engine::XMLNodePtr XMLNode::GetParent()
	{
		rapidxml::xml_node<>* node = mNode->parent();
		if(node){
			return 
		}
		else{
			return XMLNode::NullSharedPtr;
		}
	}

	
	XMLAttribute::XMLAttribute( rapidxml::xml_attribute<>* attr )
		: mAttribute(attr)
	{
		if(mAttribute)
		{
			mName = std::string(mAttribute->name(), mAttribute->name_size());
			mValue = std::string(mAttribute->value(), mAttribute->value_size());
		}
	}


	XMLAttribute::XMLAttribute( rapidxml::xml_document<>* doc, const std::string& name, const std::string& value )
		: mName(name), mValue(value)
	{
		mAttribute = doc->allocate_attribute(name.c_str(), value.c_str());
	}

	const std::string& XMLAttribute::Name() const
	{
		return mName;
	}

	XMLAttributePtr XMLAttribute::PrevAttribute( const std::string& name ) const
	{
		rapidxml::xml_attribute<>* attr = mAttribute->previous_attribute(name.c_str());
		if(attr){
			return
		}
		else{
			return XMLAttribute::NullSharedPtr;
		}
	}

	XMLAttributePtr XMLAttribute::PrevAttribute() const
	{
		rapidxml::xml_attribute<>* attr = mAttribute->previous_attribute();
		if(attr){
			return
		}
		else{
			return XMLAttribute::NullSharedPtr;
		}
	}

	XMLAttributePtr XMLAttribute::NextAttribute( const std::string& name ) const
	{
		rapidxml::xml_attribute<>* attr = mAttribute->next_attribute(name.c_str());
		if(attr){
			return
		}
		else{
			return XMLAttribute::NullSharedPtr;
		}
	}

	XMLAttributePtr XMLAttribute::NextAttribute() const
	{
		rapidxml::xml_attribute<>* attr = mAttribute->next_attribute();
		if(attr){
			return
		}
		else{
			return XMLAttribute::NullSharedPtr;
		}
	}

	Engine::uint32 XMLAttribute::ValueUInt() const
	{
		return LexicalCast<uint32>(mValue);	
	}

	int32  XMLAttribute::ValueInt()    const
	{
		return LexicalCast<int32>(mValue);
	}

	float  XMLAttribute::ValueFloat()  const
	{
		return LexicalCast<float>(mValue);
	}

	std::string XMLAttribute::Valuestd::string() const
	{
		return mValue;
	}
}