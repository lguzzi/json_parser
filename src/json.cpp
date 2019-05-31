#include "../include/json.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <csignal>
#include <typeinfo>
#include <sstream>
#include <array>
namespace jopt{
    int PREC = 4;
}
json::json(){
    _depth  = 0    ;
    _last   = true ;
    _mother = nullptr ;
}
json::json(const char* main_label){
    _label = main_label ;
    _depth = 0 ;
    _last  = true ; 
    _mother = nullptr ;
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

void json::_add_child(json* child){
    _content.str("") ;
    if (_subnodes.size()){
        _subnodes.back()->_last = false ;
    }
    
    _subnodes.push_back(child)  ;
    _keys.push_back(std::string(child->_label)) ;
    
    child->_depth  = _depth + 1 ;
    child->_mother = this ;
}

std::list<std::string>& json::keys()   { return _keys ; }

bool json::_is_first(){
    if (_mother){
        return _mother->_subnodes.front() == this ;
    } else{
        return false ;
    }
}

bool json::_is_last(){
    if (_mother){
        return _mother->_subnodes.back() == this ;
    } else{
        return true ;
    }
}

std::string labely(std::string& label){
    if (label.length()){
        return '"' + label + '"' + ':' ;
    } else{
        return "" ;
    }
}

std::ostream& operator<<(std::ostream& stream, json& node){
    /*
    json formatter method */
    // print the overture char, be it a '{' (first child) or a ',' (not-first child)
    stream  << std::string(  node._is_first()                       , '{') 
            << std::string(! node._is_first() && bool(node._mother) , ',') 
            << std::endl ;
    // indent
    stream  << std::string(node._depth, '\t') ;
    // print the couple (key : value)
    stream  << labely(node._label)
            << node._content.str() ;

    // work on subnodes recursively
    for (std::vector<json*>::iterator it = node._subnodes.begin(); it != node._subnodes.end(); ++it){
        stream << *(*it) ;
    } 
    
    // print the closure char '}' if closing a node with children (indent correctly) 
    stream  << std::string(bool(node._subnodes.size())              , '\n')
            << std::string(node._depth * bool(node._subnodes.size()), '\t') 
            << std::string(bool(node._subnodes.size())              , '}');

    return stream ;
}