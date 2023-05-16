#ifndef _CHANNEL
# define _CHANNEL

#include <iostream>
#include <vector>
#include <string>

class Channel {

    public:
        Channel( void );
        ~Channel( void );
        Channel( const Channel& other );

        int JoinClient( int client_sd );
        void PartClient( int client_sd );
        void SetChannelConcept( std::string new_concept );

        std::string GetChannelConcep( void );
        std::vector<int> GetClients( void );
    private:
        std::string         channel_concept;
        std::vector<int>    client_sd_list;
        //permisos
};

#endif
