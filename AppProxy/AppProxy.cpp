#pragma warning(push)
#pragma warning( disable : 4996)
#include <steam/steam_api.h>
#pragma warning( pop ) 

#include <iostream>
#include <fstream>

uint64_t getSteamID()
{
    //fake start spacewar game recieveing the SteamID
    //maybe should find a better solution than faking spacewars
    std::ofstream steamAppIdF("steam_appid.txt");
    steamAppIdF.write("480", 3);
    steamAppIdF.close();

    SteamAPI_Init();
    CSteamID userid = SteamUser()->GetSteamID();
    SteamAPI_Shutdown();

    return userid.GetStaticAccountKey();
}

int main(int argc, char* argv[])
{
    bool running = SteamAPI_IsSteamRunning();
    bool initOK = SteamAPI_Init();
    if (argc > 1)
    {
        std::ofstream o("SteamUserID.txt");
        o << getSteamID();
        SteamAPI_Shutdown();
        return 0;
    }

    SteamUserStats()->RequestCurrentStats();
    uint32_t numAchievements = SteamUserStats()->GetNumAchievements();
    std::cout << "#Achievements: " << numAchievements << std::endl;
    for (uint32_t i = 0; i < numAchievements; ++i)
    {
        const char* name = SteamUserStats()->GetAchievementName(i);
        std::cout << name << "\t-\t";
        std::cout << SteamUserStats()->GetAchievementDisplayAttribute(name, "desc") << "\n";

        //SteamUserStats()->SetAchievement(name);
        //SteamUserStats()->StoreStats();
    }
    SteamAPI_Shutdown();
}