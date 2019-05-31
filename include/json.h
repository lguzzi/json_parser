#ifndef NOD_CLS
#define NOD_CLS

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
#include <typeinfo>
#include <sstream>
#include <array>
#include <iomanip>
//TODO
// set automatic precision

namespace jopt{
    extern int PREC ;
}

namespace jout{
    /*
    input-output namespace for safe operator overloading */
    template<typename S>
    S& operator<(S& stream, const char* value){
        stream << '"' << value << '"' ;
        return stream ;
    }
    template<typename S>
    S& operator<(S& stream, std::string& value){
        stream << '"' << value << '"' ;
        return stream ;
    }
    template<typename S, typename O>
    S& operator<(S& stream, O& value){
        stream  << std::setprecision(jopt::PREC) 
                << value ;
        return stream ;
    }
    template<typename S, typename T, size_t N>
    S& operator<(S& stream, std::array<T, N>& array){
        stream << '[' ;
        for (int i = 0; i < array.size(); i++){
        stream  < array[i] ;
        stream  << std::string(i != array.size() - 1, ',') 
                << std::string(i != array.size() - 1, ' ') ;
        }
        stream << ']' ;
        return stream ;
    }
}

class json{
    public:
        json();
       ~json(); 
        
        std::list<std::string>& keys();


        json&   operator[](const char*  node_name) ;
        template <typename T>
        void    operator= (T content){
            _content.str("") ;
            _content << std::boolalpha ;
            jout::operator< (_content, content) ;
        }

        friend std::ostream& operator<<(std::ostream& stream, json& node) ;
    
    private:
        json(const char* main_label);

        void _add_child (json* child);
        bool _is_last()  ;
        bool _is_first() ;
        std::string _labely(std::string& label) ;

        std::vector<json*>      _subnodes   ;
        std::list<std::string>  _keys       ;
        std::stringstream       _content    ;
        std::string             _label      ;

        json*   _mother ;

        bool    _last   ;
        int     _depth  ;
};

std::ostream& operator<<(std::ostream& stream, json& node) ;

#endif