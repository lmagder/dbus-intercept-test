#include <iostream>

#include "generated_interface.h"

static const char* SERVER_NAME = "com.test.fake.nativeguictrl";
static const char* SERVER_PATH = "/com/test/fake/nativeguictrl";

class nativeguictrl_interface : public com::test::fake::nativeguictrl_proxy,
        public DBus::ObjectProxy
{
public:
    nativeguictrl_interface(DBus::Connection& c) : DBus::ObjectProxy(c, SERVER_PATH, SERVER_NAME)
    {

    }
};



DBus::BusDispatcher dispatcher;

int main(int argc, const char** argv)
{

    DBus::default_dispatcher = &dispatcher;

    DBus::Connection conn = DBus::Connection::SessionBus();
    conn.register_bus();

    nativeguictrl_interface i(conn);
    std::cout << "Client: Sending" << std::endl;

    std::string sval = "Some bullshit";
    int16_t ival = 123;
    if (argc > 1)
    {
        sval = argv[1];
        if (argc > 2)
        {
            ival = int16_t(atoi(argv[2]));
        }
    }

    i.SetRequiredSurfaces(sval, ival);

    return 0;
}
