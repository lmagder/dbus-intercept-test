#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "generated_interface.h"

#define SERVER_PATH "/com/test/fake/nativeguictrl"

class SetRequiredSurfacesHandler : public DBus::Callback_Base<bool, const DBus::Message&>
{
public:
    bool call(const DBus::Message& param) const override
    {
        if (param.type() == DBUS_MESSAGE_TYPE_METHOD_CALL)
        {
            const DBus::CallMessage& methodMessage = static_cast<const DBus::CallMessage&>(param);
            printf("SetRequiredSurfaces message caught sender: %s dest: %s path: %s interface %s member %s\n", methodMessage.sender(), methodMessage.destination(), methodMessage.path(), methodMessage.interface(), methodMessage.member());
            DBus::MessageIter messageIter = methodMessage.reader();
            std::string sval = messageIter.get_string();
            messageIter++;
            int16_t ival = messageIter.get_int16();
            std::cout << "p0 \"" << sval << "\" p1 = " << ival << std::endl;

            if (ival != 123)
            {
                return false;
            }
        }
        return true;
    }
};

DBus::BusDispatcher dispatcher;

void quit(int sig)
{
    std::cout << "Quitting interceptor" << std::endl;
    dispatcher.leave();
}


int main()
{
    signal(SIGTERM, quit);
    signal(SIGINT, quit);


    DBus::default_dispatcher = &dispatcher;

    DBus::Connection conn = DBus::Connection::SessionBus();
    conn.register_bus();

    conn.add_match("eavesdrop=true,type='method_call',path='" SERVER_PATH "', member='SetRequiredSurfaces'");
    DBus::MessageSlot messageSlot;
    messageSlot = new SetRequiredSurfacesHandler(); //freed by MessageSlot
    conn.add_filter(messageSlot);

    std::cout << "Running interceptor" << std::endl;
    dispatcher.enter();

    conn.remove_filter(messageSlot);

    return 0;
}
