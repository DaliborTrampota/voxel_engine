#include "EventSource.h"

void EventSource::subscribe(EventSite* site) {
    m_sources.insert(site);
}

void EventSource::unsubscribe(EventSite* site) {
    m_sources.erase(site);
}