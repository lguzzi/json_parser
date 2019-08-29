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
    if (jin::getChar(stream, input, true) != '{')   throw std::logic_error("[0] invalid json format") ;

// goto reference used to "recursively" parse the stream
start_parsing_key:
    if (jin::getChar(stream, input, true) == stream.eof()){
        return true ;
    } else {
        stream.unget() ;
    }

    std::string         key ;
    std::string         val ;
    std::stringstream   jsn ;
    bool                is_vec = false ;
    bool                is_str = false ;
    bool                is_flt = false ;

    //get the key and the content and save them inside a string
    if (! _getKey(stream, key))                                 throw std::logic_error("[1] invalid json format") ;
    if (jin::getChar(stream, input, true) != ':')               throw std::logic_error("[2] invalid json format") ;
    // if the value is a json node
    if (jin::getChar(stream, input, true) == '{'){
        if (! _dumpJsn(stream, jsn))                            throw std::logic_error("[3] invalid json format") ;
        // recursivelly dump the json node read from the file into the json object
        jsn >> node[(const char*) key.c_str()] ;
    } else{
        stream.unget() ;
        // if it's not a subnode, it's a value (string, bool, number or vector)
        if (! _getCont(stream, val, is_str, is_vec, is_flt))    throw std::logic_error("[4] invalid json format") ;
        // guess the type of the value (is there a smart way to do it?)
        if      (val == "true"  and ! is_str)   node[ (const char*) key.c_str()] = true  ;
        else if (val == "false" and ! is_str)   node[ (const char*) key.c_str()] = false ;
        else if (is_str)                        node[ (const char*) key.c_str()] = val   ;
        else if (is_vec){
            std::vector<int>    vint ;
            std::vector<double> vdbl ;
           
            std::vector<bool>   vboo ;
            if (val[0] != '[')              throw std::logic_error("[5] invalid json format") ;
            if (val[val.size() - 1] != ']') throw std::logic_error("[6] invalid json format") ;

            std::string                 entry   ;
            std::vector<std::string>    entries ;
            for (std::string::iterator it = val.begin(); it != val.end(); ++it){
                if (*it == '[') continue ;
                if (*it == ',' || *it == ']'){
                    entries.push_back(entry) ;
                    entry = std::string("")  ;
                    if (*(it + 1) == ' ') ++it ;
                } else{
                    entry += *it ;
                }
            }
            if (entries[0].find('"') != std::string::npos ){
                std::vector<char*>  vec ;
                for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); ++it){
                    vec.push_back((char*) it->c_str()) ;
                }
                node[ (const char*) key.c_str()] = vec ;
            } else if (entries[0].find('.') != std::string::npos ){
                std::vector<double>  vec ;
                for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); ++it){
                    vec.push_back( std::stod(*it)) ;
                }
                node[ (const char*) key.c_str()] = vec ;
            } else if (entries[0] == "true" || entries[0] == "false"){
                std::vector<bool>  vec ;
                for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); ++it){
                    if      ((*it) == "true" ) vec.push_back(true ) ;
                    else if ((*it) == "false") vec.push_back(false) ;
                    else throw std::logic_error("[0] invalid boolean stream") ;
                }
                node[ (const char*) key.c_str()] = vec ;
            } else{
                std::vector<int>  vec ;
                for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); ++it){
                    vec.push_back( std::stoi(*it)) ;
                }
                node[ (const char*) key.c_str()] = vec ;
            }
        }
        else if (is_flt)                        node[ (const char*) key.c_str()] = std::stod(val) ;
        else                                    node[ (const char*) key.c_str()] = std::stoi(val) ;
    }

    // if a comma is found, another key follows
    if (jin::getChar(stream, input, true) == ',')   goto start_parsing_key ;
    else if (input != '}')                          throw std::logic_error("[5] invalid json format") ;

    return true ;
}

bool _getKey(std::istream& stream, std::string& key){
    /* 
    get the node key (ie the label) */
    char input ;
    if   (jin::getChar(stream, input) != '"') return false ;
    while(jin::getChar(stream, input) != '"') key += input ;
    return true ;
}
bool _getCont(std::istream& stream, std::string& cont, bool& is_str, bool& is_vec, bool& is_flt){
    /*
    get the node content (ie its value) */
    char input ;
    jin::getChar(stream, input) ;
    if (input == '"') {
        is_str = true ;
        return _getStr(stream, cont) ;
    } else if (input == '[') {
        is_vec = true ;
        return _getVec(stream, cont) ;
    } else {
        return _getNum(stream, cont, is_flt) ;
    }

    return false ;
}
bool _getNum(std::istream& stream, std::string& num, bool& is_flt){
    /*
    if it's a number */
    stream.unget() ;
    char input ;
    while(jin::getChar(stream, input) != ',' and input != '}') {
        num += input ;
        if (input == '.') is_flt = true ;
    }
    stream.unget() ;
    return true ;
}
bool _getStr(std::istream& stream, std::string& str){
    /*
    if it's a string */
    char input ;
    while(jin::getChar(stream, input) != '"') str += input ;
    return true ;
}
bool _getVec(std::istream& stream, std::string& vec){
    /*
    if it's a vector */
    char input ;
    vec += '[' ;
    while(jin::getChar(stream, input) != ']') vec += input ;
    vec += ']' ;
    return true ;
}
bool _dumpJsn(std::istream& stream, std::stringstream& jsn){
    /*
    pull out a json subnode from a stream and dump it to a standalone temporary stream */
    char input ;
    // use curly brackets to isolate the json subnode
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