#include "../include/json.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <csignal>
#include <typeinfo>

namespace json_options{
    int  INDENTATION = 4     ;
}

json::json(){
    _depth  = 0    ;
    _last   = true ;
}
json::json(const char* main_label){
    _label = main_label ;
    _depth = 0 ;
    _last  = true ; 
}
json::~json(){
}

json& json::operator[] (const char* label){
    for (std::vector<json*>::iterator it = _subnodes.begin(); it != _subnodes.end(); ++it){
        if ((*it)->_label == std::string(label)){
            return *(*it);
        }
    }
    _add_child(new json(label)) ;
    return *(_subnodes.back()) ;
}

void json::operator=(const char*  content){ 
    _content = std::string   (content) ; 
    _type = "str" ;
}
void json::operator=(const float  content){ 
    _content = std::to_string(content) ; 
    _type = "flt" ;
}
void json::operator=(const double  content){ 
    _content = std::to_string(content) ; 
    _type = "flt" ;
}
void json::operator=(const int    content){ 
    _content = std::to_string(content) ; 
    _type = "int" ;
}
void json::operator=(const bool   content){ 
    _content = std::to_string(content) ; 
    _type = "boo" ;
}

void json::_add_child(json* child){
    _type = "jsn" ;

    if (_subnodes.size()){
        _subnodes.back()->_last = false ;
    }

    _subnodes.push_back(child)  ;
    _keys.push_back(std::string(child->_label)) ;
    
    child->_depth = _depth + 1 ;
}

std::list<std::string>& json::keys()   { return _keys ; }

std::ostream& operator<<(std::ostream& stream, json& node){
    if (node._depth != 0){
        if      (node._type == "jsn"){  stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                                << "\"" << node._label      << "\": " 
                                                << "{"  << std::endl;
        }
        else if (node._type == "str"){  stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                                << "\"" << node._label      << "\": " 
                                                << "\"" << node._content    << "\"" ;
        }
        else if (node._type == "flt"){  stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                                << "\"" << node._label      << "\": " 
                                                << std::stof(node._content);
        }
        else if (node._type == "int"){  stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                                << "\"" << node._label      << "\": " 
                                                << std::stoi(node._content);     
        }
        else if (node._type == "boo"){  stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                                << "\"" << node._label      << "\": ";
                                                if (std::stoi(node._content))   stream << "true" ;
                                                else                            stream << "false";
        }
        else if (node._type == "vec"){  stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                                << "\"" << node._label      << "\": "
                                                << "TBA";
        }
    } else{
        stream << "{" << std::endl;
    }
    
    for (std::vector<json*>::iterator it = node._subnodes.begin(); it != node._subnodes.end(); ++it){
        stream << *(*it) ;
    }        
    if (node._type == "jsn")    stream  << std::string(node._depth * json_options::INDENTATION, ' ')
                                        << "}" ;
    if (! node._last)           stream  << "," ;

    return stream << std::endl;
}
