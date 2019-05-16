#ifndef NOD_CLS
#define NOD_CLS

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
#include <typeinfo>

namespace json_options{
    extern int  INDENTATION ;
}

class json{
    public:
        json();
       ~json(); 
        
        std::list<std::string>& keys();

        void _add_child (json* child);

        json&               operator[](const char*  node_name) ;
        void                    operator= (const char*  content  ) ;
        void                    operator= (const float  content  ) ;
        void                    operator= (const double content  ) ;
        void                    operator= (const bool  content   ) ;
        void                    operator= (const int    content  ) ;

        friend std::ostream&    operator<<(std::ostream& stream, json& node) ;
    
    private:
        json(const char* main_label);

        std::vector<json*>  _subnodes   ;
        std::list<std::string>  _keys       ;
        std::string             _content    ;
        std::string             _label      ;
        std::string             _type       ;

        bool    _last   ;
        int     _depth  ;
};

std::ostream& operator<<(std::ostream& os, json& node) ;

#endif