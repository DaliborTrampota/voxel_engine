#include <unordered_set>

namespace engine {
    class EventSite;

    class EventSource {
      public:
        EventSource() = default;
        virtual ~EventSource() = default;

        void subscribe(EventSite* site);
        void unsubscribe(EventSite* site);

        template <typename TEventSite, typename TEvent>
        void fireEvent(void (TEventSite::*fire)(TEvent* event), TEvent* event) {
            for (EventSite* site : m_sources) {
                TEventSite* appSite = static_cast<TEventSite*>(site);
                (appSite->*fire)(event);
            }
        }

      private:
        std::unordered_set<EventSite*> m_sources;
    };
}  // namespace engine