#pragma warning(push)
#pragma warning( disable : 4996)
#include <steam/steam_api.h>
#pragma warning( pop ) 


#define POCO_NO_AUTOMATIC_LIBS
#define POCO_STATIC

//Generel
#include <Poco/Exception.h>
#include <Poco/StreamCopier.h>

//process
#include <Poco/Process.h>
#include <Poco/PipeStream.h>

//NET
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/URI.h>

//xml parser
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/TreeWalker.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/SAX/InputSource.h>
#include <Poco/AutoPtr.h>


#include <iostream>
#include <fstream>
#include <memory>
#include <vector>


uint64_t getSteamID()
{
    uint64_t r;

    std::ifstream f("SteamUserID.txt");
    if (!f.is_open())
    {
        std::cout << "Dont know SteamID. Caching\n";
        Poco::Pipe outPipe;
        std::vector<std::string> args;
        args.emplace_back("-get_id");
        Poco::ProcessHandle ph = Poco::Process::launch("SteamAppProxy.exe", args);
        f.open("SteamUserID.txt");
    }
    f >> r;
    return r;
}

Poco::AutoPtr<Poco::XML::Document> downloadGamesXML(uint64_t steamID)
{
    using namespace Poco;
    using namespace Poco::Net;


    const URI uri("http://steamcommunity.com");
    const std::string path = "/profiles/" + std::to_string(steamID) + "/games?tab=all&xml=1";
    try {

        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_0);
        HTTPResponse response;

        session.sendRequest(request);
        std::istream& is = session.receiveResponse(response);
        Poco::XML::InputSource xis (is);
        Poco::XML::DOMParser p;
        return Poco::AutoPtr<Poco::XML::Document>(p.parse(&xis));
    }
    catch (Exception &ex)
    {
        std::cout << ex.displayText().c_str() << std::endl;
        return Poco::AutoPtr<Poco::XML::Document>(nullptr);
    }
}

int main()
{
    std::cout << "Get SteamID\n";
    uint64_t steamID = getSteamID();
    std::cout << "SteamID Found: " << steamID << "\n";
    
    std::cout << "Download Gamelist. Takes some time\n";
    auto xml = downloadGamesXML(steamID);
  
    Poco::XML::TreeWalker tw(xml, Poco::XML::NodeFilter::SHOW_ALL);
    Poco::XML::Node* n = xml;
    std::cout << "List all achievements from all games with description\n";
#ifdef _DEBUG
    int i = 10;
#endif // _DEBUG

    while (n)
    {
        if (n->nodeName() == "appID")
        {
            Poco::XML::XMLString appid_str = n->firstChild()->nodeValue();

            std::ofstream steamAppIdF("steam_appid.txt");
            steamAppIdF << appid_str;
            //steamAppIdF << "232790";
            steamAppIdF.close();

            //start process
            Poco::Pipe outPipe;
            std::vector<std::string> args;
            std::cout << "Achievements from game with appID: " << appid_str << "\n";
            Poco::ProcessHandle ph = Poco::Process::launch("SteamAppProxy.exe", args,  0, &outPipe, 0);
            Poco::PipeInputStream istr(outPipe);
            Poco::StreamCopier::copyStream(istr, std::cout);
#ifdef _DEBUG
            --i;
#endif //_DEBUG
        }
#ifdef _DEBUG
        if (i == 0)
            n = nullptr;
        else
            n = tw.nextNode();
#else
        n = tw.nextNode();
#endif //_DEBUG
    }
    return 0;
}