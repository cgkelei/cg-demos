import xml.dom.minidom
import sys
import os

def CreateSamplerNode(dom, name, sampler):
    samplerNode = dom.createElement("Sampler")
    samplerNode.setAttribute("name", name)

    for k, v in sampler.iteritems():    
        stateNode = dom.createElement("State")
        stateNode.setAttribute("name", k)
        stateNode.setAttribute("value", v)
        samplerNode.appendChild(stateNode)

    return samplerNode

def CreateNode(dom, name, desc):
    node = dom.createElement(name)
    for k, v in desc.iteritems():
        node.setAttribute(k, v)
    return node

def ProcessTexture(texPath):
    """ Convert texture image to dds format"""
    filePath = os.path.basename(texPath)
    fileName, fileExtension = os.path.splitext(filePath)
    return fileName + ".dds"

def OutputMaterial(subMeshMat):
    impl = xml.dom.minidom.getDOMImplementation()
    dom = impl.createDocument(None,'Material', None)
    root  =  dom.documentElement
    
    #BNF sampler
    samplerNodeDesc = {"Filter":"Min_Mag_Mip_Point", "AddressU":"Clamp", "AddressV":"Clamp"}
    root.appendChild( CreateSamplerNode(dom, "BNFSampler", samplerNodeDesc) )
 
    hasAlphaMap = subMeshMat.has_key("TransparentMap")  #AlphaMap, if has alpha map, alpha packed in diffuse map w channel 
    hasDiffuseMap = subMeshMat.has_key("DiffuseMap")
    hasSpecularMap = subMeshMat.has_key("SpecularMap")
    hasGlossyMap = subMeshMat.has_key("GlossyMap")      #Shininess, if has glossy map, glossy packed in specular map w channel
    hasNormalMap = subMeshMat.has_key("NormalMap")

    if hasAlphaMap or hasDiffuseMap or hasSpecularMap or hasGlossyMap or hasNormalMap:
        #Linear sampler
        samplerNodeDesc = {"Filter":"Min_Mag_Mip_Linear", "AddressU":"Wrap", "AddressV":"Wrap"}
        root.appendChild( CreateSamplerNode(dom, "LinearSampler", samplerNodeDesc) )
    
    #Trasform
    root.appendChild( CreateNode(dom, "Parameter", {"name":"WorldView", "semantic":"WorldViewMatrix", "type":"float4x4"}) )
    root.appendChild( CreateNode(dom, "Parameter", {"name":"Proj", "semantic":"ProjectionMatrix", "type":"float4x4"}) )
    
    #Diffuse 
    if hasDiffuseMap:
        diffuseMapDesc = { "name":"DiffuseMap",
                           "sampler":"LinearSampler",
                           "semantic":"DiffuseMaterialMap",
                           "stage":"PixelShader",
                           "texUnit":"0",
                           "type":"texture2d",
                           "value":subMeshMat["DiffuseMap"] }
        root.appendChild( CreateNode(dom, "Parameter", diffuseMapDesc) )
    else:
        diffuseColorDesc = { "name":"DiffuseColor",
                             "semantic":"DiffuseMaterialColor",
                             "type":"float3",
                             "value":subMeshMat["DiffuseColor"] }  
        root.appendChild( CreateNode(dom, "Parameter", diffuseColorDesc) )

    #Specular
    if hasSpecularMap:
        specularMapDesc = { "name":"SpecularMap",
                            "sampler":"LinearSampler",
                            "semantic":"SpecularMaterialMap",
                            "stage":"PixelShader",
                            "texUnit":"1",
                            "type":"texture2d",
                            "value":subMeshMat["SpecularMap"] }
        root.appendChild( CreateNode(dom, "Parameter", specularMapDesc) )
    else:
        specularColorDesc = { "name":"SpecularColor",
                              "semantic":"SpecularMaterialColor",
                              "type":"float3",
                              "value":subMeshMat["SpecularColor"] }
        root.appendChild( CreateNode(dom, "Parameter", specularColorDesc) )
   
    #Shininess   
    if hasGlossyMap == False:
        powerDesc = { "name":"Shininess",
                      "semantic":"SpecularMaterialPower",
                      "type":"float",
                      "value":subMeshMat["Power"] }
        root.appendChild( CreateNode(dom, "Parameter", powerDesc) )    

    #Normal map
    if hasNormalMap:
        normalMapDesc = { "name":"NormalMap",
                          "sampler":"LinearSampler",
                          "semantic":"NormalMaterialMap",
                          "stage":"PixelShader",
                          "texUnit":"2",
                          "type":"texture2d",
                          "value":subMeshMat["NormalMap"] }
        root.appendChild( CreateNode(dom, "Parameter", normalMapDesc) )
    
    # BNF texture
    BNFSamplerDesc = { "name":"BestNormalFitMap",
                       "sampler":"BNFSampler",
                       "stage":"PixelShader",
                       "texUnit":"3",
                       "type":"texture2d",
                       "value":"BestFitNormal.dds" }
    root.appendChild( CreateNode(dom, "Parameter", BNFSamplerDesc) )

    effectName = None
    if hasAlphaMap:
        effectName = "AlphaTest.effect.xml"
    else:
        effectName = "Model.effect.xml"
        
    effectNode = CreateNode(dom, "Effect", {"name":effectName})
    if hasAlphaMap:
        effectNode.appendChild( CreateNode(dom, "Flag", {"name":"_AlphaTest"}) )
    if hasDiffuseMap:
        effectNode.appendChild( CreateNode(dom, "Flag", {"name":"_DiffuseMap"}) )
    if hasSpecularMap:
        effectNode.appendChild( CreateNode(dom, "Flag", {"name":"_SpecularMap"}) )
    if hasGlossyMap:
        effectNode.appendChild( CreateNode(dom, "Flag", {"name":"_GlossyMap"}) )
    if hasNormalMap:
        effectNode.appendChild( CreateNode(dom, "Flag", {"name":"_NormalMap"}) )
    root.appendChild(effectNode)
    
    queueNode = CreateNode(dom, "Queue", {"name":"Opaque"})
    root.appendChild(queueNode)

    xmlName = subMeshMat["name"] + ".material.xml"
    f =  open(xmlName,'w')
    dom.writexml(f,  addindent = '' , newl='\n', encoding='utf-8')
    f.close()

