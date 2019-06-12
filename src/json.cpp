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

std::ostream& operator<<(std::ostream& stream, json& node){
    /*
    json formatter output method */
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

bool operator>>(std::istream& stream, json& node){
   // json formatter input method 
    bool open_quote = 0 ;
    int open_curly = 0 ;
    char input ;
    if (jin::getChar(stream, input, true) != '{'){
        std::cout << "ERROR: input file is not a json file" << std::endl ;
        std::cout << "\tfile " << __FILE__ << " @ line " << __LINE__ << std::endl;
        std::cout << "\tlast char is " << input << std::endl;
        return false ;
    }

start_parsing_key:
    if (jin::getChar(stream, input, true) == stream.eof()){
        return true ;
    } else {
        stream.unget() ;
    }

    std::string         key ;
    std::string         val ;
    std::stringstream   jsn ;

    if (! _getKey(stream, key))                     throw std::logic_error("[0] invalid json format") ;
    if (jin::getChar(stream, input, true) != ':')   throw std::logic_error("[1] invalid json format") ;
    if (jin::getChar(stream, input, true) == '{'){
        open_curly += 1 ;
        if (! _dumpJsn(stream, jsn))                throw std::logic_error("[2] invalid json format") ;
        jsn >> node[(const char*) key.c_str()] ;
    } else{
        stream.unget() ;
        if (! _getCont(stream, val))                throw std::logic_error("[3] invalid json format") ;
        node[ (const char*) key.c_str()] = val ;
    }

    if (jin::getChar(stream, input, true) == ','){
        goto start_parsing_key ;
    } else {
        stream.unget() ;
    }

    while (jin::getChar(stream, input, true) == '}'){
        open_curly -= 1 ;
    }
    
//    if (open_curly)                                 throw std::logic_error("[4] invalid json format") ;
    return true ;
}

// TODO: add eof check
bool _getKey(std::istream& stream, std::string& key){
    char input ;
    if   (jin::getChar(stream, input) != '"') return false ;
    while(jin::getChar(stream, input) != '"') key += input ;
    return true ;
}
bool _getCont(std::istream& stream, std::string& cont){
    char input ;
    jin::getChar(stream, input) ;
    if      (input == '"')  return _getStr(stream, cont) ;
    else if (input == '[')  return _getVec(stream, cont) ;
    else                    return _getNum(stream, cont) ;

    return false ;
}
bool _getNum(std::istream& stream, std::string& num){
    stream.unget() ;
    char input ;
    while(jin::getChar(stream, input) != ',' and input != '}') num += input ;
    stream.unget() ;
    return true ;
}
bool _getStr(std::istream& stream, std::string& str){
    char input ;
    while(jin::getChar(stream, input) != '"') str += input ;
    return true ;
}
bool _getVec(std::istream& stream, std::string& vec){
    char input ;
    vec += '[' ;
    while(jin::getChar(stream, input) != ']') vec += input ;
    vec += ']' ;
    return true ;
}
bool _dumpJsn(std::istream& stream, std::stringstream& jsn){
    char input ;
    int  open_curly = 1 ;
    
    jsn << '{' ;
    while (open_curly){
        jin::getChar(stream, input) ;
        if (input == '{') open_curly += 1 ;
        if (input == '}') open_curly -= 1 ;
        jsn << input ;
    }
    return true ;
}
