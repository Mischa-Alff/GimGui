#include "events.hpp"
#include <helpers/color.hpp>
#include <gimgui/logic/absolutemap.hpp>
#include <gimgui/logic/boundarypropagator.hpp>
#include <gimgui/logic/allpropagator.hpp>

void changeHealth(gim::Element& element, int32_t healthValue)
{
    gim::AllPropagator propagator(element);

    while(gim::Element* currentElement = propagator.next())
    {
        if(currentElement->tags().count("health_bar") != 0)
        {
            currentElement->setAttribute("health", healthValue);
        }
    }
}

void click(gim::Element& element, const Vec2& position)
{
    gim::BoundaryPropagator<Vec2> propagator(element, {position});
    propagator.reverse();

    while(gim::Element* currentElement = propagator.next())
    {
        CallbackExecutor<Vec2> executor("on_click");
        executor.execute(*currentElement, position);

        if(bool* blocks = currentElement->findAttribute<bool>("block_event"))
            break;
    }
}

void mouseRelease(gim::Element& element, const Vec2& position)
{
    gim::AllPropagator propagator(element);

    while(gim::Element* currentElement = propagator.next())
    {
        CallbackExecutor<Vec2> executor("on_mouse_release");
        executor.execute(*currentElement, position);
    }
}

void moveMouse(gim::Element& element, const Vec2& currentPosition, const Vec2& lastPosition)
{
    gim::BoundaryPropagator<Vec2> propagator(element, {currentPosition, lastPosition});

    gim::AbsoluteMap<Vec2> absoluteMap("position");

    while(gim::Element* currentElement = propagator.next())
    {
        const Vec2& position = absoluteMap.getAbsoluteOf(*currentElement);
        const Vec2& size = currentElement->getAttribute<Vec2>("size");

        bool currentPosOverlaps = currentPosition.x > position.x &&
                                  currentPosition.x < position.x + size.x &&
                                  currentPosition.y > position.y &&
                                  currentPosition.y < position.y + size.y;

        bool lastPosOverlaps = lastPosition.x > position.x &&
                               lastPosition.x < position.x + size.x &&
                               lastPosition.y > position.y &&
                               lastPosition.y < position.y + size.y;

        if(currentElement->getAttribute<bool>("dragged", false))
        {
            Vec2 delta;
            delta.x = currentPosition.x - lastPosition.x;
            delta.y = currentPosition.y - lastPosition.y;

            if(!currentElement->getAttribute<bool>("resize", false))
            {
                Vec2 relativePosition = currentElement->getAttribute<Vec2>("position");
                Vec2 newPosition;
                newPosition.x = relativePosition.x + delta.x;
                newPosition.y = relativePosition.y + delta.y;

                currentElement->setAttribute("position", newPosition);
            }
            else
            {
                Vec2 newSize;
                newSize.x = size.x + delta.x;
                newSize.y = size.y + delta.y;

                currentElement->setAttribute("size", newSize);
            }
        }

        if(currentPosOverlaps && !lastPosOverlaps)
        {//got hovered
            CallbackExecutor<Vec2> executor("on_hover");
            executor.execute(*currentElement, position);
        }
        else if(lastPosOverlaps && !currentPosOverlaps)
        {//got blurred
            CallbackExecutor<> executor("on_blur");
            executor.execute(*currentElement);
        }
    }
}
