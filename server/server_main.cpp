#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "generated_impl.h"

static const char* SERVER_NAME = "com.test.fake.nativeguictrl";
static const char* SERVER_PATH = "/com/test/fake/nativeguictrl";

class nativeguictrl_impl : public com::test::fake::nativeguictrl_adaptor,
        public DBus::IntrospectableAdaptor,
        public DBus::ObjectAdaptor
{
public:
    nativeguictrl_impl(DBus::Connection& c) : DBus::ObjectAdaptor(c, SERVER_PATH)
    {

    }
    virtual void SetRequiredSurfaces(const std::string& surfaces, const int16_t& bFadeOpera) override
    {
        std::cout << "Server::SetRequiredSurfaces(" << surfaces << ", " << bFadeOpera << ")" << std::endl;
    }
};



DBus::BusDispatcher dispatcher;

void quit(int sig)
{
    std::cout << "Quitting " << SERVER_PATH << std::endl;
    dispatcher.leave();
}


int main()
{
    signal(SIGTERM, quit);
    signal(SIGINT, quit);


    DBus::default_dispatcher = &dispatcher;

    DBus::Connection conn = DBus::Connection::SessionBus();
    conn.request_name(SERVER_NAME, DBUS_NAME_FLAG_ALLOW_REPLACEMENT);

    nativeguictrl_impl server(conn);

    std::cout << "Running " << SERVER_PATH << std::endl;
    dispatcher.enter();

    return 0;
}
