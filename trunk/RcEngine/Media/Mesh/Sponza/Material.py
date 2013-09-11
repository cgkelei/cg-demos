from xml.dom.minidom import parse, parseString

materials = parse("Sponza.materials.xml")
template = parse("leaf1.material.xml")

for material in materials.getElementsByTagName('material'):
    name = material.attributes['material'].value

    texture = material.getElementsByTagName('DiffuseColor')
    if len(texture) == 0:
        continue
    
    texture = texture[0].attributes['name'].value
    texture = texture.split('\\')[-1]
    print texture
    
    outMat = template.getElementsByTagName('Material')[0]
    outMat.attributes['name'].value = name

    for node in template.getElementsByTagName('Parameter'):
        if node.attributes['type'].value == 'texture2d' and node.attributes['semantic'].value == 'DiffuseMaterialMap':
               node.attributes['value'].value = texture
              

    outName = name + ".material.xml"
    outXML = open(outName, "w")
    outXML.write(template.toxml())
    outXML.close()

    
            

    