def GenerateMaterial(meshMaterialFile):  

    textureList = {"Diffuse":[], "Specular":[], "Normal":[]}
    
    MeshMatNode = xml.dom.minidom.parse(meshMaterialFile)
    for subMeshMatNode in MeshMatNode.getElementsByTagName('material'):
        name = subMeshMatNode.attributes['material'].value

        subMeshMat = {"name":name} 
        layer = subMeshMatNode.getElementsByTagName('DiffuseColor')
        if len(layer):
            file = layer[0].attributes['name'].value
            textureList["Diffuse"].append(file)
            
            file = ProcessTexture(file)
            subMeshMat["DiffuseMap"] = file
        else:
            specular = subMeshMatNode.getElementsByTagName('diffuse')
            r = specular[0].attributes['r'].value
            g = specular[0].attributes['g'].value
            b = specular[0].attributes['b'].value
            subMeshMat["DiffuseColor"] = r + ' ' + g + ' ' + b

        
        layer = subMeshMatNode.getElementsByTagName('SpecularColor')
        if len(layer):
            file = layer[0].attributes['name'].value
            textureList["Specular"].append(file)
            
            file = ProcessTexture(file)
            subMeshMat["SpecularMap"] = file
        else:
            specular = subMeshMatNode.getElementsByTagName('specular')
            r = specular[0].attributes['r'].value
            g = specular[0].attributes['g'].value
            b = specular[0].attributes['b'].value
            subMeshMat["SpecularColor"] = r + ' ' + g + ' ' + b

        layer = subMeshMatNode.getElementsByTagName('ShininessExponent')
        if len(layer):
            if subMeshMat.has_key("SpecularMap"):
                subMeshMat["GlossyMap"] = subMeshMat["SpecularMap"]
            else:
                del subMeshMat["SpecularColor"]

                file = layer[0].attributes['name'].value
                textureList["Specular"].append(file)
            
                file = ProcessTexture(file) 
                subMeshMat["SpecularMap"] = file
                subMeshMat["GlossyMap"] = subMeshMat["SpecularMap"]
        else:
            specular = subMeshMatNode.getElementsByTagName('specular')
            power = specular[0].attributes['power'].value
            subMeshMat["Power"] = power
        
        layer = subMeshMatNode.getElementsByTagName('TransparentColor')
        if len(layer):
            subMeshMat["TransparentMap"] = subMeshMat["DiffuseMap"] #combine alpha channel in diffuse map

        layer = subMeshMatNode.getElementsByTagName('Bump')
        if len(layer):
            file = layer[0].attributes['name'].value
            textureList["Normal"].append(file)
            
            file = ProcessTexture(file)
            subMeshMat["NormalMap"] = file
        
        print("Output material: %s" % subMeshMat["name"] + ".material.xml")
        for k, v in subMeshMat.iteritems():
            if k != "name":
                print(k, v)
        print("\n")
        
        OutputMaterial(subMeshMat)

        #output texture file name list  
        for k, v in textureList.iteritems():
            f = open(k+".lst", 'w')
            for tex in v:
                f.write("%s\n"%tex)
            f.close()
    
if __name__== "__main__":
    GenerateMaterial("dude.materials.xml")
