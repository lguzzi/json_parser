#ifndef NSP_JSN
#define NSP_JSN

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

namespace jeq{
    /*
    namespace for assign operations*/
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
    char getChar(S& stream, bool skip_spaces = false){
        char input ;
        input = stream.get() ;
        while ( input == '\n' || input == '\t' || (!skip_spaces && input == ' ')){
            input = stream.get() ;
        }
    
        return input ;
    }
    
    template <typename S>
    bool getKey(S& stream, std::string& key){
    /* 
    get the node key (ie the label) */
        char input ;
        if   (jin::getChar(stream) != '"') return false ;
        while((input = jin::getChar(stream)) != '"') key += input ;
        return true ;
    }

    template <typename S>
    bool getCont(S& stream, std::string& cont){
    /*
    get the node content (ie its value) */
        char input ;
        bool open_quote  = false ;
        bool open_square = false ;
        while(true){
            input = jin::getChar(stream, false) ;
            if (input == '\"') open_quote = !open_quote ;
            if (input == '[' && !open_quote){
                if (!open_square) open_square  = true  ;
                else return false ;
            }
            if (input == ']' && !open_quote){
                if (open_square) open_square = false ;
                else return false ;
            }
            if ( (input == ',' || input == '}') && !open_quote && !open_square){
                stream.unget() ;   
                return true ;
            }    
            cont += input ;
        }
        return false ;
    }

    template <typename S>
    bool dumpJsn(S& stream, std::stringstream& jsn){
    /*
    pull out a json subnode from a stream and dump it to a standalone temporary stream */
        char input ;
        // use curly brackets to isolate the json subnode
        int  open_curly = 1 ;

        jsn << '{' ;
        while (open_curly){
            input = jin::getChar(stream) ;
            if (input == '{') open_curly += 1 ;
            if (input == '}') open_curly -= 1 ;
            jsn << input ;
        }
        return true ;
    }
}

#endif