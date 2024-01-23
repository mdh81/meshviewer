#ifndef MESH_VIEWER_CALLBACK_BASE_H
#define MESH_VIEWER_CALLBACK_BASE_H

#include <tuple>
#include <functional>

namespace mv { namespace events {

// An object of this type holds a sequence of indices that are used to index
// into a tuple
template<unsigned...> struct IndexSequence {};

// Recursive template to expand the index sequence in numerical order
template<unsigned Index, unsigned... RemainingIndices> struct IndexGenerator : public IndexGenerator<Index-1, Index-1, RemainingIndices...> {};

// Template specialization for the generator when the first index is 0
template<unsigned... RemainingIndices> struct IndexGenerator<0, RemainingIndices...> { using type = IndexSequence<RemainingIndices...>;
using intType = int; };

class Callback_Old {
    public:
        // TODO: This function can be a const
        virtual void call() const = 0;
        virtual ~Callback_Old() = default;
};

} }


#endif
