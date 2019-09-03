#ifndef NOD_CLS
#define NOD_CLS

#include "../include/namespace.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
#include <typeinfo>
#include <sstream>
#include <array>
#include <iomanip>

class json{
    public:
        json();
       ~json(); 
        
        /* TODO:
           add vector getters */
        // std::vector<bool>        bVec() ;
        // std::vector<int>         iVec() ;
        // std::vector<float>       fVec() ;
        // std::vector<double>      dVec() ;
        // std::vector<std::string> sVec() ;

        /*
        getters for node content */
        std::list<std::string>& keys()   ;
        std::string             String() ;
        int                     Int()    ;
        float                   Float()  ;
        double                  Double() ;
        bool                    Bool()   ;

        /*
        operators */
        json&   operator[](const char*  node_name) ;
        
        template <typename T>
        void    operator= (T content){
            _content.str("") ;
            _content << std::boolalpha ;
            jeq::operator< (_content, content) ;
        }

        /*
        json class friends */
        friend std::ostream& operator<<(std::ostream& stream, json& node) ;
        friend bool          operator>>(std::istream& stream, json& node) ;

    
    private:
        json(const char* main_label);               // private constructor used by the class for creating subnodes

        void _add_child (json* child);              // add subnode
        bool _is_last()  ;                          // about the node position in the mother's subnode vector
        bool _is_first() ;                          // about the node position in the mother's subnode vector

        std::vector<json*>      _subnodes   ;
        std::list<std::string>  _keys       ;
        std::stringstream       _content    ;       // content is saved (whatever type) into a sringstream
        std::string             _label      ;

        json*   _mother ;   // address of the node mother

        bool    _last   ;   // same as _is_last (not a shortcut: set by the constructor during node creation)
        int     _depth  ;   // depth of the node (position in the tree)
};

/* 
other operators and functions */
std::ostream& operator<<(std::ostream& stream, json& node) ;    // overload output to a string (convert the json tree to a correct json stream)
bool          operator>>(std::istream& stream, json& node) ;    // input from a file
std::string labely(std::string& label) ;                        // add the correct json-label syntax to a string (foo -> "foo" :)

#endif