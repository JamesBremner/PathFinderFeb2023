#pragma once
#include "cVE.h"

namespace raven
{
    namespace graph {

    /// @brief graph vertex and edge attributes

    class cAttribute 
    {
    public:

        void clear();

        void resizeVertex( int c )
        {
            vVertexName.resize(c);
            vVertexAttr.resize(c);
        }
        void add( const std::string& name )
        {
            vVertexName.push_back( name );
        }
        void wVertexName( int iv, const std::string& name )
        {
            vVertexName[iv] = name;
        }

        /// @brief write vertex attributes
        /// @param iv vertex index
        /// @param vsAttr vector of strings to replace vertex attributes
        void wVertexAttr(int iv, const std::vector<std::string> &vsAttr);

        /// @brief write edge attributes
        /// @param ei edge index
        /// @param vsAttr vector of strings to replace vertex attributes
        void wEdgeAttr(int ei, const std::vector<std::string> &vsAttr);

        void wEdgeAttr( int v1, int v2,  const std::vector<std::string> &vsAttr );

        /// @brief read vertex attributes
        /// @param vi vertex index
        /// @param ai attrinute index
        /// @return string representation of attribute
        std::string rVertexAttr(int vi, int ai ) const;

        /// @brief read edge attributes
        /// @param ei edge index
        /// @param ai attrinute index
        /// @return string representation of attribute
        std::string rEdgeAttr(int ei, int ai ) const;

        int find( const std::string& name ) const;

        std::string userName( int vi ) const
        {
            return vVertexName[vi];
        }



    private:
        /** vertex attributes
         *
         *  vVertexAttr[vi][ai] is a string representing the aith attribute of the vith vertex
         */

        std::vector<std::string> vVertexName;
        std::vector<std::vector<std::string>> vVertexAttr;

        /* edge attributes

            vEdgeAttr[ei][ai] is a string representing the aith attribute of the eith edge
        */
        std::vector<std::vector<std::string>> vEdgeAttr;
    };

}
}