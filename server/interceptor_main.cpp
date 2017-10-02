#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "generated_interface.h"
#include "generated_impl.h"

#define SERVER_PATH "/com/test/fake/nativeguictrl"
#define SERVER_NAME "com.test.fake.nativeguictrl"

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

class DBus_interface : public org::freedesktop::DBus_proxy,
        public DBus::ObjectProxy
{
public:
    DBus_interface(DBus::Connection& c) : DBus::ObjectProxy(c, "/org/freedesktop/DBus", "org.freedesktop.DBus")
    {

    }

    virtual void NameOwnerChanged(const std::string& argin0, const std::string& argin1, const std::string& argin2) override
    {

    }
    virtual void NameLost(const std::string& argin0) override
    {

    }
    virtual void NameAcquired(const std::string& argin0) override
    {

    }
};

class nativeguictrl_wrapper_interface : public com::test::fake::nativeguictrl_proxy,
        public DBus::ObjectProxy
{
public:
    nativeguictrl_wrapper_interface(DBus::Connection& c, const char* s) : DBus::ObjectProxy(c, SERVER_PATH, s)
    {

    }
};


class nativeguictrl_impl : public com::test::fake::nativeguictrl_adaptor,
        public DBus::IntrospectableAdaptor,
        public DBus::ObjectAdaptor
{
    DBus_interface connInterface;
    std::unique_ptr<nativeguictrl_wrapper_interface> inner;
public:
    nativeguictrl_impl(DBus::Connection& c) : DBus::ObjectAdaptor(c, SERVER_PATH), connInterface(c)
    {
        std::vector<std::string> owners = connInterface.ListQueuedOwners(SERVER_NAME);
        for (const std::string& s : owners)
        {
            if (s == service())
            {
                //it's us
                continue;
            }
            std::cout << "Found " << s << std::endl;
            inner.reset(new nativeguictrl_wrapper_interface(c, s.c_str()));
            break;
        }

    }
    virtual void SetRequiredSurfaces(const std::string& surfaces, const int16_t& bFadeOpera) override
    {
        std::cout << "Interceptor::SetRequiredSurfaces(" << surfaces << ", " << bFadeOpera << ")" << std::endl;
        if (inner)
        {
            std::string modified = "Modified: " + surfaces;
            inner->SetRequiredSurfaces(modified, -bFadeOpera);
        }
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
    if (!conn.has_name(SERVER_NAME))
    {
        std::cout << "No server" << std::endl;
        return 1;
    }

    conn.request_name(SERVER_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING);
    nativeguictrl_impl impl(conn);

//    conn.add_match("eavesdrop=true,type='method_call',path='" SERVER_PATH "', member='SetRequiredSurfaces'");
//    DBus::MessageSlot messageSlot;
//    messageSlot = new SetRequiredSurfacesHandler(); //freed by MessageSlot
//    conn.add_filter(messageSlot);

    std::cout << "Running interceptor" << std::endl;
    dispatcher.enter();

//    conn.remove_filter(messageSlot);

    return 0;
}
