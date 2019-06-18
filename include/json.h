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
    extern int PRECISION ;
}

namespace jout{
    /*
    namespace for safe operator overloading */
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
    // must unreference the bool reference (see std::vector<bool> discussions)
    // https://en.cppreference.com/w/cpp/container/vector_bool
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
        
        std::list<std::string>& keys();
        std::string             String() ;
        int                     Int()    ;
        float                   Float()  ;
        double                  Double() ;
        json&   operator[](const char*  node_name) ;
        template <typename T>
        void    operator= (T content){
            _content.str("") ;
            _content << std::boolalpha ;
            jout::operator< (_content, content) ;
        }

        friend std::ostream& operator<<(std::ostream& stream, json& node) ;
        friend bool          operator>>(std::istream& stream, json& node) ;

    
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
bool          operator>>(std::istream& stream, json& node) ;

bool _getKey (std::istream& stream, std::string& key)  ;
bool _getNum (std::istream& stream, std::string& num, bool& is_flt)  ;
bool _getStr (std::istream& stream, std::string& str)  ;
bool _getVec (std::istream& stream, std::string& vec)  ;
bool _getCont(std::istream& stream, std::string& cont, bool& is_str, bool& is_vec, bool& is_flt) ;
bool _dumpJsn(std::istream& stream, std::stringstream& jsn) ;

#endif