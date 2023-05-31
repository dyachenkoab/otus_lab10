#include "observer.h"

ObserverType Observer::type() const
{
    return m_type;
}

Observer::Observer(ObserverType type) : m_type(type)
{

}

