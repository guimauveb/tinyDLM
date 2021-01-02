#include "../include/Connector.hxx"

int main()
{
    std::unique_ptr<Connector> C = std::make_unique<Connector>("postgres", "localhost", "dd@awylds", "download");
    C->performQuery("");

    return 0;
}
