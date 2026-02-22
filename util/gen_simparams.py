#!/usr/bin/python
import os
import subprocess
import datetime
import xml.etree.ElementTree as ET

def parseParam(groupNode, paramNode):
    decl, default_stmt, getattr_stmt = '', '', ''

    dataType = 'double'
    default_stmt = paramNode.attrib.get('value')

    member_name = '_'.join([groupNode.tag, paramNode.tag])

    lval_template = 'result.{m} = '
    rval_template = 'getAttribute{d}(&paramsXml, {{"{g}", "{p}"}}, "{a}")'
    getattr_stmt += lval_template.format(m=member_name)

    if paramNode.attrib.get('path'):
        dataType = 'std::string'
        attr_dataType = 'string'
        default_stmt = f'"{paramNode.attrib.get('path')}"'
        getattr_stmt += rval_template.format(d=attr_dataType.capitalize(), g=groupNode.tag, p=paramNode.tag, a='path')
    elif all(a in paramNode.keys() for a in ('min', 'max', 'increment', 'default')):
        dataType = 'std::vector<double>'
        attr_dataType = 'double'
        default_stmt = f'{{{paramNode.attrib.get('min')}, {param.attrib.get('max')}, {param.attrib.get('increment')}, {param.attrib.get('default')}}}'
        attrList = ('min', 'max', 'increment', 'default')
        getattr_stmt += '{' + ','.join([rval_template.format(d=attr_dataType.capitalize(), g=groupNode.tag, p=paramNode.tag, a=attr) for attr in attrList]) + '}'
    elif all(a in paramNode.keys() for a in ('x', 'y', 'z')):
        dataType = 'Vec3'
        attr_dataType = 'double'
        default_stmt = f'Vec3({paramNode.attrib.get('x')}, {param.attrib.get('y')}, {param.attrib.get('z')})'
        attrList = ('x', 'y', 'z')
        getattr_stmt += 'Vec3(' + ','.join([rval_template.format(d=attr_dataType.capitalize(), g=groupNode.tag, p=paramNode.tag, a=attr) for attr in attrList]) + ')'
    elif all(a in paramNode.keys() for a in ('width', 'height')):
        dataType = 'Vec3'
        attr_dataType = 'double'
        attrList = ('width', 'height')
        if paramNode.attrib.get('depth'):
            attrList += ('depth',)
        default_stmt = f'Vec3({','.join([param.attrib.get(x) for x in attrList])})'
        getattr_stmt += 'Vec3(' + ','.join([rval_template.format(d=attr_dataType.capitalize(), g=groupNode.tag, p=paramNode.tag, a=attr) for attr in attrList]) + ')'
    elif paramNode.attrib.get('type'):
        dataType = paramNode.attrib.get('type')
        getattr_stmt += rval_template.format(d=dataType.capitalize(), g=groupNode.tag, p=paramNode.tag, a='value')
    else:
        getattr_stmt += rval_template.format(d=dataType.capitalize(), g=groupNode.tag, p=paramNode.tag, a='value')

    decl = ' '.join([dataType, member_name])
    return decl, default_stmt, getattr_stmt

h_template = """#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H

#include "vec3d.h"
#include <string>

#include <vector>

struct SimParameters {{
{s}
}};

const SimParameters defaultParams = {{
{d}
}};

SimParameters parseXmlConfig(std::string fileName);

#endif
"""

c_template = """#include "simParams.h"

#include <tinyxml2.h>

#include <iostream>

std::string getAttributeString(tinyxml2::XMLDocument *doc,
                          std::vector<std::string> elementChain,
                          std::string attribute) {{
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {{
    el = el->FirstChildElement(elName.c_str());
  }}
  return el->Attribute(attribute.c_str());
}}

double getAttributeDouble(tinyxml2::XMLDocument *doc,
                          std::vector<std::string> elementChain,
                          std::string attribute) {{
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {{
    el = el->FirstChildElement(elName.c_str());
  }}
  return el->DoubleAttribute(attribute.c_str());
}}

bool getAttributeBool(tinyxml2::XMLDocument *doc,
                      std::vector<std::string> elementChain,
                      std::string attribute) {{
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {{
    el = el->FirstChildElement(elName.c_str());
  }}
  return el->BoolAttribute(attribute.c_str());
}}

int getAttributeInt(tinyxml2::XMLDocument *doc,
                    std::vector<std::string> elementChain,
                    std::string attribute) {{
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {{
    el = el->FirstChildElement(elName.c_str());
  }}
  return el->IntAttribute(attribute.c_str());
}}

SimParameters parseXmlConfig(std::string fileName) {{
  SimParameters result = defaultParams;

  tinyxml2::XMLDocument paramsXml;
  if (paramsXml.LoadFile(fileName.c_str())) {{
    std::cerr << "warning: could not open \\"" << fileName
              << "\\"; using default parameters\\n";
    return result;
  }};
{a}
  return result;
}}
"""

struct_list = ''
default_list = ''
getattr_list = ''

config = ET.parse('config/default.xml')

for group in config.getroot():
    for param in group:
        decl, default_stmt, getattr_stmt = parseParam(group, param)
        # print(getattr_stmt)
        struct_list += decl + ';'
        default_list += default_stmt + ','
        getattr_list += getattr_stmt + ';'

for file in ['simParams.h', 'simParams.cpp']:
    if os.path.exists(f'./src/{file}'):
        os.rename(f'./src/{file}', f'./src/{file}.{datetime.datetime.now(datetime.UTC).strftime('%H%M%S')}')

with open('./src/simParams.h', 'w') as file:
    file.write(h_template.format(s=struct_list, d=default_list))
subprocess.run(['clang-format', '-i', './src/simParams.h'])

with open('./src/simParams.cpp', 'w') as file:
    file.write(c_template.format(a=getattr_list))
subprocess.run(['clang-format', '-i', './src/simParams.cpp'])
