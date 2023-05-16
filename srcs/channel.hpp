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

        int JoinClient( int client_sd, bool oprtor );
        void PartClient( int client_sd );
        void SetChannelConcept( std::string new_concept );

        std::string GetChannelConcept( void );
        std::vector<int> GetClients( void );
        std::vector<int> GetOperators( void );
        std::vector<int> GetNoOperatorClients( void );
    private:
        std::string         channel_concept;
        std::vector<int>    client_sd_list;
        std::vector<int>    oprtor_sd_list;
        //permisos
};

#endif
