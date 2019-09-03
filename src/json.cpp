#include "../include/json.h"
#include "../include/namespace.h"

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
#include <algorithm>


namespace jopt{
    int PRECISION = 4;
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

std::string json::String(){
    return _content.str() ;
}
int json::Int(){
    return std::stoi(_content.str()) ;
}
float json::Float(){
    return std::stof(_content.str()) ;
}
double json::Double(){
    return std::stod(_content.str()) ;
}
bool json::Bool(){
    return _content.str() == "true" ? true : false ;
}

std::ostream& operator<<(std::ostream& stream, json& node){
    /*
    output formatter method */
    // print the overture char, be it a '{' (first child) or a ',' (not-first child)
    stream  << std::string(  node._is_first()                       , '{') 
            << std::string(! node._is_first() && bool(node._mother) , ',') 
            << std::endl ;
    // indent
    stream  << std::string(node._depth, '\t') ;
    // print the couple ("key" : value)
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

bool operator>>(std::istream& stream, json& node){
    /*
    json formatter input method */
    char input ;
    
    //first check of validity
    if (jin::getChar(stream, true) != '{')   throw std::logic_error("[0] invalid json format") ;

// goto reference used to "recursively" parse the stream
start_parsing_key:
    if (jin::getChar(stream, true) == stream.eof()){
        return false ;
    } else {
        stream.unget() ;
    }

    std::string         key ;
    std::string         val ;
    std::stringstream   jsn ;

    //get the key and the content and save them inside a string
    if (!jin::getKey(stream, key))              throw std::logic_error("[1] invalid json format") ;
    if (jin::getChar(stream, true) != ':')      throw std::logic_error("[2] invalid json format") ;
    // if the value is a json node
    if (jin::getChar(stream, true) == '{'){
        if (!jin::dumpJsn(stream, jsn))             throw std::logic_error("[3] invalid json format") ;
        // recursivelly dump the json node read from the file into the json object
        jsn >> node[(const char*) key.c_str()] ;
    } else{
        stream.unget() ;
        // if it's not a subnode, it's a value (string, bool, number or vector)
        if (!jin::getCont(stream, val))         throw std::logic_error("[4] invalid json format") ;
        node[key.c_str()]._content.str(val) ;
    }
    // if a comma is found, another key follows
    input = jin::getChar(stream, true) ;
    if (input == ',')           goto start_parsing_key ;
    else if (input != '}')      throw std::logic_error("[5] invalid json format") ;

    return true ;
}
