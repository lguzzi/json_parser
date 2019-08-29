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

namespace jopt{
    /*
    napespace for configurations and options */
    extern int PRECISION ;  // argument of std::setprecision
}

namespace jout{
    /*
    namespace for operator overloading (output)*/
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
        stream  << std::setprecision(jopt::PRECISION) 
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
    template<typename S, typename T>
    S& operator<(S& stream, std::vector<T>& array){
        stream << '[' ;
        for (int i = 0; i < array.size(); i++){
        stream  < array[i] ;
        stream  << std::string(i != array.size() - 1, ',') 
                << std::string(i != array.size() - 1, ' ') ;
        }
        stream << ']' ;
        return stream ;
    }  
    // see https://en.cppreference.com/w/cpp/container/vector_bool for bool vectors caveant
    template<typename S>
    S& operator<(S& stream, std::vector<bool>& array){
        stream << '[' ;
        // l-type only for the reference
        bool fingolfin = true  ;
        bool finfarin  = false ;

        for (int i = 0; i < array.size(); i++){
        if (array[i] == true)   stream  < fingolfin  ;
        else                    stream  < finfarin   ;
        stream  << std::string(i != array.size() - 1, ',') 
                << std::string(i != array.size() - 1, ' ') ;
        }
        stream << ']' ;
        return stream ;
    }  
}

namespace jin{
    /*
    namespace for operator overloading (input) */
    template <typename S>
    char& getChar(S& stream, char& input, bool skip_spaces = false){
        input = stream.get() ;
        while ( input == '\n' || input == '\t' || (!skip_spaces && input == ' ')){
            input = stream.get() ;
        }
    
        return input ;
    }
}

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
            jout::operator< (_content, content) ;
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
        std::string _labely(std::string& label) ;   // add the correct json-label syntax to a string (foo -> "foo" :)

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

bool _getKey (std::istream& stream, std::string& key)  ;                    // get the next key when reading a json file
bool _getNum (std::istream& stream, std::string& num, bool& is_flt)  ;      // get a numeric node content when reading from a stream
bool _getStr (std::istream& stream, std::string& str)  ;                    // get a string node content when reading from a stream
bool _getVec (std::istream& stream, std::string& vec)  ;                    // get a vectorial node content when reading from a stream
bool _getCont(std::istream& stream, std::string& cont, bool& is_str, bool& is_vec, bool& is_flt) ;  // get s gerneric content from a json stream
bool _dumpJsn(std::istream& stream, std::stringstream& jsn) ;               // isolate a json subnode into a temporary stream when reading from a stream

#endif