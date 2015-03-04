#include <gimgui/logic/allpropagator.hpp>
#include <gimgui/data/element.hpp>

namespace gim
{
    AllPropagator::AllPropagator(gim::Element& first)
    {
        mElements.push_back(&first);

        for(auto elementIterator = mElements.begin(); elementIterator != mElements.end(); elementIterator++)
        {
            for(auto& child : (*elementIterator)->children())
            {
                mElements.push_back(child.get());
            }
        }
        
        mCurrentElement = mElements.begin();
    }

    gim::Element* AllPropagator::next()
    {
        if(mCurrentElement != mElements.end())
        {
            gim::Element* toReturn = *mCurrentElement;
            ++mCurrentElement;
            return toReturn;
        }
        else
        {
            return nullptr;
        }
    }
}
